## About WinRobot

* A powerful screen capturer and Keyboard/mouse input simulator(Support Windows UAC\Winlogon\DirectShowOverlay).
* (Optional)An implementation of java.awt.Robot under windows

## User Manual

### Compiling from source on Windows

> About the files:

    WinRobot\JNI The c++ part of the project
    WinRobot\Java The java part of the project
    
* **Build C++ part:** Add/adjust the following environment variables (via Control Panel/System/Advanced/Environment Variables):set JDKDIR to the path of jdk include,for example "c:\Program Files\Java\jdk1.6.0_22\include"
The c++ part files are provided for Visual Studio 2008.The next step is to open "WinRobot\JNI\WinRobot.sln" and build the solution.The output directory is "WinRobot\bin\debug" and "WinRobot\bin\release".If all succeed,the following targets will be created:WinRobotHost.exe,JNIAdapter.dll,WinRobotCore.dll,WinRobotHook.dll,WinRobotHostPS.dll. 

    > About the output files:

        JNIAdapter*.dll WinRobot JNI library,load by JAVA part.
        WinRobotCore*.dll COM component:WinRobotService and WinRobotSession.
        WinRobotHook*.dll Provide API hook ability,used to hook DirectDraw Overlay API.
        WinRobotHost*.exe the service executable file.
        WinRobotHostPS*.dll Proxy dll for WinRobotHost*.exe,needed by ATL service.


* **Build java part:** make sure you have install Java sdk 1.6+. Use NetBeans to open the project("WinRobot\java\WinRobot").Copy c++ output files(WinRobotHost*.exe,JNIAdapter*.dll,WinRobotCore*.dll,WinRobotHook*.dll,WinRobotHostPS*.dll) to WinRobot\Java\WinRobot\).

### How to use

> C++

    #ifdef _WIN64
    #import "WinRobotCorex64.dll" raw_interfaces_only, raw_native_types,auto_search,no_namespace
    #import "WinRobotHostx64.exe" auto_search,no_namespace
    #else
    #import "WinRobotCorex86.dll" raw_interfaces_only, raw_native_types,auto_search,no_namespace
    #import "WinRobotHostx86.exe" auto_search,no_namespace
    #endif

    CComPtr<IWinRobotService> pService;
    hr = pService.CoCreateInstance(__uuidof(ServiceHost) );

    //get active console session
    CComPtr<IUnknown> pUnk;
    hr = pService->GetActiveConsoleSession(&pUnk);
    CComQIPtr<IWinRobotSession> pSession = pUnk;

    // capture screen
    pUnk = 0;
    hr = pSession->CreateScreenCapture(-1280,0,1280*2,800,&pUnk);

    // get screen image data(with file mapping)
    CComQIPtr<IScreenBufferStream> pBuffer = pUnk;
    CComBSTR name;
    ULONG size = 0;
    pBuffer->get_FileMappingName(&name);
    pBuffer->get_Size(&size);
    CFileMapping fm;
    fm.Open(name,size,false);
    // do something with fm...

    // get screen image data(with stream read)
    CComQIPtr<ISequentialStream> pStream = pUnk;
    char buf[1024];
    ULONG cbBuffer  = 0 ;
    while (pStream->Read(buf,sizeof(buf),&cbBuffer) == S_OK ){
        //	do something
    }

> JAVA

    import com.caoym.WinRobot;
    //...
    WinRobot robot;
    BufferedImage screen = robot.createScreenCapture(new Rectangle(-65535, -65535, 65535 * 2, 65535 * 2));
