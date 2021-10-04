#pragma once

#include "cvglContext.hpp"
#include "cvglCV.hpp"
#include "cvglObject.hpp"
#include "cvglUDPServer.hpp"
#include "cvglProfile.hpp"
#include "MapOSC.hpp"
#include "AnimismCues.hpp"


class cvglMainProcess :  public cvglCV, public cvglUDPServer
{
    
public:
    
    // GL context
    cvglContext context;
    
    // globals
    
    void setVignette(float x, float y, float r);

    GLint vignette_attr_idx = -1, gamma_attr_idx = -1, contrast_attr_idx = -1, brightness_attr_idx = -1, saturation_attr_idx = -1;

    float contrast = 1, brightness = 0, saturation = 1, gamma = 1;
    glm::vec4 vignette_xyr_aspect;

    std::unique_ptr<cvglObject>     rect, contourMesh, hullMesh, minrectMesh, flowMesh, glitchRect;
    std::unique_ptr<cvglTexture>    frameTex, contourTex, contourTriTex, hullTex, minrectTex, flowTex;
    
    bool objects_initialized = false;
    
    // methods
    

    // --- called from camera thread ---
    void processFrame(cv::UMat & frame, int camera_id ) ;
    void analysisToGL(const AnalysisData& analysis);

    // --- called from cv analysis worker thread when m_data has been set ---
    void processAnalysis(const AnalysisData& data) override;
    
    // --- called from udp thread ---
    void receivedBundle( MapOSC & b ) override;
    
    void setMainParams( MapOSC & b );

    // --- called from gl thread (main thread) ---
    void draw();
    void initObjs();
    
    void initCues();
    
//    std::vector<float> getRGBA( const OdotMessage & msg );
    
    inline void useCameraID( int i ){ m_use_camera_id = i; }
    
    /*
    inline void initMixer()
    {
        MapOSC out;
        m_mixer.initMidi(out);
        std::cout << "initializing midi mixer" << std::endl;
        sendBundle( out );
    }
    */

private:
    
    AnimismCues m_cues;
    //cvglMixer m_mixer;
    
    AnalysisData m_data;
    
    cvglProfile profile;
    
    int m_use_camera_id = 1;
    float m_overlap_cameras = 0;
    float m_overlap_flip = 0;
    bool m_show_webcam_tile = false;

    std::unordered_map<int, cv::UMat> frames;

    std::mutex m_gl_lock, m_osc_lock;
    
    bool m_newframe = false;
        
    bool m_draw_frame = true;
    bool m_draw_black = false;
    
    bool m_draw_contour = false;
    bool m_draw_contour_triangles = false;
    
    bool m_draw_hull = true;
    bool m_draw_minrect = true;
    
    std::vector<float> m_contour_rgba;
    std::vector<float> m_contour_triangles_rgba;

    std::vector<float> m_hull_rgba;
    std::vector<float> m_minrect_rgba;

    float m_hull_line_thickness = 1;
    float m_minrect_line_thickness = 1;
    float m_contour_line_thickness = 1;
        
    bool m_contour_analysis = true;

    float m_contrast = 1;
    float m_saturation = 1;
};



