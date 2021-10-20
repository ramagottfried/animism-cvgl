/*
 * probably just cut everything here, and let the instruments do the water bubbles
 */

#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_underwater( cueArgs args )
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;


    if( isNewCue )
    {

        b.addMessage("/contrast", 1.3); // *** increased contrast for dots
        b.addMessage("/use/camera",  1);


        out.addMessage("/dpo/pregain/dB", -28 );

        double f1 = 128;
        double hz1 = mtof(f1);
        double f2 = ftom( erb(hz1, 1.5) );

        out.addMessage("/dpo/f1/val",   10, 0, f1, 150 );
        out.addMessage("/dpo/f2/val",   10, 0, f2, 120 );

        out.addMessage("/dpo/amp",              1, 0, dbtoa(-36), 500, 0, 100 );

        out.addMessage("/dpo/sarah/amp",        0);

        out.addMessage("/dpo/sarah/pregain/dB",    -6);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/fuzz/pregain/dB",         -100);

        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         -6);

        out.addMessage("/sine/pregain/dB",         -70);

        b.addMessage("/video/black",  0);
        b.addMessage("/use/preprocess",  3);


        b.addMessage("/overlap/cameras", 0.);
        b.addMessage("/overlap/flip", 0.);

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

        out.addMessage("/loop/transpose", 12); // ***

        out.addMessage("/loop/buffer/idx", 0);

        out.addMessage("/korg/maths/cycle", 1);


        out.addMessage("/dpo/mod_bus_phase/sync", 0);

        out.addMessage("/dpo/index2/scale", 0, 0.2);
        out.addMessage("/dpo/shape/scale", 0, 1);
        out.addMessage("/dpo/vcf1_q/scale", -2, 0.3);
        out.addMessage("/dpo/vcf2_q/scale", 0, -0.5);
        out.addMessage("/dpo/sarah/slide/up", 0);
        out.addMessage("/dpo/sarah/slide/down", 0);

    }
    else
    {
        double fm_lfo = cos(elapsed_section * M_PI * 2 * 0.03 );
        double freq = scale(fm_lfo, -1., 1., 0.15, 0.05);
        double lfo = cos(elapsed_section * M_PI * 2 * freq * 300);
       // double signOf = lfo > 0 ? -1 : 1;
        double lfo_rect = tanh(lfo * 7);
       // b.addMessage("/glitch_tri/z_offset", scale( lfo_rect, -1., 1., 0.1, 0.75));

        int32_t maxDiv = scale(cos(elapsed_section * M_PI * 2 * 0.01), -1, 1, 13, 7) ;
        int32_t div = scale( lfo, -1., 1., 0.1, maxDiv);
        out.addMessage("/dpo/mod_bus_phase/ms",  div ) ;
        out.addMessage("/dpo/mod_bus_phase/loop", 1);
        out.addMessage("/dpo/mod_bus_phase/phasecycle", 1);

        out.addMessage("/dpo/sarah/amp",        scale( lfo, -1., 1., dbtoa(-0), dbtoa(-12)));

        out.addMessage("/dpo/vcf1_hz",   scale(fm_lfo, -1., 1., 0., 0.61) );
        out.addMessage("/dpo/vcf1_q",   scale(fm_lfo, -1., 1., -0.3, 0.1) );
        out.addMessage("/dpo/vcf2_hz",   scale(fm_lfo, -1., 1., 0.4, 0.) );


    }



    return out;
}
