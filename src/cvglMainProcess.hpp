#pragma once

#include "cvglContext.hpp"
#include "cvglCV.hpp"
#include "cvglObject.hpp"
#include "cvglFramebuffer.hpp"
#include "cvglShader.hpp"

#include "animShader1.hpp"

#include "cvglUDPServer.hpp"
#include "cvglProfile.hpp"
#include "MapOSC.hpp"
#include "AnimismCues.hpp"

#include "MirrorTriangles.hpp"

class cvglMainProcess :  public cvglCV, public cvglUDPServer
{
    
public:
    
    // GL context
    cvglContext context;

    cvglShader screen_shader, luma_shader, flow_shader;

    std::unique_ptr<animShader1> shader1;

    // globals
    
    void setVignette(float x, float y, float r);


    double rotateTriangles = 0, tri_step = 0.01, tri_damping = 0.99;
    std::vector<double> tri_vel;

    std::unique_ptr<cvglObject>     rect, contourMesh, hullMesh, minrectMesh, flowMesh;
    std::unique_ptr<cvglObject>     glitchRect, bigTriMirror, bigTriMirror2, halfMirror;

    std::unique_ptr<cvglTexture>    frameTex, contourTex, contourTriTex, hullTex, minrectTex, flowTex, prevFrame;
    
    int fbIDX = 0;
    std::unique_ptr<cvglFramebuffer> framebuffer[2];
    std::unique_ptr<cvglFramebuffer> pass_buffer;

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
    
    void initCues();

  //  vector<cvglVertex> genTriangle(int i, float nTriangles, float yrange, float overlap, float x_offset = 0 );
  //  vector<cvglVertex> genTriangle(float x, float y, float xrange, float yrange);

    void makeMirrorTriangles();
//    void triCollision(double x, double y);
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

    int loadShaders();


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
    
    float glitch_tri_alpha = 0.5;
    float glitch_tri_offset_z = 1.;

    float big_tri1_alpha = 0;

    float big_tri2_alpha = 0.;

    float half_mirror_alpha = 0.;

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


    MirrorTriangles mirrorTriangles;

};



