
#include "cvglMainProcess.hpp"

#include "cvglRandom.hpp"
#include "cvglHelperFunctions.hpp"

using namespace std;
using namespace cv;


void cvglMainProcess::initCues()
{
    MapOSC bndl;
    bndl.addMessage("/init/cueNames", m_cues.getCueNames() );
    sendBundle(bndl);
}

/**
 virtual function called from UDP thread
 */
void cvglMainProcess::receivedBundle( MapOSC & b )
{
    unique_lock<mutex> lock_osc(m_osc_lock);
    
    // process mixer first since the cues need to apply the pregain to the mixer faders
    //m_mixer.proc(b);
    
    // then process cues in case there are additions to bundle b,
    // or overrides in case of new cue, to control the CV and GL processes
    
    
    // >>>> process cue function is called from UDP and CV threads -- causng some problems now
    // >>>> maybe not necessary to do data analysis here? ... seems maybe reasonable to me
    
    MapOSC out = m_cues.procDataAndMixer(m_data, b);
   
    // on input process OSC with current data and mixer (with osc lock)
    sendBundle( out );
    
    setCVParams(b);
    
    // seems like this should be locked to avoid reading the values in the draw loop, but it's hanging...
    
    //unique_lock<mutex> lock(m_gl_lock);
    setMainParams(b);
   
}

/**
 *  note: must be be set  with m_osc_lock mutex since reader is on a gl thread
 */
