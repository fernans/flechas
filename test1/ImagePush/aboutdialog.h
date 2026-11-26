#ifndef SM_API_TESTAPPIMAGEPUSH_ABOUTDIALOG_H
#define SM_API_TESTAPPIMAGEPUSH_ABOUTDIALOG_H

#include "uic_aboutdialog.h"

namespace sm
{
    namespace faceapi
    {
        namespace samplecode
        {
            class AboutDialog: public QDialog, public Ui::UICAboutDialog
            {
            public:
                AboutDialog(QWidget *parent=0, Qt::WindowFlags f = 0):
                QDialog(parent,f)
                {
                    setupUi(this);
                    setWindowIcon(sm::faceapi::qt::faceAPIIcon());
                    _faceapi_version_label->setText(qt::faceAPIVersion());
                    _faceapi_licenseinfo_label->setText(qt::faceAPILicenseInfo());
                    _faceapi_icon_label->setPixmap(qt::faceAPIIcon().pixmap(64,64));
                }
            };
        }
    }
}

#endif

