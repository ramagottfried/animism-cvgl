#include "cvglCues.hpp"


/*
 * with this approach you don't have to manually declare the functions and add them in the hpp file,
 * seems maybe better.
 *
 * the downside is the this function could get very long, and not as easy to find the cues
 */


// b messages get sent to internal CV and GL params

// >> Need to have limits on number of voices below -- the UDP port is getting blocked when the size is too big

// #include "cvglProfile.hpp"



void cvglCues::set_lambda_cues()
{

    using namespace cvgl;
    using namespace Eigen;

    printf("setting cues ... \n");

    string descr;

    descr = "start black";
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        if( isNewCue )
        {
            out.addMessage("/descr", descr);

            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);
            out.addMessage("/loop/pregain/dB",         -100);
            out.addMessage("/korg/pregain/dB",         -100);
            out.addMessage("/spring/pregain/dB",       -100);
            out.addMessage("/sine/pregain/dB",         -100);

            // video settings
            b.addMessage("/video/black",  1);

        }

        return out;
    });

    // add starting sound
    // use contour for mapping in blur part?
    descr = "landscape -- (fade in from board) -- scene opens with handblur, then slowly bring long shot into focus, move in with camera, going into plants";
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();

        if( isNewCue )
        {
            out.addMessage("/descr", descr);


            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);
            out.addMessage("/loop/pregain/dB",         -100);
            out.addMessage("/korg/pregain/dB",         -100);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  0);

            b.addMessage("/use/camera",  1);
            b.addMessage("/overlap/cameras", 0.0 );

            b.addMessage("/enable/hull", 0);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 0);
            b.addMessage("/contour/color", 0.25, 0.5, 1., 0.125 );

          //  cout << "use camera" << 2 << endl;
            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 41 );
            b.addMessage("/invert", 0 );

            out.addMessage("/loop/amp", 1);
            out.addMessage("/loop/length/ms", -1);
            out.addMessage("/loop/retrigger/enable", 0);
            out.addMessage("/loop/start/ratio", 0);
            out.addMessage("/loop/transpose", 0);
            out.addMessage("/loop/buffer/idx", 0);
            out.addMessage("/loop/retrigger/click", 1);


        }

        double fadetime = 30;
        if( elapsed_section <= fadetime )
        {
            // transition, to sped up version
            out.addMessage("/loop/pregain/dB",  scale(elapsed_section, 0., fadetime, -6., 0) );
            out.addMessage("/loop/transpose",   scale(elapsed_section, 0., fadetime, 100., 0) );
        }



        out.addMessage("/elapsed_section", elapsed_section );
        double area_sum = data.contour_area.sum();
        out.addMessage("/contour_area_sum", area_sum );


        return out;

    });

    // need to add foot pedal options
    descr = "loop with fuzzball sounds";
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();

        double fadetime = 10;
        if( elapsed_section <= fadetime )
        {
            // transition, to sped up version
           // out.addMessage("/loop/pregain/dB",  scale(elapsed_section, 0., fadetime, 0., -12));
            out.addMessage("/korg/pregain/dB",  scale(elapsed_section, 0., fadetime, -70., -6));
            out.addMessage("/loop/transpose",   scale(elapsed_section, 0., fadetime, 0., 12) );

        }

        if( isNewCue )
        {
            out.addMessage("/descr", descr);


            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);

            out.addMessage("/loop/pregain/dB",         0);
            out.addMessage("/korg/pregain/dB",         -70);

            out.addMessage("/sine/pregain/dB",  -70);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  3);

            b.addMessage("/use/camera",  2);
            b.addMessage("/overlap/cameras", 0.0 );

            b.addMessage("/enable/hull", 0);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 1);
            b.addMessage("/contour/color", 0.25, 0.5, 1., 0.125 );

          //  cout << "use camera" << 2 << endl;
            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 41 );
            b.addMessage("/invert", 0 );

            out.addMessage("/loop/length/ms", -1);
            out.addMessage("/loop/retrigger/enable", 0);
            out.addMessage("/loop/start/ratio", 0);
            out.addMessage("/loop/transpose", 0);
            out.addMessage("/loop/buffer/idx", 0);

            out.addMessage("/korg/maths/cycle", 1);

        }

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

                out.addMessage("/data/norm_mag_avg", norm_mag_avg);

                out.addMessage("/korg/amp", norm_2);
                out.addMessage("/korg/slide/down", 0);
                out.addMessage("/korg/slide/up", 5);
                out.addMessage("/korg/q1/val", 0.9 );
                out.addMessage("/korg/q2/val", 0.6 );
                out.addMessage("/korg/maths/speed/val", scale(norm_mag_avg, 0., 0.5, 0.1, -1) );
                out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
                out.addMessage("/korg/maths/offset/val", scale(pow( norm_mag_avg, exp(-0.1)), 0., 1., 0.25, -1));

                out.addMessage("/avg/mag", mag_avg );
                out.addMessage("/avg/x", avg_dist_x);
                out.addMessage("/avg/y", avg_dist_y );
            }


        }
        else
        {
            out.addMessage("/korg/amp", 0);

        }

        return out;
    });

    // improve mix
    // improve pitches
    // what is the part for vln / fl?
    // need to make sure to init the envelopes
    // Q is maybe too high for sarah
    // eventually, reduce fuzzball action so that thread is more exciting
    // this scene should hopefully be more like water ripples
    descr = "fuzzball at pond";
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        double elapsed_section = m_elapsed_section.count();
/*
        out.addMessage("/noteOn_idx", data.noteOn_idx);
        out.addMessage("/sustain_idx", data.sustain_idx);
        out.addMessage("/noteOff_prev_idx", data.noteOff_prev_idx);
        out.addMessage("/elapsed_contour", data.elapsed_contour);
*/

        if( isNewCue )
        {
            out.addMessage("/descr", descr);


            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);
            out.addMessage("/spring/pregain/dB",       -100 );

            out.addMessage("/loop/pregain/dB",         -12);
            out.addMessage("/korg/pregain/dB",         -6);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  3);

            b.addMessage("/use/camera",  1);
            b.addMessage("/overlap/cameras", 0.0 );

            b.addMessage("/enable/hull", 0);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 1);
            b.addMessage("/contour/color", 0.25, 0.5, 1., 0.125 );

          //  cout << "use camera" << 2 << endl;
            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 41 );
            b.addMessage("/invert", 0 );

            out.addMessage("/loop/length/ms", -1);
            out.addMessage("/loop/retrigger/enable", 0);
            out.addMessage("/loop/start/ratio", 0);
            out.addMessage("/loop/transpose", 24);
            out.addMessage("/loop/buffer/idx", 0);

            out.addMessage("/dpo/intervals", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
            out.addMessage("/dpo/baseMIDI", 0);

            out.addMessage("/dpo/mod_bus_phase/sync", 0);

            out.addMessage("/dpo/index2/scale", 0, 0.2);
            out.addMessage("/dpo/shape/scale", 0, 1);
            out.addMessage("/dpo/vcf1_q/scale", -2, 0.3);
            out.addMessage("/dpo/vcf2_q/scale", 0, 0.5);
            out.addMessage("/dpo/sarah/slide/up", 0);
            out.addMessage("/dpo/sarah/slide/down", 0);

            out.addMessage("/dpo/amp/val", 1);



        }


        double fadetime = 10;
        double sarah_fade = 45;
        if( elapsed_section <= sarah_fade )
        {
            // fade in DPO
            out.addMessage("/dpo/pregain/dB",  scale_clip(elapsed_section, 0., fadetime, -70., -28) );
            out.addMessage("/dpo/sarah/pregain/dB",  scale_clip(elapsed_section, 0., sarah_fade, -70., -12) );
            out.addMessage("/sine/pregain/dB",  scale_clip(elapsed_section, 0., fadetime, -100., -33) );

            //  out.addMessage("/loop/transpose",   scale(elapsed_section, 0., 10., 0., 24) );

        }

      /*
        vector< vector<double> > interval_set;

        interval_set.emplace_back(vector<double>{0, 12+2, 2, 12+3.86, 3.86, 12+7, 10.86});
        interval_set.emplace_back(vector<double>{0, 24+1, 2, 24+3.16, 3.86, 12+7, -10.86});
        interval_set.emplace_back(vector<double>{0, 2, 3.86});
       */

        vector<string> v1({     "bb:4-31",   "c:5",         "d:5+4",    "c:5-27",   "c:5-27"   });
        vector<string> v2({     "c:5",       "b:4-12",      "b:4+20",   "d:5+4",    "c:5+45"   });
        vector<double> fold({    0.1,       0.125,           0.12,        0.1,        0.11         });

        ArrayXd transp_seq_t(5);
        transp_seq_t <<         0,           4,             8,          10,         20;


        double t_offset = m_state_cache.addressExists("/t_offset") ? m_state_cache["/t_offset"].getFloat() : 0;