void cvglMainProcess::setMainParams( MapOSC & b )
{
    for( const auto& m : b.getMap() )
    {
        const string& addr = m.first;
        const OSCAtomVector& val = m.second;
        
        if( addr == "/video/enable" )
        {
            m_draw_frame = val.getInt() > 0;
        }
        else if( addr == "/analysis/resize" )
        {
            m_resize = val.getFloat();
        }
        else if( addr == "/video/black" )
        {
            m_draw_black = val.getInt() > 0;
        }
        else if( addr == "/vignette/xyr" )
        {
           setVignette( val.get<float>(0), val.get<float>(1), val.get<float>(2) );
        }
        else if( addr == "/vignette/fadeSize" )
        {
           vignette_fadeSize = val.getFloat();
        }
        else if( addr == "/use/camera" )
        {
            m_use_camera_id = val.getInt();
        }
        else if( addr == "/overlap/cameras")
        {
            m_overlap_cameras = cvgl::clip( val.getFloat(), 0., 1.);
        }
        else if( addr == "/overlap/flip")
        {
            m_overlap_flip = cvgl::clip( val.getFloat(), 0., 1.);
        }
        else if( addr == "/show/webcam_tile" )
        {
            m_show_webcam_tile = val.getInt() > 0;
        }
        else if( addr == "/enable/contour" )
        {
            int setting = val.getInt();
            switch(setting)
            {
            case 1:
                m_draw_contour = true;
                m_draw_contour_triangles = false;
                break;
            case 2:
                m_draw_contour = false;
                m_draw_contour_triangles = true;
                break;
            case 3:
                m_draw_contour = true;
                m_draw_contour_triangles = true;
                break;
            default:
                m_draw_contour = false;
                m_draw_contour_triangles = false;
                break;
            }
        }
        else if( addr == "/contour/color" )
        {
            m_contour_rgba = cvgl::getRGBA(const_cast<OSCAtomVector &>(val));
        }
        else if( addr == "/contour_triangles/color" )
        {
            m_contour_triangles_rgba = cvgl::getRGBA(const_cast<OSCAtomVector &>(val));
        }
        else if( addr == "/contour/width" )
        {
            m_contour_line_thickness = val.getFloat();
        }
        else if( addr == "/enable/hull" )
        {
            m_draw_hull = val.getInt() > 0;
        }
        else if( addr == "/hull/color" )
        {
            m_hull_rgba = cvgl::getRGBA(const_cast<OSCAtomVector &>(val));
        }
        else if( addr == "/hull/width" )
        {
            m_hull_line_thickness = val.getFloat();
        }
        else if( addr == "/enable/minrect" )
        {
            m_draw_minrect = val.getInt() > 0;
        }
        else if( addr == "/minrect/color" )
        {
            m_minrect_rgba = cvgl::getRGBA(const_cast<OSCAtomVector &>(val));
        }
        else if( addr == "/minrect/width" )
        {
            m_minrect_line_thickness = val.getFloat();
        }
        else if( addr == "/video/flip" )
        {
            if( val.size() == 2 )
            {
                context.flip( val.get<int>(0), val.get<int>(1) );
            }
        }
        else if( addr == "/brightness")
        {
            brightness = val.getFloat();
        }
        else if( addr == "/contrast")
        {
            contrast = val.getFloat();
        }
        else if( addr == "/saturation")
        {
            saturation = val.getFloat();
        }
        else if( addr == "/gamma")
        {
            gamma = val.getFloat();
            cout << "setting gamma " << gamma << endl;
        }        
        /*
        else if( addr == "/triangle/interact/x")
        {
            const auto& vec = val.getAtomVector();
            for( const auto& v : vec )
            {
         //       triCollision(v->getFloat(), 0);
            }

        }*/
        else if( addr == "/big_triangle1/alpha")
        {
            big_tri1_alpha = val.getFloat();
        }        
        else if( addr == "/big_triangle2/alpha")
        {
            big_tri2_alpha = val.getFloat();
        }
        else if( addr == "/half_mirror/alpha")
        {
            half_mirror_alpha = val.getFloat();
        }
        else if( addr == "/glitch_tri/alpha")
        {
            glitch_tri_alpha = val.getFloat();
        }
        else if( addr == "/glitch_tri/z_offset" )
        {
            glitch_tri_offset_z = val.getFloat();
        }
        else if( addr == "/hsflow_scale")
        {
            if( val.size() == 2 )
                hsflow_scale = glm::vec2( val.get<float>(0), val.get<float>(1) );
            else
                hsflow_scale = glm::vec2( val.get<float>(0), val.get<float>(0) );
        }
        else if( addr == "/hsflow_offset")
        {
            if( val.size() == 2 )
                hsflow_offset = glm::vec2( val.get<float>(0), val.get<float>(1) );
            else
                hsflow_offset = glm::vec2( val.getFloat() );
        }
        else if( addr == "/hsflow_lambda")
        {
            hsflow_lambda = val.getFloat();
        }
        else if( addr == "/repos_amt")
        {
            if( val.size() == 2 )
                repos_amt = glm::vec2( val.get<float>(0), val.get<float>(1) );
            else
                repos_amt = glm::vec2( val.getFloat() );
        }
        else if( addr == "/repos_scale")
        {
            if( val.size() == 4 )
                repos_scale = glm::vec4( val.get<float>(0), val.get<float>(1), val.get<float>(2), val.get<float>(3) );
            else
                repos_scale = glm::vec4( val.getFloat() );
        }
        else if( addr == "/repos_bias")
        {
            if( val.size() == 4 )
                repos_bias = glm::vec4( val.get<float>(0), val.get<float>(1), val.get<float>(2), val.get<float>(3) );
            else
                repos_bias = glm::vec4( val.getFloat() );
        }
        else if( addr == "/luma_target")
        {
            luma_target = val.getFloat();
        }
        else if( addr == "/luma_tol")
        {
            luma_tol = val.getFloat();
        }
        else if( addr == "/luma_fade")
        {
            luma_fade = val.getFloat();
        }
        else if( addr == "/luma_mix")
        {
            luma_mix = val.getFloat();
        }
        else if( addr == "/flow_mix")
        {
            flow_mix = val.getFloat();
        }
        else if( addr == "/noise_mult")
        {
            noise_mult = val.getFloat();
        }
        else if( addr == "/noise_mix")
        {
            noise_mix = val.getFloat();
        }
        else if( addr == "/captureFrame")
        {
            if( val.size() == 2 )
            {
                captureFrame( val.get<int>(0), val.get<string>(1) );
            }
            else if( val.size() == 1 )
            {
                using namespace std::chrono;
                milliseconds ms = duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
                );

                string filename = "./capture_" + to_string( ms.count() ) + ".png";
                captureFrame( val.get<int>(0), filename );
            }
        }
    }
}

void cvglMainProcess::captureFrame( int camera_id, const string& filename )
{
    cv::imwrite(filename, frames[camera_id == 0 ? m_use_camera_id : camera_id]);
}

/**
 *  callback from camera thread
 *  wondering if maybe the camera thread is getting slowed down by the analysis
 */
