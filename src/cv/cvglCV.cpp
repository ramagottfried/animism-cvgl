#include "opencv2/cudaoptflow.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/video/tracking.hpp"

#include "cvglCV.hpp"
#include "cvglHelperFunctions.hpp"
//#include "cvglMainProcess.hpp"


using namespace cv;
using namespace std;
using namespace Eigen;

void cvglCV::setCVParams( MapOSC & b )
{
    for( auto& msg : b.getMap() )
    {
        const string& addr = msg.first;
        MapOSCArray& m = msg.second;
        
        if( addr == "/invert" )
        {
            m_invert = m.getInt() > 0;
        }
        else if( addr == "/thresh" )
        {
            m_thresh = m.getFloat();
        }
        else if( addr == "/parentsonly" )
        {
            m_parents_only = m.getInt() > 0;
        }
        else if( addr == "/canny/min" )
        {
            m_canny_min = m.getFloat();
        }
        else if( addr == "/canny/max" )
        {
            m_canny_max = m.getFloat();
        }
        else if( addr == "/size/min" )
        {
            m_minsize = m.getFloat();
        }
        else if( addr == "/size/max" )
        {
            m_maxsize = m.getFloat();
        }
        
    }
}

void cvglCV::preprocess()
{
    
    // later could wrap each function with a "set to output" flag, which could be used for debugging
    // or keep these all as class members and then query them as needed..
    if( m_img.empty() )
    {
        cout << "img is missing!" << endl;
        return;
    }
    
    float resize = m_resize; // avoid lock
    cv::resize(m_img, src_color_sized, cv::Size(), resize, resize, cv::INTER_AREA);
    cv::cvtColor(src_color_sized, src_gray, cv::COLOR_RGB2GRAY);
    
    // cout << "nchannels " << src_color_sized.channels() << endl;
    
    switch (m_color_mode) {
        case 1:
            cvtColor(src_color_sized, src_color_sized, COLOR_RGB2HLS_FULL);
            break;
        case 2:
            cvtColor(src_color_sized, src_color_sized, COLOR_RGB2Lab);
            break;
        default:
            break;
    }
    
    if( m_invert )
    {
        bitwise_not(src_gray, src_gray);
    }
    
    
    GaussianBlur(src_gray, src_blur_gray, cv::Size(m_gauss_ksize, m_gauss_ksize), m_gauss_sigma, m_gauss_sigma);
    erode( src_blur_gray, src_blur_gray, m_er_element );
    dilate( src_blur_gray, src_blur_gray, m_di_element );
    
    threshold( src_blur_gray, threshold_output, m_thresh, 255, cv::THRESH_BINARY );
    
    Sobel(src_gray, sob, CV_32F, 1, 1);
    
}


void cvglCV::preprocessDifference()
{
    
    if( m_img.empty() )
    {
        return;
    }
    
    float resize = m_resize; // avoid lock
    cv::resize(m_img, src_color_sized, cv::Size(), resize, resize, cv::INTER_AREA);
    
    if( m_prev_frame.empty() )
    {
       m_prev_frame = src_color_sized.clone();
    }

    Mat diff = m_prev_frame - src_color_sized;
       
    m_prev_frame = src_color_sized.clone();
       
    
    cv::cvtColor(diff, src_gray, cv::COLOR_RGB2GRAY);
    
    switch (m_color_mode) {
        case 1:
            cvtColor(src_color_sized, src_color_sized, COLOR_RGB2HLS_FULL);
            break;
        case 2:
            cvtColor(src_color_sized, src_color_sized, COLOR_RGB2Lab);
            break;
        default:
            break;
    }
    
    if( m_invert )
    {
        bitwise_not(src_gray, src_gray);
    }
    
    
    GaussianBlur(src_gray, src_blur_gray, cv::Size(m_gauss_ksize, m_gauss_ksize), m_gauss_sigma, m_gauss_sigma);
    erode( src_blur_gray, src_blur_gray, m_er_element );
    dilate( src_blur_gray, src_blur_gray, m_di_element );
    
    threshold( src_blur_gray, threshold_output, m_thresh, 255, cv::THRESH_BINARY );
    
    Sobel(src_gray, sob, CV_32F, 1, 1);

    
}

