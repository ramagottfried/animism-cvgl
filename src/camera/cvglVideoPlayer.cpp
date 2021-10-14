
#include "cvglVideoPlayer.hpp"

using namespace std;

cvglVideoPlayer::cvglVideoPlayer(const string& file)
{
    cap.open(file);
    if(!cap.isOpened())  // check if we succeeded
    {
        return;
    }
    
    m_opencamera = true;
    
    m_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    m_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    s_per_frame =  1.0 / cap.get(cv::CAP_PROP_FPS);
    
    nFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
    // could implement forward/backward system
    cout << "opened file " << file << " fps " << fps << endl;
}

// opencv camera callback
void cvglVideoPlayer::cvCamLoop()
{
    cv::UMat _frame;
    
    chrono::time_point<chrono::system_clock> current_time;
    chrono::time_point<chrono::system_clock> prevTime = chrono::system_clock::now();
    
    while( cap.isOpened() && m_stop_cv_loop == false )
    {
        if( !m_paused )
        {
            current_time = chrono::system_clock::now();
            chrono::duration<double> elapsed_seconds = current_time-prevTime;
            
            if( elapsed_seconds.count() >= s_per_frame )
            {
                bool do_callback = true;
                
                
                if( !cap.read(_frame) )
                {
                    if( m_loop )
                    {
                        cap.set(cv::CAP_PROP_POS_MSEC, 0);
                        
                        if( !cap.read(_frame) ){
                            do_callback = false;
                            stop();
                        }
                    }
                    else
                    {
                        do_callback = false;
                    }
                }
                /**
                    note: processFrameCallback may take ownership of Mat
                 */
                if( do_callback && m_processFrameCallback )
                    m_processFrameCallback( _frame );
                
                prevTime = current_time;
             //   cout << "frame idx " << cap.get(cv::CAP_PROP_POS_FRAMES) << endl;
            }
            
            
        }
        
    }
    
    cout << "cameraLoop end" << endl;
}

void cvglVideoPlayer::start()
{
    // start camera loop (on same thread here...)
    m_cvCam_thread = thread(&cvglVideoPlayer::cvCamLoop, this);

}

void cvglVideoPlayer::pause(bool state)
{
    m_paused = state;
}

void cvglVideoPlayer::stop()
{
    cout << "stop()" << endl;

    // signal cv thread stop
    m_stop_cv_loop = true;
    
    if( m_cvCam_thread.joinable() )
        m_cvCam_thread.join();
    
    // close camera after join
    if( cap.isOpened() )
        cap.release();
}

