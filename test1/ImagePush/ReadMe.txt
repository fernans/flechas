TestAppImagePush
----------------

This sample makes use of the C++ and Qt wrappers for faceAPI, and demonstrates how to 
use the "Image-Push Interface".

The Image-Push Interface is just a way of providing raw image data into faceAPI engines.

The demo uses the freely available custom camera driver for the Sony PS3 Eyetoy and
runs a worker thread to takes images from the driver, and efficiently pass them into the faceAPI.

You can run this sample without the camera, however the Image-Push Interface part of this demo will be inactive.
See below for instructions on how to do this.

Similarly, non-commercial license holders will not be able to use the Image-Push Interface. The sony camera can still
be accessed as a DirectShow camera, although it does not offer the same frame-rate, which is a limitation of the 
DirectShow wrapper written by Alexander Popovich.

Installation
------------

To get the demo working with the PS3 Eyetoy, you will need to download and install PS3Eyelib by
Alexander Popovich. This works on Windows XP 32-bit, Vista 32 and 64 bit.

This demo was tested with V2.1.0.0130 available here: http://www.codelaboratories.com/files/PS3Eye%20Camera%20Setup%20v2.1.0.0130.exe

You may want to check for updates here: http://alexpopovich.wordpress.com/2008/10/02/sony-ps3eye-camera-directshow-capture-source-filter/

After installing it, please use the "PS3Eye Test App" to verify the camera is working first.
Please note, if the camera driver does not install or the camera behaves weirdly on your system then this is not covered by
faceAPI technical support.

You will then need to create an environment variable: SM_API_PS3EYELIB 
Set this to be the install location of PS3Eyelib.
On Windows XP and Vista-32 this is normally "C:\Program Files\AlexP", and for Vista-64 it is "C:\Program Files(x86)\AlexP"

Running with a standard Webcam
------------------------------
Comment out the line "#define USE_PS3CAMERA" in the project's precompiled header file stdafx.h and rebuild.
