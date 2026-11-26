// Precompiled header file
#include "stdafx.h"

#ifdef USE_PS3CAMERA // See stdafx.h

#include "ps3eyecamera.h"
#include "ps3eyeimagepushthread.h"
#include "ps3eyecameracontrolpanel.h"

#include <sstream>
#include <iomanip>

using namespace sm::faceapi;
using namespace sm::faceapi::samplecode;

#define LENS_SETTING "PS3EyeCamera/LensSetting"

namespace
{
    smCameraLensParams wideLensParams()
    {
        smCameraLensParams p;
        smSize2i res;
        res.w = 640;
        res.h = 480;
        smCameraLensParamsFromHFOV(60.0f,res,&p);
        return p;
    }

    smCameraLensParams zoomedLensParams()
    {
        smCameraLensParams p;
        smSize2i res;
        res.w = 640;
        res.h = 480;
        smCameraLensParamsFromHFOV(45.0f,res,&p);
        return p;
    }

    // Format with same image dimensions, closest to frame_rate
    int getFormatIndexForFrameRate(int current_index, int &frame_rate)
    {
        int closest_format_index = current_index;
        PS3EYE_FORMAT fmt = IPS3EyeLib::GetFormats()[current_index];
        const int current_width = fmt.width;
        const int current_height = fmt.height;
        int closest_diff = abs((int)fmt.rate - frame_rate);
        const int num_formats = IPS3EyeLib::GetNumFormats();
        for (int i=0; (closest_diff>0) && (i<num_formats); i++)
        {
            PS3EYE_FORMAT fmt = IPS3EyeLib::GetFormats()[i];
            int framerate_diff = abs((int)fmt.rate - frame_rate);
            if ((fmt.width == current_width) &&
                (fmt.height == current_height) &&
                (framerate_diff < closest_diff))
            {
                closest_format_index = i;
                closest_diff = framerate_diff;
            }
        }
        frame_rate = IPS3EyeLib::GetFormats()[closest_format_index].rate;
        return closest_format_index;
    }

    int getFormatIndex(int width, int height, int &frame_rate)
    {
        const int num_formats = IPS3EyeLib::GetNumFormats();
        for (int i=0; i<num_formats; i++)
        {
            PS3EYE_FORMAT fmt = IPS3EyeLib::GetFormats()[i];
            if ((fmt.width == width) && (fmt.height == height))
            {
                return getFormatIndexForFrameRate(i,frame_rate);
            }
        }
        throw Error("Image dimensions not supported");
    }

    smCameraVideoFormat createFormat(PS3EyeCamera::Resolution r, int frame_rate, int &format_index, bool color)
    {
        smCameraVideoFormat format;
        zeroStruct(format);
        format.format = color ? SM_API_IMAGECODE_BGR_24U : SM_API_IMAGECODE_GRAY_8U; 
        format.is_upside_down = false;
        switch (r)
        {
        case PS3EyeCamera::RES_320x240:
            format.res.w = 320;
            format.res.h = 240;
            format_index = getFormatIndex(320,240,frame_rate);
            format.framerate = frame_rate;
            break;
        case PS3EyeCamera::RES_640x480:
            format.res.w = 640;
            format.res.h = 480;
            format_index = getFormatIndex(640,480,frame_rate);
            format.framerate = frame_rate;
            break;
        default:
            throw Error("Unrecognized resolution");
        }
        return format;
    }
}

std::vector<int> PS3EyeCamera::possibleFrameRates(Resolution res)
{
    std::vector<int> rates;
    
    int width;
    int height;
    switch (res)
    {
    case PS3EyeCamera::RES_320x240:
        width = 320;
        height = 240;
        break;
    case PS3EyeCamera::RES_640x480:
        width = 640;
        height = 480;
        break;
    }
    const int num_formats = IPS3EyeLib::GetNumFormats();
    for (int i=0; i<num_formats; i++)
    {
        PS3EYE_FORMAT fmt = IPS3EyeLib::GetFormats()[i];
        if ((fmt.width == width) && (fmt.height == height))
        {
            rates.push_back(fmt.rate);
        }
    }
    return rates;
}

PS3EyeCamera::PS3EyeCamera(Resolution res, int frame_rate, bool color):
ImagePushCamera(createFormat(res,frame_rate,_format_index,color),wideLensParams()),
_res(res),
_ps3_eyelib(0),
_push_thread(0),
_config(LENS_WIDE),
_control_panel(0),
_cam_settings(QSettings::SystemScope,"SeeingMachines","TestAppImagePush")
{
    try
    {
        _ps3_eyelib = IPS3EyeLib::Create();
        _ps3_eyelib->SetFormat(_format_index);
    }
    catch (...)
    {
        qDebug() << "Unknown error starting PS3Eye camera";
        // Error
        _ps3_eyelib = 0;
    }
    setAutoAGC(true);
    setAutoAEC(false);
    setExposure(255);
    // Load any previous lens setting from registry
    setLensConfig((LensConfig)_cam_settings.value(LENS_SETTING,(int)LENS_WIDE).toInt());
    if (_ps3_eyelib)
    {
        _push_thread = new PS3EyeImagePushThread(this,_ps3_eyelib);
    }
}

