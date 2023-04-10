
#include "cvglAnalysisData.hpp"

using namespace std;

MapOSC AnalysisData::getOSC() const
{
    MapOSC osc;

    // osc.addMessage("/id_idx", id_idx);
    // osc.addMessage("/start_time", start_time);

    // aspect
    osc.addMessage("/ids", id);
    osc.addMessage("/center/x", center_x);
    osc.addMessage("/center/y", center_y);
    osc.addMessage("/size/x", size_x);
    osc.addMessage("/size/y", size_y);
    osc.addMessage("/centroid/x", centroid_x);
    osc.addMessage("/centroid/y", centroid_y);
    osc.addMessage("/eccentricity", eccentricity);
    osc.addMessage("/rotrect/major", rotrect_major);
    osc.addMessage("/rotrect/minor", rotrect_minor);
    osc.addMessage("/angle", angle);
    osc.addMessage("/area", contour_area);
    osc.addMessage("/hull/area", hull_area);
    osc.addMessage("/parimeter", parimeter);
    osc.addMessage("/isconvex", convex);
    osc.addMessage("/parent", parent);

// dim_xy
    osc.addMessage("/defect/count", defect_count);
    osc.addMessage("/defect/dist_sum", defect_dist_sum);
    osc.addMessage("/hull/count", hull_count);
// contour count
    osc.addMessage("/count", ncontours);


    osc.addMessage("/focus", focus);


    osc.addMessage("/elongation", elongation);
    osc.addMessage("/verticality", verticality);
    osc.addMessage("/centroid/dist", centroid_dist);
    osc.addMessage("/centroid/angle", centroid_angle);


    osc.addMessage("/defect/rel/mean/angle", defect_rel_mean_angle);
    osc.addMessage("/defect/rel/depthweight", defect_rel_depthweight);

    osc.addMessage("/elapsed/contour", elapsed_contour);
    osc.addMessage("/start/centroid/x", start_centroid_x);
    osc.addMessage("/start/centroid/y", start_centroid_y);

    osc.addMessage("/delta/centroid/x", delta_centroid_x);
    osc.addMessage("/delta/centroid/y", delta_centroid_y);
    osc.addMessage("/delta/centroid/dist", delta_centroid_dist);
    osc.addMessage("/moved", delta_centroid_dist);

    osc.addMessage("/noteOn/idx", noteOn_idx);
    osc.addMessage("/sustain/prev/idx", sustain_idx);
    osc.addMessage("/noteOff/prev/idx", noteOff_prev_idx);


    vector<int> idx(ncontours);
    vector<double> pix_0_mean(ncontours);
    vector<double> pix_1_mean(ncontours);
    vector<double> pix_2_mean(ncontours);

    vector<double> pix_0_var(ncontours);
    vector<double> pix_1_var(ncontours);
    vector<double> pix_2_var(ncontours);

    vector<double> pix_0_max(ncontours);
    vector<double> pix_1_max(ncontours);
    vector<double> pix_2_max(ncontours);

    vector<double> pix_0_std(ncontours);
    vector<double> pix_1_std(ncontours);
    vector<double> pix_2_std(ncontours);

    for( size_t i=0; i<ncontours; i++)
    {
        idx[i] = i;
        pix_0_mean[i] = pix_channel_stats[i][0].mean;
        pix_1_mean[i] = pix_channel_stats[i][1].mean;
        pix_2_mean[i] = pix_channel_stats[i][2].mean;
        pix_0_var[i] = pix_channel_stats[i][0].variance;
        pix_1_var[i] = pix_channel_stats[i][1].variance;
        pix_2_var[i] = pix_channel_stats[i][2].variance;
        pix_0_max[i] = pix_channel_stats[i][0].max;
        pix_1_max[i] = pix_channel_stats[i][1].max;
        pix_2_max[i] = pix_channel_stats[i][2].max;

        pix_0_std[i] = sqrt(pix_0_var[i]) * 0.3921568627;
        pix_1_std[i] = sqrt(pix_1_var[i]);
        pix_2_std[i] = sqrt(pix_2_var[i]);
    }

    osc.addMessage("/idx", idx);

    osc.addMessage("/pix/l/mean", pix_0_mean );
    osc.addMessage("/pix/a/mean", pix_1_mean );
    osc.addMessage("/pix/b/mean", pix_2_mean );

    osc.addMessage("/pix/l/mean", pix_0_var );
    osc.addMessage("/pix/a/mean", pix_1_var );
    osc.addMessage("/pix/b/mean", pix_2_var );

    osc.addMessage("/pix/l/std", pix_0_std );
    osc.addMessage("/pix/a/std", pix_1_std );
    osc.addMessage("/pix/b/std", pix_2_std );


    return osc;
}
