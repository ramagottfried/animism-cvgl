
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
            m_draw_contour = val.getInt() > 0;
        }
        else if( addr == "/contour/color" )
        {
            m_contour_rgba = cvgl::getRGBA(const_cast<OSCAtomVector &>(val));
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
        if( addr == "/video/flip" )
        {
            if( val.size() == 2 )
            {
                context.flip( val.get<int>(0), val.get<int>(1) );
            }
        }
    }
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

void setTriangleTexcords(unique_ptr<cvglObject> & obj )
{
    for( size_t i = 0; i < obj->getSize(); i++  )
    {
        cvglVertex vert = obj->getVertex(i);

        obj->setTexCord(i, cvgl::scale( vert.position[0], -1., 1., 0., 1.), cvgl::scale( vert.position[1], -1., 1., 0., 1.));
    }
}

void cvglMainProcess::setVignette(float x, float y, float r)
{
    if( r > 0 )
    {
        vignette_xyr_aspect = glm::vec4(1 - x, y / context.getAspectRatio(), r / context.getAspectRatio(), context.getAspectRatio() );
    }
}

/**
 *  initObjects()
 *  init function to setup VBOs for GL objects, must be called after context setup is complete
 */

void cvglMainProcess::initObjs()
{

    vignette_attr_idx = context.getShaderAttrLocation("vignette_xyr_aspect");

    cout << "found attr vignette " << vignette_attr_idx << endl;

    setVignette(0.5, 0.5, 1);

    //glm::vec4 vignette_xyr_aspect(0.5, 0.25, 1, context.getAspectRatio() );
    //glUniform4fv( vignette_attr_idx, 1, &vignette_xyr_aspect[0]);


    rect = unique_ptr<cvglObject>(new cvglObject);
    contourMesh = unique_ptr<cvglObject>(new cvglObject);
    hullMesh = unique_ptr<cvglObject>(new cvglObject);
    minrectMesh = unique_ptr<cvglObject>(new cvglObject);
    flowMesh = unique_ptr<cvglObject>(new cvglObject);
    gitchRect = unique_ptr<cvglObject>(new cvglObject);

    frameTex =  unique_ptr<cvglTexture>(new cvglTexture);
    contourTex = unique_ptr<cvglTexture>(new cvglTexture);
    contourTriTex = unique_ptr<cvglTexture>(new cvglTexture);
    hullTex =  unique_ptr<cvglTexture>(new cvglTexture);
    minrectTex =  unique_ptr<cvglTexture>(new cvglTexture);

    m_hull_rgba = vector<float>({1, 0, 1, 1});
    m_minrect_rgba = vector<float>({1, 1, 1, 0.9});
    m_contour_rgba = vector<float>({0.25, 0.5, 1., 0.125});
    m_contour_triangles_rgb = vector<float>({1, 1, 1, 0.9});


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


    gitchRect->newObj(GL_TRIANGLES);
    gitchRect->addVertex(cvglVertex({{x[0], y[0], 0.0f},  {0.0f, 0.0f} }));
    gitchRect->addVertex(cvglVertex({{x[1], y[1], 0.0f},  {1.0f, 0.0f} }));
    gitchRect->addVertex(cvglVertex({{x[2], y[2], 0.0f},  {1.0f, 1.0f} }));

    gitchRect->addVertex(cvglVertex({{x[2], y[2], 0.0f},  {1.0f, 1.0f} }));
    gitchRect->addVertex(cvglVertex({{x[3], y[3], 0.0f},  {0.0f, 1.0f} }));
    gitchRect->addVertex(cvglVertex({{x[0], y[0], 0.0f},  {0.0f, 0.0f} }));

    cvglRandom rand;
    float xrange = 5;
    float yrange = 0.25;
    for(int i = 0 ; i < xrange; i++)
    {
        float minxrange =  i == 0 ? 0 : i / xrange;
        float maxxrange =  ((i + 1) / xrange) ;

        float rx1 = cvgl::scale( rand.uniformRand(), 0., 1., minxrange, maxxrange);
        float ry1 = cvgl::scale( rand.uniformRand(), 0., 1., 1 - yrange, 1.);

        float minx = rx1;
        float miny = ry1;
        float maxx = rx1;
        float maxy = ry1;

        float rx2 = cvgl::scale( rand.uniformRand(), 0., 1., minxrange, maxxrange);
        float ry2 = cvgl::scale( rand.uniformRand(), 0., 1., 0., yrange);

        minx = rx2 < minx ? rx2 : minx;
        miny = ry2 < miny ? ry2 : miny;
        maxx = rx2 > minx ? rx2 : maxx;
        maxy = ry2 > miny ? ry2 : maxy;

        float rx3 = cvgl::scale( rand.uniformRand(), 0., 1., minxrange, maxxrange);
        float ry3 = cvgl::scale( rand.uniformRand(), 0., 1., 1 - yrange, 1.);

        minx = rx3 < minx ? rx3 : minx;
        miny = ry3 < miny ? ry3 : miny;
        maxx = rx3 > minx ? rx3 : maxx;
        maxy = ry3 > miny ? ry3 : maxy;

        float w = maxx - minx;
        float h = maxy - miny;

        float xx = cvgl::scale( rx1, 0., 1., -1., 1);
        float yy = cvgl::scale( ry1, 0., 1., -1., 1);
        gitchRect->addVertex(cvglVertex({{xx, yy, 0.0f}, {rx1, ry1}}));

        xx = cvgl::scale( rx2, 0., 1., -1., 1);
        yy = cvgl::scale( ry2, 0., 1., -1., 1);
        gitchRect->addVertex(cvglVertex({{xx, yy, 0.0f},  {rx2, ry2}}));

        xx = cvgl::scale( rx3, 0., 1., -1., 1);
        yy = cvgl::scale( ry3, 0., 1., -1., 1);
        gitchRect->addVertex(cvglVertex({{xx, yy, 0.0f},  {rx3, ry3}}));

    }
    gitchRect->endObj();

//    gitchRect->triangulate();
    gitchRect->initStaticDraw();


    objects_initialized = true;

    context.clearColor(0, 0, 0, 1);

    cout << objects_initialized << endl;
}


