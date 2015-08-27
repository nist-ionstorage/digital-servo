# SuperLaserLand GUI

Ion Storage Group  
Time and Frequency Division  
National Institute of Standards and Technology  
Boulder, CO USA

Authors:  
David Leibrandt (david.leibrandt@nist.gov)

SuperLaserLand is open source software which runs on a PC to control the NIST digital servo.

This directory contains the source code, which was written and compiled with Microsoft Visual Studio 2013, Qt 5.5.0, and Qwt 6.1.2.  It does not contain the library files for the Opal Kelly FrontPanel SDK, which are required to compile the source code.  This code was developed and tested with version 4.0.8 of the FrontPanel SDK.

To compile on Windows:  
1. Download and install Microsoft Visual Studio Community 2013 from https://www.visualstudio.com/downloads/download-visual-studio-vs.  Note that Qt does not supply precompiled binaries compatible with Visual Studio 2015 as of August 2015.  
2. Download and install Qt 5.5.0 for Windows 32-bit VS 2013 from http://www.qt.io/download-open-source/#section-2.  
3. Add the directory that holds qmake.exe to the search PATH.  By default, this is `C:\Qt\Qt5.5.0\5.5\msvc2013\bin`.  
4. Download and unzip Qwt 6.1.2 from http://sourceforge.net/projects/qwt/files
into the `thirdparty\qwt-6.1.2` folder  .
5. Open a Visual Studio command prompt and navigate to the `qwt-6.1.2` folder.  Execute the following commands:  
    ```
    qmake
    nmake
    copy /Y lib\*.dll C:\Qt\Qt5.5.0\5.5\msvc2013\bin
    copy /Y lib\*.lib C:\Qt\Qt5.5.0\5.5\msvc2013\lib
    mkdir  C:\Qt\Qt5.5.0\5.5\msvc2013\include\qwt
    copy /Y src\*.h C:\Qt\Qt5.5.0\5.5\msvc2013\include\qwt
    nmake clean
    ```  
6. Obtain the Opal Kelly library files `okFrontPanelDLL.h` and `okFrontPanel.cpp` (located in the `C:\Program Files\Opal Kelly\FrontPanelUSB\API-32` folder after the FrontPanel SDK has been installed) and copy them to the digital-servo\gui folder.  Copy the file `okFrontPanel.dll` to the folder from which you will run the GUI.  
7. Copy `SuperLaserLand_local_template.pro` to `local.pro` and modify as desired.
8. Execute the `qupdate.bat` script.  
9. Open the `SuperLaserLand.vcxproj` project in Visual Studio.  If you have set `CONFIG = release` in `local.pro`, you'll have to also manually switch the active solution configuration in the BUILD->Configuration Manager menu to release.  Now you should finally be ready to compile!  