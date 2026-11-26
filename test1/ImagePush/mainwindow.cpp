#include "stdafx.h"
#include "mainwindow.h"
#include "aboutdialog.h"
#include "ps3eyecameracontrolpanel.h"
#include "ps3eyecamera.h"

#include <sstream>

using namespace sm::faceapi;
using namespace sm::faceapi::qt;
using namespace sm::faceapi::samplecode;

MainWindow::MainWindow(QSharedPointer<CameraBase> camera, 
                       QSharedPointer<EngineBase> engine,
                       qt::LoggingWidget *logging_widget,
                       QWidget *parent, Qt::WindowFlags f):
QMainWindow(parent,f),
_engine(engine),
_camera(camera),
_logging_widget(logging_widget),
_display(0),
_ps3_controls(0),
_engine_controls(0),
_network_controls(0),
_camera_controls(0),
_recording_controls(0),
_display_options(0),
_head_pose_panel(0)
{
    Q_ASSERT(_engine);
    setupUi(this);
    setWindowIcon(faceAPIIcon());
    actionQuit->setShortcut(tr("Ctrl+Q"));
    QObject::connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    actionLog->setEnabled(_logging_widget);
    actionCamera->setEnabled((_camera && _camera->hasControlPanel()) || _engine->hasCameraControlPanel());
    QVBoxLayout *l = new QVBoxLayout(_video_display_frame);
    l->setMargin(0);
    l->setSpacing(0);
    _display = new VideoDisplayWidget(engine,_video_display_frame);
    l->addWidget(_display);

    std::stringstream s;
    s << "Engine: ";
    switch (engine->type())
    {
    case SM_API_ENGINE_TYPE_HEAD_TRACKER_V1:
        s << "HeadTrackerV1"; break;
    case SM_API_ENGINE_TYPE_HEAD_TRACKER_V2:
        s << "HeadTrackerV2"; break;
    }
    if (_camera)
    {
        s << " Camera: " << _camera->videoFormat();
    }
    _status_info_label = new QLabel(QString::fromStdString(s.str()),_status_bar);
    _status_bar->addWidget(_status_info_label);

   _engine->start();
}

MainWindow::~MainWindow()
{}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    QCoreApplication::quit();
}

void MainWindow::actionAboutTriggered() 
{
    AboutDialog about_dialog(this);
    about_dialog.exec();
}

void MainWindow::actionLogTriggered() 
{
    if (_logging_widget)
    {
        _logging_widget->show();
        _logging_widget->raise();
    }
}

void MainWindow::restartButtonClicked() 
{
   _engine->start();
}

void MainWindow::actionCameraTriggered() 
{
    if (!_camera)
    {
        _engine->showCameraControlPanel();
    }
    else
    {
#       ifdef USE_PS3CAMERA // See stdafx.h
        QSharedPointer<PS3EyeCamera> _ps3_camera = qSharedPointerCast<PS3EyeCamera,CameraBase>(_camera);
        if (_ps3_camera)
        {
            if (!_ps3_controls)
            {
                _ps3_controls = new PS3EyeCameraControlPanel(_ps3_camera.data(),this,Qt::Dialog);
                if (!_engine_controls)
                {
                    _engine_controls = new EngineControls(_engine,true,false,this,Qt::Dialog);
                }
                // If the lens changes, we need to update the max/min range sliders
                connect(_ps3_controls,SIGNAL(lensChanged()),_engine_controls,SLOT(engineWasChanged()));
                connect(_ps3_controls,SIGNAL(frameRateChanged()),this,SLOT(cameraFormatChanged()));
            }
            _ps3_controls->show();
            _ps3_controls->raise();
        }
        else
        {
            _camera->showControlPanel();
        }
#       else
        _camera->showControlPanel();
#       endif
    }
}

void MainWindow::actionEngineTriggered() 
{
    if (!_engine_controls)
    {
        _engine_controls = new EngineControls(_engine,true,false,this,Qt::Dialog);
    }
    _engine_controls->show();
    _engine_controls->raise();
}

void MainWindow::actionHeadPoseTriggered() 
{
    if (!_head_pose_panel)
    {
        _head_pose_panel = new HeadPosePanel(qSharedPointerDynamicCast<HeadTracker,EngineBase>(_engine),this,Qt::Dialog);
    }
    _head_pose_panel->show();
    _head_pose_panel->raise();
}

void MainWindow::actionNetworkTriggered() 
{
    if (!_network_controls)
    {
        _network_controls = new NetworkPanel(_engine,this,Qt::Dialog);
    }
    _network_controls->show();
    _network_controls->raise();
}

void MainWindow::actionOptionsTriggered() 
{
    if (!_display_options)
    {
        _display_options = _display->createDisplayOptions(this,Qt::Dialog); 
    }
    _display_options->show();
    _display_options->raise();
}

void MainWindow::actionRecordingTriggered() 
{
    if (!_recording_controls)
    {
        _recording_controls = _display->createRecordingControls(this,Qt::Dialog); 
    }
    _recording_controls->show();
    _recording_controls->raise();
}

void MainWindow::cameraFormatChanged()
{
    if (_camera)
    {
        std::stringstream s;
        s << " Camera: " << _camera->videoFormat();
        _status_info_label->setText(QString::fromStdString(s.str()));
    }
}