//        elapsed_section -= t_offset;

        double seq_t = fmod( elapsed_section, 30.);

        size_t seq_minIdx = (seq_t >= transp_seq_t ).cast<int>().sum() - 1;
     //   size_t seq_maxIdx = transp_seq_t.size() - ( seq_t < transp_seq_t ).sum();

        double f1 = ntom( v1[ seq_minIdx ] );
        double f2 = ntom( v2[ seq_minIdx ] );


        // maybe better to go more linearly

        size_t prev_step = m_state_cache.addressExists("/prev/seq_step") ? m_state_cache["/prev/seq_step"].getInt() : seq_minIdx;
        int32_t f1_oct_offset = m_state_cache.addressExists("/f1_oct_offset") ? m_state_cache["/f1_oct_offset"].getInt() : 0;
        int32_t f2_oct_offset = m_state_cache.addressExists("/f2_oct_offset") ? m_state_cache["/f2_oct_offset"].getInt() : 0;

        out.addMessage("/cache", m_state_cache);
        if( prev_step != seq_minIdx )
        {
            // new step
            out.addMessage("/dpo/index1/val", m_rand_generator.uniformRand() * 0.1);

            int min = scale(elapsed_section, 0, 120, 0, -5);

            f1_oct_offset = 0;//cvgl::round(scale( m_rand_generator.uniformRand(), 0., 1, -1, 0));
            f2_oct_offset = 0;//cvgl::round(scale( m_rand_generator.uniformRand(), 0., 1, -1, 1));
            t_offset = m_rand_generator.uniformRand() * 5;
        }

        out.addMessage("/dpo/f1/val", f1 + (f1_oct_offset * 12) );
        out.addMessage("/dpo/f2/val", f2 + (f2_oct_offset * 12) );
       // out.addMessage("/dpo/index1/val", 0 );
        out.addMessage("/dpo/fold/val", fold[seq_minIdx] );

        m_state_cache.addMessage("/prev/seq_step", (int32_t)seq_minIdx);
        m_state_cache.addMessage("/f1_oct_offset", f1_oct_offset);
        m_state_cache.addMessage("/f2_oct_offset", f2_oct_offset);
        m_state_cache.addMessage("/t_offset", t_offset);

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
                out.addMessage("/korg/q1/val", 0.9 );
                out.addMessage("/korg/q2/val", 0.6 );
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

                out.addMessage("/dpo/sarah/slide/down", scale_clip(elapsed_section, 0, 300, 0, 500) );

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
    });



    // for feedback, cut drone, or transform it so we're not in the dream state anymore
    // maybe make it sudden into WAHWAHWAH for feedback?

    // bird loop (retrigger sometimes
    // fade drone sine in with movement -- added faded out slide in dpo, might need to reset that to zero elsewhere
    // dpo/sine fade not working

    // maybe better to really do feedback with the instrument mics here?

    descr = "feedback view, breath starts in fl/vln, pitches in electronics"; // *
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        double elapsed_section = m_elapsed_section.count();

        if( isNewCue )
        {
            out.addMessage("/descr", descr);


            out.addMessage("/dpo/pregain/dB",          -28);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);
            out.addMessage("/spring/pregain/dB",       -100);

            out.addMessage("/loop/pregain/dB",         -12);
            out.addMessage("/korg/pregain/dB",         -6);
            out.addMessage("/sine/pregain/dB",          -33);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  3);

            b.addMessage("/use/camera",  1);
            b.addMessage("/overlap/cameras", 0.0 );

            b.addMessage("/enable/hull", 1);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 0);
         //  b.addMessage("/contour/color", 0.25, 0.5, 1., 0.125 );

          //  cout << "use camera" << 2 << endl;
            b.addMessage("/size/min", 0.0001 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 20 );
            b.addMessage("/invert", 0 );

            out.addMessage("/dpo/intervals", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
            out.addMessage("/dpo/baseMIDI", 0);

            out.addMessage("/dpo/mod_bus_phase/sync", 0);

            out.addMessage("/dpo/index2/scale", 0, 0.2);
            out.addMessage("/dpo/shape/scale", 0, 1);
            out.addMessage("/dpo/vcf1_q/scale", -2, 0.3);
            out.addMessage("/dpo/vcf2_q/scale", 0, 0.5);
            out.addMessage("/dpo/sarah/slide/up", 100);
            out.addMessage("/dpo/sarah/slide/down", 200);

            out.addMessage("/dpo/amp/val", 1);

            out.addMessage("/dpo/slide/up", 100);
            out.addMessage("/dpo/slide/down", 500);

            out.addMessage("/spring/feedback/val",  0.24 );



        }



        out.addMessage("/loop/length/ms", 3917);
        out.addMessage("/loop/retrigger/enable", 0);
        out.addMessage("/loop/retrigger/hz", 21);
        out.addMessage("/loop/start/ratio", 0.067);
        out.addMessage("/loop/transpose", 10);
        out.addMessage("/loop/buffer/idx", 0);
        out.addMessage("/loop/amp", 0);


        out.addMessage("/korg/amp", 0);
        out.addMessage("/korg/slide/down", 2000);
        out.addMessage("/korg/slide/up", 0);
        out.addMessage("/korg/q1/val", 0.01 );
        out.addMessage("/korg/q2/val", 0.01 );
        out.addMessage("/korg/maths/speed/val", -0.36 );
        out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
        out.addMessage("/korg/maths/offset/val", 0.24 );

        double sum_mag = 0, sum_area = 0, sum_x = 0, sum_y = 0;

        out.addMessage("/data/sum_area", data.contour_area.sum());

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

                    if( isnan(sum_mag) )
                    {
                        out.addMessage("/error/sum_mag", (int32_t)i,  data.pix_channel_stats[i][1].mean, w );

                    }
                }
                else
                {
                    out.addMessage("/error/bad_channel_count", (int32_t)i, (int32_t)data.pix_channel_stats[i].size() );
                }

            }

            out.addMessage("/data/sum_mag", sum_mag);


            if( sum_mag > 0 )
            {
                double scalar = sum_area == 0 ? 1 : sum_area;
                double mag_avg = sum_mag / scalar;

                double avg_dist_x = sum_x / scalar;
                double avg_dist_y = sum_y / scalar;

                double norm_mag_avg = mag_avg / 255. ;
                double norm_2 = clip( norm_mag_avg * 100, 0., 1.);

                out.addMessage("/data/mag_avg", mag_avg);

                out.addMessage("/norm_mag_avg", norm_mag_avg );

                out.addMessage("/korg/amp", norm_2);
                out.addMessage("/loop/amp", norm_2, 100);

                out.addMessage("/dpo/amp/val", norm_2, 200  );
                out.addMessage("/dpo/sarah/amp/val", norm_2, 200  );

                out.addMessage("/sine/amp", norm_2, 20  );
                //out.addMessage("/loop/retrigger/enable", 1);
                out.addMessage("/korg/maths/speed", -0.6 ); // adjusted for 32 vector size in max

           //     out.addMessage("/dpo/vcf1_hz/scale", scale(norm_mag_avg, 0., 0.5, -0.1, 0.5), scale(norm_mag_avg, 0., 0.5, 0.5, 1) );
           //     out.addMessage("/dpo/vcf2_hz/scale", scale(norm_mag_avg, 0., 0.5, -1, 0.5), scale(norm_mag_avg, 0., 0.5, -0.5, 1) );

//                double f1 = mtof( m_state_cache.addressExists("/prev/f1") ? m_state_cache["/prev/f1"].getFloat() : ntom("c:5") );
//                out.addMessage("/loop/retrigger/hz", f1 / cvgl::round(scale(norm_mag_avg, 0., 0.5, 1, 5)) );

            }


        }
        else
        {
            out.addMessage("/dpo/amp/val", 0  );
            out.addMessage("/dpo/sarah/amp/val", 0  );
            out.addMessage("/sine/amp", 0  );
            out.addMessage("/loop/amp", 0, 1000);

        //    out.addMessage("/spring/feedback/val", 0, 100 );
        }


        return out;
    });

    // vln/fl heart for a second, then stop
    // perc clicks with thread
    // lights off -> flashlight / red
    // vln/fl heart starts again, then cut electronics and go to black

    descr = "feedback + breath + string movement (808 gran?)"; // *
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();

        if( isNewCue )
        {
            out.addMessage("/descr", descr);


            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/gran/pregain/dB",         0);
            out.addMessage("/gran/*/amp/val", 0);

            out.addMessage("/fuzz/pregain/dB",         -100);
            out.addMessage("/loop/pregain/dB",         -100);
            out.addMessage("/korg/pregain/dB",         -100);
            out.addMessage("/spring/pregain/dB",       -100);
            out.addMessage("/sine/pregain/dB",         -100);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  3);

            b.addMessage("/use/camera",  1);
            b.addMessage("/overlap/cameras", 0.0 );

            b.addMessage("/enable/hull", 0);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 1);
            b.addMessage("/contour/color", 1, 0., 1, 0.25 );

          //  cout << "use camera" << 2 << endl;
            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 41 );
            b.addMessage("/invert", 0 );

            out.addMessage("/loop/length/ms", -1);
            out.addMessage("/loop/retrigger/enable", 0);
            out.addMessage("/loop/start/ratio", 0);
            out.addMessage("/loop/transpose", 24);
            out.addMessage("/loop/buffer/idx", 0);

            out.addMessage("/gran/*/motor/scale", 0.001, 500.);
            out.addMessage("/gran/*/overlap/scale", 0.01, 0.8);
            out.addMessage("/gran/*/rate/scale", 15., 117.);
            out.addMessage("/gran/*/amp/val", 1);
            out.addMessage("/gran/*/buffer/scale", 3, 9 );
            out.addMessage("/gran/send", 1);

        }


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

                out.addMessage("/data/norm_2", norm_2);
                out.addMessage("/data/norm_mag_avg", norm_mag_avg);

                out.addMessage("/gran/1/position", scale_clip(norm_mag_avg, 0.2, 1, 0., 1) );
                out.addMessage("/gran/1/amp/val", norm_2);

                out.addMessage("/gran/2/play", 1);
                out.addMessage("/gran/2/retrigger", 1);
                out.addMessage("/gran/2/ms", 4000);
                out.addMessage("/gran/2/loop", 0);
                out.addMessage("/gran/2/amp/val", norm_2 * norm_2 );
             //   out.addMessage("/gran/1/motor/scale", 1 + 10 * norm_2 * norm_2, 471.);
                out.addMessage("/gran/2/motor/scale", 0.07 + norm_2, 471.);



            }


        }
        else
        {
            out.addMessage("/gran/1/amp/val", 0);
            out.addMessage("/gran/2/amp/val", 0, 500);
            out.addMessage("/gran/2/motor/scale", 10 + m_rand_generator.uniformRand() * 10, 471.);

        }


        return out;
    });

    descr = "blackout, breath sounds in fl/vln  -- no electronics";
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        if( isNewCue )
        {
            out.addMessage("/descr", descr);

            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);
            out.addMessage("/loop/pregain/dB",         -100);
            out.addMessage("/korg/pregain/dB",         -100);

            // video settings
            b.addMessage("/video/black",  1);

        }

        return out;
    });

    descr = "cut to heart, cam 2 -- no electronics";
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();

        if( isNewCue )
        {
            out.addMessage("/descr", descr);
            // wait until after flute and violin have moved to part with more action

            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);

            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);

            out.addMessage("/loop/pregain/dB",         -100);
            out.addMessage("/korg/pregain/dB",         -100);