void cvglMainProcess::analysisToGL(const AnalysisData &analysis)
{
    
    
    // lock to prevent conflict with gl thread (locked in processFrame)
    
    size_t npoints = 0;
    for( auto& c : analysis.contours )
        npoints += (c.cols * c.rows);
    
    if( m_draw_contour ){
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
        
        if( m_draw_contour )
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
    
   // context.set_time_uniform( (float)glfwGetTime() );
    context.clear();
    
    context.updateViewport();


    if( m_draw_black )
    {
        context.clearColor(0, 0, 0, 1);
        context.drawAndPoll();
        m_newframe = false;
        return;
    }

    glUniform4fv( vignette_attr_idx, 1, &vignette_xyr_aspect[0]);

    if( m_draw_frame && m_use_camera_id > 0 )
    {
        rect->bind();
        UMat merge = getFrame();

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
            //UMat frame2, frame1;
            //cout << "draw " << m_overlap_cameras << endl;


            cv::addWeighted(frames[1], 1.0-m_overlap_cameras, frames[2], m_overlap_cameras, 0.0, merge);

        }


        frameTex->setTexture( merge.getMat(ACCESS_READ) );
        rect->draw();
    }
    
    if( m_draw_contour )
    {
        contourMesh->bind();
        contourTex->setTexture(m_contour_rgba);//getFrame());
        contourMesh->draw(GL_TRIANGLES);
    }
    
    if( m_draw_contour_triangles )
    {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        contourTriTex->setTexture(m_contour_triangles_rgb);
        contourMesh->draw(GL_TRIANGLES);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
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
    
    context.drawAndPoll();
    
    // context.printFPS();
    
    //m_use_camera_id = m_use_camera_id == 1 ? 2 : 1;

    m_newframe = false;
    
}





// multi-block thread lock version, moving back to longer gl block
// so that it's easier to draw to the image from the cv processing

/*
void cvglMainProcess::processFrame(cv::UMat & frame, int camera_id )
{
    if( m_use_camera_id == camera_id )
    {

        // this first lock is for the gl thread, and here is were we need to perform any
        // operations that should be displayed, it's separated from the analysis thread
        // in order to speed up the display part a bit
       {
            unique_lock<mutex> lock(m_gl_lock);

            if( frame.empty() || !objects_initialized )
                return;

            m_newframe = true;
            setFrame(frame); // takes ownership of frame in local storage m_img

            // note that any graphics processing to display needs to happen here,
            // otherwise, the frame is not redrawn after releasing the gl lock
            switch(m_use_preprocess) {
                case 3:
                   // getFlow();
                break;
            default:
                break;
            }


        }


        // second block is for the analysis data, preprocessing based on the settings that
        // have been set from the OSC input, and so there is a lock for the udp/osc thread

        AnalysisData data;

        {
            unique_lock<mutex> lock_osc(m_osc_lock);
          // cout << " preprocessing mode " << m_use_preprocess << endl;

           // profile.markStart();
            // pre-processes inherited from cvglCV
            switch (m_use_preprocess) {
                case 0:
                    preprocess();
                    break;
                case 1:
                    preprocessDifference();
                    break;
                case 2:
                    preprocessCanny();
                    break;
                case 3:
                    preprocessDenseFlow();
                  //  m_contour_analysis = false;
                default:
                    break;
            }

            if( m_contour_analysis ){
                data = analyzeContour();
                m_data = data;
            }
        }
        //    cvx.getFlow( flowMesh );
       // profile.markEnd("preproc");

        //profile.markStart();

        //profile.markEnd("analyzeContour");


        // finally we add the analysis to the gl objects to draw, after the analysis
        // this is not part of the first part, because if the analysis is too slow,
        // then this should hypothetically make it possible to draw the video part first,
        // and maybe the next frame might have the objects from the previous frame, but
        // but that is ok, better than blocking the gl thread for the analysis

        {
            unique_lock<mutex> lock(m_gl_lock);
            analysisToGL( data );
        }

    }

}
*/
