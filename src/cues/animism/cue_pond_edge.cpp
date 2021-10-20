#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_pond_edge( cueArgs args )
{
    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;

    MapOSC& m_state_cache = args.cache;
    cvglRandom& m_rand_generator = args.randGen;


    if( isNewCue )
    {

        b.addMessage("/contrast", 1);

        b.addMessage("/luma_mix", 0.);
        b.addMessage("/flow_mix", 0.);
        b.addMessage("/noise_mix", 0.);

    //    out.addMessage("/dpo/pregain/dB",          -28);
        out.addMessage("/loop/pregain/dB",         -8);
        out.addMessage("/korg/pregain/dB",         0);

        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/fuzz/pregain/dB",         -100);

        out.addMessage("/spring/pregain/dB",       -100);
        out.addMessage("/sine/pregain/dB",         -100);

        // video settings
        b.addMessage("/video/black",  0);
        b.addMessage("/use/preprocess",  3);

        b.addMessage("/use/camera",  1);
        b.addMessage("/overlap/cameras", 0.0 );
        b.addMessage("/overlap/flip", 0.);


        b.addMessage("/enable/hull", 0);
        b.addMessage("/enable/minrect", 0);
        b.addMessage("/enable/contour", 0);

      //  cout << "use camera" << 2 << endl;
        b.addMessage("/size/min", 0.000 );
        b.addMessage("/size/max", 0.9 );
        b.addMessage("/thresh", 10 );
        b.addMessage("/invert", 0 );


        // synth
        out.addMessage("/dpo/intervals", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
        out.addMessage("/dpo/baseMIDI", 0);

        out.addMessage("/dpo/mod_bus_phase/sync", 0);

        out.addMessage("/dpo/index2/scale", 0, 0.2);
        out.addMessage("/dpo/shape/scale", 0, 1);
        out.addMessage("/dpo/vcf1_q/scale", -2, 0.3);
        out.addMessage("/dpo/vcf2_q/scale", 0, 0.5);
        out.addMessage("/dpo/sarah/slide/up", 0);
        out.addMessage("/dpo/sarah/slide/down", 0);

        if( m_state_cache.addressExists("/f1_offset") )
        {
           out.addMessage("/dpo/f1/val", m_state_cache["/f1_offset"].getFloat() );
           out.addMessage("/dpo/f2/val", m_state_cache["/f2_offset"].getFloat()  );
        }
        else
        {
            out.addMessage("/dpo/f1/val", ntom( "eb:5" ) );
            out.addMessage("/dpo/f2/val", ntom( "eb:5" ) );
        }

        out.addMessage("/dpo/amp/val", 1);

        out.addMessage("/sine/amp", 1);

    }

    double dpo_db = scale( sin(args.total_elapsed.count() * M_PI * 0.15), -1, 1, -48, -28);
    out.addMessage("/dpo/pregain/dB",  dpo_db);


    vector<string> v1({     "bb:4-31",   "c:5",         "d:5+4",    "c:5-27",   "c:5-27"   });
    vector<string> v2({     "c:5",       "b:4-12",      "b:4+20",   "d:5+4",    "c:5+45"   });
    vector<double> fold({    0.1,       0.125,           0.12,        0.1,        0.11         });

    ArrayXd transp_seq_t(5);
    transp_seq_t <<         0,           4,             8,          10,         20;


    double t_offset = m_state_cache.addressExists("/t_offset") ? m_state_cache["/t_offset"].getFloat() : 0;

//        elapsed_section -= t_offset;

    double seq_t = fmod( elapsed_section, 30.);

    size_t seq_minIdx = (seq_t >= transp_seq_t ).cast<int>().sum() - 1;
    size_t seq_maxIdx = transp_seq_t.size() - ( seq_t < transp_seq_t ).sum();

    double f1 = ntom( "d:5" ); // v1[ seq_minIdx ]
    double f2 = ntom( "d:5" ); //v2[ seq_minIdx ]


    // maybe better to go more linearly

    size_t prev_step = m_state_cache.addressExists("/prev/seq_step") ? m_state_cache["/prev/seq_step"].getInt() : seq_minIdx;
    double f1_offset = m_state_cache.addressExists("/f1_offset") ? m_state_cache["/f1_offset"].getInt() : 0;
    double f2_offset = m_state_cache.addressExists("/f2_offset") ? m_state_cache["/f2_offset"].getInt() : 0;
    double duration = m_state_cache.addressExists("/duration") ? m_state_cache["/duration"].getInt() : 0;


    out.addMessage("/cache", m_state_cache);
    if( prev_step != seq_minIdx )
    {
        // new step
        out.addMessage("/dpo/index1/val", m_rand_generator.uniformRand() * 0.1);

  //      int min = scale(elapsed_section, 0, 120, 0, -5);


        f1_offset = ftom( erb( mtof(f1), scale( m_rand_generator.uniformRand(), 0., 1, -1.3, 1.3) ) );
        f2_offset = ftom( erb( mtof(f2), scale( m_rand_generator.uniformRand(), 0., 1, -1.3, 1.3) ) );

        out.addMessage("/dpo/f1/val", f1_offset, 10000 );
        out.addMessage("/dpo/f2/val", f2_offset, 10000 );
        m_state_cache.addMessage("/f1_offset", f1_offset);
        m_state_cache.addMessage("/f2_offset", f2_offset);

       // f2_offset = 0;//cvgl::round(scale( m_rand_generator.uniformRand(), 0., 1, -1, 1));
        t_offset = m_rand_generator.uniformRand() * 5;
    }

   // out.addMessage("/dpo/index1/val", 0 );
    out.addMessage("/dpo/fold/val", fold[seq_minIdx] );

    m_state_cache.addMessage("/prev/seq_step", (int32_t)seq_minIdx);

    m_state_cache.addMessage("/duration", duration);

    m_state_cache.addMessage("/prev/f1", f1);

    out.addMessage("/elapsed_section", elapsed_section );
    double area_sum = data.contour_area.sum();
    out.addMessage("/contour_area_sum", area_sum );

    double sum_mag = 0, sum_area = 0, sum_x = 0, sum_y = 0;

    if( data.ncontours > 0 )
    {
        for( size_t i = 0 ; i < data.ncontours; i++ )
        {
            double w = data.contour_area[i];
            sum_area += w;

            if( data.pix_channel_stats[i].size() == 5 ) // focus channel is added to whatever the channel count is
            {
           //     sum_angle += data.pix_channel_stats[i][0].mean * w;
                sum_mag += data.pix_channel_stats[i][1].mean * w;
                sum_x += data.pix_channel_stats[i][2].mean * w;
                sum_y += data.pix_channel_stats[i][3].mean * w;
            }
            else
            {
                out.addMessage("/nchannels", (int32_t)i, (int32_t)data.pix_channel_stats[i].size() );
            }


        }

        //if( sum_mag > 0 )
        {
            double scalar = sum_area == 0 ? 1 : sum_area;
            double mag_avg = sum_mag / scalar;

            double avg_dist_x = sum_x / scalar;
            double avg_dist_y = sum_y / scalar;

            double norm_mag_avg = mag_avg / 255. ;
            double norm_2 = clip( pow( norm_mag_avg, exp(1.5)) * 100, 0., 1.);

            out.addMessage("/korg/amp", norm_2);
            out.addMessage("/korg/slide/down", 0);
            out.addMessage("/korg/slide/up", 5);
            out.addMessage("/korg/q1/val", 0.65 );
            out.addMessage("/korg/q2/val", 0.68 ); // q-drive now at 12:00
            out.addMessage("/korg/maths/speed/val", scale(norm_mag_avg, 0., 0.5, 0.1, -1) );
            out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
            out.addMessage("/korg/maths/offset/val", scale(pow( norm_mag_avg, exp(-0.1)), 0., 1., 0.25, -1));

/*
            out.addMessage("/korg/amp", norm_2);
            out.addMessage("/korg/slide/down", 0);
            out.addMessage("/korg/slide/up", 5);
            out.addMessage("/korg/q1/val", 0.9 );
            out.addMessage("/korg/q2/val", 0.6 );
            out.addMessage("/korg/maths/speed/val", scale(norm_mag_avg, 0., 0.5, 0.1, -0.6) );
            out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
            out.addMessage("/korg/maths/offset/val", scale(pow( norm_mag_avg, exp(-0.5)), 0., 1., 1, -0.8));
*/
            out.addMessage("/avg/mag", mag_avg );
            out.addMessage("/avg/x", avg_dist_x);
            out.addMessage("/avg/y", avg_dist_y );

// >> to do: make the sarah part gel with the drone and fuzzball
            out.addMessage("/dpo/sarah/amp/val", norm_2, 100 );

            out.addMessage("/dpo/sarah/slide/down", 500 );

            out.addMessage("/dpo/mod_bus_phase/ms", scale(norm_mag_avg, 0., 0.5,
                                                          scale_clip(elapsed_section, 0, 300, 1, 500),
                                                          scale_clip(elapsed_section, 0, 120, 200, 2000)) );

            out.addMessage("/dpo/mod_bus_phase/loop", 1);
            out.addMessage("/dpo/mod_bus_phase/phasecycle", 1);

            out.addMessage("/dpo/vcf1_hz/scale", scale(norm_mag_avg, 0., 0.5, -0.1, 0.8), scale(norm_mag_avg, 0., 0.5, 0.6, 1) );
            out.addMessage("/dpo/vcf2_hz/scale", scale(norm_mag_avg, 0., 0.5, -0.3, 0.8), scale(norm_mag_avg, 0., 0.5, -0.1, 1) );

            //      out.addMessage("/dpo/fold/scale", scale(norm_mag_avg, 0., 0.5, 0., 0.3), scale(norm_mag_avg, 0., 0.5, 0.1, 0.5)  );

        }


    }
    else
    {
        out.addMessage("/dpo/sarah/amp/val", 0, 100 );
        out.addMessage("/korg/amp", 0);

    }
    return out;
}
