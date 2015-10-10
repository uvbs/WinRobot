// WinRobotHost.cpp : WinMain


#include "stdafx.h"
#include "resource.h"
#include "WinRobotHost_i.h"
#include "SessionHost.h"
#include "HookHost.h"
#include <stdio.h>
#include "debuglogger.h"
#include "dcomperm/dcomperm.h"
class CWinRobotHostModule : public CAtlServiceModuleT< CWinRobotHostModule, IDS_SERVICENAME >
{
public :
	CWinRobotHostModule()
	{
		m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS;
	}
	DECLARE_LIBID(LIBID_WinRobotHostLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WINROBOTHOST, "{387045EA-B668-4411-90B3-B212178E11C4}")
	HRESULT InitializeSecurity() throw()
	{
		return CoInitializeSecurity(
			NULL, -1, NULL, NULL,
			RPC_C_AUTHN_LEVEL_NONE,
			RPC_C_IMP_LEVEL_DELEGATE ,
			NULL,
			EOAC_NONE,
			NULL);
	}
	//! replace %APP_NAME% to module file name
  	virtual HRESULT AddCommonRGSReplacements(IRegistrarBase* pRegistrar) throw()
  	{
  		TCHAR szFilePath[MAX_PATH];
  		TCHAR szBuffer[MAX_PATH];
  		LPTSTR lpFilePart = NULL;
  		::GetModuleFileName(0, szFilePath, MAX_PATH);
  		::GetFullPathName(szFilePath,MAX_PATH,szBuffer,&lpFilePart);
  		if( lpFilePart == NULL ) return E_UNEXPECTED;
  
  		HRESULT hr = pRegistrar->AddReplacement(L"APP_NAME",lpFilePart );
  		if( FAILED(hr) ) return hr;
  		return CAtlServiceModuleT< CWinRobotHostModule, IDS_SERVICENAME >::AddCommonRGSReplacements(pRegistrar);
  	}
	//! parse command line
	/*!
		install service		-i 
		uninstall service	-u 
		run as service		-runas service
		run as session		-runas session -parent n
		check and install	-c
		run hook			-runas hook -parent n
	*/
	bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw()
	{
		if (!CAtlServiceModuleT< CWinRobotHostModule, IDS_SERVICENAME >::ParseCommandLine(lpCmdLine, pnRetCode))
			return false;

		TCHAR szTokens[] = _T("-/");
		*pnRetCode = S_OK;

		LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
		while (lpszToken != NULL)
		{
			if (WordCmpI(lpszToken, _T("I"))==0) // install
			{
				*pnRetCode = RegisterAppId(true);
				if (SUCCEEDED(*pnRetCode))
					*pnRetCode = RegisterServer(TRUE);
				return false;
			}
			else if (WordCmpI(lpszToken, _T("U"))==0)// uninstall
			{
				*pnRetCode = UnregisterServer(TRUE);
				if (SUCCEEDED(*pnRetCode))
					*pnRetCode = UnregisterAppId();
				return false;
			}
			else if (WordCmpI(lpszToken, _T("runas"))==0)
			{
				lpszToken = FindOneOf(lpszToken, _T(" "));
				if (WordCmpI(lpszToken, _T("service")) == 0) // run as service
				{
					return true;
				}
				else if(WordCmpI(lpszToken, _T("session")) == 0) // run as session
				{
					lpszToken = FindOneOf(lpszToken, _T("-/"));
					if(WordCmpI(lpszToken, _T("parent")) == 0)
					{
						lpszToken = FindOneOf(lpszToken, _T(" "));
						CSessionHost host(_tcstoul(lpszToken,0,10));
						host.Run();
						host.WaitThreadEnd();
					}
				}
				else if(WordCmpI(lpszToken, _T("hook")) == 0) // run as session
				{
					lpszToken = FindOneOf(lpszToken, _T("-/"));
					if(WordCmpI(lpszToken, _T("parent")) == 0)
					{
						lpszToken = FindOneOf(lpszToken, _T(" "));
						CHookHost host(_tcstoul(lpszToken,0,10));
						host.Run();
					}
				}
				return false;
			}
			lpszToken = FindOneOf(lpszToken, szTokens);
		}
		return true;
	}
	bool RegisterComDLL(HINSTANCE hin,LPCTSTR dll)
	{
		TCHAR curpath[MAX_PATH];
		GetModuleFileName( hin, curpath, MAX_PATH );
		PathRemoveFileSpec(curpath);
		ATL::CString file;
		file += "regsvr32 \"";
		file += curpath;
		file += "\\";
		file += dll;
		file += "\" /s";
		STARTUPINFO si={0};
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi={0};
		if(!CreateProcess( NULL,(LPWSTR)file.GetString(),  NULL,NULL,FALSE,0, NULL,NULL,&si, &pi ))
		{
			DebugOutF(filelog::log_error,"CreateProcess failed with %d",GetLastError() );
			return false;
		}
		WaitForSingleObject(pi.hProcess,-1);
		DWORD exitcode = 0;
		GetExitCodeProcess(pi.hProcess,&exitcode);
		if(pi.hProcess)CloseHandle(pi.hProcess);
		if(pi.hThread)CloseHandle(pi.hThread);
		return exitcode == 0;
	}
	inline HRESULT RegisterAppId(bool bService = false) throw()
	{
		if (!Uninstall())
			return E_FAIL;

		HRESULT hr = UpdateRegistryAppId(TRUE);
		if (FAILED(hr))
			return hr;

		CRegKey keyAppID;
		LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
		if (lRes != ERROR_SUCCESS)
			return AtlHresultFromWin32(lRes);

		CRegKey key;

		lRes = key.Create(keyAppID, GetAppIdT());
		if (lRes != ERROR_SUCCESS)
			return AtlHresultFromWin32(lRes);

		key.DeleteValue(_T("LocalService"));

		if (!bService)
			return S_OK;

		key.SetStringValue(_T("LocalService"), m_szServiceName);

		// change LaunchACL and AccessACL,so that no "Administrator" privileges is need.
		//S-1-5-32-545 is USERS's SID
		DWORD error = ChangeAppIDLaunchACL(GetAppIdT(),_T("S-1-5-32-545"),true,true,COM_RIGHTS_ACTIVATE_LOCAL);
		if(error){
			DebugOutF(filelog::log_error,"ChangeAppIDLaunchACL failed with %d",error);
		}
		error = ChangeAppIDAccessACL(GetAppIdT(),_T("S-1-5-32-545"),true,true,COM_RIGHTS_EXECUTE_LOCAL);
		
		if(error){
			DebugOutF(filelog::log_error,"ChangeAppIDAccessACL failed with %d",error);
		}

		// Create service
		if (!Install())
			return E_FAIL;
		return S_OK;
	}
	BOOL Install() throw()
	{
		if (IsInstalled())
			return TRUE;

		//step1.register dlls:
		//WinRobotCore.dll,WinRobotHostPS.dll
#if _WIN64
		if(!RegisterComDLL(0,_T("WinRobotCorex64.dll")))
#else
		if(!RegisterComDLL(0,_T("WinRobotCorex86.dll")))
#endif
		{
			DebugOutF(filelog::log_info,"RegisterComDLL WinRobotCore failed");
			return false;
		}
#if _WIN64
		if(!RegisterComDLL(0,_T("WinRobotHostPSx64.dll")))
#else
		if(!RegisterComDLL(0,_T("WinRobotHostPSx86.dll")))
#endif
		{
			DebugOutF(filelog::log_info,"RegisterComDLL WinRobotHostPS failed");
			return false;
		}
		// Get the executable file path
		TCHAR szFilePath[MAX_PATH + _ATL_QUOTES_SPACE];
		DWORD dwFLen = ::GetModuleFileName(NULL, szFilePath + 1, MAX_PATH);
		if( dwFLen == 0 || dwFLen == MAX_PATH )
			return FALSE;

		// Quote the FilePath before calling CreateService
		szFilePath[0] = _T('\"');
		szFilePath[dwFLen + 1] = _T('\"');
		szFilePath[dwFLen + 2] = 0;

		SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (hSCM == NULL)
		{
			TCHAR szBuf[1024];
			if (AtlLoadString(ATL_SERVICE_MANAGER_OPEN_ERROR, szBuf, 1024) == 0)
#ifdef UNICODE
				Checked::wcscpy_s(szBuf, _countof(szBuf), _T("Could not open Service Manager"));
#else
				Checked::strcpy_s(szBuf, _countof(szBuf), _T("Could not open Service Manager"));
#endif
			MessageBox(NULL, szBuf, m_szServiceName, MB_OK);
			return FALSE;
		}
		_tcscat(szFilePath,_T(" -runas service"));
		SC_HANDLE hService = ::CreateService(
			hSCM, m_szServiceName, m_szServiceName,
			SERVICE_ALL_ACCESS, 	m_status.dwServiceType,
			SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
			szFilePath, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

		if (hService == NULL)
		{
			::CloseServiceHandle(hSCM);
			TCHAR szBuf[1024];
			if (AtlLoadString(ATL_SERVICE_START_ERROR, szBuf, 1024) == 0)
#ifdef UNICODE
				Checked::wcscpy_s(szBuf, _countof(szBuf), _T("Could not start service"));
#else
				Checked::strcpy_s(szBuf, _countof(szBuf), _T("Could not start service"));
#endif
			MessageBox(NULL, szBuf, m_szServiceName, MB_OK);
			return FALSE;
		}

		::CloseServiceHandle(hService);
		::CloseServiceHandle(hSCM);
		return TRUE;
	}
	HRESULT PreMessageLoop(int nShowCmd) throw()
	{
		
		HRESULT hr = S_OK;
		hr = CAtlServiceModuleT< CWinRobotHostModule, IDS_SERVICENAME >::PreMessageLoop(nShowCmd);
		if(FAILED(hr)) return hr;
		//if(m_pServiceHost) return S_OK;
		//return m_pServiceHost.CoCreateInstance(__uuidof(ServiceHost));
		return S_OK;
	}
	HRESULT PostMessageLoop() throw()
	{
		return CAtlServiceModuleT< CWinRobotHostModule, IDS_SERVICENAME >::PostMessageLoop();
	}

	//CComPtr<IServiceHost>m_pServiceHost;
};

CWinRobotHostModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	InitLogEnv(0);
    return _AtlModule.WinMain(nShowCmd);
}