void cvglMainProcess::processFrame(cv::UMat & frame, int camera_id )
{
    if( frame.empty() || !objects_initialized )
        return;

    unique_lock<mutex> lock(m_gl_lock);

    if( frames.count(camera_id) != 0 )
        frames[camera_id].release();

    cv::UMat copy;
    frame.copyTo(copy);

    frames[camera_id] = std::move(copy);

    if( m_use_camera_id == camera_id )
    {

        if( m_overlap_flip > 0 )
        {
            UMat frame1(frame), flipped, merge;
            cv::flip(frame1, flipped, 1);
            cv::addWeighted(frame1, 1.0-m_overlap_flip, flipped, m_overlap_flip, 0.0, merge);
            setFrame(merge); // takes ownership of frame in local storage m_img

        }
        else
        {
            setFrame(frame); // takes ownership of frame in local storage m_img

        }

        m_newframe = true;


        AnalysisData data;


        unique_lock<mutex> lock_osc(m_osc_lock);
      // cout << " preprocessing mode " << m_use_preprocess << endl;


        preprocess();

        if( m_contour_analysis ){
            data = analyzeContour();
            m_data = data;
        }


            //    cvx.getFlow( flowMesh );
       // profile.markEnd("preproc");
        
        //profile.markStart();
        
        //profile.markEnd("analyzeContour");
        


         analysisToGL( data );
        
        
    }
    
}

/**
 *  virtual function callback called from detached openCV worker thread
 *  could add mappings here
 */
void cvglMainProcess::processAnalysis(const AnalysisData& data)
{
    // on new data, process with mixer (no osc lock?)
    MapOSC out, b;
    
    {
        unique_lock<mutex> lock_osc(m_osc_lock);
        out = m_cues.procDataAndMixer(data, b);

        setCVParams(b);

       // unique_lock<mutex> lock(m_gl_lock);
        setMainParams(b);

    }

    sendBundle( out );

}


void cvglMainProcess::setVignette(float x, float y, float r)
{
    if( r > 0 )
    {
        vignette_xyr_aspect = glm::vec4(1 - x, y / context.getAspectRatio(), r / context.getAspectRatio(), context.getAspectRatio() );
    }
}

void cvglMainProcess::makeMirrorTriangles()
{


    cvglRandom rand;
    float nTriangles = 15;
    float yrange = 0.4;
    float overlap = 5;

    float xRatio = 1. / nTriangles;

    glitchRect->clear();
//    glitchRect->reserve( nTriangles * 3 );
    glitchRect->newObj();

/*
    for(int i = 0 ; i < nTriangles; i++)
    {
        auto tri = genTriangle(i, nTriangles, yrange, overlap, -2);

        glitchRect->addVertex(tri[0]);
        glitchRect->addVertex(tri[1]);
        glitchRect->addVertex(tri[2]);
    }

    for(int i = 0 ; i < nTriangles; i++)
    {
        auto tri = genTriangle(i, nTriangles, yrange, overlap, -1);

        glitchRect->addVertex(tri[0]);
        glitchRect->addVertex(tri[1]);
        glitchRect->addVertex(tri[2]);
    }
*/

   // float minxrange =  (i == 0 ? 0 : (i-overlap) * xRatio);
   // float maxxrange =  ((i + 1 + overlap) * xRatio);

    for(int ix = 0 ; ix < nTriangles; ix++)
    {
        for(int iy = 0 ; iy < nTriangles; iy++)
        {
            auto tri = mirrorTriangles.genTriangle(ix*xRatio, iy*xRatio, overlap*xRatio, overlap*xRatio);
            glitchRect->addVertex(tri[0]);
            glitchRect->addVertex(tri[1]);
            glitchRect->addVertex(tri[2]);

        }

//        tri_vel.emplace_back(0.);

    }


    glitchRect->endObj();
    glitchRect->initStaticDraw();
}


