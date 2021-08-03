#include "cvglCues.hpp"
// b messages get sent to internal CV and GL params
// #include "cvglProfile.hpp"


// >> Need to have limits on number of voices below -- the UDP port is getting blocked when the size is too big
void cvglCues::set_lambda_cues()
{
     //const AnalysisData& data, cvglMixer& mixer, MapOSC& b
    using namespace cvgl;

    /*
     * with this approach you don't have to manually declare the functions and add them in the cues hpp file
     * seems maybe better
     *
     * the downside is the this function could get very long
     */
    printf("setting CUE\n");

    m_cueFunctions.emplace_back([&](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();

        if( isNewCue )
        {
            out.addMessage("/descr", "lambda cue");

            out.addMessage("/pregain/perc",      -12);
            out.addMessage("/pregain/gran",      -3);
            out.addMessage("/pregain/res",       -70);
            out.addMessage("/pregain/filter",    -21);

            out.addMessage("/filter/field/direct/amp", 0);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  2);

            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 16 );
            b.addMessage("/invert", 0 );

        }

        return out;
    });

    m_cueFunctions.emplace_back([&](const AnalysisData& data, MapOSC& b)->MapOSC
    {

        MapOSC out;
        const double elapsed_section = m_elapsed_section.count();

        if( isNewCue )
        {
            out.addMessage("/descr", "lambda cue 2");


            out.addMessage("/dpo/pregain/dB",    0);
            out.addMessage("/dpo/sarah/pregain/dB",    0);
            out.addMessage("/gran/pregain/dB",    0);
            out.addMessage("/fuzz/pregain/dB",    0);

            out.addMessage("/filter/field/direct/amp", 0);

            b.addMessage("/video/black",  0);
            b.addMessage("/use/preprocess",  1);

            b.addMessage("/size/min", 0.000 );
            b.addMessage("/size/max", 0.9 );
            b.addMessage("/thresh", 5 );
            b.addMessage("/invert", 0 );

        }

        out.addMessage("/elapsed_section", elapsed_section );

        double wire_sum = 0; // wires
        double bioplastic_sum = 0; // larger opject with smaller points
        size_t nwire = 0, nplastic = 0;

        double wire_avg_x = 0;
        double plastic_avg_x = 0;

        for( size_t i = 0 ; i < data.ncontours; i++ )
        {
            double area = data.contour_area[i];
            if( area > 0.0001 && area < 0.001 )
            {
                nplastic++;
                bioplastic_sum += area;
                plastic_avg_x += clip(data.centroid_x[i], 0, 1);

            }
            else
            {
                nwire++;
                wire_sum += area;
                wire_avg_x += clip(data.centroid_x[i], 0, 1);
            }

        }

        if( nwire > 0 ){
            wire_avg_x = wire_avg_x / (double)nwire;
            double az = scale(wire_avg_x, 0., 1, -90, 90.);
            out.addMessage("/dpo/sarah/spat/1/az", az - 10);
            out.addMessage("/dpo/sarah/spat/2/az", az + 10);

        }

        if( nplastic > 0 ){
            plastic_avg_x = plastic_avg_x / (double)nplastic;
            double az = scale(plastic_avg_x, 0., 1, -90, 90.);
            out.addMessage("/dpo/spat/1/az", az - 10);
            out.addMessage("/dpo/spat/2/az", az + 10);

        }


        double sum_area = bioplastic_sum + wire_sum;

        out.addMessage("/dpo/amp/val", clip( scale(bioplastic_sum, 0., 0.01, 0., 1.), 0., 1. ) );
        out.addMessage("/dpo/amp/smooth", 20);
        out.addMessage("/dpo/sarah/amp/smooth", 20);

        out.addMessage("/dpo/sarah/amp/val", clip( scale(bioplastic_sum, 0., 0.01, 0., 1.), 0., 1. ) );


        out.addMessage("/dpo/fold/val", clip( scale(bioplastic_sum, 0., 0.01, 0., 0.5), 0., 0.5) );
        out.addMessage("/dpo/fold/smooth", 10);

//        out.addMessage("/dpo/shape/val", clip( scale(bioplastic_sum, 0., 0.01, 0., 0.5), 0., 0.2) );
        out.addMessage("/dpo/shape/smooth", 20);

        out.addMessage("/dpo/index1/val", scale(bioplastic_sum, 0., 0.01, 0.2, 0.3) );
        out.addMessage("/dpo/shape/smooth", 10);

        out.addMessage("/dpo/index2/val", scale(bioplastic_sum, 0., 0.01, 0.1, 0.5) );

        out.addMessage("/dpo/baseMIDI/val", 120 );
        out.addMessage("/dpo/intervals", 0, 1, 2, 3, 4, 5, 6, 7 );

        out.addMessage("/dpo/gliss", 0 );

        out.addMessage("/dpo/f1/val", scale(bioplastic_sum, 0., 0.01, 1., 20) );
        out.addMessage("/dpo/f2/val", clip( scale(bioplastic_sum, 0., 0.01, 1., 2), 1, 2) );
//        out.addMessage("/dpo/f2/val", 0 );

        out.addMessage("/wire_sum", wire_sum );

        return out;
    });




    //cueFunction_t
}
