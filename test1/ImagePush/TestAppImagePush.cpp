// TestAppImagePush.cpp : 
// A Qt4 application that uses the PS3 eyetoy camera via the raw memory (image-push) interface.

// Precompiled header file
#include "stdafx.h"

#include "ps3eyecamera.h"
#include "mainwindow.h"

using namespace std;
using namespace sm::faceapi;
using namespace sm::faceapi::qt;
using namespace sm::faceapi::samplecode;

void updateSplash(QSplashScreen &splash, const QString &message)
{
    splash.showMessage(message,Qt::AlignHCenter | Qt::AlignBottom);
    qApp->processEvents();
}

void run()
{
    QSplashScreen splash(QPixmap(":/Pixmaps/splash.png"));
    splash.show();

    // Initialize the faceAPI Qt library
    sm::faceapi::qt::initialize();

    // Create logging window first to catch API startup messages
    LoggingWidget logging_window;

    // Initialize the API
    updateSplash(splash,"Initializing faceAPI");
    APIScope faceapi_scope;

    // Ensure engine and camera are destroyed before the API instance.
    {
        QSharedPointer<CameraBase> camera;
        QSharedPointer<EngineBase> engine;

        // Check license
        if (APIScope::isNonCommercialLicense())
        {
            QMessageBox::information(0,"Information","Non-Commercial License.\n\n"
                                                     "This demo cannot use the PS3 Eyetoy camera low-level driver,"
                                                     "as it requires the image-push camera interface.\n"
                                                     "The demo will run instead with the first detected DirectShow camera");
            // Create head-tracking engine v2 using first detected webcam
            CameraInfo::registerType(SM_API_CAMERA_TYPE_WDM);
            engine = QSharedPointer<HeadTrackerV2>(new HeadTrackerV2());
        }
        else
        {
            updateSplash(splash,"Creating Camera");

#           ifdef USE_PS3CAMERA // See stdafx.h
            const bool use_color = false;
            // See ps3eyecamera.h
            camera = QSharedPointer<PS3EyeCamera>(new PS3EyeCamera(PS3EyeCamera::RES_640x480,60,use_color));
#           else
            CameraInfo::registerType(SM_API_CAMERA_TYPE_WDM);
            CameraInfo::registerType(SM_API_CAMERA_TYPE_PTGREY);
            camera = QSharedPointer<Camera>(new Camera);
#           endif

            updateSplash(splash,"Creating Engine");

            // Create a head-tracking engine that uses the camera
            engine = QSharedPointer<HeadTrackerV2>(new HeadTrackerV2(*camera));
        }

        updateSplash(splash,"Building Widgets");

        // QMainWindow derived class. See mainwindow.h
        MainWindow main_window(camera,engine,&logging_window);

        // Move to top-centre of screen so user looks towards the camera.
        QDesktopWidget desktop;
        main_window.move(desktop.screenGeometry().width()/2-main_window.width()/2,0);

        main_window.show();
        splash.finish(&main_window);
        qApp->exec();
    }
}

// Qt application entry point
int main(int argc, char* argv[])
{
    // Create Qt main application object passing in command-line args
    QApplication app(argc, argv);
    try
    {
        run();
    }
    SM_API_CATCH_AND_SHOW_ERROR(0);
    return 0;
}

