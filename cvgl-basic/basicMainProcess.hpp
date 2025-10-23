#pragma once

#include "cvglContext.hpp"
#include "cvglCV.hpp"
#include "cvglObject.hpp"
#include "cvglFramebuffer.hpp"
#include "cvglShader.hpp"

#include "cvglUDPServer.hpp"
#include "cvglProfile.hpp"
#include "MapOSC.hpp"
#include "cvglVideoPlayer.hpp"

// #include "AnimismCues.hpp"


class basicMainProcess :  public cvglCV, public cvglUDPServer
{
    
public:
    
    // GL context
    cvglContext context;

    cvglShader screen_shader;

    // globals
    
    void setVignette(float x, float y, float r);

    std::unique_ptr<cvglObject> rect, contourMesh, hullMesh, minrectMesh, flowMesh, defect_dots, orange_dot_pattern;

    std::unique_ptr<cvglTexture> frameTex, contourTex, contourTriTex, hullTex, minrectTex, defect_tex, flowTex, prevFrame, orange_dot_tex;
    
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
    void drawShapes(cvglShader& shapeRenderShader);
    void initObjs();
    
    void handleKeyInput();


    void captureFrame( int camera_id = 1, const std::string& filename = "imageCap.png");


    void initCues();
    
    inline void useCameraID( int i ){ m_use_camera_id = i; }

    int loadShaders();

    
    MapOSC dataToMap(const AnalysisData& data);

//    AnimismCues& cues(){ return m_cues; }

    cvglVideoPlayer movie;

private:

    // AnimismCues m_cues;

    size_t frame_count = 0;
    AnalysisData m_data;
    
    cvglProfile profile;
    
    int m_use_camera_id = 1;
    float m_overlap_cameras = 0;
    float m_overlap_flip = 0;
    bool m_show_webcam_tile = false;

    std::unordered_map<int, cv::UMat> frames;
    std::unordered_map<int, int> camera_flip;

    std::mutex m_gl_lock, m_osc_lock;
    
    bool m_newframe = false;
        
    bool m_draw_frame = true;
    bool m_draw_black = false;
    
    bool m_draw_contour = false;
    bool m_draw_contour_triangles = false;
    
    bool m_draw_hull = true;
    bool m_draw_minrect = true;
    bool m_draw_defect_dots = false;


    std::vector<float> m_contour_rgba;
    std::vector<float> m_contour_triangles_rgba;

    std::vector<float> m_hull_rgba;
    std::vector<float> m_minrect_rgba;

    float m_hull_line_thickness = 1;
    float m_minrect_line_thickness = 1;
    float m_contour_line_thickness = 1;
        
    bool m_contour_analysis = true;

    int tri_update_counter = 0;

    float luma_target = 0.1;
    float luma_tol = 0.1;
    float luma_fade = 0.48;
    float luma_mix = 0.0;

    glm::vec2 hsflow_scale = glm::vec2(0.02);
    glm::vec2 hsflow_offset = glm::vec2(0.01);
    float hsflow_lambda = 2.;

    glm::vec2 repos_amt = glm::vec2(3.);
    glm::vec4 repos_scale = glm::vec4(1.);
    glm::vec4 repos_bias = glm::vec4(0.);

    float flow_mix = 0.;

    float noise_mult = 1;
    float noise_mix = 0.;

    float contrast = 1, brightness = 0, saturation = 1, gamma = 1;
    glm::vec4 vignette_xyr_aspect;
    float vignette_fadeSize = 0.5;

    glm::vec2 drawRange_y = glm::vec2(0.0f, 1.0f);
    glm::vec2 drawRange_x = glm::vec2(0.0f, 1.0f);




};



