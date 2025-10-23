#include "basicMainProcess.hpp"

#include "cvglRandom.hpp"
#include "cvglHelperFunctions.hpp"

using namespace std;
using namespace cv;


void basicMainProcess::initCues()
{
    MapOSC bndl;
    
    // bndl.addMessage("/init/cueNames", m_cues.getCueNames() );
    
    sendBundle(bndl);
}

/**
 virtual function called from UDP thread
 */
void basicMainProcess::receivedBundle( MapOSC & b )
{
    unique_lock<mutex> lock_osc(m_osc_lock);
    
    // process mixer first since the cues need to apply the pregain to the mixer faders
    //m_mixer.proc(b);
    
    // then process cues in case there are additions to bundle b,
    // or overrides in case of new cue, to control the CV and GL processes
    
    
    // >>>> process cue function is called from UDP and CV threads -- causng some problems now
    // >>>> maybe not necessary to do data analysis here? ... seems maybe reasonable to me
    
    //MapOSC out = m_cues.procDataAndMixer(m_data, b);
    MapOSC out;
    
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
void basicMainProcess::setMainParams( MapOSC & b )
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
        else if( addr == "/camera/cutoff" )
        {
            if( val.size() == 2 )
            {
                drawRange_y.x = val.get<float>(0);
                drawRange_y.y = val.get<float>(1);
            }
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
        else if(addr == "/camera/flip")
        {
            if( val.size() == 2 )
            {
                int id = val.get<int>(0);
                int flip = val.get<int>(1);
                
                camera_flip[id] = flip;
            }
        }
        else if( addr == "/overlap/cameras")
        {
            m_overlap_cameras = cvgl::clip( val.getFloat(), 0., 1.);
        }
        else if( addr == "/overlap/flip")
        {
            m_overlap_flip = cvgl::clip( val.getFloat(), 0., 1.);
        }
        else if( addr == "/enable/contour" || addr == "/contour/enable" )
        {
            m_draw_contour = val.getInt();
        }
        else if( addr == "/contour/color" )
        {
            m_contour_rgba = cvgl::getRGBA(const_cast<OSCAtomVector &>(val));
            m_contour_rgba[0] *= m_contour_rgba[3]; // kludge for alpha blending, only needed on mac?
            m_contour_rgba[1] *= m_contour_rgba[3];
            m_contour_rgba[2] *= m_contour_rgba[3];
        }
        else if( addr == "/contour_triangles/enable" )
        {
            m_draw_contour_triangles = val.getInt();
        }
        else if( addr == "/contour_triangles/color" )
        {
            m_contour_triangles_rgba = cvgl::getRGBA(const_cast<OSCAtomVector &>(val));
            m_contour_triangles_rgba[0] *= m_contour_triangles_rgba[3]; // kludge for alpha blending, only needed on mac?
            m_contour_triangles_rgba[1] *= m_contour_triangles_rgba[3];
            m_contour_triangles_rgba[2] *= m_contour_triangles_rgba[3];
        }
        else if( addr == "/contour/width" )
        {
            m_contour_line_thickness = val.getFloat();
        }
        else if( addr == "/enable/hull" || addr == "/hull/enable" )
        {
            m_draw_hull = val.getInt() > 0;
        }
        else if( addr == "/hull/color" )
        {
            m_hull_rgba = cvgl::getRGBA(const_cast<OSCAtomVector &>(val));
            m_hull_rgba[0] *= m_hull_rgba[3]; // kludge for alpha blending, only needed on mac?
            m_hull_rgba[1] *= m_hull_rgba[3];
            m_hull_rgba[2] *= m_hull_rgba[3];
        }
        else if( addr == "/hull/width" )
        {
            m_hull_line_thickness = val.getFloat();
        }
        else if( addr == "/enable/minrect" || addr == "/minrect/enable"  )
        {
            m_draw_minrect = val.getInt() > 0;
        }
        else if( addr == "/minrect/color" )
        {
            m_minrect_rgba = cvgl::getRGBA(const_cast<OSCAtomVector &>(val));
            m_minrect_rgba[0] *= m_minrect_rgba[3]; // kludge for alpha blending, only needed on mac?
            m_minrect_rgba[1] *= m_minrect_rgba[3];
            m_minrect_rgba[2] *= m_minrect_rgba[3];
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
        else if( addr == "/show/webcam_tile" )
        {
            m_show_webcam_tile = val.getInt() > 0;
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
        else if (addr == "/video/movie/frame")
        {
            //cout << "got movie frame? \n";
            movie.setFrame( val.getInt() );
        }
    }
}


void basicMainProcess::setVignette(float x, float y, float r)
{
    if( r > 0 )
    {
        vignette_xyr_aspect = glm::vec4(1 - x, y / context.getAspectRatio(), r / context.getAspectRatio(), context.getAspectRatio() );
    }
}


void basicMainProcess::captureFrame( int camera_id, const string& filename )
{
    cv::imwrite(filename, frames[camera_id == 0 ? m_use_camera_id : camera_id]);
}

/**
 *  callback from camera thread
 *  wondering if maybe the camera thread is getting slowed down by the analysis
 */
void basicMainProcess::processFrame(cv::UMat & frame, int camera_id )
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
        else if( camera_flip.find(camera_id) != camera_flip.end() )
        {
            UMat flipped(frame);
            cv::flip(frame, flipped, camera_flip[camera_id] );
            setFrame(flipped); // takes ownership of frame in local storage m_img
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

MapOSC basicMainProcess::dataToMap(const AnalysisData& data)
{
    MapOSC osc;
    
    // osc.addMessage("/id_idx", id_idx);
    // osc.addMessage("/start_time", start_time);
    
    // aspect
    osc.addMessage("/ids", data.id);
    osc.addMessage("/center/x", data.center_x);
    osc.addMessage("/center/y", data.center_y);
    osc.addMessage("/size/x", data.size_x);
    osc.addMessage("/size/y", data.size_y);
    osc.addMessage("/centroid/x", data.centroid_x);
    osc.addMessage("/centroid/y", data.centroid_y);
    osc.addMessage("/eccentricity", data.eccentricity);
    osc.addMessage("/rotrect/major", data.rotrect_major);
    osc.addMessage("/rotrect/minor", data.rotrect_minor);
    osc.addMessage("/angle", data.angle);
    osc.addMessage("/area", data.contour_area);
    osc.addMessage("/hull/area", data.hull_area);
    osc.addMessage("/parimeter", data.parimeter);
    osc.addMessage("/isconvex", data.convex);
    osc.addMessage("/parent", data.parent);
    
// dim_xy
    osc.addMessage("/defect/count", data.defect_count);
    osc.addMessage("/defect/dist_sum", data.defect_dist_sum);
    osc.addMessage("/hull/count", data.hull_count);
// contour count
    osc.addMessage("/count", data.ncontours);


    osc.addMessage("/focus", data.focus);


    osc.addMessage("/elongation", data.elongation);
    osc.addMessage("/verticality", data.verticality);
    osc.addMessage("/centroid/dist", data.centroid_dist);
    osc.addMessage("/centroid/angle", data.centroid_angle);

    
    osc.addMessage("/defect/rel/mean/angle", data.defect_rel_mean_angle);
    osc.addMessage("/defect/rel/depthweight", data.defect_rel_depthweight);

    osc.addMessage("/elapsed/contour", data.elapsed_contour);
    osc.addMessage("/start/centroid/x", data.start_centroid_x);
    osc.addMessage("/start/centroid/y", data.start_centroid_y);
    
    osc.addMessage("/delta/centroid/x", data.delta_centroid_x);
    osc.addMessage("/delta/centroid/y", data.delta_centroid_y);
    osc.addMessage("/delta/centroid/dist", data.delta_centroid_dist);
    osc.addMessage("/moved", data.delta_centroid_dist);
   
    osc.addMessage("/noteOn/idx", data.noteOn_idx);
    osc.addMessage("/sustain/prev/idx", data.sustain_idx);
    osc.addMessage("/noteOff/prev/idx", data.noteOff_prev_idx);
    
    /*
    osc.addMessage("/test/prev/id", data.prev_id);
    osc.addMessage("/test/count", frame_count++);
     */
    
    vector<int> idx(data.ncontours);
    vector<double> pix_0_mean(data.ncontours);
    vector<double> pix_1_mean(data.ncontours);
    vector<double> pix_2_mean(data.ncontours);
    
    vector<double> pix_0_var(data.ncontours);
    vector<double> pix_1_var(data.ncontours);
    vector<double> pix_2_var(data.ncontours);
    
    vector<double> pix_0_max(data.ncontours);
    vector<double> pix_1_max(data.ncontours);
    vector<double> pix_2_max(data.ncontours);
    
    vector<double> pix_0_std(data.ncontours);
    vector<double> pix_1_std(data.ncontours);
    vector<double> pix_2_std(data.ncontours);
    
    for( int i=0; i<data.ncontours; i++)
    {
        idx[i] = i;
        pix_0_mean[i] = data.pix_channel_stats[i][0].mean;
        pix_1_mean[i] = data.pix_channel_stats[i][1].mean;
        pix_2_mean[i] = data.pix_channel_stats[i][2].mean;
        pix_0_var[i] = data.pix_channel_stats[i][0].variance;
        pix_1_var[i] = data.pix_channel_stats[i][1].variance;
        pix_2_var[i] = data.pix_channel_stats[i][2].variance;
        pix_0_max[i] = data.pix_channel_stats[i][0].max;
        pix_1_max[i] = data.pix_channel_stats[i][1].max;
        pix_2_max[i] = data.pix_channel_stats[i][2].max;
        
        pix_0_std[i] = sqrt(pix_0_var[i]) * 0.3921568627;
        pix_1_std[i] = sqrt(pix_1_var[i]);
        pix_2_std[i] = sqrt(pix_2_var[i]);
    }
    
    osc.addMessage("/idx", idx);
    
    osc.addMessage("/pix/l/mean", pix_0_mean );
    osc.addMessage("/pix/a/mean", pix_1_mean );
    osc.addMessage("/pix/b/mean", pix_2_mean );
    
    osc.addMessage("/pix/l/var", pix_0_var );
    osc.addMessage("/pix/a/var", pix_1_var );
    osc.addMessage("/pix/b/var", pix_2_var );
    
    osc.addMessage("/pix/l/std", pix_0_std );
    osc.addMessage("/pix/a/std", pix_1_std );
    osc.addMessage("/pix/b/std", pix_2_std );

    if( movie.getNumFrames() > 0 ){
        osc.addMessage("/movie/frame", movie.getCurrentFrame());
        osc.addMessage("/movie/num_frames", movie.getNumFrames());
        osc.addMessage("/movie/fps", movie.getFrameRate());
    }

    return osc;
}

/**
 *  virtual function callback called from detached openCV worker thread
 *  could add mappings here
 */
void basicMainProcess::processAnalysis(const AnalysisData& data)
{
    // on new data, process with mixer (no osc lock?)
    MapOSC out, b;
    
    {
        unique_lock<mutex> lock_osc(m_osc_lock);

//        out = m_cues.procDataAndMixer(data, b);

        setCVParams(b);

       // unique_lock<mutex> lock(m_gl_lock);
        setMainParams(b);

    }

    sendBundle( dataToMap(data) );

}


int basicMainProcess::loadShaders()
{
    std::string shader_path = "./shaders/"; // relative to exec location (doesn't work with xcode I think)
    //std::string shader_path = "/Users/rgottfri/Documents/dev/animism-cvgl/src/shaders/"; //"/home/rama/animism-cvgl/src/";

    if( !screen_shader.loadShaderFiles( shader_path + "vertex.vs", shader_path + "crop_fragment.fs" ) ){
        cout << "failed to load screen shader" << endl;
        return 0;
    }

    glm::mat4 identityMatrix = glm::identity<glm::mat4>();

    setVignette(0.5, 0.5, 1);


    screen_shader.use();
    screen_shader.setInt("framebuffer_tex", 0);
    screen_shader.setVec4("vignette_xyr_aspect", vignette_xyr_aspect);
    screen_shader.setFloat("vignette_fadeSize", vignette_fadeSize);
    screen_shader.setFloat("noise_mix", noise_mix);
    screen_shader.setFloat("noise_mix", noise_mult);
    screen_shader.setVec2("drawRange_y", drawRange_y);


    return 1;

}

void drawCircle(unique_ptr<cvglObject>& obj, float cx, float cy, float radius, float aspect_ratio, int c_steps = 20)
{
    
    constexpr double twopi = 2.0 * 3.14159265358979323846;

    const float aspect_adjust = 1.0f / aspect_ratio;
    const float theta_incr = twopi / c_steps;
    
    obj->newObj(GL_TRIANGLE_FAN);
    obj->addVertex(cvglVertex({{cx, cy, 0.0f},  {0.0f, 0.0f} })); // center point
    for(int i = 0; i <= c_steps; i++)
    {
        float vx = cx + (radius * aspect_adjust * cos(i * theta_incr));
        float vy = cy + (radius * sin(i * theta_incr));
        obj->addVertex(cvglVertex({{vx, vy, 0.0f},  {0.0f, 0.0f} }));
    }
    obj->endObj();
}

/**
 *  initObjects()
 *  init function to setup VBOs for GL objects, must be called after context setup is complete
 */

void basicMainProcess::initObjs()
{

    //glm::vec4 vignette_xyr_aspect(0.5, 0.25, 1, context.getAspectRatio() );
    //glUniform4fv( vignette_attr_idx, 1, &vignette_xyr_aspect[0]);


    
    contourMesh = unique_ptr<cvglObject>(new cvglObject);
    hullMesh = unique_ptr<cvglObject>(new cvglObject);
    minrectMesh = unique_ptr<cvglObject>(new cvglObject);
    flowMesh = unique_ptr<cvglObject>(new cvglObject);
    defect_dots = unique_ptr<cvglObject>(new cvglObject);
    
    frameTex =  unique_ptr<cvglTexture>(new cvglTexture);
    contourTex = unique_ptr<cvglTexture>(new cvglTexture);
    contourTriTex = unique_ptr<cvglTexture>(new cvglTexture);
    hullTex =  unique_ptr<cvglTexture>(new cvglTexture);
    minrectTex =  unique_ptr<cvglTexture>(new cvglTexture);
    prevFrame =  unique_ptr<cvglTexture>(new cvglTexture);
    defect_tex =  unique_ptr<cvglTexture>(new cvglTexture);

    m_hull_rgba = vector<float>({1, 0, 1, 1});
    m_minrect_rgba = vector<float>({1, 1, 1, 0.9});
    m_contour_rgba = vector<float>({0.25, 0.5, 1., 0.125});

    
    rect = unique_ptr<cvglObject>(new cvglObject);
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

    orange_dot_pattern = unique_ptr<cvglObject>(new cvglObject); // do something with mac orange dot?
    orange_dot_tex =  unique_ptr<cvglTexture>(new cvglTexture);
    
    float radius = 0.005;
    float cx = 1 - radius - 0.001;
    float cy = 1 - radius - 0.001;
    
    drawCircle(orange_dot_pattern, cx, cy, radius, context.getAspectRatio());
    orange_dot_pattern->initStaticDraw();
    

    objects_initialized = true;

    context.clearColor(0, 0, 0, 1);
    
    cout << objects_initialized << endl;
}



void basicMainProcess::analysisToGL(const AnalysisData &analysis)
{
    
    
    // lock to prevent conflict with gl thread (locked in processFrame)
    
    // note: analysis only has part of the data when it gets here!! don't try to use the other data

    size_t npoints = 0;
    size_t ndefect_pts = 0;
    for( int i = 0; i < analysis.ncontours; i++ )
    {
        npoints += (analysis.contours[i].cols * analysis.contours[i].rows);
        ndefect_pts += (analysis.defects_vec[i].size() * 21); // circle tri_fan steps + 1 for center
    }
    
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
    
    if( m_draw_defect_dots )
    {
        defect_dots->clear();
        defect_dots->reserve(ndefect_pts);
    }
    
    for( size_t i = 0 ; i < analysis.contour_idx.size(); i++ )
    {
        
        if( m_draw_contour || m_draw_contour_triangles )
        {
            cvgl::pointMatToVertex( analysis.hullP_vec[i] , contourMesh, analysis.halfW, analysis.halfH ); 
            // analysis.contours[ analysis.contour_idx[i] ] // now drawing hull instead of contour
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

        if( m_draw_defect_dots )
        {
            for( size_t j = 0; j < analysis.defects_vec[i].size(); j++ )
            {
                const Vec4i& v = analysis.defects_vec[i][j];
                const float depth = v[3] / 256.;
                if( depth > 5 )
                {
                    const Point2i* ptFar = analysis.contours[analysis.contour_idx[i]].ptr<Point2i>( v[2] );

                    float defect_x = ((float)ptFar->x - analysis.halfW) / analysis.halfW;
                    float defect_y = -((float)ptFar->y - analysis.halfH) / analysis.halfH;
                    drawCircle(defect_dots, defect_x, defect_y, 0.005, context.getAspectRatio());
                }
            }
        }

    }
    
    
}


/**
 *  called from GL thread
 */


void basicMainProcess::draw()
{

    //cout << ">> draw LOCK" << endl;
    // auto start = std::chrono::system_clock::now();

    unique_lock<mutex> lock(m_gl_lock, std::defer_lock); // lock or wait for gl_lock and then lock
    
    //   cout << (start - std::chrono::system_clock::now()).count()  << endl;
    
    if( m_draw_black )
    {
        context.bindDefaultFramebuffer();
        screen_shader.use();
        glClearColor(0.f, 0.f, 0.f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        context.updateViewport(1);
        context.clear();

        context.drawAndPoll();
        m_newframe = false;
        return;
    }
    
    // this can get slowed down if a new frame comes in while the old one is still being drawn?
    if( !lock.try_lock() || !context.isActive() || !objects_initialized || m_img.empty() || !m_newframe ){
        return;
    }
    

    // maybe at end stop clearing
    // or figure out how to copy output image to new texture and
    // make some feedback stuff
    // see: https://learnopengl.com/Advanced-OpenGL/Framebuffers

    glm::mat4 transform = context.getTransform();


    context.bindDefaultFramebuffer();
    context.clear();
    
    screen_shader.use();
    

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

        size_t w = merge.size().width;
        size_t h = merge.size().height;
//        auto mask = UMat::zeros(merge.size(), merge.type());
        cv::rectangle(merge,
                      Point(drawRange_x[1] * w, (1. - drawRange_y[1]) * h),
                      Point(drawRange_x[0] * w, drawRange_y[0] * h), 0, FILLED);
        
//        merge.copyTo(merge, mask);
        
        frameTex->setTexture( merge.getMat(ACCESS_READ) );
    }

    // draw main frame

    if( m_draw_frame )
    {
        rect->bind();
        frameTex->bind();
        rect->draw();
        
        orange_dot_pattern->bind();
        orange_dot_tex->bind();
        orange_dot_tex->setTexture(1, 0, 0, 1);
        orange_dot_pattern->draw();
        
        drawShapes( screen_shader );
    }
    



    // ----------------------------------------------------------------
    // screen framebuffer

    screen_shader.setMat4("transform_matrix", transform);
    screen_shader.setFloat("time", (float)glfwGetTime() );

    screen_shader.setFloat("noise_mix", noise_mix );
    screen_shader.setFloat("noise_mult", noise_mult );
    screen_shader.setVec4("vignette_xyr_aspect", vignette_xyr_aspect);
    screen_shader.setFloat("vignette_fadeSize", vignette_fadeSize);
    screen_shader.setVec2("drawRange_y", {0,1});
    screen_shader.setVec2("drawRange_x", {0,1});

    context.drawAndPoll();

    // context.printFPS();
    
    m_newframe = false;
    
}


void basicMainProcess::drawShapes(cvglShader& shapeRenderShader)
{
    glm::mat4 transform = glm::identity<glm::mat4>(); // not screen version here...

    //glActiveTexture(GL_TEXTURE0);

    if( m_draw_contour )
    {
        contourMesh->bind();
        contourTex->setTexture(m_contour_rgba);//getFrame());
        contourMesh->draw(GL_TRIANGLES);
    }

    if( m_draw_contour_triangles )
    {
        contourMesh->bind();
        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        contourTriTex->setTexture(m_contour_triangles_rgba);
        contourMesh->draw(GL_TRIANGLES);
        //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }

    if( m_draw_hull )
    {
        hullMesh->bind();
        hullTex->setTexture(m_hull_rgba);
        hullMesh->draw(GL_TRIANGLE_STRIP);//vector<int>({GL_TRIANGLES, GL_POINTS}));
    }

    if( m_draw_minrect )
    {
        //   glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        minrectMesh->bind();
        minrectTex->setTexture(m_minrect_rgba);
        minrectMesh->draw(GL_TRIANGLE_STRIP);
    }

    if( m_draw_defect_dots )
    {
        defect_dots->bind();
        defect_tex->setTexture(vector<float>({1,1,0,0.5}));
        defect_dots->draw();
    }

}