// spring?
            out.addMessage("/sine/pregain/dB",         -100);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  3);

            b.addMessage("/use/camera",  2);
            b.addMessage("/overlap/cameras", 0. );

            b.addMessage("/enable/hull", 0);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 1);
            b.addMessage("/contour/color", 1, 0., 0., 0.01 );

           // cout << "use camera" << 2 << endl;
            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 41 );
            b.addMessage("/invert", 0 );

            out.addMessage("/loop/length/ms", -1);
            out.addMessage("/loop/retrigger/enable", 0);
            out.addMessage("/loop/start/ratio", 0);
            out.addMessage("/loop/transpose", 24);
            out.addMessage("/loop/buffer/idx", 0);

            out.addMessage("/gran/*/amp/val", 0);

            out.addMessage("/gran/1/motor/val", 84.);
            out.addMessage("/gran/1/overlap/val", 1);
            out.addMessage("/gran/1/rate/val", 0.2);
            out.addMessage("/gran/1/amp/val", 1);
            out.addMessage("/gran/1/buffer/val", 7);
            out.addMessage("/gran/send", 1);

            out.addMessage("/fuzz/drive/val", 0.5);
            out.addMessage("/fuzz/fat/val", 0.);
            out.addMessage("/fuzz/impedance/val", 0.13);
            out.addMessage("/fuzz/comp/val", 0.); //0.4
            out.addMessage("/fuzz/stab/val", 0.64);
            out.addMessage("/fuzz/gate/val", 0.);
            out.addMessage("/fuzz/pan/val", 0.);
            out.addMessage("/fuzz/amp", 1);




        }

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
                out.addMessage("/korg/q1/val", 0.9 );
                out.addMessage("/korg/q2/val", 0.6 );
                out.addMessage("/korg/maths/speed/val", scale(norm_mag_avg, 0., 0.5, 0.1, -0.6) );
                out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
                out.addMessage("/korg/maths/offset/val", scale(pow( norm_mag_avg, exp(-0.5)), 0., 1., -0.8, 1));

                out.addMessage("/avg/mag", mag_avg );
                out.addMessage("/avg/x", avg_dist_x);
                out.addMessage("/avg/y", avg_dist_y );


                out.addMessage("/gran/1/motor/val", scale( abs(avg_dist_x), 0., 10,  20, 250));
                out.addMessage("/gran/1/amp/val", norm_2 * 0.5 );


                if( avg_dist_y < 0 ) // up
                {
                    out.addMessage("/gran/1/rate/val", clip( scale( abs(avg_dist_y), 0., 10,  0.2, 0.1), 0.1, 0.2) );
                    out.addMessage("/gran/1/overlap/val", clip( scale( abs(avg_dist_x), 0., 10,  20, 100), 0.1, 100) );



                    //out.addMessage("/gran/1/overlap/val", 20);

                    out.addMessage("/fuzz/drive/val", 0.5);
                    out.addMessage("/fuzz/fat/val", 0.);
                    out.addMessage("/fuzz/stab/val", clip( scale( abs(avg_dist_y), 0., 10,  0.3, 0.5), 0.3, 0.5) );
                    out.addMessage("/fuzz/amp", norm_2 );

                }
                else
                {

                    out.addMessage("/gran/1/rate/val", 0.3);
                    out.addMessage("/gran/1/overlap/val", 0.1);

                    out.addMessage("/fuzz/drive/val", 0.);
                    out.addMessage("/fuzz/fat/val", 1);
                    out.addMessage("/fuzz/stab/val", clip( scale( abs(avg_dist_y), 0., 10,  0.5, 0.6), 0.5, 0.6) );
                    out.addMessage("/fuzz/amp", norm_2 * 0.5 );

                }


            }


        }
        else
        {
            out.addMessage("/fuzz/amp", 0 );
            out.addMessage("/gran/1/amp/val", 0 );


        }


        return out;
    });

    descr = "heart fade in sound, and camera 1 overlap";
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();



        if( isNewCue )
        {
            out.addMessage("/descr", descr);
            // wait until after flute and violin have moved to part with more action

            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/loop/pregain/dB",         -100);
            out.addMessage("/korg/pregain/dB",         -100);
            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  3);

            b.addMessage("/use/camera",  2);
        //    b.addMessage("/overlap/cameras", 0. );

            b.addMessage("/enable/hull", 0);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 1);
            b.addMessage("/contour/color", 1, 0., 0., 0.01 );

           // cout << "use camera" << 2 << endl;
            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 41 );
            b.addMessage("/invert", 0 );

            out.addMessage("/loop/length/ms", -1);
            out.addMessage("/loop/retrigger/enable", 0);
            out.addMessage("/loop/start/ratio", 0);
            out.addMessage("/loop/transpose", 24);
            out.addMessage("/loop/buffer/idx", 0);

            out.addMessage("/gran/*/amp/val", 0);

            out.addMessage("/gran/1/motor/val", 84.);
            out.addMessage("/gran/1/overlap/val", 1);
            out.addMessage("/gran/1/rate/val", 0.2);
            out.addMessage("/gran/1/amp/val", 1);
            out.addMessage("/gran/1/buffer/val", 7);
            out.addMessage("/gran/send", 1);

            out.addMessage("/fuzz/drive/val", 0.5);
            out.addMessage("/fuzz/fat/val", 0.);
            out.addMessage("/fuzz/impedance/val", 0.13);
            out.addMessage("/fuzz/comp/val", 0.); //0.4
            out.addMessage("/fuzz/stab/val", 0.64);
            out.addMessage("/fuzz/gate/val", 0.);
            out.addMessage("/fuzz/pan/val", 0.);
            out.addMessage("/fuzz/amp", 1);




        }

        out.addMessage("/gran/1/use/position", 1);


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
                out.addMessage("/korg/q1/val", 0.9 );
                out.addMessage("/korg/q2/val", 0.6 );
                out.addMessage("/korg/maths/speed/val", scale(norm_mag_avg, 0., 0.5, 0.1, -0.6) );
                out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
                out.addMessage("/korg/maths/offset/val", scale(pow( norm_mag_avg, exp(-0.5)), 0., 1., -0.8, 1));

                out.addMessage("/avg/mag", mag_avg );
                out.addMessage("/avg/x", avg_dist_x);
                out.addMessage("/avg/y", avg_dist_y );


                out.addMessage("/gran/1/motor/val", scale( abs(avg_dist_x), 0., 10,  20, 250));
                out.addMessage("/gran/1/amp/val", norm_2 * 0.5 );


                if( avg_dist_y < 0 ) // up
                {
                    out.addMessage("/gran/1/rate/val", clip( scale( abs(avg_dist_y), 0., 10,  0.2, 0.1), 0.1, 0.2) );
                    out.addMessage("/gran/1/overlap/val", clip( scale( abs(avg_dist_x), 0., 10,  20, 100), 0.1, 100) );



                    //out.addMessage("/gran/1/overlap/val", 20);

                    out.addMessage("/fuzz/drive/val", 0.5);
                    out.addMessage("/fuzz/fat/val", 0.);
                    out.addMessage("/fuzz/stab/val", clip( scale( abs(avg_dist_y), 0., 10,  0.3, 0.5), 0.3, 0.5) );
                    out.addMessage("/fuzz/amp", norm_2 );

                }
                else
                {

                    out.addMessage("/gran/1/rate/val", 0.3);
                    out.addMessage("/gran/1/overlap/val", 0.1);

                    out.addMessage("/fuzz/drive/val", 0.);
                    out.addMessage("/fuzz/fat/val", 1);
                    out.addMessage("/fuzz/stab/val", clip( scale( abs(avg_dist_y), 0., 10,  0.5, 0.6), 0.5, 0.6) );
                    out.addMessage("/fuzz/amp", norm_2 * 0.5 );

                }


            }


        }
        else
        {
            out.addMessage("/fuzz/amp", 0 );
            out.addMessage("/gran/1/amp/val", 0, 200 );


        }


        if( elapsed_section <= 10. )
        {
            b.addMessage("/overlap/cameras",  scale(elapsed_section, 0., 10., 1., 0.5) );
            out.addMessage("/gran/pregain/dB", scale(elapsed_section, 0., 10., -70, 0.) );
            out.addMessage("/fuzz/pregain/dB",  scale(elapsed_section, 0., 10., -70, 0.) );

            out.addMessage("/overlap/cameras", scale(elapsed_section, 0., 10., 1., 0.5) );
        }


        return out;
    });

    descr = "lungs"; // *
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();

        if( isNewCue )
        {
            out.addMessage("/descr", descr);
            // wait until after flute and violin have moved to part with more action

            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);

            out.addMessage("/gran/pregain/dB",         0);
            out.addMessage("/fuzz/pregain/dB",         0);

            out.addMessage("/loop/pregain/dB",         0);
            out.addMessage("/korg/pregain/dB",         0);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  3);

            b.addMessage("/use/camera",  1);
            b.addMessage("/overlap/cameras", 0.5 );

            b.addMessage("/enable/hull", 0);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 1);
            b.addMessage("/contour/color", 1, 0., 0., 0.01 );

           // cout << "use camera" << 2 << endl;
            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 41 );
            b.addMessage("/invert", 0 );

            out.addMessage("/loop/length/ms", -1);
            out.addMessage("/loop/retrigger/enable", 0);
            out.addMessage("/loop/start/ratio", 0);
            out.addMessage("/loop/transpose", 24);
            out.addMessage("/loop/buffer/idx", 0);

            out.addMessage("/gran/*/amp/val", 0);

            out.addMessage("/gran/1/motor/val", 84.);
            out.addMessage("/gran/1/overlap/val", 1);
            out.addMessage("/gran/1/rate/val", 0.2);
            out.addMessage("/gran/1/amp/val", 1);
            out.addMessage("/gran/1/buffer/val", 7);
            out.addMessage("/gran/send", 1);

            out.addMessage("/fuzz/drive/val", 0.5);
            out.addMessage("/fuzz/fat/val", 0.);
            out.addMessage("/fuzz/impedance/val", 0.13);
            out.addMessage("/fuzz/comp/val", 0.); //0.4
            out.addMessage("/fuzz/stab/val", 0.64);
            out.addMessage("/fuzz/gate/val", 0.);
            out.addMessage("/fuzz/pan/val", 0.);
            out.addMessage("/fuzz/amp", 1);




        }

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
                out.addMessage("/korg/q1/val", 0.9 );
                out.addMessage("/korg/q2/val", 0.6 );
                out.addMessage("/korg/maths/speed/val", scale(norm_mag_avg, 0., 0.5, 0.1, -0.6) );
                out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
                out.addMessage("/korg/maths/offset/val", scale(pow( norm_mag_avg, exp(-0.5)), 0., 1., -0.8, 1));

                out.addMessage("/avg/mag", mag_avg );
                out.addMessage("/avg/x", avg_dist_x);
                out.addMessage("/avg/y", avg_dist_y );


                out.addMessage("/gran/1/motor/val", scale( abs(avg_dist_x), 0., 10,  20, 250));
                out.addMessage("/gran/1/amp/val", norm_2 * 0.5 );


                if( avg_dist_y < 0 ) // up
                {
                    out.addMessage("/gran/1/rate/val", clip( scale( abs(avg_dist_y), 0., 10,  0.2, 0.1), 0.1, 0.2) );
                    out.addMessage("/gran/1/overlap/val", clip( scale( abs(avg_dist_x), 0., 10,  20, 100), 0.1, 100) );



                    //out.addMessage("/gran/1/overlap/val", 20);

                    out.addMessage("/fuzz/drive/val", 0.5);
                    out.addMessage("/fuzz/fat/val", 0.);
                    out.addMessage("/fuzz/stab/val", clip( scale( abs(avg_dist_y), 0., 10,  0.3, 0.5), 0.3, 0.5) );
                    out.addMessage("/fuzz/amp", norm_2 );

                }
                else
                {

                    out.addMessage("/gran/1/rate/val", 0.3);
                    out.addMessage("/gran/1/overlap/val", 0.1);

                    out.addMessage("/fuzz/drive/val", 0.);
                    out.addMessage("/fuzz/fat/val", 1);
                    out.addMessage("/fuzz/stab/val", clip( scale( abs(avg_dist_y), 0., 10,  0.5, 0.6), 0.5, 0.6) );
                    out.addMessage("/fuzz/amp", norm_2 * 0.5 );

                }


            }


        }
        else
        {
            out.addMessage("/fuzz/amp", 0 );
            out.addMessage("/gran/1/amp/val", 0 );


        }


        return out;
    });

    // back to landscape?
    descr = "landscape, after letting breath out";
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();

        if( isNewCue )
        {
            out.addMessage("/descr", descr);


            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);

            out.addMessage("/loop/pregain/dB",         0);
            out.addMessage("/korg/pregain/dB",         -100);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  0);

            b.addMessage("/use/camera",  1);
            b.addMessage("/overlap/cameras", 0.0 );

            b.addMessage("/enable/hull", 0);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 0);
            b.addMessage("/contour/color", 0.25, 0.5, 1., 0.125 );

          //  cout << "use camera" << 2 << endl;
            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 41 );
            b.addMessage("/invert", 0 );

            out.addMessage("/loop/length/ms", -1);
            out.addMessage("/loop/retrigger/enable", 0);
            out.addMessage("/loop/start/ratio", 0);
            out.addMessage("/loop/transpose", 24);
            out.addMessage("/loop/buffer/idx", 0);

        }

        out.addMessage("/elapsed_section", elapsed_section );
        double area_sum = data.contour_area.sum();
        out.addMessage("/contour_area_sum", area_sum );


        return out;

    });

    // end
    descr = "landscape, hand sun rise with bells (fade out from board, acoustic sound only)";
    m_cueFunctions.emplace_back([&, descr](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();

        if( isNewCue )
        {
            out.addMessage("/descr", descr);


            out.addMessage("/dpo/pregain/dB",          -100);
            out.addMessage("/dpo/sarah/pregain/dB",    -100);
            out.addMessage("/gran/pregain/dB",         -100);
            out.addMessage("/fuzz/pregain/dB",         -100);

            out.addMessage("/loop/pregain/dB",         0);
            out.addMessage("/korg/pregain/dB",         -100);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  0);

            b.addMessage("/use/camera",  1);
            b.addMessage("/overlap/cameras", 0.0 );

            b.addMessage("/enable/hull", 0);
            b.addMessage("/enable/minrect", 0);
            b.addMessage("/enable/contour", 0);
            b.addMessage("/contour/color", 0.25, 0.5, 1., 0.125 );

            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 41 );
            b.addMessage("/invert", 0 );

            out.addMessage("/loop/length/ms", -1);
            out.addMessage("/loop/retrigger/enable", 0);
            out.addMessage("/loop/start/ratio", 0);
            out.addMessage("/loop/transpose", 24);
            out.addMessage("/loop/buffer/idx", 0);

        }

        return out;

    });

}


