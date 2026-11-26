#ifndef SM_API_TESTAPPIMAGEPUSH_PS3EYECAMERA_H
#define SM_API_TESTAPPIMAGEPUSH_PS3EYECAMERA_H

#ifdef USE_PS3CAMERA // See stdafx.h

namespace sm
{
    namespace faceapi
    {
        namespace samplecode
        {
            class PS3EyeImagePushThread;
            class PS3EyeCameraControlPanel;

            class PS3EyeCamera: public ImagePushCamera
            {
            public:
                typedef enum { RES_320x240, RES_640x480 } Resolution;

                // Construct with initial resolution, frame-rate and color.
                // If !color, grayscale images are pushed into the faceAPI which is more
                // efficient for processing, but color will not be visible on the display (cosmetic).
                PS3EyeCamera(Resolution res = RES_640x480, int frame_rate = 60, bool color = false);

                virtual ~PS3EyeCamera();

                static std::vector<int> possibleFrameRates(Resolution res);
                std::vector<int> possibleFrameRates() const;

                virtual bool hasControlPanel() const;
                virtual void showControlPanel() const;

                SMString cameraVersion() const;

                typedef enum LensConfig { LENS_WIDE, LENS_ZOOM };
                void setLensConfig(LensConfig config);
                LensConfig lensConfig() const;

                int frameRate() const;
                // Framerate will be set to closest available given constructed image dimensions.
                virtual void setFrameRate(float frame_rate);

                void setLED(bool on);
                bool autoAGC() const;
            	void setAutoAGC(bool enable);
                bool autoAEC() const;
            	void setAutoAEC(bool enable);

                // Gain: [0 .. 79]
            	int gain() const;
                void setGain(int value);

                // Exposure [0..255]
	            quint8 exposure() const;
	            void setExposure(uint8 value);

                bool autoAWB() const;
	            void setAutoAWB(bool enable);
	            void whiteBalance(uint8 &r, uint8 &g, uint8 &b) const;
	            void setWhiteBalance(uint8 r, uint8 g, uint8 b);

            private:
                Resolution _res;
                IPS3EyeLib *_ps3_eyelib;
                PS3EyeImagePushThread *_push_thread;
                LensConfig _config;
                int _format_index;
                mutable PS3EyeCameraControlPanel *_control_panel;
                QSettings _cam_settings;
            };
        }
    }
}

#endif
#endif