int cvglMainProcess::loadShaders()
{
    std::string shader_path = "./shaders/"; // relative to exec location

    if( !flow_shader.loadShaderFiles( shader_path + "vertex.vs", shader_path + "flow_repos.fs" ) ){
        cout << "failed to load screen shader" << endl;
        return 0;
    }

    if( !luma_shader.loadShaderFiles( shader_path + "vertex.vs", shader_path + "luma_alpha_color.fs" ) ){
        cout << "failed to load base shader" << endl;
        return 0;
    }

    if( !screen_shader.loadShaderFiles( shader_path + "vertex.vs", shader_path + "screen_vignette.fs" ) ){
        cout << "failed to load screen shader" << endl;
        return 0;
    }

    glm::mat4 identityMatrix = glm::identity<glm::mat4>();

    setVignette(0.5, 0.5, 1);

    // setup first shader
    luma_shader.use();
    luma_shader.setInt("tex", 0);
    luma_shader.setInt("prevTex", 1);
    luma_shader.setMat4("transform_matrix", identityMatrix );

    luma_shader.setFloat("gamma", gamma);
    luma_shader.setFloat("contrast", contrast);
    luma_shader.setFloat("saturation", saturation);
    luma_shader.setFloat("brightness", brightness);
    luma_shader.setFloat("scale_alpha", 1);

    luma_shader.setFloat("luma_target", luma_target);
    luma_shader.setFloat("luma_tol", luma_tol);
    luma_shader.setFloat("luma_fade", luma_fade);
    luma_shader.setFloat("luma_mix", luma_mix);


    // setup first shader
    flow_shader.use();
    flow_shader.setInt("tex0", 0);
    flow_shader.setInt("tex1", 1);
    flow_shader.setMat4("transform_matrix", identityMatrix );

    flow_shader.setVec2("hsflow_scale", hsflow_scale );
    flow_shader.setVec2("hsflow_offset", hsflow_offset);
    flow_shader.setFloat("hsflow_lambda", hsflow_lambda );

    flow_shader.setVec2("repos_amt", repos_amt );
    flow_shader.setVec4("repos_scale", repos_scale);
    flow_shader.setVec4("repos_bias", repos_bias );

    flow_shader.setFloat("flow_mix", flow_mix);

    screen_shader.use();
    screen_shader.setInt("framebuffer_tex", 0);
    screen_shader.setVec4("vignette_xyr_aspect", vignette_xyr_aspect);
    screen_shader.setFloat("vignette_fadeSize", vignette_fadeSize);
    screen_shader.setFloat("noise_mix", noise_mix);
    screen_shader.setFloat("noise_mix", noise_mult);

    return 1;

}

/**
 *  initObjects()
 *  init function to setup VBOs for GL objects, must be called after context setup is complete
 */

