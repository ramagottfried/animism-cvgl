#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_fog_hands(cueArgs args)
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
        out.addMessage("/fuzz/pregain/dB",         -100);
        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         0);

        b.addMessage("/video/black",  0);

        b.addMessage("/use/camera",  2);
        b.addMessage("/overlap/cameras", 0.0 );

        b.addMessage("/enable/hull", 1);
        b.addMessage("/enable/minrect", 0);
        b.addMessage("/enable/contour", 0);
        b.addMessage("/hull/color", 1, 1., 1., 0.125 );

        b.addMessage("/overlap/cameras", 0.);
        b.addMessage("/overlap/flip", 0.);


        b.addMessage("/use/preprocess",  0);
      //  cout << "use camera" << 2 << endl;
        b.addMessage("/size/min", 0.000 );
        b.addMessage("/size/max", 1 );
        b.addMessage("/thresh", 40 );
        b.addMessage("/invert", 0 );

        out.addMessage("/loop/amp", 1);
        out.addMessage("/loop/length/ms", -1);
        out.addMessage("/loop/retrigger/enable", 0);
        out.addMessage("/loop/start/ratio", 0);
        out.addMessage("/loop/transpose", 266);
        out.addMessage("/loop/buffer/idx", 1); // traffic RI

        out.addMessage("/loop/send/korg", 1);
        out.addMessage("/loop/send/fuzz", 1);

        out.addMessage("/korg/q1/val", 0. );
        out.addMessage("/korg/q2/val", 0.1 );

        out.addMessage("/korg/maths/speed/val", 0);
        out.addMessage("/korg/maths/cycle", 1);


        out.addMessage("/korg/slide/down", 0);
        out.addMessage("/korg/slide/up", 5);
        out.addMessage("/korg/hz1", 0);
        out.addMessage("/korg/hz2", 0);
        out.addMessage("/korg/amp", 0);

    }

    /*
     * hz1 LP -1 is off, -0.4 nice, stay < 0
     * hz2 HP 1 is off, 0.5 is nice, stay > 0
     * maths offset effects both
     */


    if( data.ncontours > 0 )
    {

        double avg_x = data.centroid_x.mean();

        double avg_y = data.centroid_y.mean();

        double max_y = 0;
        double max_x = 0;
        for( const auto& rot_rec : data.minRect_vec )
        {
            auto rect = rot_rec.boundingRect2f();
            double bottom_y = rect.y + abs(rect.height);
            double left = rect.x;

            if( bottom_y > max_y )
            {
                max_y = bottom_y;
            }

            if( left > max_x )
                max_x = left;
        }

        if( max_y != 0 )
            max_y = max_y / (data.halfH * 2.);

        if( max_x != 0 )
            max_x = max_x / (data.halfW * 2.);

//       cout << min_y << endl;

        double x_range = data.centroid_x.maxCoeff() - data.centroid_x.minCoeff();
        double y_range = data.centroid_y.maxCoeff() - data.centroid_y.minCoeff();
        double xy_area = x_range * y_range;

        double defectSum = data.defect_dist_sum.sum() / 1080.; // scaling by height as a rough pix to xy

        out.addMessage("/xy_area", xy_area);
        double sum_area = data.contour_area.sum();
        out.addMessage("/sum_area", sum_area);
        out.addMessage("/defectSum", defectSum  );


        double x_ctr = scale(avg_x, 0., 1., -90, 90) ;

        double normed = scale_clip(sum_area, 0., 0.9, 0., 1.);
        out.addMessage("/korg/spat/1/az", x_ctr - 5);
        out.addMessage("/korg/spat/2/az", x_ctr + 5);

        out.addMessage("/korg/amp", pow( normed, 2)  );
        out.addMessage("/korg/hz1", scale_clip( sum_area, 0., 1., -0.8,  -0.3));
        out.addMessage("/korg/hz2", scale_clip( ( pow((max_x+normed)/2, 0.5) ), 0., 1., 0.3,  0.8)); // mainly hp here, since there isn't much low end freq in loop

      //  out.addMessage("/korg/maths/speed", scale( sin( pow(defectSum, 0.25) * M_PI), 0., 1., 0., -0.25));
//        out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
        out.addMessage("/korg/maths/offset", scale( pow( sin( ( sin(max_x * M_PI) + normed) * 0.5 * M_PI), 2), 0., 1.,  -1., 1 ));
        out.addMessage("/loop/transpose", scale( pow((max_y*0.3 + normed*0.6 + defectSum*0.1), 2), 0., 1.,  500, -100 ));

        //cout << (0.5 * (max_y+normed)) << endl;

    }
    else
    {
        out.addMessage("/korg/amp", 0);
    }


    return out;

}