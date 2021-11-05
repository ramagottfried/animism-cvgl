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

    double fadetime = 1;
    if( elapsed_section <= (fadetime+1) )
    {
        out.addMessage("/dpo/pregain/dB",  scale_clip(elapsed_section, 0., fadetime, -12., -100) );
    }

    if( isNewCue )
    {
        b.addMessage("/contrast", 1.3);

        b.addMessage("/luma_target", 0.24);
        b.addMessage("/luma_tol", 0.5);
        b.addMessage("/luma_fade", 0.0);

        b.addMessage("/hsflow_lambda", 0.);
        b.addMessage("/hsflow_scale", 0.01);
        b.addMessage("/hsflow_offset", 0.3);

        b.addMessage("/repos_amt", 0.17);
        b.addMessage("/repos_scale", 0.997);
        b.addMessage("/repos_bias", 0.00);

        b.addMessage("/luma_mix", 0.);
        b.addMessage("/flow_mix", 0.);
        b.addMessage("/noise_mix", 0.);


        b.addMessage("/use/camera",  2);

        out.addMessage("/dpo/pregain/dB",          -12);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/fuzz/pregain/dB",         -100);
        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         -100);
        out.addMessage("/spring/pregain/dB",       -100);
        out.addMessage("/sine/pregain/dB",         -100);



        b.addMessage("/enable/hull", 0);
        b.addMessage("/enable/minrect", 0);
        b.addMessage("/enable/contour", 0);

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
