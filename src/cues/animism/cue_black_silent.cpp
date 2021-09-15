#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC AnimismCues::cue_black_silent( const AnalysisData& data, MapOSC& b)
{

    string descr = "start black";
    string next_cue = "A - first sound";

    MapOSC out;
    if( isNewCue )
    {
        out.addMessage("/descr", descr);
        out.addMessage("/next_cue", next_cue);

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

        b.addMessage("/overlap/flip", 0.);
        b.addMessage("/overlap/cameras", 0.);
        b.addMessage("/video/flip", 1, 0);

    }

    return out;
}
