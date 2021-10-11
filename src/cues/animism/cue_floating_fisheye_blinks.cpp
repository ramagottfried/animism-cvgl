#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

/*
 * sheet of paper in front of eye, light behind paper shining on eye
 * dont' blink! until this cue
 *
 */

MapOSC cue_floating_fisheye_blinks(cueArgs args)
{
    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;

    if( isNewCue )
    {
        b.addMessage("/contrast", 1.3);
        b.addMessage("/use/camera",  1);

        out.addMessage("/dpo/pregain/dB",          -100);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/fuzz/pregain/dB",         -100);
        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         -100);
        out.addMessage("/spring/pregain/dB",       -100);
        out.addMessage("/sine/pregain/dB",         -100);

        // video settings
        b.addMessage("/video/black",  0);
        b.addMessage("/use/preprocess",  0);

        b.addMessage("/size/min", 0.00001 );
        b.addMessage("/size/max", 1 );
        b.addMessage("/thresh", 60 );
        b.addMessage("/invert", 0 );

        // trigger blink sound here

    }

    return out;
}
