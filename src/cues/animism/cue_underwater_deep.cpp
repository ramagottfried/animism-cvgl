#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_underwater_deep( cueArgs args )
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
//    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;

    //MapOSC& m_state_cache = args.cache;
    //cvglRandom& m_rand_generator = args.randGen;

    // all off basically
    if( isNewCue )
    {
        b.addMessage("/contrast", 1.3); // *** increased contrast for eye ball
        b.addMessage("/use/camera",  2);

        out.addMessage("/dpo/pregain/dB",          -100);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/fuzz/pregain/dB",         -100);

        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         -100);

        out.addMessage("/sine/pregain/dB",         -100);

        b.addMessage("/video/black",  0);
        b.addMessage("/use/preprocess",  3);

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

    }


    return out;
}
