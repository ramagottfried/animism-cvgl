#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_branch_rips_and_falls(cueArgs args)
{
    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    bool isNewCue = args.isNewCue;
    const double elapsed_section = args.elapsed_section.count();

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
        b.addMessage("/contrast", 1);
        b.addMessage("/glitch_tri/alpha", 0);
        b.addMessage("/big_triangle1/alpha", 0 );
        b.addMessage("/big_triangle2/alpha", 0 );
        b.addMessage("/half_mirror/alpha", 0 );

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




        out.addMessage("/dpo/pregain/dB",          -100);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         0);
        out.addMessage("/fuzz/pregain/dB",         -100);

        out.addMessage("/loop/pregain/dB",         -100); // sudden cut
        out.addMessage("/korg/pregain/dB",         -70);

        out.addMessage("/sine/pregain/dB",  -70);

        b.addMessage("/video/black",  0);
        b.addMessage("/use/preprocess",  3);

        b.addMessage("/use/camera",  2);

        b.addMessage("/overlap/cameras", 0.);
        b.addMessage("/overlap/flip", 0.);

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
        out.addMessage("/loop/transpose", 0);
        out.addMessage("/loop/buffer/idx", 3);

        out.addMessage("/korg/maths/cycle", 1);


        out.addMessage("/gran/*/motor/scale", 0.001, 500.);
        out.addMessage("/gran/*/overlap/scale", 0.01, 0.8);
        out.addMessage("/gran/*/rate/scale", 15., 117.);
        out.addMessage("/gran/*/amp/val", 1);
        out.addMessage("/gran/*/buffer/scale", 3, 9 );
        out.addMessage("/gran/send/fuzz", 1);
        out.addMessage("/gran/*/loop", 0);

        out.addMessage("/gran/*/retrigger", 1);

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
        trigger2.addMessage("/buffer", 12);


  //      out.addMessage("/sample/multi",  trigger2);

    }

    double burdock_fade = 40;
    if( elapsed_section < (burdock_fade+1) )
    {
        double fadeout_amp = pow( dbtoa( scale_clip(elapsed_section, 0, burdock_fade, 0, -70.) ), 0.5 );
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


    return out;
}
