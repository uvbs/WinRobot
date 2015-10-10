// WinRobotHook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "WinRobotHook.h"
#include "GDIHook.h"
#include "ShareData.h"
#include "D3D9Hook.h"
#include "debuglogger.h"
#include "DDrawHook.h"
#include "Shlwapi.h"



HMODULE g_hModule = 0;
BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			//DisableThreadLibraryCalls( hModule );
			g_hModule = hModule;
			TCHAR szName[MAX_PATH] = {0};
			GetModuleFileName(hModule,szName,MAX_PATH);
			// do not call OutputDebugString while we are in Dbgview.exe
			if (_tcsicmp(PathFindFileName(szName),_T("Dbgview.exe")) == 0) 
			{
				GetLoggerInstance().Flag(0); 
			}
			else
			{
				GetLoggerInstance().Flag(~(filelog::toFile|filelog::log_debug));// do not output log to file
			}
			//InjectD3D9Hook();
			InjectDDrawHook();
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		
		UninjectDDrawHook();
		break;
	}
	return TRUE;
}



WINROBOTHOOK_API bool InstallHook()
{
	return InstallDDrawHook(g_hModule);
	//InstallD3D9Hook();
	//return InstallGDIHook(g_hModule);
}
WINROBOTHOOK_API bool UninstallHook()
{
	//UninstallD3D9Hook();
	//return UninstallGDIHook();
	return  UninstallDDrawHook();
}
WINROBOTHOOK_API ULONG GetChangesCount()
{
	return GetGDIChangesCount();
}
WINROBOTHOOK_API ULONG GetChanges(RECT*pDest,ULONG size)
{
	return GetGDIChanges(pDest,size);
}

