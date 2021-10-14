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
    cvglVideoPlayer(const std::string& file);
    ~cvglVideoPlayer()
    {
        stop();
    }
    
    void cvCamLoop();
    
    void start();
    void pause(bool state);
    void stop();
    
    
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
};
