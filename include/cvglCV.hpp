#pragma once
#include <iostream>
#include <vector>
#include <thread>

#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/ocl.hpp>

#include "cvglObject.hpp"
#include "cvglConversions.hpp"
#include "cvglConvexHull.hpp"

#include "cvglAnalysisData.hpp"

#include "cvglTheadPool.hpp"

#include "MapOSC.hpp"
/*
 to do: don't send OSC bundle for mapping, send struct with analysis data like in fabrica
        go through the CV path and clean up a bit
        consider which parts should be part of the class (data-wise) and which parts should be more stateless
 */

class cvglCV
{
    
public:
    
    cvglCV() : m_id_used(m_maxIDs)
    {
        m_thread_pool = std::make_unique<ThreadPool>(4);
        printf("cv optimization %i\n", cv::useOptimized() );
    }
    
    void preprocess();
    void preprocessDifference();
    void preprocessCanny();
    
    void getFlow();
        
//    struct cvglAnalysisReturnStruct {
//        std::vector< cv::Mat > contours;
//        std::vector< int > contour_idx;
//        std::vector< cv::Mat > hullP_vec;
//        std::vector< cv::RotatedRect > minRec_vec;
//        float halfW;
//        float halfH;
//    };
    
//    void analyzeContour();
    AnalysisData analyzeContour();

    /*
     *  implement in sub-class for callback to process CV vectors for drawing
     *
     */
    // virtual void processAnalysisVectors(std::vector< cv::Mat >& contours, std::vector< int >& contour_idx, std::vector< cv::Mat >& hullP_vec, std::vector< cv::RotatedRect >& minRec_vec, float& halfW, float& halfH) {};
    
    
    /**
        called from worker thread, when m_data has been set
     */
    virtual void processAnalysis(const AnalysisData& data) {}

    /*
     *  implement in sub-class for callback to process CV bundle before output
     *
     */
    virtual void processAnalysisBundle(MapOSC &bndl) {}
    

    
    
    
    
    void analysisThread(AnalysisData data);
    void analysisTracking(AnalysisData& data, const AnalysisData& prev_data);
    
    std::vector<PixStats> getStatsChar( const cv::Mat& src, const cv::Mat& sobel, const cv::Mat& mask, const cv::Rect& roi);
    
    
    inline const cv::Mat& getFrame()
    {
        return m_img;
    }
    
    // setters
    void setCVParams( MapOSC & b );
    
    inline void setFrame( cv::Mat & frame )
    {
        m_img.release();
        m_img = std::move(frame);
    }
    
    inline void gaussSigma(int k)
    {
        m_gauss_sigma = 3;
        m_gauss_ksize = m_gauss_sigma * 5;
    }
    
    inline void erosionSize(int er)
    {
        m_er_element = getStructuringElement( cv::MORPH_RECT, cv::Size( 2*er + 1, 2*er+1 ), cv::Point( er, er ) );
    }
    
    inline void diationSize(int di)
    {
        m_di_element = getStructuringElement( cv::MORPH_RECT, cv::Size( 2*di + 1, 2*di+1 ), cv::Point( di, di ) );
    }
    
    inline void cannyMin(float min)
    {
        m_canny_min = min;
    }
    
    inline void cannyMax(float max)
    {
        m_canny_max = max;
    }
    
    
    inline size_t imageSize(){
        return m_img.rows * m_img.cols;
    }
    
protected:
    
    std::unique_ptr<ThreadPool>  m_thread_pool;
    
    std::mutex m_lock;
    
    // std::unordered_map<string, float> m_osc_values;
    
    AnalysisData m_prev_data;
    
    cv::Mat m_img, m_prev_frame;
    cv::Mat src_color_sized, threshold_output, src_gray, src_blur_gray, sob;
    
    float m_resize = 0.5; // { (512. / 1920), (512. / 1080.) };
    
    bool m_invert = false;
    int m_thresh = 102;
    float m_minsize = 0.000;
    float m_maxsize = 0.9;
    bool m_parents_only = 0;
    double m_track_radius = 0.1;
    
    int m_color_mode = 2; 
    
    float m_canny_min = 0;
    float m_canny_max = 30;
    
    int m_gauss_sigma = 3;
    int m_gauss_ksize = m_gauss_sigma*5;
    cv::Mat m_er_element = cv::getStructuringElement( cv::MORPH_RECT, cv::Size(1,1), cv::Point(0,0) );
    cv::Mat m_di_element = getStructuringElement( cv::MORPH_RECT, cv::Size(1,1), cv::Point(0,0) );
    
    //cv::Mat m_prev_points;
    std::vector<cv::Point2f> m_prev_points;
    
    size_t m_prev_n_flow_points = 0;
    cv::Ptr<cv::BackgroundSubtractorMOG2> pBackSub = cv::createBackgroundSubtractorMOG2();
    cv::Mat fgMask;

    
 //   std::vector<cv::Point2f>    m_prev_centroids;
 //   std::vector<int>            m_prev_centroid_id;
    
    
    
    const size_t m_maxIDs = 2048;
    
    std::vector<int> m_id_used;
    
};


