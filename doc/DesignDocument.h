/*!
\page design_document Design Document

\section class_diagrams_sect Class Diagrams
WinRobot is written in Java and C++,and can be used by both of those projects. The main class of java part is caoym::WinRobot,which inherits from java.awt.Robot but provide more features. And it is a JNI class,most of the functionality are achieved by the JNIAdapter.dll.We named it as "Adapter" because it looks like a "Adapter" ,translates JNI for WinRobot from C++ part interface.The C++ part is provided as COM components(IWinRobotSession,IWinRobotService),implements screen capturing and events inputing(CWinRobotSession,CWinRobotService),and can be used without Java/JDK.\n\n
When JNIAdapter.dll is loaded,it will auto install "WinRobotHost" service(WinRobotHost.exe) if it was not installed,and try to register COM dlls(WinRobotCore.dll,WinRobotHostPS.dll)if necessarily,so the process use WinRobot also need "Administrator" authority.\n\n
For more details,see the following Class Diagrams.
- High-level overview\n
The following diagram shows the high-level overview of WinRobot,there is not the all classes, but the main classes or modules.The arrows "->" means "depend on",for example,A->B means A uses B, or A has a field of type B.The rectangle means there are the C++ part interfaces .
\dot
digraph G {
compound=true;
node [shape=rect];

WinRobot[label = "WinRobot(JAVA)" URL="\ref caoym::WinRobot"];
JNIAdapter[label = "JNIAdapter(c++)" URL="\ref CWinRobotJNIAdapter"];


subgraph cluster0
{
rankdir="RL";
label = "";
WinRobotSession[URL="\ref CWinRobotSession"];
WinRobotService [URL="\ref CWinRobotService"];

WinRobotService->WinRobotSession;

CInputThread [URL="\ref CInputThread"];
CScreenBuffer [URL="\ref CScreenBuffer "];
CCursorBuffer[URL="\ref CCursorBuffer"];
CScreenCapture[URL="\ref CScreenCapture"];
CMouseEvent[URL="\ref CMouseEvent"];
CKeyboardEvent[URL="\ref CKeyboardEvent"];

WinRobotSession->CMouseEvent;
WinRobotSession->CKeyboardEvent;

WinRobotSession->CInputThread;
WinRobotSession->CScreenCapture->CScreenBuffer;
WinRobotSession->CCursorBuffer;
}


WinRobot->JNIAdapter;
JNIAdapter->WinRobotService;
JNIAdapter->WinRobotSession;
}
\enddot

- WinRobot \n
There are java classes and JNI library\n
\image html WinRobot.png
- WinRobotCore\n
About WinRobotService and WinRobotSession\n
\image html WinRobotCore.png
- WinRobotHost\n
See \ref capture_screen_sect .As you know ,we need have a exe to run as Windows Service,and a exe to run on Windows Sessions,and a exe to run API Hook.In face we only have one exe,WinRobotHost.exe ,and use commad to open different feature.\n
\image html WinRobotHost.png

\section sequence_diagrams_sect Sequence Diagrams

\subsection session_sect Get active console session
Any process which has enough authority,can capture images from(or input mouse and keyboard events to) the desktop of current session(for more about Session,see http://www.cppblog.com/dawnbreak/articles/90278.html),but it can't capture images from another session,and can't input events too.So if we want to capture images from another session(for example UAC window or WinLogon window),we must create a child process in the session.To create a child process,the parent process must has "system" authority,so we need a process that run as Windows Service.Any time there is only on session is active as console session(display on local physical monitor),we use WTSGetActiveConsoleSessionId to get it.
\msc

 hscale = "1.5";

 adapter [label="CWinRobotJNIAdapter\n(c++)"],service [label="WinRobotService\n(c++,COM object)"],session [label="WinRobotSession\n(c++,COM object)"];
 adapter=>service [label="GetActiveConsoleSession"];
 |||;
 service=>session [label="if no WinRobotService instance \nexist in active console session ,\nrun WinRobotHost.exe as the host \nof WinRobotSession"];
 |||;
 session=>service [label="RegSession"];
 service>>adapter [label="return a pointer of \nIWinRobotSession"];
 |||;

\endmsc

\subsection capture_screen_sect Capture screen
In a session,there are may several desktops,for example default desktop and Screensaver desktop,if current desktop changed,we need swith the capture and input thread to the new desktop.To switch the desktop,get the current input desktop name and compare with the desktop the call thread attached ,if not equal,then switch to the input desktop.It should be done every time input a event or capture a image.
\n\n
We can get screen image by using GDI API("CreateDC"),but it can't get DirectDraw Overlay image,which's image data is not storaged in the dc memory,but is overlay on screen by video card every time display refreshed.To get the overlay data ,we need hook DirectDraw API,then save the DirectDraw surface when DirectDraw API(IDirectDrawSurface7::UpdateOverlay) is called.\n\n
About "API hook",call SetWindowsHookEx can make all processes in system load a specifies dll(WinRobotHook.dll),then we can do anything in the hooked process.when a DirectDraw application start to display images in the screen,the api IDirectDrawSurface7::UpdateOverlay will be called,and we need to know when IDirectDrawSurface7::UpdateOverlay was called and get the parameters that pass by "UpdateOverlay",to do that ,I find the memory address of the API,inject custom code,so if somewhere call "UpdateOverlay", in face our custom code is called.In addition,under x64 system,64-bits process can't hook a 32-bits process,v.v.,so we need a 32-bits exe to do the hook for 32-bits process.\n\n
To transfer image data from processes,we use FileMapping object to storage image data.
\msc

 hscale = "1.5";

 application,session [label="CWinRobotSession(c++)"],capture [label="CScreenCapture(c++)"],hook [label="WinRobotHook.dll"],app [label="DirectDraw applications"];
 application=>session [label="CreateScreenCapture"];
 session=>session [label="call SwitchInputDesktop\nswitch to input desktop"];
 session=>hook [label="if hook is not install,\ncall InstallHook"];
 hook=>hook [label="\ncall InstallDDrawHook"];
 hook>>session;
 hook->app [label="set hook by call SetWindowsHookEx"];
 app=>app [label="load WinRobotHook.dll"];
 |||;
 app=>hook [label="call InjectDDrawHook,\ninject useful code,\nfor overwrite API call \nand handle CAPTURE_SCREEN message"];
 |||;
 session=>capture [label="call Capture"];
 capture=>capture [label="capture screen by GDI"];
 |||;
 capture=>hook [label="call AppendDDrawOverlay\ncapture DirectDraw overlay image"];
 |||;
 hook=>hook [label="create a named file mapping\n object to storage overlay image"];
 |||;
 hook->app [label="broadcast CAPTURE_SCREEN message"];
 |||;
 app->app [label="save overlay image to\n named file mapping object"];
 |||;
 app>>hook;
 |||;
 hook->hook [label="get overlay image from\n named file mapping object"];
 |||;
 hook>>capture;
 capture>>session;
 session>>application;
 |||;

\endmsc

\subsection input_sect Mouse/Keyboard input
A input thread is only for input events,so we can make sure the events are inputed serially,and can set auto delay.We also need make the thread alway keep on the current input deskop,that means we need do the same thing like \ref capture_screen_sect .
\msc

 hscale = "1.5";

 application,session [label="CWinRobotSession\n(c++)"],thread [label="CInputThread\n(c++)"];
 application=>session [label="input keyboard or mouse event"];
 session=>thread [label="push new event object\n(CMouseEvent/CKeyboardEvent/CSetKeyboardLayoutEvent)"];
 thread>>session;
 session>>application;
 thread=>thread [label="input event(call IInputEvent::Input)"];
 |||;

\endmsc
*/