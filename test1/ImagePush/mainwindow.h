#ifndef SM_API_TESTAPPIMAGEPUSH_MAINWINDOW_H
#define SM_API_TESTAPPIMAGEPUSH_MAINWINDOW_H

#include "uic_mainwindow.h"

namespace sm
{
    namespace faceapi
    {
        namespace samplecode
        {
            class PS3EyeCameraControlPanel;

            class MainWindow: public QMainWindow, public Ui::UICMainWindow
            {
                Q_OBJECT
            public:
                MainWindow(QSharedPointer<CameraBase> camera,
                           QSharedPointer<EngineBase> engine, 
                           qt::LoggingWidget *logging_widget = 0, 
                           QWidget *parent = 0, 
                           Qt::WindowFlags f = 0);
                virtual ~MainWindow();
            public slots:
                virtual void restartButtonClicked();
                virtual void actionAboutTriggered();
                virtual void actionCameraTriggered();
                virtual void actionEngineTriggered();
                virtual void actionHeadPoseTriggered();
                virtual void actionLogTriggered();
                virtual void actionNetworkTriggered();
                virtual void actionOptionsTriggered();
                virtual void actionRecordingTriggered();
                virtual void cameraFormatChanged();
            protected:
                void closeEvent(QCloseEvent *event);
            private:
                QSharedPointer<CameraBase> _camera;
                QSharedPointer<EngineBase> _engine;
                qt::LoggingWidget *_logging_widget;
                qt::VideoDisplayWidget *_display;
                PS3EyeCameraControlPanel *_ps3_controls;
                QWidget *_engine_controls;
                QWidget *_network_controls;
                QWidget *_camera_controls;
                QWidget *_recording_controls;
                QWidget *_display_options;
                QWidget *_head_pose_panel;
                QLabel *_status_info_label;
            };
        }
    }
}

#endif