void cvglMainProcess::initObjs()
{

    //glm::vec4 vignette_xyr_aspect(0.5, 0.25, 1, context.getAspectRatio() );
    //glUniform4fv( vignette_attr_idx, 1, &vignette_xyr_aspect[0]);


    rect = unique_ptr<cvglObject>(new cvglObject);
    contourMesh = unique_ptr<cvglObject>(new cvglObject);
    hullMesh = unique_ptr<cvglObject>(new cvglObject);
    minrectMesh = unique_ptr<cvglObject>(new cvglObject);
    flowMesh = unique_ptr<cvglObject>(new cvglObject);
    glitchRect = unique_ptr<cvglObject>(new cvglObject);
    bigTriMirror = unique_ptr<cvglObject>(new cvglObject);
    bigTriMirror2 = unique_ptr<cvglObject>(new cvglObject);
    halfMirror = unique_ptr<cvglObject>(new cvglObject);

    frameTex =  unique_ptr<cvglTexture>(new cvglTexture);
    contourTex = unique_ptr<cvglTexture>(new cvglTexture);
    contourTriTex = unique_ptr<cvglTexture>(new cvglTexture);
    hullTex =  unique_ptr<cvglTexture>(new cvglTexture);
    minrectTex =  unique_ptr<cvglTexture>(new cvglTexture);
    prevFrame =  unique_ptr<cvglTexture>(new cvglTexture);

    framebuffer[0] = unique_ptr<cvglFramebuffer>(new cvglFramebuffer);
    framebuffer[1] = unique_ptr<cvglFramebuffer>(new cvglFramebuffer);
    pass_buffer = unique_ptr<cvglFramebuffer>(new cvglFramebuffer);

    m_hull_rgba = vector<float>({1, 0, 1, 1});
    m_minrect_rgba = vector<float>({1, 1, 1, 0.9});
    m_contour_rgba = vector<float>({0.25, 0.5, 1., 0.125});
    m_contour_triangles_rgba = vector<float>({1, 1, 1, 0.1});


    float x[] = {-1, 1, 1, -1 };
    float y[] = {1, 1, -1, -1 };
    rect->newObj(GL_TRIANGLES);
    rect->addVertex(cvglVertex({{x[0], y[0], 0.0f},  {0.0f, 0.0f} }));
    rect->addVertex(cvglVertex({{x[1], y[1], 0.0f},  {1.0f, 0.0f} }));
    rect->addVertex(cvglVertex({{x[2], y[2], 0.0f},  {1.0f, 1.0f} }));
    rect->addVertex(cvglVertex({{x[2], y[2], 0.0f},  {1.0f, 1.0f} }));
    rect->addVertex(cvglVertex({{x[3], y[3], 0.0f},  {0.0f, 1.0f} }));
    rect->addVertex(cvglVertex({{x[0], y[0], 0.0f},  {0.0f, 0.0f} }));
    rect->endObj();
    rect->initStaticDraw();

    makeMirrorTriangles();

    float triRatio = 3. / 4.;

    bigTriMirror->newObj(GL_TRIANGLES);
/*
    bigTriMirror->addVertex(cvglVertex({{-1,                    -1,      0.0f},        {0.0f,       0.0f} }));
    bigTriMirror->addVertex(cvglVertex({{ 1,                    -1,      0.0f},        {0.5,   0.f} }));
    bigTriMirror->addVertex(cvglVertex({{(triRatio * 2 - 1),    0.,     0.0f},         {triRatio,   0.5f} }));
*/

    bigTriMirror->addVertex(cvglVertex({{-1,                    -1,      0.0f},      {0.0f,     0.f} }));
    bigTriMirror->addVertex(cvglVertex({{ 1,                    -1,      0.0f},      {1,        0.0f} }));
    bigTriMirror->addVertex(cvglVertex({{(triRatio * 2 - 1),     0.,     0.0f},      {triRatio, 0.5f} }));

    bigTriMirror->endObj();
    bigTriMirror->initStaticDraw();

    float triRatio2 = 4. / 5.;
    float offset_tex_x = 0.0;
    bigTriMirror2->newObj(GL_TRIANGLES);
    bigTriMirror2->addVertex(cvglVertex({{-1,                   1,      0.0f},       {0 + offset_tex_x,          0.0f} }));
    bigTriMirror2->addVertex(cvglVertex({{-1,                   -1,      0.0f},      {triRatio2 + offset_tex_x,  0} }));
    bigTriMirror2->addVertex(cvglVertex({{(triRatio2 * 2 - 1),  -1.,    0.0f},       {triRatio2 + offset_tex_x,  1.f} }));

/*
    bigTriMirror2->addVertex(cvglVertex({{-1,  1,   0.0f},      {0.,  1.0} }));
    bigTriMirror2->addVertex(cvglVertex({{ 1,  1,   0.0f},      {1.,  1.0} }));
    bigTriMirror2->addVertex(cvglVertex({{-1, -1.,  0.0f},      {0.,  0.0} }));
*/
    bigTriMirror2->endObj();
    bigTriMirror2->initStaticDraw();


    halfMirror->newObj(GL_TRIANGLES);
    halfMirror->addVertex(cvglVertex({{ 0,  1, 0.0f},  {0.5f, 0.0f} }));
    halfMirror->addVertex(cvglVertex({{ 1,  1, 0.0f},  {0.f, 0.0f} }));
    halfMirror->addVertex(cvglVertex({{ 1, -1, 0.0f},  {0.f, 1.0f} }));
    halfMirror->addVertex(cvglVertex({{ 1, -1, 0.0f},  {0.f, 1.0f} }));
    halfMirror->addVertex(cvglVertex({{ 0, -1, 0.0f},  {0.5f, 1.0f} }));
    halfMirror->addVertex(cvglVertex({{ 0,  1, 0.0f},  {0.5f, 0.0f} }));
    halfMirror->endObj();
    halfMirror->initStaticDraw();


/*
    halfMirror->newObj(GL_TRIANGLES);
    halfMirror->addVertex(cvglVertex({{ 1,  0, 0.0f},  {1.f, 0.5f} }));
    halfMirror->addVertex(cvglVertex({{ 1,  1, 0.0f},  {1.f, 1.f} }));
    halfMirror->addVertex(cvglVertex({{ -1, 1, 0.0f},  {0.f, 1.f} }));
    halfMirror->addVertex(cvglVertex({{ -1, 1, 0.0f},  {0.f, 1.f} }));
    halfMirror->addVertex(cvglVertex({{ -1, 0, 0.0f},  {0.f, 0.5f} }));
    halfMirror->addVertex(cvglVertex({{ 1,  0, 0.0f},  {1.f, 0.5f} }));
    halfMirror->endObj();
    halfMirror->initStaticDraw();
*/

/*
    halfMirror->newObj(GL_TRIANGLES);
    halfMirror->addVertex(cvglVertex({{ 1,  -1, 0.0f},  {1.f, 0.f} }));
    halfMirror->addVertex(cvglVertex({{ 1,  0, 0.0f},  {1.f, 0.5f} }));
    halfMirror->addVertex(cvglVertex({{ -1, 0, 0.0f},  {0.f, 0.5f} }));
    halfMirror->addVertex(cvglVertex({{ -1, 0, 0.0f},  {0.f, 0.5f} }));
    halfMirror->addVertex(cvglVertex({{ -1, -1, 0.0f},  {0.f, 0.f} }));
    halfMirror->addVertex(cvglVertex({{ 1,  -1, 0.0f},  {1.f, 0.f} }));
    halfMirror->endObj();
    halfMirror->initStaticDraw();
*/

    objects_initialized = true;

    context.clearColor(0, 0, 0, 1);

    /*
     * this will be in the second shader...
    GLuint texLocation = context.getShaderAttrLocation("tex");
    GLuint prevTexLocation = context.getShaderAttrLocation("prevTex");

    glUniform1i(texLocation, 0);
    glUniform1i(prevTexLocation, 1);
*/

    cout << objects_initialized << endl;
}



