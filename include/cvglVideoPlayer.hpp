#pragma once

#include <iostream>
#include <thread>
#include <chrono>

#include "opencv2/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include "cvglCameraBase.hpp"

class cvglVideoPlayer : public cvglCameraBase
{
public:
    cvglVideoPlayer(){}
    cvglVideoPlayer(const std::string& file){
        loadFile(file);
    }

    ~cvglVideoPlayer()
    {
        stop();
    }
    
    void cvCamLoop();
    
    void start();
    void pause(bool state);
    void stop();
    
    void setFrame(size_t frameNum);

    void loadFile(const std::string& file);

    inline double getCurrentFrame()
    {
        if( cap.isOpened() )
            return cap.get(cv::CAP_PROP_POS_FRAMES);
        else
            return -1;
    }

    inline double getNumFrames()
    {
        if( cap.isOpened() )
            return cap.get(cv::CAP_PROP_FRAME_COUNT);
        else
            return -1;
    }

    inline double getFrameRate()
    {
        if( cap.isOpened() )
            return cap.get(cv::CAP_PROP_FPS);
        else
            return -1;
    }

private:
    
    cv::VideoCapture cap;
    std::thread m_cvCam_thread;
    
    bool m_stop_cv_loop = false;
    
    bool m_paused = false;
    bool m_loop = true;

    std::mutex m_mutex;
    
    double fps, s_per_frame;
    size_t frameCount = 0, nFrames = 0;
    int direction = 1;
    
    long manual_frame = -1; //100;
};
