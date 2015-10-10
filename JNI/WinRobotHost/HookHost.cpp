#include "StdAfx.h"
#include "HookHost.h"
#include "debuglogger.h"
#include "../WinRobotHook/WinRobotHook.h"

#ifdef _WIN64
#pragma comment(lib,"WinRobotHookx64.lib")
#else
#pragma comment(lib,"WinRobotHookx86.lib")
#endif

CHookHost::CHookHost(ULONG id)
:m_hParent((HANDLE)id)
{
}

CHookHost::~CHookHost(void)
{
	if(m_hParent)CloseHandle(m_hParent);
}

// we need a message loop to set windows hook
// so we start a thread
bool CHookHost::Run()
{
	
	MSG msg;
	BOOL bRet;
	// we must call PeekMessage , or SetWindowsHookEx will failed
	PeekMessage(&msg, 0, 0, 0,PM_NOREMOVE );
	// install hooks
	if(!InstallHook() )
	{
		DebugOutF(filelog::log_error,"InstallHook failed failed with %d",GetLastError() );
		return false;
	}
	else
	{
		DebugOutF(filelog::log_info,"InstallHook ok");
	}
	// we must keep message loop running ,or WindowsHook will stop.
	while( (bRet = MsgWaitForMultipleObjects( 1, &m_hParent,TRUE, -1,QS_ALLEVENTS )) != 0)
	{ 
		while (PeekMessage(&msg, 0,  0, 0, PM_REMOVE) != 0)
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	}
	// uninstall hooks
	UninstallHook();
	return true;
}