void cvglMainProcess::analysisToGL(const AnalysisData &analysis)
{
    
    
    // lock to prevent conflict with gl thread (locked in processFrame)
    
    // note: analysis only has part of the data when it gets here!! don't try to use the other data

    size_t npoints = 0;
    for( auto& c : analysis.contours )
        npoints += (c.cols * c.rows);
    
    if( m_draw_contour || m_draw_contour_triangles){
        contourMesh->clear();
        contourMesh->reserve( npoints );
    }
   
    if( m_draw_hull )
    {
        hullMesh->clear();
        hullMesh->reserve( npoints );
    }
    
    if( m_draw_minrect )
    {
        minrectMesh->clear();
        minrectMesh->reserve( analysis.contours.size() * 4 * 2 );
    }
    
    for( size_t i = 0 ; i < analysis.contour_idx.size(); i++ )
    {
        
        if( m_draw_contour || m_draw_contour_triangles )
        {
            cvgl::pointMatToVertex( analysis.contours[ analysis.contour_idx[i] ], contourMesh, analysis.halfW, analysis.halfH );
            contourMesh->triangulate();
            //setTriangleTexcords( contourMesh );
        }
       
        if( m_draw_hull )
        {
           cvgl::pointMatToPolygonLineVertex(analysis.hullP_vec[i], hullMesh, analysis.halfW, analysis.halfH, m_hull_line_thickness * m_resize);
        }
        
        if( m_draw_minrect )
        {
            // cvgl::rotatedRectToVertex(minRec_vec[i], minrectMesh, halfW, halfH );
            
            Point2f rectPts[4];
            analysis.minRect_vec[i].points( rectPts );
            vector<Point2f> rect_v(rectPts, rectPts+4);
            
            cvgl::linePointsToPolygon(rect_v, minrectMesh, analysis.halfW, analysis.halfH, m_minrect_line_thickness * m_resize, true);
            
            //cvgl::pointsToPolygonLineVertex(rect_v, minrectMesh, analysis.halfW, analysis.halfH, 10);
        }


    }
    
    
}


/**
 *  called from GL thread
 */

void cvglMainProcess::handleKeyInput()
{

    if( glfwGetKey(context.getWindow(), GLFW_KEY_RIGHT ) == GLFW_PRESS )
    {
        cout << m_cues.getNext() << endl;
    }
    else if( glfwGetKey(context.getWindow(), GLFW_KEY_LEFT ) == GLFW_PRESS )
    {
        cout << m_cues.getPrev() << endl;

    }

}

