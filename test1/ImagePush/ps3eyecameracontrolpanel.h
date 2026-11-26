#ifndef SM_API_TESTAPPIMAGEPUSH_PS3EYECAMERACONTROLPANEL_H
#define SM_API_TESTAPPIMAGEPUSH_PS3EYECAMERACONTROLPANEL_H

#include "uic_ps3eyecameracontrolpanel.h"
#include "ps3eyecamera.h"
#include "stdafx.h"

namespace sm
{
    namespace faceapi
    {
        namespace samplecode
        {
#           ifdef USE_PS3CAMERA // See stdafx.h

            class PS3EyeCameraControlPanel: public QWidget, public Ui::UICPS3EyeCameraControlPanel
            {
                Q_OBJECT
            public:
                PS3EyeCameraControlPanel(PS3EyeCamera *camera, QWidget *parent=0, Qt::WindowFlags = 0);
                virtual ~PS3EyeCameraControlPanel();
            public slots:
                virtual void ledChangedSlot();
                virtual void autoAGCChangedSlot();
                virtual void autoAECChangedSlot();
                virtual void lensConfigChangedSlot(int index);
                virtual void frameRateChangedSlot(int index);
                virtual void setGainChangedSlot(int value);
                virtual void setExposureChangedSlot(int value);
            signals:
                void lensChanged();
                void frameRateChanged();
            private:
                void updateControls();
                PS3EyeCamera *_camera;
                std::vector<int> _rates;
            };

#           endif

            // Dummy class to satisfy MOC when USE_PS3CAMERA is not defined
            class Dummy: public QWidget
            {
                Q_OBJECT
                Dummy(QWidget *parent=0, Qt::WindowFlags f= 0): QWidget(parent,f) {}
            };
        }
    }
}
#endif