PS3EyeCamera::~PS3EyeCamera()
{
    if (_push_thread)
    {
        delete _push_thread;
    }
    if (_ps3_eyelib)
    {
        delete _ps3_eyelib;
    }
    if (_control_panel)
    {
        delete _control_panel;
    }
}

bool PS3EyeCamera::hasControlPanel() const
{
    return true;
}

void PS3EyeCamera::showControlPanel() const
{
    if (!_control_panel)
    {
        _control_panel = new PS3EyeCameraControlPanel(const_cast<PS3EyeCamera *>(this));
    }
    _control_panel->show();
}

SMString PS3EyeCamera::cameraVersion() const
{
    if (_ps3_eyelib)
    {
        return SMString((const char *)_ps3_eyelib->GetCameraVersion());
    }
    return SMString();
}

void PS3EyeCamera::setLensConfig(LensConfig config)
{
    if (config != _config)
    {
        switch (config)
        {
        case LENS_WIDE:
            setLensParams(wideLensParams());
            break;
        case LENS_ZOOM:
            setLensParams(zoomedLensParams());
            break;
        }
        _config = config;
        // Save settings
        _cam_settings.setValue(LENS_SETTING,(int)_config);
    }
}

PS3EyeCamera::LensConfig PS3EyeCamera::lensConfig() const
{
    return _config;
}

int PS3EyeCamera::frameRate() const
{
    if (_ps3_eyelib)
    {
        return _ps3_eyelib->GetRate();
    }
    return 0;
}

std::vector<int> PS3EyeCamera::possibleFrameRates() const
{
    return PS3EyeCamera::possibleFrameRates(_res);
}

void PS3EyeCamera::setFrameRate(float frame_rate)
{
    if (_ps3_eyelib && _push_thread)
    {
        int closest_frame_rate = static_cast<int>(frame_rate);
        int new_format_index = getFormatIndexForFrameRate(_format_index,closest_frame_rate);
        if (new_format_index != _format_index)
        {
            _format_index = new_format_index;
            _push_thread->setFormatIndex(new_format_index);
            ImagePushCamera::setFrameRate(closest_frame_rate);
        }
    }
}

void PS3EyeCamera::setLED(bool on)
{
    if (_ps3_eyelib)
    {
        _ps3_eyelib->LED(on);
    }
}

bool PS3EyeCamera::autoAGC() const
{
    if (_ps3_eyelib)
    {
        return _ps3_eyelib->IsAutoAGC();
    }
    return false;
}

void PS3EyeCamera::setAutoAGC(bool enable)
{
    if (_ps3_eyelib)
    {
        _ps3_eyelib->AutoAGC(enable);
    }
}

int PS3EyeCamera::gain() const
{
    if (_ps3_eyelib)
    {
        return _ps3_eyelib->GetGain();
    }
    return 0;
}

void PS3EyeCamera::setGain(int value)
{
    if (_ps3_eyelib && (value >= 0) && (value <= 79))
    {
        _ps3_eyelib->SetGain(value);
    }
}

bool PS3EyeCamera::autoAEC() const
{
    if (_ps3_eyelib)
    {
        return _ps3_eyelib->IsAutoAEC();
    }
    return false;
}

void PS3EyeCamera::setAutoAEC(bool enable)
{
    if (_ps3_eyelib)
    {
        return _ps3_eyelib->AutoAEC(enable);
    }
}

uint8 PS3EyeCamera::exposure() const
{
    if (_ps3_eyelib)
    {
        return _ps3_eyelib->GetExposure();
    }
    return 0;
}

void PS3EyeCamera::setExposure(uint8 value)
{
    if (_ps3_eyelib)
    {
        _ps3_eyelib->SetExposure(value);
    }
}

bool PS3EyeCamera::autoAWB() const
{
    if (_ps3_eyelib)
    {
        return _ps3_eyelib->IsAutoAWB();
    }
    return false;
}

void PS3EyeCamera::setAutoAWB(bool enable)
{
    if (_ps3_eyelib)
    {
        _ps3_eyelib->AutoAWB(enable);
    }
}

void PS3EyeCamera::whiteBalance(uint8 &r, uint8 &g, uint8 &b) const
{
    if (_ps3_eyelib)
    {
        _ps3_eyelib->GetWhiteBalance(&r,&g,&b);
    }
    else
    {
        r = 0; 
        g = 0; 
        b = 0;
    }
}

void PS3EyeCamera::setWhiteBalance(uint8 r, uint8 g, uint8 b)
{
    if (_ps3_eyelib)
    {
        _ps3_eyelib->SetWhiteBalance(r,g,b);
    }
}

#endif

