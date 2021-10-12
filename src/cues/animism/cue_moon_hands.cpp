#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

/*
 * at the moment fuzz is only hooked up to gran, maybe would be cool to use fuzz here?
 * or maybe just use the maths cycle thing?
 */

MapOSC cue_moon_hands(cueArgs args)
{
    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;

    if( isNewCue )
    {

        out.addMessage("/dpo/pregain/dB",          -100);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/fuzz/pregain/dB",         -12);
        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         0);

        b.addMessage("/video/black",  0);

        b.addMessage("/use/camera",  1);
        b.addMessage("/overlap/cameras", 0.0 );

        b.addMessage("/enable/hull", 0);
        b.addMessage("/hull/color", 1, 0, 1., 1 );
        b.addMessage("/enable/minrect", 1);
        b.addMessage("/enable/contour", 0);
        //b.addMessage("/contour/color", 1, 1, 1., 0.5 );

        b.addMessage("/overlap/cameras", 0.);
        b.addMessage("/overlap/flip", 0.);

        b.addMessage("/use/preprocess",  0);
      //  cout << "use camera" << 2 << endl;
        b.addMessage("/size/min", 0.000 );
        b.addMessage("/size/max", 0.9 );
        b.addMessage("/thresh", 60 );
        b.addMessage("/invert", 0 );

        out.addMessage("/loop/amp", 1);
        out.addMessage("/loop/length/ms", -1);
        out.addMessage("/loop/retrigger/enable", 0);
        out.addMessage("/loop/start/ratio", 0);
        out.addMessage("/loop/transpose", 0);
        out.addMessage("/loop/buffer/idx", 0);
        out.addMessage("/loop/retrigger/click", 1);

        out.addMessage("/loop/send/korg", 1);
        out.addMessage("/loop/send/fuzz", 0);

        out.addMessage("/korg/maths/cycle", 1);

        out.addMessage("/korg/q1/val", 0.65 );
        out.addMessage("/korg/q2/val", 0.68 ); // q-drive now at 12:00
        out.addMessage("/korg/slide/down", 0);
        out.addMessage("/korg/slide/up", 5);
        out.addMessage("/korg/hz1", 0);
        out.addMessage("/korg/hz2", 0);

        out.addMessage("/fuzz/drive/val", 0.5);
        out.addMessage("/fuzz/fat/val", 0.);
        out.addMessage("/fuzz/impedance/val", 0.13);
        out.addMessage("/fuzz/comp/val", 0.); //0.4
        out.addMessage("/fuzz/stab/val", 0.64);
        out.addMessage("/fuzz/gate/val", 0.);
        out.addMessage("/fuzz/pan/val", 0.);
        out.addMessage("/fuzz/amp", 1);
    }


    if( data.ncontours > 0 )
    {

        double avg_x = data.centroid_x.mean();

        double x_range = data.centroid_x.maxCoeff() - data.centroid_x.minCoeff();
        double y_range = data.centroid_y.maxCoeff() - data.centroid_y.minCoeff();
        double xy_area = x_range * y_range;

        out.addMessage("/xy_area", xy_area);
        double sum_area = data.contour_area.sum();

        double x_ctr = scale(avg_x, 0., 1., -90, 90) ;

        double normed = scale(sum_area, 0., 0.03, 0., 1.);
        out.addMessage("/korg/spat/1/az", x_ctr - 5);
        out.addMessage("/korg/spat/2/az", x_ctr + 5);

        out.addMessage("/korg/amp", 1  );
        //out.addMessage("/korg/hz1", scale_clip(normed, 0., 1., 0.5,  -0.3));
        //out.addMessage("/korg/hz2", scale_clip(normed, 0., 1., 0.6, -0.4));

        out.addMessage("/korg/maths/speed/val", scale(normed, 0., 1., -0.1, 0.1));
        out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
        out.addMessage("/korg/maths/offset/val", scale(normed, 0., 1.,  0.2, -0.2 ));

    }
    else
    {
        out.addMessage("/korg/amp", 0);
        out.addMessage("/fuzz/amp", 0);

    }



    return out;

}
