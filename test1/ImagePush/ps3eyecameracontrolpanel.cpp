#include "stdafx.h"

#ifdef USE_PS3CAMERA // See stdafx.h

#include "ps3eyecameracontrolpanel.h"
#include "uic_ps3eyecameracontrolpanel.h"

using namespace sm::faceapi::qt;
using namespace sm::faceapi::samplecode;

PS3EyeCameraControlPanel::PS3EyeCameraControlPanel(PS3EyeCamera *camera, QWidget *parent, Qt::WindowFlags f):
QWidget(parent,f),
_camera(camera),
_rates(camera->possibleFrameRates())
{
    Q_ASSERT(camera);
    setupUi(this);
    setWindowIcon(faceAPIIcon());

    _cam_version_label->setText(toQString(_camera->cameraVersion()));

    _lens_config_cb->insertItem(PS3EyeCamera::LENS_WIDE,"Wide (blue)");
    _lens_config_cb->insertItem(PS3EyeCamera::LENS_ZOOM,"Zoom (red)");

    for (unsigned int i=0; i<_rates.size(); i++)
    {
        _frame_rate_cb->insertItem(i,QString::number(_rates[i]));
    }
    updateControls();

    _led_cb->setChecked(true);

    connect(_lens_config_cb,SIGNAL(currentIndexChanged(int)),this,SLOT(lensConfigChangedSlot(int)));
    connect(_frame_rate_cb,SIGNAL(currentIndexChanged(int)),this,SLOT(frameRateChangedSlot(int)));
}

PS3EyeCameraControlPanel::~PS3EyeCameraControlPanel()
{}

void PS3EyeCameraControlPanel::ledChangedSlot()
{
    _camera->setLED(_led_cb->isChecked());
}

void PS3EyeCameraControlPanel::autoAGCChangedSlot()
{
    if (_camera->autoAGC() != _agc_cb->isChecked())
    {
        _camera->setAutoAGC(_agc_cb->isChecked());
        updateControls();
    }
}

void PS3EyeCameraControlPanel::autoAECChangedSlot()
{
    if (_camera->autoAEC() != _aec_cb->isChecked())
    {
        _camera->setAutoAEC(_aec_cb->isChecked());
        updateControls();
    }
}

void PS3EyeCameraControlPanel::lensConfigChangedSlot(int index)
{
    if (index != (int)_camera->lensConfig())
    {
        QMessageBox::StandardButton result;
        if (index == (int)PS3EyeCamera::LENS_WIDE)
        {
            result = QMessageBox::information(this,"Adjust Lens","Please set the lens dial to the WIDE (blue) setting");
        }
        else
        {
            result = QMessageBox::information(this,"Adjust Lens","Please set the lens dial to the ZOOMED (red) setting");
        }
        if (result == QMessageBox::Ok)
        {
            _camera->setLensConfig((PS3EyeCamera::LensConfig)index);
            emit lensChanged();
        }
    }
}

void PS3EyeCameraControlPanel::frameRateChangedSlot(int index)
{
    if (_rates[index] != _camera->frameRate())
    {
        _camera->setFrameRate(_rates[index]);
        _led_cb->setChecked(true);
        emit frameRateChanged();
    }
}

void PS3EyeCameraControlPanel::setGainChangedSlot(int value)
{
    if (value != _camera->gain())
    {
        _camera->setGain(value);
    }
}

void PS3EyeCameraControlPanel::setExposureChangedSlot(int value)
{
    if (value != _camera->exposure())
    {
        _camera->setExposure(value);
    }
}

void PS3EyeCameraControlPanel::updateControls()
{
    static bool updating = false;
    if (!updating)
    {
        updating = true;
        int frame_rate_index = -1;
        for (unsigned int i=0; (frame_rate_index<0) && (i<_rates.size()); i++)
        {
            if (_rates[i] == _camera->frameRate())
            {
                frame_rate_index = i;
                _frame_rate_cb->setCurrentIndex(i);
            }
        }
        _lens_config_cb->setCurrentIndex((int)_camera->lensConfig());
        _agc_cb->setChecked(_camera->autoAGC());
        _aec_cb->setChecked(_camera->autoAEC());
        _exp_slider->setValue(_camera->exposure());
        _gain_slider->setValue(_camera->gain());
        updating = false;
    }
}

#endif
