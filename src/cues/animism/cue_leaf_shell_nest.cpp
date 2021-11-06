#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

/*
 * actually maybe better would be triggering long envelopes here
 * the ensemble is playing a sequence of chords
 *
 */

MapOSC cue_leaf_shell_nest(cueArgs args)
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;
    MapOSC& cache = args.cache;

    if( isNewCue )
    {

        out.addMessage("/dpo/pregain/dB",          -100);
        out.addMessage("/dpo/sarah/pregain/dB",    -100);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         -0);
        out.addMessage("/loop/pregain/dB",         -60);

        out.addMessage("/fuzz/pregain/dB",         -100);
        out.addMessage("/spring/pregain/dB",       -100);
        out.addMessage("/sine/pregain/dB",         -100);

        b.addMessage("/video/black", 0);
        b.addMessage("/use/camera",  2);
        b.addMessage("/overlap/cameras", 0.0 );
        b.addMessage("/overlap/flip", 0.);

        /*
        b.addMessage("/use/preprocess", 0);
        b.addMessage("/size/min", 0.0001 );
        b.addMessage("/size/max", 1 );
        b.addMessage("/thresh", 107 );
        */


        b.addMessage("/use/preprocess", 3);
        b.addMessage("/size/min", 0.001 );
        b.addMessage("/size/max", 0.01 );
        b.addMessage("/thresh", 41 );

        b.addMessage("/invert", 0 );

        b.addMessage("/enable/hull", 0);
        b.addMessage("/enable/minrect", 1);
        b.addMessage("/enable/contour", 2);
        b.addMessage("/contour/color", 0., 0.83, 0.334, 0.2 );
        b.addMessage("/contour_triangles/color", 0.821, 1, 0.785, 0.05 );

     //   b.addMessage("/hull/color", 0., 0.83, 0.334, 1 );

        out.addMessage("/loop/amp", 1);
        out.addMessage("/loop/length/ms", -1);
        out.addMessage("/loop/retrigger/enable", 0);
        out.addMessage("/loop/start/ratio", 0);
        out.addMessage("/loop/buffer/idx", 0); // iceland

        out.addMessage("/loop/send/korg", 1);
        out.addMessage("/loop/send/fuzz", 0);

        out.addMessage("/korg/q1/val", 0.2);
        out.addMessage("/korg/q2/val", 0.2 );

        out.addMessage("/korg/maths/speed/val", -0.5);
        out.addMessage("/korg/maths/cycle", 1);


        out.addMessage("/korg/slide/down", 0);
        out.addMessage("/korg/slide/up", 5);
        out.addMessage("/korg/hz1", 0);
        out.addMessage("/korg/hz2", 0);
        out.addMessage("/korg/amp", 0);


        cache.addMessage("/min", 1);
        cache.addMessage("/max", 0);
        cache.addMessage("/prev_t", elapsed_section);

    }

    // adaptive range
    double range_reset_s = 10;
    double prev_t = cache["/prev_t"].getFloat();
    if( (elapsed_section - prev_t) >= range_reset_s )
    {
        cache.addMessage("/min", 1);
        cache.addMessage("/max", 0);
        cache.addMessage("/prev_t", elapsed_section);
    }




    if( !isNewCue && data.ncontours > 0 )
    {

        double area_sum = data.contour_area.sum();        

        double min = cache["/min"].getFloat();
        double max = cache["/max"].getFloat();

        min = min > area_sum ? area_sum : min;
        max = max < area_sum ? area_sum : max;

        cache.addMessage("/min", min);
        cache.addMessage("/max", max);


     //   double centroid_dist_avg = data.delta_centroid_dist.mean();
     //   double area_avg = data.contour_area.mean();


        double x_ctr = scale(data.centroid_x.mean(), 0., 1., -90, 90) ;


        double area_sum_norm = scale(area_sum, min, max, 0., 1);

        double ease = easeInOutSine(area_sum_norm);


        out.addMessage("/korg/amp", ease, 500  );

        double area_phase = area_sum_norm * M_PI;
        out.addMessage("/korg/hz1", scale_clip( cos(area_phase), 0., 1., -1,  -0.5), 200);
        out.addMessage("/korg/hz2", scale_clip( cos(area_phase), 0., 1.,  1,   0.5), 200);

        //out.addMessage("/loop/transpose", scale( sin(elapsed_section*M_PI), -1., 1,  100, 220));
        out.addMessage("/loop/transpose", scale( sin(area_phase), 0., 1.,  300., 100 + sin(elapsed_section*M_PI) ) );

        //out.addMessage("/korg/maths/speed", scale( area_sum_norm, 0., 1., 0., 0.5), 20);
        out.addMessage("/korg/maths/offset", scale( cos(area_phase), 0., 1.,  0., -0.75 ), 20);

        // mabye something like this for sync wires
//         out.addMessage("/korg/maths/offset", scale( cos(area_phase), 0., 1.,  -0.5, 0.5 ), 20);


    }
    else
    {

        out.addMessage("/korg/amp", dbtoa(-24), 500);
        out.addMessage("/loop/transpose", 200. + sin(elapsed_section*M_PI) * 20 );

        out.addMessage("/korg/hz1", 1, 200);
        out.addMessage("/korg/hz2", 1, 200);
        out.addMessage("/korg/maths/offset", 1, 200);


    }


  //  out.addMessage("/cache", cache);

    return out;
}
