#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_forest_frogs( cueArgs args )
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;

    //MapOSC& m_state_cache = args.cache;
    //cvglRandom& m_rand_generator = args.randGen;

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
        b.addMessage("/luma_mix", 0.);
        b.addMessage("/flow_mix", 0.);
        b.addMessage("/noise_mix", 0.);


        b.addMessage("/luma_target", 0.24);
        b.addMessage("/luma_tol", 0.5);
        b.addMessage("/luma_fade", 0.0);

        b.addMessage("/hsflow_lambda", 0.);
        b.addMessage("/hsflow_scale", 0.01);
        b.addMessage("/hsflow_offset", 0.3);

        b.addMessage("/repos_amt", 0.17);
        b.addMessage("/repos_scale", 0.997);
        b.addMessage("/repos_bias", 0.00);

        b.addMessage("/glitch_tri/alpha", 0);
        b.addMessage("/big_triangle1/alpha", 0 );
        b.addMessage("/big_triangle2/alpha", 0 );
        b.addMessage("/half_mirror/alpha", 0 );

        out.addMessage("/dpo/pregain/dB",          -100);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         -0);
        out.addMessage("/fuzz/pregain/dB",         -100);

        out.addMessage("/loop/pregain/dB",         -70);
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
        out.addMessage("/loop/buffer/idx", 0);

        out.addMessage("/korg/maths/cycle", 1);

    }

   // double burdock_fade = 40;
   // if( elapsed_section < (burdock_fade+1) )
    {
        double fadeout_amp = dbtoa(-36);

        double lfo = sin( args.total_elapsed.count() * M_PI * 0.01);

        double fadeout_ms4 = scale(lfo, -1, 1,  500, 1000);
        double fadeout_ms5 = scale(lfo, -1, 1,  100, 1000);
        double fadeout_motor_max = scale(lfo, -1, 1, 20, 200);
        double fadeout_motor_min = scale(lfo, -1, 1, 200, 2);

        double fadeout_motor5_min = scale(lfo, -1, 1, 10, 1);

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