void cvglCV::preprocessCanny()
{
    
    if( m_img.empty() )
    {
        return;
    }
    
    //  m_img = mat.clone();
    
    float resize = m_resize; // avoid lock
    cv::resize(m_img, src_color_sized, cv::Size(), resize, resize, cv::INTER_AREA);
    cv::cvtColor(src_color_sized, src_gray, cv::COLOR_RGB2GRAY);
    
    switch (m_color_mode) {
        case 1:
            cvtColor(src_color_sized, src_color_sized, COLOR_RGB2HLS_FULL);
            break;
        case 2:
            cvtColor(src_color_sized, src_color_sized, COLOR_RGB2Lab);
            break;
        default:
            break;
    }
    
    if( m_invert )
    {
        bitwise_not(src_gray, src_gray);
    }
    
    
    GaussianBlur(src_gray, src_blur_gray, cv::Size(m_gauss_ksize, m_gauss_ksize), m_gauss_sigma, m_gauss_sigma);
    erode( src_blur_gray, src_blur_gray, m_er_element );
    dilate( src_blur_gray, src_blur_gray, m_di_element );
    
    Mat can;
    cv::Canny(src_blur_gray, can, m_canny_min, m_canny_max, 3);
    
    Mat can_blur;
    GaussianBlur(can, can_blur, cv::Size(m_gauss_ksize, m_gauss_ksize), m_gauss_sigma, m_gauss_sigma);
    erode( can_blur, can_blur, m_er_element );
    dilate( can_blur, can_blur, m_di_element );
    
    //    src_gray = can;
    
    threshold( can_blur, threshold_output, m_thresh, 255, cv::THRESH_BINARY );
    
    //Sobel(src_gray, sob, CV_32F, 1, 1);
    
    
    // cv::cvtColor(src_gray+can, mat, cv::COLOR_GRAY2RGB);
    
    
    
}


