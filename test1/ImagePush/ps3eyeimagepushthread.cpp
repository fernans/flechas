#include "stdafx.h"

#ifdef USE_PS3CAMERA // See stdafx.h

#include "ps3eyeimagepushthread.h"

#define MAX_POOL_SIZE 20

using namespace sm::faceapi;
using namespace sm::faceapi::samplecode;

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)

/* The following function is used to name a windows thread so it appears 
   in the debugger. */
namespace
{
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType; // must be 0x1000
        LPCSTR szName; // pointer to name (in user addr space)
        DWORD dwThreadID; // thread ID (-1=caller thread)
        DWORD dwFlags; // reserved for future use, must be zero
    }
    THREADNAME_INFO;

    void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
    {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = szThreadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;
        __try
        {
            RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
        }
        __except(EXCEPTION_CONTINUE_EXECUTION)
        {}
    }
}
#endif

PS3EyeImagePushThread::PS3EyeImagePushThread(sm::faceapi::ImagePushCamera *image_push_camera, IPS3EyeLib *ps3_eyelib):
QThread(),
_image_push_camera(image_push_camera),
_ps3_eyelib(ps3_eyelib),
_frame_num(0),
_abort(false)
{
    Q_ASSERT(image_push_camera);
    Q_ASSERT(_ps3_eyelib);

    smCameraVideoFormat video_format = _image_push_camera->videoFormat();
    _bpp = bitsPerPixel(video_format.format);

    // Setup image-information struct
    zeroStruct(_image_info);
    _image_info.format = video_format.format;
    _image_info.res = video_format.res;
    _image_info.step_bytes[0] = _image_info.res.w * _bpp/8; // bytes per scanline

#   ifdef PS3_CAMERA_USE_MEMORY_POOL
    // See freeImageMemory 
    _image_info.user_data = static_cast<void *>(this);
#   endif

    // Size of image memory buffer
    _buffer_bytes = _image_info.step_bytes[0]*_image_info.res.h;

    // Register callback for freeing image memory when it is no longer used by the engine
    smRegisterImageReleaseCallback(PS3EyeImagePushThread::freeImageMemory);
    if (!_ps3_eyelib->IsCapturing())
    {
        _ps3_eyelib->StartCapture();
    }
    // Run the thread
    start();
}

PS3EyeImagePushThread::~PS3EyeImagePushThread()
{
    _abort = true;
    wait();
    if (_ps3_eyelib->IsCapturing())
    {
        _ps3_eyelib->StopCapture();
    }

#   ifdef PS3_CAMERA_USE_MEMORY_POOL
    while (!_memory_pool.isEmpty())
    {
        _aligned_free(_memory_pool.pop());
    }
#   endif
}

void PS3EyeImagePushThread::freeImageMemory(smImageInfo image_info)
{
#   ifdef PS3_CAMERA_USE_MEMORY_POOL
    // This is a static function. 
    // Get pointer to PS3EyeImagePushThread instance from the smImageInfo.user_data.
    PS3EyeImagePushThread *instance = static_cast<PS3EyeImagePushThread *>(image_info.user_data);
    Q_ASSERT(instance);
    // Lock mutex as this function called from internal engine thread(s).
    // We need a separate mutex for the pool to avoid deadlock.
    QMutexLocker lock(&(instance->_pool_mutex));
    // Recycle allocated memory by storing in the pool.
    if (instance->_memory_pool.size() < MAX_POOL_SIZE)
    {
        instance->_memory_pool.push(image_info.plane_addr[0]);
    }
    else
    {
        _aligned_free(image_info.plane_addr[0]);
    }
#   else
    _aligned_free(image_info.plane_addr[0]);
#   endif
}

void PS3EyeImagePushThread::setFormatIndex(int format_index)
{
    QMutexLocker lock(&_mutex);
    if (_ps3_eyelib->IsCapturing())
    {
        _ps3_eyelib->StopCapture();
    }
    _ps3_eyelib->SetFormat(format_index);
    _ps3_eyelib->StartCapture();
}

void PS3EyeImagePushThread::run()
{
#   if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
	SetThreadName(GetCurrentThreadId(),"PS3EyeImagePushThread");
#   endif

    while (true)
    {
        QMutexLocker lock(&_mutex);

        if (_abort || !_ps3_eyelib->IsCapturing())
        {
            qDebug() << "PS3EyeImagePushThread exiting";
            return;
        }

        // Create an image buffer that is 32-byte aligned.
        // This ensures we can share image memory with the tracking engine.
        unsigned char *buffer = 0;

#       ifdef PS3_CAMERA_USE_MEMORY_POOL
        {
            QMutexLocker lock(&_pool_mutex);
            if (!_memory_pool.isEmpty())
            {
                buffer = _memory_pool.pop();
            }
            else
            {
                buffer = (unsigned char *)_aligned_malloc(_buffer_bytes,32);
            }
        }
#       else
        buffer = (unsigned char *)_aligned_malloc(_buffer_bytes,32);
#       endif

        // Copy image from camera into buffer
        const bool flip = false;
        const bool grab_ok = _ps3_eyelib->GetFrame(buffer,_bpp,flip);

        if (grab_ok)
        {
            // Get the real-time clock
            Time grab_time(Time::UTCTime());

            // Set the address of the image memory into the smImageInfo
            _image_info.plane_addr[0] = buffer;

            // Construct a Image from the smImageInfo that shares the buffer
            smImageMemoryCopyMode copy_mode = SM_API_IMAGE_MEMORYCOPYMODE_SHARED;
            Image image(_image_info,copy_mode);
            // Check the copy was a shared-copy. This will be true if _image_info.plane_addr[0] is a 32-byte aligned address.
            Q_ASSERT(copy_mode == SM_API_IMAGE_MEMORYCOPYMODE_SHARED);

            // Construct the next video-frame from the time, frame-num and the image
            smCameraVideoFrame video_frame;
            video_frame.frame_num = _frame_num++;
            video_frame.time = grab_time.smTime();
            video_frame.image_handle = image.handle();

            // Push video-frame into the ImagePushCamera
            if (!_image_push_camera->push(video_frame))
            {
                // Push failed, the engine is not consuming frames as fast as we are producing them.
                msleep(16);
            }
            // Image will be destroyed here, but the engine is now holding onto the image memory.
            // We don't delete the memory until freeImageMemory() (above) is called by the engine.
        }
        else
        {
            // Version 2.1.0.0130 of the PS3 webcam driver sometimes just gives up.
            // It can sometimes be recovered by restarting the capture.
            _ps3_eyelib->StopCapture();
            msleep(16);
            _ps3_eyelib->StartCapture();
        }
    }
}
#endif
