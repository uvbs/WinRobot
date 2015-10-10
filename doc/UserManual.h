/*! \page user_manual User Manual

\section compiling_sec  Compiling from source on Windows
About the files:
- WinRobot\\Java
The java part of the project
- WinRobot\\JNI
The c++ part of the project

First you need to add/adjust the following environment variables (via Control Panel/System/Advanced/Environment Variables):

- set JDKDIR to the path of jdk include,for example "c:\Program Files\Java\jdk1.6.0_22\include"

The c++ part files are provided for Visual Studio 2008.The next step is to open "WinRobot\JNI\WinRobot.sln" and build the solution.The output directory is "WinRobot\bin\debug" and "WinRobot\bin\release".If all succeed,the following targets will be created:WinRobotHost.exe,JNIAdapter.dll,WinRobotCore.dll,WinRobotHook.dll,WinRobotHostPS.dll.
\n
To build the java part,make sure you have install Java sdk 1.6.Use NetBeans to open the project("WinRobot\java\WinRobot").
- Copy c++ output files(WinRobotHost*.exe,JNIAdapter*.dll,WinRobotCore*.dll,WinRobotHook*.dll,WinRobotHostPS*.dll) to WinRobot\\Java\\WinRobot\\).

\n
\section installing_sec Use the binaries
- WinRobot load requested libraries from jar package,when WinRobot class is used,the libraries are exported to the system temp path,then ,for x64 system,the file JNIAdapterx64.dll will be loaded,and for x86 system ,the file JNIAdapterx86.dll will be loaded, All the following files are needed by WinRobot JNI:
-# JNIAdapter*.dll WinRobot JNI library,load by JAVA part .
-# WinRobotCore*.dll COM component:WinRobotService and WinRobotSession.
-# WinRobotHook*.dll Provide API hook ability,used to hook DirectDraw Overlay API.
-# WinRobotHost*.exe the service executable file.
-# WinRobotHostPS*.dll Proxy dll for WinRobotHost*.exe,needed by ATL service.
- If you want to see how WinRobot works,just run WinRobotDemo.jar.
- If you want to use WinRobot in your java project,add WinRobot.jar to your project.
- open "WinRobot\Java\WinRobotDemo\src\WinRobotDemo.java" to see how to use WinRobot.

*/