// not sure if this is a good idea, gl should probably be separated
void cvglCV::getFlow()
{
    
    //printf("hi \n");
    
    if( m_img.empty() )
    {
        return;
    }
    
    float resize = m_resize * 0.5;
    cv::resize(m_img, src_color_sized, cv::Size(), resize, resize, cv::INTER_AREA);
    cv::cvtColor(src_color_sized, src_gray, cv::COLOR_RGB2GRAY);
    
    if( m_prev_frame.empty() ){
        m_prev_frame = src_gray.clone();
        return;
    }

    pBackSub->apply(src_gray, fgMask);
    
    const double resize_scalar = 1.0 / resize;

    Mat mask(src_gray.size(), CV_8UC1);

    int maxCorners = 50;
    maxCorners = MAX(maxCorners, 1);
       vector<Point2f> corners;
       double qualityLevel = 0.05;
       double minDistance = 10;
       int blockSize = 3, gradientSize = 3;
       bool useHarrisDetector = false;
       double k = 0.04;
        if( m_prev_points.size() < 10 )
        {
            
            //Mat can;
            //cv::Canny(src_blur_gray, can, m_canny_min, m_canny_max, 3);
            goodFeaturesToTrack( fgMask,
                m_prev_points,
                maxCorners,
                qualityLevel,
                minDistance,
                mask,
                blockSize,
                gradientSize,
                useHarrisDetector,
                k );
        }
       
    
     //   cv::resize(mask, mask, cv::Size(), (1 / m_resize),  (1 / m_resize), cv::INTER_AREA);
    
       // add(m_img, mask, m_img);

  //  bitwise_not(m_img,m_img, mask);

        if( !m_prev_frame.empty() && m_prev_points.size() != 0 )
        {

            vector<uchar> status;
            vector<float> err;

            calcOpticalFlowPyrLK( m_prev_frame, fgMask, m_prev_points, corners, status, err);
            
            vector<Point2f> good_new;
            

            for(uint i = 0; i < m_prev_points.size(); i++)
            {
                // Select good points
                if(status[i] == 1) {
                    good_new.push_back(corners[i]);
                    // draw the tracks
                   // cout << corners[i] << endl;
                    line(m_img,
                         corners[i] * resize_scalar,
                         m_prev_points[i] * resize_scalar,
                         cv::Scalar(255,255,0), 1);
                    auto diff = (corners[i] - m_prev_points[i]);
                    float dist = cv::sqrt(diff.x*diff.x + diff.y*diff.y);

                    circle(m_img, corners[i] * resize_scalar, 1, cv::Scalar(255,255,0, 0.5) * dist, -1);
                }
            }
            
            m_prev_points = good_new;

        }
    else
    {
        m_prev_points = corners;
    }

    
    
    /*
        if( m_prev_n_flow_points != corners.size() ){
           cout << "** Number of corners detected: " << corners.size() << endl;
            m_prev_n_flow_points = corners.size();
        }
       int radius = 10;
       for( size_t i = 0; i < corners.size(); i++ )
       {
           circle( m_img, corners[i] * (1 / m_resize), radius, Scalar(255,255,255), FILLED );
       }
    */

    /*
       @param prev first 8-bit single-channel input image.
       @param next second input image of the same size and the same type as prev.
       @param flow computed flow image that has the same size as prev and type CV_32FC2.
       @param pyr_scale parameter, specifying the image scale (\<1) to build pyramids for each image;
       pyr_scale=0.5 means a classical pyramid, where each next layer is twice smaller than the previous
       one.
       @param levels number of pyramid layers including the initial image; levels=1 means that no extra
       layers are created and only the original images are used.
       @param winsize averaging window size; larger values increase the algorithm robustness to image
       noise and give more chances for fast motion detection, but yield more blurred motion field.
       @param iterations number of iterations the algorithm does at each pyramid level.
       @param poly_n size of the pixel neighborhood used to find polynomial expansion in each pixel;
       larger values mean that the image will be approximated with smoother surfaces, yielding more
       robust algorithm and more blurred motion field, typically poly_n =5 or 7.
       @param poly_sigma standard deviation of the Gaussian that is used to smooth derivatives used as a
       basis for the polynomial expansion; for poly_n=5, you can set poly_sigma=1.1, for poly_n=7, a
       good value would be poly_sigma=1.5.
       @param flags operation flags that can be a combination of the following:
        -   **OPTFLOW_USE_INITIAL_FLOW** uses the input flow as an initial flow approximation.
        -   **OPTFLOW_FARNEBACK_GAUSSIAN** uses the Gaussian \f$\texttt{winsize}\times\texttt{winsize}\f$
            filter instead of a box filter of the same size for optical flow estimation; usually, this
            option gives z more accurate flow than with a box filter, at the cost of lower speed;
            normally, winsize for a Gaussian window should be set to a larger value to achieve the same
            level of robustness.

       The function finds an optical flow for each prev pixel using the @cite Farneback2003 algorithm so that
       */
    
    /*
    Mat flow(src_gray.size(), CV_32FC2);
    double pyr_scale = 0.5;
    int levels = 3;
    int winsize = 25;
                                               
    int iterations = 3;
    int poly_n = 7;
    double poly_sigma = 1.5;
                                               
    int flags = OPTFLOW_USE_INITIAL_FLOW;
   
    calcOpticalFlowFarneback(m_prev_frame, src_gray, flow, pyr_scale, levels, winsize, iterations, poly_n, poly_sigma, flags);

    for (int y = 0; y < m_prev_frame.rows; y += 5) {
     for (int x = 0; x < m_prev_frame.cols; x += 5)  {
         
         // get the flow from y, x position * 10 for better visibility
         const Point2f flowatxy = flow.at<Point2f>(y, x) * 10;
         //cout << flowatxy << endl;
              // draw line at flow direction
        if( flowatxy == flowatxy )
        {
            line(m_img,
                  Point(x, y) * resize_scalar,
                  Point( cvRound(x + flowatxy.x), cvRound(y + flowatxy.y) ) * resize_scalar,
                  Scalar(255,0,0) );
             // draw initial point
            // circle(m_img, Point(x, y) * resize_scalar, 1, Scalar(0, 0, 0), -1);
        }
       
     }
    }
*/
    m_prev_frame = fgMask.clone();

    
}

