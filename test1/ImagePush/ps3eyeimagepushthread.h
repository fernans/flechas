#ifndef SM_API_TESTAPPIMAGEPUSH_IMAGEPUSHTHREAD_H
#define SM_API_TESTAPPIMAGEPUSH_IMAGEPUSHTHREAD_H

#ifdef USE_PS3CAMERA // See stdafx.h

// This option reduces the frequency of calls to malloc/free
#define PS3_CAMERA_USE_MEMORY_POOL

namespace sm
{
    namespace faceapi
    {
        namespace samplecode
        {
            class PS3EyeImagePushThread: private QThread
            {
            public:
                // Starts the thread immediately
                PS3EyeImagePushThread(sm::faceapi::ImagePushCamera *image_push_camera, IPS3EyeLib *ps3_eyelib);
                // Destructor waits for the thread to stop
                virtual ~PS3EyeImagePushThread();
                // Camera is stopped, format changed, then camera restarted.
                // Do not change to a format with different image dimensions. This can only be used for fps adjustments.
                void setFormatIndex(int format_index);
            private:
                friend class Lock;
                void run();
                // Callback function
                static void STDCALL freeImageMemory(smImageInfo);
            private:
                sm::faceapi::ImagePushCamera *_image_push_camera;
                IPS3EyeLib *_ps3_eyelib;
                smImageInfo _image_info;
                int _bpp;
                int _buffer_bytes;
                int _frame_num;
                bool _abort;
                QMutex _mutex;
#               ifdef PS3_CAMERA_USE_MEMORY_POOL
                QStack<unsigned char *> _memory_pool;
                QMutex _pool_mutex;
#               endif
            };
        }
    }
}
#endif
#endif
