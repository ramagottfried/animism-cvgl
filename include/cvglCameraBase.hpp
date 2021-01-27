#pragma once

#include "opencv2/core/utility.hpp"

class cvglCameraBase
{

public:
    
    inline bool hasCamera() { return m_opencamera; }
    inline int getWidth() { return m_width; }
    inline int getHeight() { return m_height; }
    
    bool newframe = false;
    
    inline void setProcessFrameCallback( std::function<void (cv::Mat&)> cb )
    {
        m_processFrameCallback = cb;
    }
    
protected:
    
    std::function<void (cv::Mat&)> m_processFrameCallback;

    int m_width = 0;
    int m_height = 0;
    
    bool m_opencamera = false;
};

