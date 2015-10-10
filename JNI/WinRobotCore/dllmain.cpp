// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "WinRobotCore_i.h"
#include "dllmain.h"
#include "dlldatax.h"
#include "debuglogger.h"

CWinRobotCoreModule _AtlModule;
HINSTANCE g_hInstance;
// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	g_hInstance = hInstance;
	if (DLL_PROCESS_ATTACH == dwReason)
	{
		InitLogEnv(hInstance);
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
