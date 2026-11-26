// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>

#define USE_PS3CAMERA   // Remove this #define if you do not have a PS3 Eyetoy camera.

#ifdef USE_PS3CAMERA
//
// To get the demo working with the PS3 Eyetoy, you will need to download and install PS3Eyelib by
// Alexander Popovich. It works on Windows XP 32-bit, Vista 32 and 64 bit.
//
// You will then need to create an environment variable: SM_API_PS3EYELIB 
// Set this to be the install location of PS3Eyelib.
// On Windows XP and Vista-32 this is normally "C:\Program Files\AlexP", and for Vista-64 it is "C:\Program Files(x86)\AlexP"
//
// This demo was tested with PS3Eyelib V2.1.0.0130 available here: http://www.codelaboratories.com/files/PS3Eye%20Camera%20Setup%20v2.1.0.0130.exe
// See also: http://www.alexpopovich.com/blog/?p=55
//
// After installing it, please use the "PS3Eye Test App" application to verify the camera is working.
//
// Please note, problems installing the camera driver or PS3 camera related issues are not covered by
// faceAPI technical support. This is sample code only.
//
#include <IPS3EyeLib.h>
#endif

// Include faceAPI Qt Widget library, which also depends upon faceAPI C++ wrapper library
// Requires environment variables:
// SM_API_WIDGETS (e.g. C:\Program Files\SeeingMachines\FaceTrackingAPI 3.1\Wrappers\Qt) (set by installer)
// SM_API_CPP_WRAPPERS (e.g. C:\Program Files\SeeingMachines\FaceTrackingAPI 3.1\Wrappers\C++) (set by installer)
// SM_API_QTDIR (set this to the same value as Qt's environment QTDIR) (you will have to set this manually)
#include <sm_api_qt.h>
