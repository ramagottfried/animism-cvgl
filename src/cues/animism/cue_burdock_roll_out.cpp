#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_burdock_roll_out(cueArgs args)
{
    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;

    MapOSC& m_state_cache = args.cache;
    cvglRandom& m_rand_generator = args.randGen;

    double fadetime = 10;
    if( elapsed_section <= fadetime )
    {
        b.addMessage("/overlap/flip",  scale_clip(elapsed_section, 0., 5, 0.5, 0.));
        out.addMessage("/dpo/pregain/dB",  scale(elapsed_section, 0., fadetime, -70, -28));

        out.addMessage("/loop/pregain/dB",  scale(elapsed_section, 0., fadetime, -36., -8) );
        out.addMessage("/korg/pregain/dB",  scale(elapsed_section, 0., fadetime, -70., 0));

        //b.addMessage("/half_mirror/alpha", scale(elapsed_section, 0., fadetime, 0., 1.) );
    }


    // fade in luma flow for burdocks, then fade out as fuzzball comes back

    double luma_fade = 40, end_luma = 0.25;
    if( elapsed_section <= luma_fade )
    {
        double t = scale(elapsed_section, 0., luma_fade, 0., 1.) ;
        b.addMessage("/luma_tol", scale(t, 0., 1., 0.5, end_luma)  );
        b.addMessage("/luma_mix", scale(t, 0., 1., 1, 0.)  );

        b.addMessage("/contour/color", 1, 0.5, 1, scale(t, 0., 1., 1., 0.) );
    }
    else
    {
        b.addMessage("/luma_mix", end_luma );
        b.addMessage("/enable/contour", 0);
    }

    double fadein2 =  scale_clip(elapsed_section, 0., luma_fade, 0., 1.);
    double freq2 = scale( cos(elapsed_section * M_PI * 2 * 0.05 * fadein2), -1., 1., 0.05, 0.03);
    double lfo2 = cos(elapsed_section * M_PI * 2 * freq2 * fadein2);
   // double rect_lfo2 = tanh(lfo2 * 7);
    b.addMessage("/repos_amt", scale( lfo2, -1., 1., 0.17, 0.5));

    b.addMessage("/hsflow_scale", scale( lfo2, -1., 1., 0.05, 0.01));
    //b.addMessage("/repos_scale", scale( lfo2, -1., 1., 0.998, 1.) );




    if( isNewCue )
    {

        b.addMessage("/luma_target", 0.24);
        b.addMessage("/luma_tol", 0.5);
        b.addMessage("/luma_fade", 0.0);

        b.addMessage("/hsflow_lambda", 0.);
        b.addMessage("/hsflow_scale", 0.01);
        b.addMessage("/hsflow_offset", 0.3);

        b.addMessage("/repos_amt", 0.17);
        b.addMessage("/repos_scale", 1.00001);
        b.addMessage("/repos_bias", 0.00);

        b.addMessage("/luma_mix", 1.);
        b.addMessage("/flow_mix", 1.);
        b.addMessage("/noise_mix", 0.);


        b.addMessage("/overlap/flip", 0.5);
        b.addMessage("/half_mirror/alpha", 0 );


        out.addMessage("/dpo/pregain/dB",          -28);
        out.addMessage("/dpo/sarah/pregain/dB",    -100); // not using sarah here...

        out.addMessage("/gran/pregain/dB",         0);
        out.addMessage("/fuzz/pregain/dB",         -100);
        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         -100);
        out.addMessage("/spring/pregain/dB",       -100);
        out.addMessage("/sine/pregain/dB",         -100);

        // video settings
        b.addMessage("/video/black",  0);
        b.addMessage("/use/preprocess",  3);

        b.addMessage("/use/camera",  1);
        b.addMessage("/overlap/cameras", 0.0 );


        b.addMessage("/enable/hull", 0);
        b.addMessage("/enable/minrect", 0);
        b.addMessage("/enable/contour", 1);
        b.addMessage("/contour/color", 1, 0.5, 1, 1 );

      //  cout << "use camera" << 2 << endl;
        b.addMessage("/size/min", 0.000 );
        b.addMessage("/size/max", 0.9 );
        b.addMessage("/thresh", 41 );
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

        out.addMessage("/dpo/f1/val", ntom( "eb:5" ) );
        out.addMessage("/dpo/f2/val", ntom( "eb:5" ) );


        out.addMessage("/sine/amp", 1);


        out.addMessage("/gran/*/motor/scale", 0.001, 500.);
        out.addMessage("/gran/*/overlap/scale", 0.01, 0.8);
        out.addMessage("/gran/*/rate/scale", 15., 117.);
        out.addMessage("/gran/*/amp/val", 1);
        out.addMessage("/gran/*/buffer/scale", 3, 9 );
        out.addMessage("/gran/send/fuzz", 1);
        out.addMessage("/gran/*/loop", 0);

        out.addMessage("/gran/*/retrigger", 1);


        out.addMessage("/loop/length/ms", -1);
        out.addMessage("/loop/retrigger/enable", 0);
        out.addMessage("/loop/start/ratio", 0.1);
        out.addMessage("/loop/transpose", 12);
        out.addMessage("/loop/buffer/idx", 0);
        out.addMessage("/loop/retrigger/click", 1);

        out.addMessage("/korg/maths/cycle", 1);

        MapOSC trigger1, trigger2;

        trigger1.addMessage("/play", 1);
        trigger1.addMessage("/dur", -1);
        trigger1.addMessage("/rate", -0.25);
        trigger1.addMessage("/amp", 0.5);
        trigger1.addMessage("/buffer", 4);


        trigger2.addMessage("/play", 1);
        trigger2.addMessage("/dur", -1);
        trigger2.addMessage("/rate", 0.7);
        trigger2.addMessage("/amp", 1);
        trigger2.addMessage("/buffer", 8);


        out.addMessage("/sample/multi",  trigger2);
        //out["/sample/multi"].appendValue(trigger2);
    }
    else
    {
        out.addMessage("/gran/*/retrigger", 0);
    }

    double burdock_fade = 20;
    if( elapsed_section < burdock_fade )
    {
        double fadeout_amp = dbtoa( scale(elapsed_section, 0, burdock_fade, 0, -70.) );
        double fadeout_ms4 = scale(elapsed_section, 0, burdock_fade,  500, 1000);
        double fadeout_ms5 = scale(elapsed_section, 0, burdock_fade,  100, 1000);
        double fadeout_motor_max = scale(elapsed_section, 0, burdock_fade, 20, 200);
        double fadeout_motor_min = scale(elapsed_section, 0, burdock_fade, 200, 2);

        double fadeout_motor5_min = scale(elapsed_section, 0, burdock_fade, 10, 1);

        out.addMessage("/gran/1/play", 0);
        out.addMessage("/gran/1/ms", 20000);
        out.addMessage("/gran/1/loop", 1);
        out.addMessage("/gran/1/rate/scale", 15., 100.);
        out.addMessage("/gran/1/amp/scale", 0, fadeout_amp * 0.5);
        out.addMessage("/gran/1/motor/scale", 50, 500);
        out.addMessage("/gran/1/overlap/scale", 0.001, 0.1);
        out.addMessage("/gran/1/pan", 0.);


        out.addMessage("/gran/2/play", 1);
        out.addMessage("/gran/2/ms", fadeout_ms4 - 250);
        out.addMessage("/gran/2/loop", 1);
        out.addMessage("/gran/2/rate/scale", 15., 100.);
        out.addMessage("/gran/2/amp", fadeout_amp );
        out.addMessage("/gran/2/motor/scale", fadeout_motor_min, fadeout_motor_max);
        out.addMessage("/gran/2/overlap/scale", 0.001, 0.1);
        out.addMessage("/gran/2/pan", -0.5);


        out.addMessage("/gran/3/play", 1);
        out.addMessage("/gran/3/ms", fadeout_ms5 + 250);
        out.addMessage("/gran/3/loop", 1);
        out.addMessage("/gran/3/rate/scale", 15., 100.);
        out.addMessage("/gran/3/amp/scale", 0, fadeout_amp);
        out.addMessage("/gran/3/motor/scale", fadeout_motor5_min, fadeout_motor_max);
        out.addMessage("/gran/3/overlap/scale", 0.001, 5);
        out.addMessage("/gran/3/pan", 0.5);



        out.addMessage("/gran/4/play", 1);
        out.addMessage("/gran/4/ms", fadeout_ms4);
        out.addMessage("/gran/4/loop", 1);
        out.addMessage("/gran/4/rate/scale", 15., 100.);
        out.addMessage("/gran/4/amp", fadeout_amp );
        out.addMessage("/gran/4/motor/scale", fadeout_motor_min, fadeout_motor_max);
        out.addMessage("/gran/4/overlap/scale", 0.001, 0.1);
        out.addMessage("/gran/4/pan", -1);


        out.addMessage("/gran/5/play", 1);
        out.addMessage("/gran/5/ms", fadeout_ms5);
        out.addMessage("/gran/5/loop", 1);
        out.addMessage("/gran/5/rate/scale", 15., 100.);
        out.addMessage("/gran/5/amp/scale", 0, fadeout_amp);
        out.addMessage("/gran/5/motor/scale", fadeout_motor5_min, fadeout_motor_max);
        out.addMessage("/gran/5/overlap/scale", 0.001, 5);
        out.addMessage("/gran/5/pan", 1);
    }
    else
    {
        double dpo_db = scale( sin(args.total_elapsed.count() * M_PI * 0.15), -1, 1, -20, 0);
        out.addMessage("/dpo/amp", dbtoa( dpo_db ) );

    }




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


   // out.addMessage("/cache", m_state_cache);
    if( prev_step != seq_minIdx )
    {
        // new step
        out.addMessage("/dpo/index1/val", m_rand_generator.uniformRand() * 0.1);

  //      int min = scale(elapsed_section, 0, 120, 0, -5);


        f1_offset = ftom( erb( mtof(f1), scale( m_rand_generator.uniformRand(), 0., 1, -1.3, 1.3) ) );
        f2_offset = ftom( erb( mtof(f2), scale( m_rand_generator.uniformRand(), 0., 1, -1.3, 1.3) ) );

        out.addMessage("/dpo/f1/val", f1_offset, 10000 );
        out.addMessage("/dpo/f2/val", f2_offset, 10000 );

       // f2_offset = 0;//cvgl::round(scale( m_rand_generator.uniformRand(), 0., 1, -1, 1));
        t_offset = m_rand_generator.uniformRand() * 5;
    }

   // out.addMessage("/dpo/index1/val", 0 );
    out.addMessage("/dpo/fold/val", fold[seq_minIdx] );

    m_state_cache.addMessage("/prev/seq_step", (int32_t)seq_minIdx);
    m_state_cache.addMessage("/f1_offset", f1_offset);
    m_state_cache.addMessage("/f2_offset", f2_offset);
    m_state_cache.addMessage("/duration", duration);

    m_state_cache.addMessage("/prev/f1", f1);

    out.addMessage("/elapsed_section", elapsed_section );
    double area_sum = data.contour_area.sum();
    out.addMessage("/contour_area_sum", area_sum );

    double sum_mag = 0, sum_area = 0, sum_x = 0, sum_y = 0, avg_x = 0;

    if( data.ncontours > 0 )
    {
        for( size_t i = 0 ; i < data.ncontours; i++ )
        {
            double w = data.contour_area[i];
            sum_area += w;

            avg_x += data.center_x[i] * w;


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

        double norm_x = sum_area == 0 ? avg_x : avg_x / sum_area;
        out.addMessage("/korg/spat/1/az", 0);
        out.addMessage("/korg/spat/2/az", scale(norm_x, 0., 1., -90, 90) );

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
            out.addMessage("/dpo/sarah/amp/val", 1 );

            out.addMessage("/dpo/sarah/slide/down", 500 );

            out.addMessage("/dpo/mod_bus_phase/ms", scale(norm_mag_avg, 0., 0.5,
                                                          scale_clip(elapsed_section, 0, 300, 1, 500),
                                                          scale_clip(elapsed_section, 0, 120, 200, 1000)) );

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
