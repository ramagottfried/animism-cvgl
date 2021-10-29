#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_wire_leaf_eating(cueArgs args)
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;
    MapOSC& cache = args.cache;

    if( isNewCue )
    {

        out.addMessage("/dpo/pregain/dB",          -36);
        out.addMessage("/dpo/sarah/pregain/dB",    -12);
        out.addMessage("/gran/pregain/dB",         -24);
        out.addMessage("/fuzz/pregain/dB",         -100);
        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         -100);
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

        out.addMessage("/dpo/vcf1_q/val", 0.2);
        out.addMessage("/dpo/vcf2_q/val", 0.478);


        out.addMessage("/gran/*/motor/scale", 2.04, 500.);
        out.addMessage("/gran/*/overlap/scale", 0.02, 1.);
        out.addMessage("/gran/*/rate/scale", 15., 117.);
        out.addMessage("/gran/*/amp/val", 0);
        out.addMessage("/gran/*/buffer/scale", 3, 16 );
        out.addMessage("/gran/send/fuzz", 1);
        cache.addMessage("/position", 0);
        cache.addMessage("/direction", 1);

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



        double area_sum_norm = scale(area_sum, min, max, 0., 1);

        double ease = easeInOutSine(area_sum_norm);

        double lfo_offsetMidi = scale( sin( elapsed_section * M_PI * 0.12 ), -1, 1, 0, 20);
        out.addMessage("/dpo/f1/val", floor(scale(ease, 0, 1, 100 + lfo_offsetMidi, 138)) );

        out.addMessage("/dpo/f2/val", floor(scale(ease, 0, 1, 138, 120)) );

        double filter_lfo = triangleWave(elapsed_section, 0.01, 2);//  sin( elapsed_section * M_PI * 1);
        out.addMessage("/dpo/vcf1_hz/val", scale(filter_lfo * ease, -1, 1, -0.5, 0.5), 100 );
        out.addMessage("/dpo/vcf2_hz/val", scale(filter_lfo * ease, -1, 1, -0.6, -1), 100);


        out.addMessage("/dpo/index1/val", scale_clip(ease, 0, 1, 0.505, 0.516) ); //+ scale(area_sum, 0, 1, 0.2, 0.8), 20

        double amp_thresh = 0.25;
        double dpo_amp = ease;//pow(area_sum_norm, 1.15);
       // if( dpo_amp < amp_thresh ){
            out.addMessage("/dpo/sarah/amp", dpo_amp, 20 ); // area_sum * area_sum + scale(dist_sum, dmin, dmax, 0.00, 1 - area_sum)
            out.addMessage("/dpo/amp/val", dpo_amp, 20  ); // area_sum * area_sum + scale(dist_sum, dmin, dmax, 0.00, 1 - area_sum)
        /*} else {
            out.addMessage("/dpo/sarah/amp/val", (1 - dpo_amp) * amp_thresh, 20 ); // area_sum * area_sum + scale(dist_sum, dmin, dmax, 0.00, 1 - area_sum)
            out.addMessage("/dpo/amp/val", 0, 20  ); // area_sum * area_sum + scale(dist_sum, dmin, dmax, 0.00, 1 - area_sum)
        }
*/


        out.addMessage("/gran/1/amp", dpo_amp, 100 ); //dpo_amp, 100

        double lfo3 = scale( fmod( elapsed_section, 20.), 0, 20, 0., 1);
        out.addMessage("/gran/1/position", scale(area_sum_norm, 0., 1, 0.04382, 0.14), 100 );


    }
    else
    {
        out.addMessage("/dpo/sarah/amp/val", 0, 20);
        out.addMessage("/dpo/amp/val", 0, 20);
        out.addMessage("/gran/1/amp/val", 0, 100 );


    }


    out.addMessage("/cache", cache);

    return out;
}