/*
// wire start cue
m_cueFunctions.emplace_back([&](const AnalysisData& data, MapOSC& b)->MapOSC
{

    MapOSC out;
    const double elapsed_section = m_elapsed_section.count();

    if( isNewCue )
    {
        out.addMessage("/descr", "lambda cue 2");


        out.addMessage("/dpo/pregain/dB",    0);
        out.addMessage("/dpo/sarah/pregain/dB",    0);
        out.addMessage("/gran/pregain/dB",    0);
        out.addMessage("/fuzz/pregain/dB",    0);


        b.addMessage("/video/black",  0);
        b.addMessage("/use/preprocess",  1);

        b.addMessage("/size/min", 0.000 );
        b.addMessage("/size/max", 0.9 );
        b.addMessage("/thresh", 5 );
        b.addMessage("/invert", 0 );

    }

    out.addMessage("/elapsed_section", elapsed_section );

    double wire_sum = 0; // wires
    double bioplastic_sum = 0; // larger opject with smaller points
    size_t nwire = 0, nplastic = 0;

    double wire_avg_x = 0;
    double plastic_avg_x = 0;

    for( size_t i = 0 ; i < data.ncontours; i++ )
    {
        double area = data.contour_area[i];
        if( area > 0.0001 && area < 0.001 )
        {
            nplastic++;
            bioplastic_sum += area;
            plastic_avg_x += clip(data.centroid_x[i], 0, 1);

        }
        else
        {
            nwire++;
            wire_sum += area;
            wire_avg_x += clip(data.centroid_x[i], 0, 1);
        }

    }

    if( nwire > 0 ){
        wire_avg_x = wire_avg_x / (double)nwire;
        double az = scale(wire_avg_x, 0., 1, -90, 90.);
        out.addMessage("/dpo/sarah/spat/1/az", az - 10);
        out.addMessage("/dpo/sarah/spat/2/az", az + 10);

    }

    if( nplastic > 0 ){
        plastic_avg_x = plastic_avg_x / (double)nplastic;
        double az = scale(plastic_avg_x, 0., 1, -90, 90.);
        out.addMessage("/dpo/spat/1/az", az - 10);
        out.addMessage("/dpo/spat/2/az", az + 10);

    }


    double sum_area = bioplastic_sum + wire_sum;

    out.addMessage("/dpo/amp/val", clip( scale(bioplastic_sum, 0., 0.01, 0., 1.), 0., 1. ) );
    out.addMessage("/dpo/amp/smooth", 20);
    out.addMessage("/dpo/sarah/amp/smooth", 20);

    out.addMessage("/dpo/sarah/amp/val", clip( scale(bioplastic_sum, 0., 0.01, 0., 1.), 0., 1. ) );


    out.addMessage("/dpo/fold/val", clip( scale(bioplastic_sum, 0., 0.01, 0., 0.5), 0., 0.5) );
    out.addMessage("/dpo/fold/smooth", 10);

//        out.addMessage("/dpo/shape/val", clip( scale(bioplastic_sum, 0., 0.01, 0., 0.5), 0., 0.2) );
    out.addMessage("/dpo/shape/smooth", 20);

    out.addMessage("/dpo/index1/val", scale(bioplastic_sum, 0., 0.01, 0.2, 0.3) );
    out.addMessage("/dpo/shape/smooth", 10);

    out.addMessage("/dpo/index2/val", scale(bioplastic_sum, 0., 0.01, 0.1, 0.5) );

    out.addMessage("/dpo/baseMIDI/val", 120 );
    out.addMessage("/dpo/intervals", 0, 1, 2, 3, 4, 5, 6, 7 );

    out.addMessage("/dpo/gliss", 0 );

    out.addMessage("/dpo/f1/val", scale(bioplastic_sum, 0., 0.01, 1., 20) );
    out.addMessage("/dpo/f2/val", clip( scale(bioplastic_sum, 0., 0.01, 1., 2), 1, 2) );
//        out.addMessage("/dpo/f2/val", 0 );

    out.addMessage("/wire_sum", wire_sum );

    return out;
});
 */
