#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_leaf_shell_nest_m(cueArgs args)
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;
    MapOSC& cache = args.cache;

    if( isNewCue )
    {

        out.addMessage("/dpo/pregain/dB",          -40);
        out.addMessage("/dpo/sarah/pregain/dB",    -40);
        out.addMessage("/gran/pregain/dB",         -12);
        out.addMessage("/korg/pregain/dB",         -6);

        out.addMessage("/fuzz/pregain/dB",         -100);
        out.addMessage("/loop/pregain/dB",         -100);
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


        b.addMessage("/use/preprocess", 1);
        b.addMessage("/size/min", 0.0001 );
        b.addMessage("/size/max", 0.01 );
        b.addMessage("/thresh", 10 );

        b.addMessage("/invert", 0 );

        b.addMessage("/enable/hull", 0);
        b.addMessage("/enable/minrect", 0);
        b.addMessage("/enable/contour", 2);
        b.addMessage("/contour/color", 0., 0.83, 0.334, 0.2 );
        b.addMessage("/contour_triangles/color", 0.821, 1, 0.785, 0.05 );

     //   b.addMessage("/hull/color", 0., 0.83, 0.334, 1 );

        out.addMessage("/dpo/mod_bus_phase/sync", 0);

        out.addMessage("/dpo/f1/val", 124);
        out.addMessage("/dpo/f2/val", 135);

        out.addMessage("/dpo/index1/val", 0.38);
        out.addMessage("/dpo/index2/val", 0.68);

        out.addMessage("/dpo/shape/val", 0.29);

        out.addMessage("/dpo/sarah/slide/up", 0);
        out.addMessage("/dpo/sarah/slide/down", 0);

        out.addMessage("/dpo/amp/val", 0);
        out.addMessage("/dpo/sarah/amp/val", 0);

        out.addMessage("/dpo/vcf1_q/val", 1);
        out.addMessage("/dpo/vcf2_q/val", 0.478);


        out.addMessage("/gran/*/motor/scale", 2.04, 500.);
        out.addMessage("/gran/*/overlap/scale", 0.02, 1.);
        out.addMessage("/gran/*/rate/scale", 15., 117.);
        out.addMessage("/gran/*/amp/val", 0);
        out.addMessage("/gran/*/buffer/scale", 3, 16 );
        out.addMessage("/gran/send/fuzz", 1);
        cache.addMessage("/position", 0);
        cache.addMessage("/direction", 1);


        out.addMessage("/loop/amp", 1);
        out.addMessage("/loop/length/ms", -1);
        out.addMessage("/loop/retrigger/enable", 0);
        out.addMessage("/loop/start/ratio", 0);
        out.addMessage("/loop/transpose", 23);
        out.addMessage("/loop/buffer/idx", 1); // traffic RI

        out.addMessage("/loop/send/korg", 1);
        out.addMessage("/loop/send/fuzz", 0);

        out.addMessage("/korg/q1/val", 0.68 );
        out.addMessage("/korg/q2/val", 0.68 );

        out.addMessage("/korg/maths/speed/val", 0);
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
        out.addMessage("/dpo/f1/val", floor(scale(ease, 0, 1, 125, 138)) );
        out.addMessage("/dpo/f2/val", floor(scale(ease, 0, 1, 138, 120)) );
        out.addMessage("/dpo/index1/val", scale_clip(ease, 0, 1, 0.505, 0.516) ); //+ scale(area_sum, 0, 1, 0.2, 0.8), 20

        double dpo_amp = ease;
        out.addMessage("/dpo/sarah/amp", dpo_amp, 20 ); // area_sum * area_sum + scale(dist_sum, dmin, dmax, 0.00, 1 - area_sum)
        out.addMessage("/dpo/amp/val", dpo_amp, 20  ); // area_sum * area_sum + scale(dist_sum, dmin, dmax, 0.00, 1 - area_sum)

        out.addMessage("/dpo/vcf1_hz/val", -area_sum_norm );
        out.addMessage("/dpo/vcf2_hz/val", scale(area_sum_norm, 0, 1, 0.6, 0.8));

        out.addMessage("/gran/1/amp", dpo_amp, 100 ); //dpo_amp, 100
        out.addMessage("/gran/1/position", scale(area_sum_norm, 0., 1, 0.04382, 0.14), 100 );


        out.addMessage("/korg/amp", dpo_amp, 100  );

        double area_phase = area_sum_norm * M_PI;
        out.addMessage("/korg/hz1", scale_clip( cos(area_phase), 0., 1., -1,  -0.5), 20);
        out.addMessage("/korg/hz2", scale_clip( cos(area_phase), 0., 1.,  1,   0.5), 20);

        out.addMessage("/korg/maths/speed", scale( area_sum_norm, 0., 1., 0., 0.5), 20);
        out.addMessage("/korg/maths/offset", scale( cos(area_phase), 0., 1.,  0., -0.75 ), 20);

        // mabye something like this for sync wires
//         out.addMessage("/korg/maths/offset", scale( cos(area_phase), 0., 1.,  -0.5, 0.5 ), 20);


    }
    else
    {
        out.addMessage("/dpo/sarah/amp/val", 0, 20);
        out.addMessage("/dpo/amp/val", 0, 20);
        out.addMessage("/gran/1/amp/val", 0, 100 );

        out.addMessage("/korg/amp", 0, 20);

    }


    out.addMessage("/cache", cache);

    return out;
}
