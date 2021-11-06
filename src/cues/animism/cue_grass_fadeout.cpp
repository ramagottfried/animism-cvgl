#include "AnimismCues.hpp"

using namespace cvgl;
using namespace Eigen;

MapOSC cue_grass_fadeout( cueArgs args )
{

    MapOSC out;
    MapOSC &b = args.b;
    AnalysisData data = args.data;
    const double elapsed_section = args.elapsed_section.count();
    bool isNewCue = args.isNewCue;

    double fadetime = 20;
    if( elapsed_section <= fadetime )
    {
        out.addMessage("/dpo/pregain/dB",  scale(elapsed_section, 0., fadetime, -24., -70));
        out.addMessage("/dpo/sarah/pregain/dB",   scale(elapsed_section, 0., fadetime, 0., -70) );

        out.addMessage("/korg/pregain/dB",  scale(elapsed_section, 0., fadetime, -70., -6));
        out.addMessage("/loop/transpose",   scale(elapsed_section, 0., fadetime, 0., 12) );
    }
    else
    {
        b.addMessage("/use/preprocess",  3);
        b.addMessage("/size/min", 0.000 );
        b.addMessage("/size/max", 0.9 );
        b.addMessage("/thresh", 41 );
        b.addMessage("/invert", 0 );
    }



    if( isNewCue )
    {
        out.addMessage("/dpo/pregain/dB",          -24);
        out.addMessage("/dpo/sarah/pregain/dB",    0);
        out.addMessage("/gran/pregain/dB",         -100);
        out.addMessage("/fuzz/pregain/dB",         -100);
        out.addMessage("/loop/pregain/dB",         -100);
        out.addMessage("/korg/pregain/dB",         -100);
        out.addMessage("/spring/pregain/dB",       -100);
        out.addMessage("/sine/pregain/dB",         -100);

        b.addMessage("/video/black", 0);
        b.addMessage("/use/preprocess",  0);

        b.addMessage("/size/min", 0.00001 );
        b.addMessage("/size/max", 1 );
        b.addMessage("/thresh", 20 );
        b.addMessage("/invert", 0 );


        b.addMessage("/use/camera",  1);
        b.addMessage("/overlap/cameras", 0.0 );

        b.addMessage("/enable/hull", 0);
        b.addMessage("/enable/minrect", 0);
        b.addMessage("/enable/contour", 0);
        b.addMessage("/contour/color", 0., 0.83, 0.334, 0.2 );

        b.addMessage("/overlap/cameras", 0.);
        b.addMessage("/overlap/flip", 0.);

        out.addMessage("/dpo/intervals", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
        out.addMessage("/dpo/baseMIDI", 0);

        out.addMessage("/dpo/mod_bus_phase/sync", 0);

        out.addMessage("/dpo/index1/val", 0.38);
        out.addMessage("/dpo/index2/val", 0.68);
        out.addMessage("/dpo/shape/val", 0.29);

        out.addMessage("/dpo/vcf1_q/scale", -2, 0.3);
        out.addMessage("/dpo/vcf2_q/scale", 0, 0.5);

        out.addMessage("/dpo/sarah/slide/up", 0);
        out.addMessage("/dpo/sarah/slide/down", 0);

        out.addMessage("/dpo/amp/val", 1);
        out.addMessage("/dpo/sarah/amp/val", 1);

        out.addMessage("/loop/length/ms", -1);
        out.addMessage("/loop/retrigger/enable", 0);
        out.addMessage("/loop/start/ratio", 0);
        out.addMessage("/loop/transpose", 0);
        out.addMessage("/loop/buffer/idx", 0);

        out.addMessage("/korg/maths/cycle", 1);
        out.addMessage("/korg/hz1", 0);
        out.addMessage("/korg/hz2", 0);


        args.cache.addMessage("/min", 1);
        args.cache.addMessage("/max", 0);

        args.cache.addMessage("/dmin", 1);
        args.cache.addMessage("/dmax", 0);

        args.cache.addMessage("/prev_t", elapsed_section);


    }

// add fuzzball stuff here..

    if( data.ncontours > 0 )
    {

        // grass creature
        if( data.delta_centroid_dist.size() > 0 )
        {
         //   out.addMessage("/data/delta", data.delta_centroid_dist);
         //   out.addMessage("/data/dur", data.elapsed_contour);


            // adaptive range

            double range_reset_s = 10;

            double area_sum = data.contour_area.sum();
            double dist_sum = data.delta_centroid_dist.sum();

            double min = args.cache["/min"].getFloat();
            double max = args.cache["/max"].getFloat();

    //            double this_min = data.contour_area.minCoeff();
    //            double this_max = data.contour_area.maxCoeff();

            min = min > area_sum ? area_sum : min;
            max = max < area_sum ? area_sum : max;


            double dmin = args.cache["/dmin"].getFloat();
            double dmax = args.cache["/dmax"].getFloat();

        //    double this_dmin = data.delta_centroid_dist.minCoeff();
        //    double this_dmax = data.delta_centroid_dist.maxCoeff();

            dmin = dmin > dist_sum ? dist_sum : dmin;
            dmax = dmax < dist_sum ? dist_sum : dmax;


            if( (elapsed_section - args.cache["/prev_t"].getFloat()) > range_reset_s )
            {
    //                min = 1;
    //                max = 0;
                dmin = 1;
                dmax = 0;
                args.cache.addMessage("/prev_t", elapsed_section);
            }

            args.cache.addMessage("/min", min);
            args.cache.addMessage("/max", max);
            args.cache.addMessage("/dmin", dmin);
            args.cache.addMessage("/dmax", dmax);




         //   double centroid_dist_avg = data.delta_centroid_dist.mean();
         //   double area_avg = data.contour_area.mean();
            out.addMessage("/dpo/f1/val", scale(dist_sum, 0, 1, 48, 128), 20);
            out.addMessage("/dpo/f2/val", scale(dist_sum, 0, 1, 135, 50), 20);
            out.addMessage("/dpo/index1/val", scale(area_sum, 0, 1, 0.4, 0.8) + scale(dist_sum, 0, 1, 0.2, 0.8), 20);

            out.addMessage("/dpo/sarah/amp/val", dist_sum * dist_sum  ); // area_sum * area_sum + scale(dist_sum, dmin, dmax, 0.00, 1 - area_sum)
            out.addMessage("/dpo/amp/val", dist_sum * dist_sum);

            out.addMessage("/dpo/vcf1_hz/val", scale(area_sum, min, max, -0.3, 0));
            out.addMessage("/dpo/vcf2_hz/val", scale(area_sum, min, max, 0, 0.8));

            out.addMessage("/dpo/vcf1_q/val", scale(area_sum, min, max, -0.5, 0.));
            out.addMessage("/dpo/vcf2_q/val", scale(area_sum, min, max, -0.5, 0.));

        }

        // fuzzball
        double sum_mag = 0, sum_area = 0, sum_x = 0, sum_y = 0;
        double avg_x = 0;

        for( size_t i = 0 ; i < data.ncontours; i++ )
        {
            double w = data.contour_area[i];
            sum_area += w;

            avg_x += data.center_x[i] * w;

            if( data.pix_channel_stats[i].size() == 5 ) // focus channel is added to whatever the channel count is
            {
           //     sum_angle += data.pix_channel_stats[i][0].mean * w;
                sum_mag += data.pix_channel_stats[i][1].mean * w;
                sum_x += data.pix_channel_stats[i][2].mean * w;
                sum_y += data.pix_channel_stats[i][3].mean * w;
            }
            else
            {
          //      out.addMessage("/nchannels", (int32_t)i, (int32_t)data.pix_channel_stats[i].size() );
            }


        }

        double norm_x = sum_area == 0 ? avg_x : avg_x / sum_area;
        out.addMessage("/korg/spat/1/az", 0);
        out.addMessage("/korg/spat/2/az", scale(norm_x, 0., 1., -90, 90) );

        //if( sum_mag > 0 )
        {
            double scalar = sum_area == 0 ? 1 : sum_area;
            double mag_avg = sum_mag / scalar;


            /*
                        double avg_dist_x = sum_x / scalar;
                        double avg_dist_y = sum_y / scalar;

                        out.addMessage("/avg/mag", mag_avg );
                        out.addMessage("/avg/x", avg_dist_x);
                        out.addMessage("/avg/y", avg_dist_y );
            */


            double norm_mag_avg = mag_avg / 255. ;
            double norm_2 = clip( pow( norm_mag_avg, exp(1.5)) * 100, 0., 1.);

            out.addMessage("/data/norm_mag_avg", norm_mag_avg);

            out.addMessage("/korg/amp", norm_2);
            out.addMessage("/korg/slide/down", 0);
            out.addMessage("/korg/slide/up", 5);
            out.addMessage("/korg/q1/val", 0.65 );
            out.addMessage("/korg/q2/val", 0.68 ); // q-drive now at 12:00
            out.addMessage("/korg/maths/speed/val", scale(norm_2, 0., 1., -0.6, 0.2) );
            out.addMessage("/korg/maths/speed/smooth", 100 ); // adjusted for 32 vector size in max
            out.addMessage("/korg/maths/offset/val", scale(pow( norm_mag_avg, exp(-0.1)), 0., 1., 0.2, -0.3));
        }


    }
    else
    {
        out.addMessage("/dpo/amp/val", 0);
        out.addMessage("/dpo/sarah/amp/val", 0);
        out.addMessage("/korg/amp", 0);

    }


  //  out.addMessage("/cache", args.cache);

    return out;
}
