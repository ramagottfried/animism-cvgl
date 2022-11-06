
#include "basicMainProcess.hpp"
#include "cvglCVCamera.hpp"
#include "cvglDeckLinkCamera.hpp"
#include "cvglVideoPlayer.hpp"

basicMainProcess app;

using namespace std;

void keyboardcallback(GLFWwindow *window, int key, int scanmode, int action, int mods)
{
    if( key == GLFW_KEY_RIGHT && action == GLFW_PRESS )
    {
        MapOSC b;
        string next = app.cues().getNext();
        cout << "right " << next << endl;

        b.addMessage("/cue", next );
        app.receivedBundle(b);
    }
    else if( key == GLFW_KEY_LEFT && action == GLFW_PRESS )
    {
        MapOSC b;
        string prev = app.cues().getPrev();
        cout << "left " << prev << endl;
        b.addMessage("/cue", prev );
        app.receivedBundle(b);
    }
    else if( key == GLFW_KEY_SPACE && action == GLFW_PRESS )
    {
        MapOSC b;
        b.addMessage("/captureFrame", 0 ); // default cam
        app.receivedBundle(b);
    }
}

int main( void )
{

    std::cout << "hardware_concurrency " << std::thread::hardware_concurrency() << std::endl;
    std::string sendIp = "192.168.100.1";

    app.init(8888, 9999, sendIp);

    cvglDeckLinkCamera bm_cam(1);
    cvglDeckLinkCamera bm_cam2(2); // 1 & 3

    cout << "blackmagic " << bm_cam.hasCamera() << " checking cv cam" << endl;
    cvglCVCamera cvcam(0);

    //cvglVideoPlayer cvcam("/home/rama/opencv/doc/js_tutorials/js_assets/cup.mp4");

    if( bm_cam.hasCamera() )
    {
        cout << "found blackmagic 2 " << endl;
        bm_cam.setProcessFrameCallback( [](cv::UMat& mat) { app.processFrame(mat, 1); } );
        app.context.setupWindow( bm_cam.getWidth(), bm_cam.getHeight() );
        bm_cam.start();
    }

    if( bm_cam2.hasCamera() )
    {
       cout << "found blackmagic 4" << endl;
       bm_cam2.setProcessFrameCallback( [](cv::UMat& mat) { app.processFrame(mat, 2); } );
       bm_cam2.start();

       if( !bm_cam.hasCamera() )
       {
           app.context.setupWindow( bm_cam2.getWidth(), bm_cam2.getHeight() );
           app.useCameraID(2);
       }
    }

    if( cvcam.hasCamera() )
    {
        cout << "doing cv camera " << endl;
        cvcam.setProcessFrameCallback( [](cv::UMat& mat){ app.processFrame(mat, 3); } );
        cvcam.start();
    }

    if( !bm_cam.hasCamera() && !bm_cam2.hasCamera() )
    {
        if(cvcam.hasCamera())
        {
            app.context.setupWindow( cvcam.getWidth(), cvcam.getHeight() );
            app.useCameraID(3);
        }
        else
        {
            app.close();
            return -1;
        }

    }


    if( !app.loadShaders() )
    {
        cout << "failed to load shaders" << endl;
        return -1;
    }

  //  app.context.setupWindow( 1920, 1080 );
    /*
    std::string shader_path = "/home/rama/animism-cvgl/src/";

    if( !app.context.loadShaderFiles( shader_path + "vertex.vs", shader_path + "fragment.fs" ) ){
        cout << "failed to load shaders" << endl;
        return -1;
    }
    */
    // start UDP server
    app.start();

    app.initObjs();

    app.initCues();

    cout << "starting draw loop " << endl;

    app.context.flip(0,0);

    glfwSetKeyCallback(app.context.getWindow(), keyboardcallback );

    // main GL loop
    while( !app.context.shouldClose() )
    {
        app.draw();
    }

  //  cvcam.stop();
//    bm_cam.stop();

    app.close();

    return 0;
}


