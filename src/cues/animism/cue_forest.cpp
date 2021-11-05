#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_forest( cueArgs args )
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;

    //MapOSC& m_state_cache = args.cache;
    //cvglRandom& m_rand_generator = args.randGen;

    double fadetime = 30;
    if( elapsed_section <= (fadetime+1) )
    {
        out.addMessage("/loop/pregain/dB",  scale_clip(elapsed_section, 0., fadetime, -60., -12) );
    }

    if( isNewCue )
    {
        b.addMessage("/contrast", 1);
        b.addMessage("/glitch_tri/alpha", 0);
        b.addMessage("/big_triangle1/alpha", 0 );
        b.addMessage("/big_triangle2/alpha", 0 );
        b.addMessage("/half_mirror/alpha", 0 );

        out.addMessage("/dpo/pregain/dB",          -100);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/fuzz/pregain/dB",         -100);

        out.addMessage("/loop/pregain/dB",         -60);
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

    }




    return out;
}
