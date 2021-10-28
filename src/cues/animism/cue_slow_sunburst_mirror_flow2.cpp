#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_slow_sunburst_mirror_flow2( cueArgs args )
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;

    const double total_elapsed =  args.total_elapsed.count();
    //MapOSC& m_state_cache = args.cache;
    //cvglRandom& m_rand_generator = args.randGen;

    double fadetime = 20;
    if( elapsed_section <= fadetime )
    {
        b.addMessage("/half_mirror/alpha", scale(elapsed_section, 0., fadetime, 1., 0)  );
    }


    double fadein =  scale_clip(elapsed_section, 0., fadetime, 0., 1.);
    double freq = scale( cos(total_elapsed * M_PI * 2 * 0.05 * fadein), -1., 1., 0.005, 0.003);
    double lfo = tanh(cos(total_elapsed * M_PI * 2 * freq * fadein) * 7);
    b.addMessage("/contrast", scale( lfo, -1., 1., 1., 1.5));

    double luma_fade = 40;
    if( elapsed_section <= fadetime )
    {
        double t = scale(elapsed_section, 0., luma_fade, 0., 1.) ;
        b.addMessage("/luma_tol", scale(t, 0., 1., 0.3, 0.8)  );
        b.addMessage("/contour/color", 0.25, 0.5, 1., scale(t, 0., 1., 0., 0.125)  );

    }

    double fadein2 =  scale_clip(elapsed_section, 0., luma_fade, 0., 1.);
    double freq2 = scale( cos(total_elapsed * M_PI * 2 * 0.05 * fadein2), -1., 1., 0.05, 0.03);
    double lfo2 = cos(total_elapsed * M_PI * 2 * freq2 * fadein2);
   // double rect_lfo2 = tanh(lfo2 * 7);
    b.addMessage("/repos_amt", scale( lfo2, -1., 1., 0.17, 0.5));

    b.addMessage("/hsflow_scale", scale( lfo2, -1., 1., 0.05, 0.01));
    b.addMessage("/repos_scale", scale( lfo2, -1., 1., 0.998, 1.) );

// need to use cache to log the values to stay smooth between cues
    // better to smooth out the speed a bit, it's too fast right now suddenly


    if( isNewCue )
    {
        b.addMessage("/glitch_tri/alpha", 0);
        b.addMessage("/big_triangle1/alpha", 0 );
        b.addMessage("/big_triangle2/alpha", 0 );
        b.addMessage("/half_mirror/alpha", 1 );
        b.addMessage("/vignette/xyr", 0.5, 0.5, 1);


        b.addMessage("/luma_target", 0.24);
        b.addMessage("/luma_tol", 0.5);
        b.addMessage("/luma_fade", 0.0);

        b.addMessage("/hsflow_lambda", 0.);
        b.addMessage("/hsflow_scale", 0.01);
        b.addMessage("/hsflow_offset", 0.3);

        b.addMessage("/repos_amt", 0.17);
        b.addMessage("/repos_scale", 0.997);
        b.addMessage("/repos_bias", 0.00);

        b.addMessage("/luma_mix", 1.);
        b.addMessage("/flow_mix", 1.);
        b.addMessage("/noise_mix", 0.);


        out.addMessage("/dpo/pregain/dB",          -100);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/gran/send/fuzz",          0.);

        out.addMessage("/fuzz/pregain/dB",         -100);

        out.addMessage("/loop/pregain/dB",         0);
        out.addMessage("/korg/pregain/dB",         -70);

        out.addMessage("/sine/pregain/dB",  -70);

        b.addMessage("/video/black",  0);
        b.addMessage("/use/preprocess",  3);

        b.addMessage("/use/camera",  1);

        b.addMessage("/overlap/cameras", 0.);
        b.addMessage("/overlap/flip", 0.);

        b.addMessage("/enable/hull", 0);
        b.addMessage("/enable/minrect", 0);
        b.addMessage("/enable/contour", 1);
        b.addMessage("/contour/color", 0.25, 0.5, 1., 0.);

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

    double avg_x = 0;

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

        out.addMessage("/korg/spat/1/az", 0);
        out.addMessage("/korg/spat/2/az", scale(avg_x / sum_area, 0., 1., -90, 90) );

        //if( sum_mag > 0 )
        {
            double scalar = sum_area == 0 ? 1 : sum_area;
            double mag_avg = sum_mag / scalar;


            /*
                        double avg_dist_x = sum_x / scalar;
                        double avg_dist_y = sum_y / scalar;

                        out.addMessage("/avg/mag", mag_avg );
                        out.addMessage("/avg/x", avg_dist_x);
                        out.addMessage("/avg/y", avg_dist_y );
            */


            double norm_mag_avg = mag_avg / 255. ;
            double norm_2 = clip( pow( norm_mag_avg, exp(1.5)) * 100, 0., 1.);

            out.addMessage("/data/norm_mag_avg", norm_mag_avg);

            out.addMessage("/korg/amp", norm_2);
            out.addMessage("/korg/slide/down", 0);
            out.addMessage("/korg/slide/up", 5);
            out.addMessage("/korg/q1/val", 0.65 );
            out.addMessage("/korg/q2/val", 0.68 ); // q-drive now at 12:00
            out.addMessage("/korg/maths/speed/val", scale(norm_2, 0., 1., -0.6, 0.2) );
            out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
            out.addMessage("/korg/maths/offset/val", scale(pow( norm_mag_avg, exp(-0.1)), 0., 1., 0.2, -0.3));
     }


    }
    else
    {
        out.addMessage("/korg/amp", 0);

    }


    return out;
}