AnalysisData cvglCV::analyzeContour()
{
    AnalysisData data;

    if( threshold_output.empty() )
    {
       // cout << "no image" << endl;
        return data;
    }
    
    findContours( threshold_output, data.contours, data.hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    
    size_t npix = threshold_output.rows * threshold_output.cols;
    
    data.halfW = threshold_output.cols / 2.0f;
    data.halfH = threshold_output.rows / 2.0f;
    
    vector< double > contour_area;
    
    for( int i = 0; i < data.contours.size(); i++ )
    {
        double contour_a = contourArea( data.contours[i] ) / npix;
        
        if( (contour_a < m_minsize) || (contour_a > m_maxsize ) || ((m_parents_only && (data.hierarchy[i][3] != -1))) )
            continue;
        
        contour_area.emplace_back(contour_a);
        data.contour_idx.emplace_back(i);
        
        // hull
        Mat hullP, hullI;
        cv::RotatedRect minRect;
        vector<Vec4i> defects;
        
        cvgl::minAreaRectHull( data.contours[i], minRect, hullP, hullI );
        
        
        size_t hullI_size = hullI.rows * hullI.cols;
        if( hullI_size > 3 )
            cvgl::convexityDefects( data.contours[i], hullI, defects );
        
        data.hullP_vec.emplace_back( hullP );
        data.hullI_vec.emplace_back( hullI );
        data.defects_vec.emplace_back( defects );
        data.minRect_vec.emplace_back(minRect);
        
    }
    
    
    data.initSizeFromIdx();
    data.contour_area = Eigen::Map<Eigen::ArrayXd, Eigen::Aligned>(contour_area.data(), contour_area.size());
    
   // lock_guard<mutex> lock(m_lock);
   
    m_thread_pool->enqueue([this](AnalysisData _data){ analysisThread(_data);}, data);
    
   // thread worker(&cvglCV::analysisThread, this, data);
    //worker.detach();
    
    return data;// std::move(data);
}


void cvglCV::analysisThread(AnalysisData data)
{
    unique_lock<mutex> lock(m_lock);
    const AnalysisData prev_data = m_prev_data;
    vector<int> id_used = m_id_used;
    m_lock.unlock();
    
    int nchans = src_color_sized.channels();
    double src_width = (double)src_color_sized.size().width;
    double src_height = (double)src_color_sized.size().height;
    double npix = src_width * src_height;
    
    vector< double > channel_means[nchans];
    vector< double > channel_varience[nchans];
        
    data.centroids.reserve( data.ncontours );
    
    for( int i = 0; i < data.ncontours; i++ )
    {
        const Mat& contour = data.contours[ data.contour_idx[i] ];
        
        
        Mat contour_mask = Mat::zeros( src_color_sized.size(), CV_8UC1 );
        drawContours(contour_mask, data.contours, i, Scalar(255), FILLED);
        
        cv::Rect boundRect = boundingRect( contour );
        
        vector<PixStats> stats = getStatsChar( src_color_sized, sob, contour_mask, boundRect );
        if (m_color_mode == 2 && stats.size() == 3) {
            stats[0].mean *= 0.3921568627; // 100/255 to sacle to 0-100
            stats[1].mean -= 128;
            stats[2].mean -= 128;
// scale std? add std? to stats?
            
            /* will have to deal with this from the plastisphere scripts
             
             if( bound(/pix/l/mean),
               progn(
                 /pix/l/mean *= 0.3921568627, # 100/255
                 /pix/a/mean -= 128,
                 /pix/b/mean -= 128,
                 /pix/l/std = sqrt(/pix/l/var) * 0.3921568627,
                 /pix/a/std = sqrt(/pix/a/var),
                 /pix/b/std = sqrt(/pix/b/var),

                 /pix/l/max = /pix/l/mean + /pix/l/std,
                 /pix/a/max = /pix/a/mean + /pix/a/std,
                 /pix/b/max = /pix/b/mean + /pix/b/std

               )
             )
             */
        }
        
        data.pix_channel_stats.emplace_back(stats);

        data.focus(i) = stats[ src_color_sized.channels() ].variance;
        
        data.parent(i) = data.hierarchy[ data.contour_idx[i] ][3] ;
        data.parimeter(i) = arcLength(contour, true) ;
        
        cv::RotatedRect& minRect = data.minRect_vec[i];
        
        double hh = minRect.size.height / src_height;
        double ww = minRect.size.width / src_width;
        
        double major = max(hh, ww);
        double minor = min(hh, ww);
        
        data.rotrect_minor(i) = minor;
        data.rotrect_major(i) = major;
        
        double centerx = minRect.center.x;
        double centery = minRect.center.y;
        
        data.center_x(i) =  centerx / src_width ;
        data.center_y(i) =  1. - (centery / src_height) ;
        
        double _a = major / 2.;
        double _b = minor / 2.;
        double ecc = sqrt(1 - pow(_b/_a, 2));
        
        data.eccentricity(i) =  ecc ;
        
        
        double r_angle = minRect.angle;
        double out_angle = 0.0;
        if( minRect.size.height > minRect.size.width )
        {
            out_angle = -r_angle + 90.0;
        }
        else
        {
            out_angle = -r_angle;
        }
        
        data.angle(i) = out_angle ;
        
        
        data.size_x(i) =  boundRect.width / src_width ;
        data.size_y(i) =  boundRect.height / src_height ;
        
        double ctrdx = -1;
        double ctrdy = -1;
        
        Moments moms = moments( contour );
        
        // Hu momemnts
        double hu[7];
        HuMoments(moms, hu);
        
        //  obj.contourPts.addMessage( "/hu", vector<double>(hu, hu+7) );
        
        if( moms.m00 != 0.0 )
        {
            ctrdx = moms.m10 / moms.m00;
            ctrdy = moms.m01 / moms.m00;
            
        }
        
        double scaled_centroidX = ctrdx / src_width;
        double scaled_centroidY = 1. - (ctrdy / src_height);
        
        data.centroids.emplace_back(cv::Point2f(scaled_centroidX, scaled_centroidY));
        
        data.centroid_x(i) =  scaled_centroidX ;
        data.centroid_y(i) =  scaled_centroidY ;
        
        // centroids.push_back( Point2f(ctrdx, ctrdy) );
        
        data.convex(i) = cvgl::isContourConvex( contour ) ;
        
        
        Mat convexcontour;
        approxPolyDP( data.hullP_vec[i], convexcontour, 0.001, true);
        
        data.hull_area(i) =  contourArea(convexcontour) / npix ;
        
        data.defect_count(i) =  (int)data.defects_vec[i].size() ;
        
        data.hull_count(i) =  (int)data.hullI_vec[i].rows * (int)data.hullI_vec[i].cols ;
        
        
        double graph_centroid_x = cvgl::scale( data.centroid_x(i), 0., 1., -1., 1.);
        double graph_centroid_y = cvgl::scale( data.centroid_y(i), 0., 1., -1., 1.);

        data.centroid_dist(i) = sqrt( graph_centroid_x*graph_centroid_x + graph_centroid_y*graph_centroid_y );
        data.centroid_angle(i) = atan2(graph_centroid_y, graph_centroid_y ) * 57.2957795131;
        
        double aspect = src_width / src_height;
        
        data.elongation(i) = (data.size_x(i) * aspect) / data.size_y(i);
        data.verticality(i) = sin( data.angle(i) * 0.01745329252);
        
        double dist_sum = 0;
//        vector<Vec4i>::iterator d =  data.defects_vec[i].begin();
//        vector<Vec4i>::iterator d_end =  data.defects_vec[i].end();
        
        ArrayXd defect_x(data.defects_vec[i].size());
        ArrayXd defect_y(data.defects_vec[i].size());
        ArrayXd defect_depth(data.defects_vec[i].size());
        
       // cout << "rows " << data.contours[i].rows << " columns " <<  data.contours[i].cols << " depth " << (data.contours[i].depth() == CV_32S) << endl;
        
        for( size_t j = 0; j < data.defects_vec[i].size(); ++j )
        {
            Vec4i& v = data.defects_vec[i][j];
            cv::Point2i * ptFar = data.contours[i].ptr<cv::Point2i>( v[2] );
            defect_x(j) = (double)ptFar->x / src_width;
            defect_y(j) = (double)ptFar->y / src_height;
            defect_depth(j) = (double)v[3] / 256.;
            
            dist_sum += defect_depth(j);
        }
        
        ArrayXd rel_x = defect_x - data.centroid_x(i);
        ArrayXd rel_y = defect_y - data.centroid_y(i);
        ArrayXd defect_theta =  rel_y.binaryExpr(rel_x, [](double a, double b) { return std::atan2(a,b);} );
        ArrayXd defect_depthweight = defect_depth / dist_sum;
        
        ArrayXd defect_w_x = ( defect_depthweight * defect_theta.sin() );
        ArrayXd defect_w_y = ( defect_depthweight * defect_theta.cos() );

        data.defect_rel_mean_angle = std::atan2( defect_w_x.sum(),  defect_w_y.sum() ) * 57.2957795131;

        data.defect_dist_sum(i) = dist_sum;
        
    }
    
            
    // id tracking
    
    if( prev_data.centroids.size() == 0 )
    {
        
        for( int i = 0; i < data.centroids.size(); i++ )
        {
            id_used[i] = 1;
            data.id.emplace_back(i);
            data.id_idx.emplace(i, i);
            
            
        }
        
    }
    else
    {
        // if prev centroids are accounted for, then keep the same ids, if not found release id for prev centroid
        data.id = vector<int>( data.centroids.size(), -1 );
        
        int closest_idx = -1;
        double radius_max = m_track_radius * src_height;
        double min = radius_max;
        int debug_count = 0;
        
        data.sustain_idx.reserve( data.centroids.size() );
        data.noteOn_idx.reserve( data.centroids.size() );
        data.noteOff_prev_idx.reserve( prev_data.centroids.size() );

        // get delta movement between tracked contours
        
        
        // fist check if previous points are found
        for( int j = 0; j < prev_data.centroids.size(); j++ )
        {
            
            min = radius_max;
            closest_idx = -1;
            debug_count = 0;
            
            for( int i = 0; i < data.centroids.size(); i++ )
            {
                
                double delta = norm(data.centroids[i] - prev_data.centroids[j]);
                
                // if within range and if not yet assigned, do assignment
                if( delta <= radius_max && data.id[i] == -1 )
                {
                    if( min >= delta )
                    {
                        min = delta;
                        closest_idx = i;
                        // could maybe break here once we find something within the range,
                        // but currnetly we look through *every* possibility and find the closest
                    }
                }
                
            }
            
            if( closest_idx > -1 )
            {
                // selected a closest match index
                data.id[closest_idx] = prev_data.id[j];
                data.id_idx.emplace( prev_data.id[j], closest_idx );
                data.sustain_idx.emplace_back( closest_idx );
            }
            else
            {
                // not found, so fee the id slot
                id_used[ prev_data.id[j] ] = 0;
                data.noteOff_prev_idx.emplace_back(j);
            }
            
        }
        
        // check for unassigned new_ids, and then find the first unused id number:
        for( int i = 0; i < data.centroids.size(); i++ )
        {
            if( data.id[i] == -1 )
            {
                for( int n = 0; n < m_maxIDs; n++ )
                {
                    if( id_used[n] == 0)
                    {
                        data.id[i] = n;
                        data.id_idx.emplace(n, i);
                        data.noteOn_idx.emplace_back(i);

                        id_used[n] = 1;
                        break;
                    }
                }
            }
            
        }
        

    }
    
    
    
    
    /*
    for( int i = 0; i < data.id.size(); i++)
    {
        cout << data.id[i] << " " << i << " " << data.id_idx[ data.id[i] ] << endl;
    }*/
    
    // >> should now be possible to do deltas between data, using id_idx to look up index based on tracked ids
    

    analysisTracking(data, prev_data);
    
    processAnalysis(data);
    
    m_lock.lock();
    m_prev_data = data;
    m_id_used = id_used;
    m_lock.unlock();
    
}


void cvglCV::analysisTracking(AnalysisData& data, const AnalysisData& prev_data)
{
    
    auto time_now = std::chrono::system_clock::now();
    
    // note: preset delta varaibles to zero in init function in case there are no matching ids from prev frame
    for( int i = 0; i < data.ncontours; ++i )
    {
        auto prev_ref = prev_data.id_idx.find( data.id[i] );
        if( prev_ref != prev_data.id_idx.end() && prev_data.ncontours > 0 )
        {
            // found id in previous frame, can be used to get delta
            int prev_idx = prev_ref->second;
            double dx = data.centroid_x(i) - prev_data.centroid_x(prev_idx);
            double dy = data.centroid_y(i) - prev_data.centroid_y(prev_idx);
            data.delta_centroid_x(i) = dx;
            data.delta_centroid_x(i) = dy;
            data.delta_centroid_dist(i) = sqrt( dx*dx + dy*dy );
                        
            data.start_centroid_x(i) = prev_data.start_centroid_x(prev_idx);
            data.start_centroid_y(i) = prev_data.start_centroid_y(prev_idx);
            
            data.start_time[i] = prev_data.start_time[prev_idx];
            
            data.elapsed_contour = (time_now - data.start_time[i]).count();
        }
        else
        {
            data.start_time[i] = time_now;
            data.start_centroid_x(i) = data.centroid_x(i);
            data.start_centroid_y(i) = data.centroid_y(i);
        }
        
    }
    
}


vector<PixStats> cvglCV::getStatsChar( const Mat& src, const Mat& sobel, const Mat& mask, const cv::Rect& roi)
{
    //const int plane, T& min, T& max, T& varience
    
    // test roi
    if( src.size() != mask.size() )
    {
        printf("size mismatch\n");
        return vector<PixStats>();
    }
    
    int nchans = src.channels();
    int nstats = nchans + 1; // focus (removed flow)
    
    int focus = nchans;
    
    vector<PixStats> stats( nstats );
    
    vector<cv::Point> index;
    index.reserve( roi.area() );
    
    const uchar *mask_p = NULL;
    const uchar *src_p = NULL;
    const float *sobel_p = NULL;
    
    int row_start = roi.y;
    int row_end = roi.y + roi.height;
    
    int col_start = roi.x;
    int col_end = col_start + roi.width;
    
    for( int i = row_start; i < row_end; ++i )
    {
        mask_p = mask.ptr<uchar>(i);
        
        // do type check above here, eventually would be nice to support float also
        src_p = src.ptr<uchar>(i);
        sobel_p = sobel.ptr<float>(i);
        
        for( int j = col_start; j < col_end; ++j )
        {
            if( mask_p[j] )
            {
                index.push_back( cv::Point(j, i) );
                
                
                // src
                for( int c = 0; c < nchans; ++c)
                {
                    const uchar val = src_p[ (j*nchans) + c];
                    
                    if( val < stats[c].min )
                        stats[c].min = val;
                    
                    if( val > stats[c].max )
                        stats[c].max = val;
                    
                    
                    stats[c].sum += val;
                    
                }
                
                //sobel
                if( sobel_p[j] < stats[focus].min )
                    stats[focus].min = sobel_p[j];
                
                if( sobel_p[j] > stats[focus].max )
                    stats[focus].max = sobel_p[j];
                
                stats[focus].sum += sobel_p[j];
                
            }
            
        }
    }
    
    size_t size = index.size();
    if( size == 0 )
        ;//cout << "size zero" << endl;
    
    for( int c = 0; c < nchans; ++c)
    {
        stats[c].mean = stats[c].sum / size;
    }
    
    stats[focus].mean = stats[focus].sum / size;
    
    int row, col;
    for( int i = 0; i < size; ++i )
    {
        col = index[i].x;
        row = index[i].y;
        
        src_p = src.ptr<uchar>(row);
        sobel_p = sobel.ptr<float>(row);
        
        for( int c = 0; c < nchans; ++c)
        {
            double dx = src_p[ (col*nchans) + c ] - stats[c].mean;
            stats[c].dev_sum += (dx*dx);
        }
        
        double dx = sobel_p[ col ] - stats[focus].mean;
        stats[focus].dev_sum += (dx*dx);
        
    }
    
    double v_size = size - 1;
    
    for( int c = 0; c < nchans; ++c)
    {
        stats[c].variance = stats[c].dev_sum / v_size;
    }
    
    stats[focus].variance = stats[focus].dev_sum / v_size;
    
    return stats;
    
}