void cvglMainProcess::draw()
{

    //cout << ">> draw LOCK" << endl;
    // auto start = std::chrono::system_clock::now();

    unique_lock<mutex> lock(m_gl_lock, std::defer_lock); // lock or wait for gl_lock and then lock
    
    //   cout << (start - std::chrono::system_clock::now()).count()  << endl;
    
    // this can get slowed down if a new frame comes in while the old one is still being drawn?
    
    if( !lock.try_lock() || !context.isActive() || !objects_initialized || m_img.empty() || !m_newframe ){
        //cout << "<< draw unlock" << endl;
        return;
    }
    

    // maybe at end stop clearing
    // or figure out how to copy output image to new texture and
    // make some feedback stuff
    // see: https://learnopengl.com/Advanced-OpenGL/Framebuffers

    glm::mat4 transform = context.getTransform();

    if( m_draw_black )
    {
        context.bindDefaultFramebuffer();
        screen_shader.use();
     /*
        screen_shader.setMat4("transform_matrix", transform);
        screen_shader.setFloat("time", (float)glfwGetTime() );

        screen_shader.setFloat("noise_mix", noise_mix );
        screen_shader.setFloat("noise_mult", noise_mult );
        screen_shader.setVec4("vignette_xyr_aspect", vignette_xyr_aspect);
        screen_shader.setFloat("vignette_fadeSize", vignette_fadeSize);
*/

        glClearColor(0.f, 0.f, 0.f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        context.updateViewport(1);
        context.clear();

        context.drawAndPoll();
        m_newframe = false;
        return;
    }


    // make frame texture
    UMat merge;
    if( m_draw_frame && m_use_camera_id > 0 )
    {
        merge = getFrame();

        if( m_show_webcam_tile && frames.count(3) > 0 )
        {
            UMat tile;
            frames[3].copyTo(tile);

            double w = 1920 * 0.25;
            double h = 1080 * 0.25;
            cv::resize(frames[3], tile, Size(w,h));

            UMat insetImage(merge, Rect(1920 - w, 1080 - h, w, h));
            tile.copyTo(insetImage);

        }

        if( m_overlap_cameras > 0 && frames.count(1) > 0 && frames.count(2) > 0 )
        {
            cv::addWeighted(frames[1], 1.0-m_overlap_cameras, frames[2], m_overlap_cameras, 0.0, merge);
        }

        frameTex->setTexture( merge.getMat(ACCESS_READ) );
    }

    // draw main frame



    int prev_fbIDX = fbIDX;
    fbIDX = (fbIDX+1) % 2;

    pass_buffer->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    context.clear();

    // note: don't update the viewport or transform, since the fbos are always the same size (1920x1080)

    flow_shader.use();

    flow_shader.setFloat("slide_up", 3.3);
    flow_shader.setFloat("slide_down", 3.3);

    flow_shader.setVec2("hsflow_scale", hsflow_scale );
    flow_shader.setVec2("hsflow_offset", hsflow_offset);
    flow_shader.setFloat("hsflow_lambda", hsflow_lambda );

    flow_shader.setVec2("repos_amt", repos_amt );
    flow_shader.setVec4("repos_scale", repos_scale);
    flow_shader.setVec4("repos_bias", repos_bias );

    flow_shader.setFloat("flow_mix", flow_mix);



    glActiveTexture(GL_TEXTURE0+1); // Texture unit 1
    context.bindTextureByID( framebuffer[prev_fbIDX]->getTexID() );

    glActiveTexture(GL_TEXTURE0); // Texture unit 0 for new frames and gl colors

    if( m_draw_frame )
    {
        rect->bind();
        frameTex->bind();
        rect->draw();
    }


    // switch to write into storage framebuffer
    // draw previous texture, applying flow stuff

    // ----------------------------------------------------------------
    // luma framebuffer, stored for feedback

    framebuffer[fbIDX]->bind();
    // note: don't update the viewport or transform, since the fbos are always the same size (1920x1080)
    luma_shader.use();
    luma_shader.setFloat("gamma", gamma);
    luma_shader.setFloat("contrast", contrast);
    luma_shader.setFloat("saturation", saturation);
    luma_shader.setFloat("brightness", brightness);

    luma_shader.setFloat("luma_target", luma_target);
    luma_shader.setFloat("luma_tol", luma_tol);
    luma_shader.setFloat("luma_fade", luma_fade);
    luma_shader.setFloat("luma_mix", luma_mix);
    luma_shader.setFloat("scale_alpha", 1);


    //luma_shader.setFloat("prev_tex_ratio", 0.94);

    // this needs to be black, since the processing creates alpha dips
    // we will see the background if the the alpha is low
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    context.clear();


    rect->bind();    
        glActiveTexture(GL_TEXTURE0);
        frameTex->bind();

        glActiveTexture(GL_TEXTURE0+1);
        context.bindTextureByID( pass_buffer->getTexID() ); // tex0 = prev render pass
    rect->draw();

    drawShapes( luma_shader );


    // ----------------------------------------------------------------
    // screen framebuffer


    // switch to this default screen framebuffer
    // draw fbIDX frame buffer, using simple output shader (not stored)

    context.bindDefaultFramebuffer();
    screen_shader.use();
    screen_shader.setMat4("transform_matrix", transform);
    screen_shader.setFloat("time", (float)glfwGetTime() );

    screen_shader.setFloat("noise_mix", noise_mix );
    screen_shader.setFloat("noise_mult", noise_mult );
    screen_shader.setVec4("vignette_xyr_aspect", vignette_xyr_aspect);
    screen_shader.setFloat("vignette_fadeSize", vignette_fadeSize);


    glClearColor(0.f, 0.f, 0.f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    context.updateViewport(1);
    context.clear();
    glActiveTexture(GL_TEXTURE0);

    rect->bind();
    context.bindTextureByID( framebuffer[fbIDX]->getTexID() ); //framebuffer->getTexID()
    rect->draw();

    context.drawAndPoll();

    // context.printFPS();
    
    m_newframe = false;
    
}


void cvglMainProcess::drawShapes(cvglShader& shapeRenderShader)
{
    glm::mat4 transform = glm::identity<glm::mat4>(); // not screen version here...

    glActiveTexture(GL_TEXTURE0);

    if( m_draw_contour )
    {
        contourMesh->bind();
        shapeRenderShader.setFloat("scale_alpha", 1);
        contourTex->setTexture(m_contour_rgba);//getFrame());
        contourMesh->draw(GL_TRIANGLES);
    }

    if( m_draw_contour_triangles )
    {
        contourMesh->bind();
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        contourTriTex->setTexture(m_contour_triangles_rgba);
        shapeRenderShader.setFloat("scale_alpha", 1);
        contourMesh->draw(GL_TRIANGLE_STRIP);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }

    if( m_draw_hull )
    {
        hullMesh->bind();
        shapeRenderShader.setFloat("scale_alpha", 1);
        hullTex->setTexture(m_hull_rgba);
        hullMesh->draw(GL_TRIANGLE_STRIP);//vector<int>({GL_TRIANGLES, GL_POINTS}));
    }

    if( m_draw_minrect )
    {
        //   glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        minrectMesh->bind();
        minrectTex->setTexture(m_minrect_rgba);
        shapeRenderShader.setFloat("scale_alpha", 1);
        minrectMesh->draw(GL_TRIANGLE_STRIP);
    }


    if( half_mirror_alpha > 0 )
    {

        halfMirror->bind();
        shapeRenderShader.setFloat("scale_alpha", half_mirror_alpha);
        frameTex->bind();
        halfMirror->draw();

    }

    if( big_tri2_alpha > 0 )
    {

        bigTriMirror2->bind();
        shapeRenderShader.setFloat("scale_alpha", big_tri2_alpha);
        frameTex->bind();
        bigTriMirror2->draw();

    }

    if( big_tri1_alpha > 0 )
    {

        bigTriMirror->bind();

        shapeRenderShader.setFloat("scale_alpha", big_tri1_alpha);
        //cout << big_tri_alpha << " ? " << endl;
        //glUniformMatrix4fv(transform_attr_idx, 1, GL_FALSE, &translated[0][0]);
        //glUniform1f(scale_alpha_attr_idx, big_tri_alpha);

        frameTex->bind();
        bigTriMirror->draw();
       // flow_shader.setMat4("transform_matrix", transform);

        //glUniform1f(scale_alpha_attr_idx, 1);
        //glUniformMatrix4fv(transform_attr_idx, 1, GL_FALSE, &transform[0][0]);
    }

    if( glitch_tri_alpha > 0 )
    {

        glitchRect->bind();

        glm::mat4 translated = glm::translate(transform, glm::vec3(0, 0, glitch_tri_offset_z ));

        shapeRenderShader.setMat4("transform_matrix", translated);
        shapeRenderShader.setFloat("scale_alpha", glitch_tri_alpha);
        frameTex->bind();
        glitchRect->draw();

        shapeRenderShader.setMat4("transform_matrix", transform);


    }
}

