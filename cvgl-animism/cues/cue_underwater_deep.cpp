#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_underwater_deep( cueArgs args )
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    bool isNewCue = args.isNewCue;

    const double elapsed_section = args.elapsed_section.count();
    const double total_elapsed = args.total_elapsed.count();


    if( isNewCue )
    {
    //    b.addMessage("/contrast", 1.3); // *** increased contrast for eye ball
        b.addMessage("/use/camera",  1);

//        out.addMessage("/dpo/pregain/dB",          -36);
//        out.addMessage("/dpo/sarah/pregain/dB",    -6);

        out.addMessage("/dpo/pregain/dB",          -12);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);


        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/fuzz/pregain/dB",         -100);

        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         -6);

        out.addMessage("/sine/pregain/dB",         -100);

        b.addMessage("/video/black",  0);
        b.addMessage("/use/preprocess",  0);

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



        double hz1 = 0;

        double f1 = ftom( hz1 );
        double f2 = ftom( erb(hz1, 1.5) );

        out.addMessage("/dpo/f1/val",   ftom( 50 ), 0, f1, 1500 );
        out.addMessage("/dpo/f2/val",   ftom( 50 ), 0, f2, 1600 );
        out.addMessage("/dpo/amp",      0, 0, 1, 1000 );

     //  out.addMessage("/dpo/sarah/amp",        1, 0, dbtoa(-36), 1000 );


/*
        double f1 = 128;
        double hz1 = mtof(f1);
        double f2 = ftom( erb(hz1, 1.5) );

        out.addMessage("/dpo/f1",  f1 );
        out.addMessage("/dpo/f2",  f2 );

*/
        out.addMessage("/loop/length/ms", -1);
        out.addMessage("/loop/retrigger/enable", 0);
        out.addMessage("/loop/start/ratio", 0);

        out.addMessage("/loop/transpose", 12); // ***

        out.addMessage("/loop/buffer/idx", 0);

        out.addMessage("/korg/maths/cycle", 1);


        out.addMessage("/dpo/mod_bus_phase/sync", 0);

        out.addMessage("/dpo/index2/scale", 0, 0.2);
        out.addMessage("/dpo/shape/scale", 0, 1);
        out.addMessage("/dpo/vcf1_q/val", -2, 100);
        out.addMessage("/dpo/vcf2_q/val", -2, 100);
        out.addMessage("/dpo/sarah/slide/up", 0);
        out.addMessage("/dpo/sarah/slide/down", 0);

    }
    else
    {
        /*
        // too sharp of a cut
        double fade_phase = scale_clip(elapsed_section, 0., 3., 0., 1);
        out.addMessage("/dpo/sarah/pregain/dB", scale(fade_phase, 0., 1., -6, -100) );

        out.addMessage("/dpo/vcf1_hz",   scale(fade_phase, 0., 1., 0., -1), 20 );
        out.addMessage("/dpo/vcf1_q",   scale(fade_phase, 0., 1., -0.3, -1), 20 );
        out.addMessage("/dpo/vcf2_hz",   scale(fade_phase, 0., 1.,0, -1), 20 );
*/

        double fm_lfo = cos(total_elapsed * M_PI * 2 * 0.03 );
        double freq = scale(fm_lfo, -1., 1., 0.15, 0.05);
        double lfo = cos(total_elapsed * M_PI * 2 * freq * 300 );
       // double signOf = lfo > 0 ? -1 : 1;
        double lfo_rect = tanh(lfo * 7);
       // b.addMessage("/glitch_tri/z_offset", scale( lfo_rect, -1., 1., 0.1, 0.75));

        int32_t maxDiv = scale(cos(total_elapsed * M_PI * 2 * 0.01), -1, 1, 13, 7) ;
        int32_t div = scale( lfo, -1., 1., 0.1, maxDiv);
        out.addMessage("/dpo/mod_bus_phase/ms",  div ) ;
        out.addMessage("/dpo/mod_bus_phase/loop", 1);
        out.addMessage("/dpo/mod_bus_phase/phasecycle", 1);

        out.addMessage("/dpo/sarah/amp",  scale( lfo_rect, -1., 1., dbtoa(-0), dbtoa(-12)));

        out.addMessage("/dpo/vcf1_hz",   scale(fm_lfo, -1., 1., -0.2, 0.1) );
        out.addMessage("/dpo/vcf2_hz",   scale(fm_lfo, -1., 1., 0., -0.5) );


    }

    return out;
}
