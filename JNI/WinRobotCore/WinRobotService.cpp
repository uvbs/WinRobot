/*! \file $Id: WinRobotService.cpp 121 2012-03-26 15:34:59Z caoym $
*  \author caoym
*  \brief CWinRobotService
*/

#include "stdafx.h"
#include "WinRobotService.h"
#include "WinSysDLL.h"
#include "WtsApi32.h" 
// #include "Singleton_T.h"
#include "debuglogger.h"
// CWinRobotService
#pragma comment(lib, "WtsApi32.lib")

HRESULT CWinRobotService::FinalConstruct()
{
	TrackDebugOut;

	m_hNewReg = CreateEvent(0,FALSE,FALSE,0);
	m_hExit = CreateEvent(0,TRUE,FALSE,0);
	return S_OK;
}

void CWinRobotService::FinalRelease()
{
	TrackDebugOut;
	if(m_hExit)
	{
		SetEvent(m_hExit);
	}
	CAutoLockEx<CCrtSection> lock(m_csLock); // lock to make sure GetActiveConsoleSession has return 
	if(m_hNewReg)
	{
		CloseHandle(m_hNewReg);
	}
	if(m_hExit)
	{
		CloseHandle(m_hExit);
	}
}
// We create a child process in each session,so we can get the image of the desktop
// of the session,and input keyboard or mouse events.

STDMETHODIMP CWinRobotService::GetActiveConsoleSession(IUnknown** ppSession)
{
	TrackDebugOut;
	if(ppSession == 0) 
	{
		DebugOutF(filelog::log_error,"GetActiveConsoleSession failed,ppSession == null");
		return E_POINTER;
	}
	*ppSession = 0 ;

	// if has exist,determine whether the process has been closed.
	CAutoLockEx<CCrtSection> lock(m_csLock);
	static Kernel32Dll dll;
	DWORD sid = dll.WTSGetActiveConsoleSessionId();

	{
		CAutoLock<CCrtSection> lock2(m_csLockSessions);
		SESSIONS::iterator it = m_sessions.find(sid);
		if ( it != m_sessions.end() )
		{
			CRefObj< CReference_T<SESSION> >  sn = it->second;
			_ASSERT(sn->pSession);
			if ( sn->IsOK() )
			{
				HRESULT hr = sn->pSession->QueryInterface(__uuidof(IWinRobotSession),(void**)ppSession);
				if(SUCCEEDED(hr))
				{
					DebugOutF(filelog::log_info,"GetActiveConsoleSession %d ok",sid);
				}
				else
				{
					DebugOutF(filelog::log_error,"GetActiveConsoleSession %d failed with 0x%x ",sid,hr);
				}
				return hr;
			}
		}
	}
	// else,create a new child process,and then wait for complete
	
	HANDLE hProcess = CreateChildProcess(sid);
	if(!hProcess)
	{
		return E_UNEXPECTED;
	}
	ResetEvent(m_hNewReg);
	HANDLE hd[] = {m_hExit,m_hNewReg,hProcess};

	HRESULT hr = S_OK;
	/*while (SUCCEEDED(hr))*/
	{
		DWORD res = WaitForMultipleObjects(RTL_NUMBER_OF(hd),hd,FALSE,-1);
		if( (res < WAIT_OBJECT_0) || (res>=res+RTL_NUMBER_OF(hd) ) )
		{
			hr = E_UNEXPECTED;
			DebugOutF(filelog::log_error,"WaitForMultipleObjects failed with %d ",GetLastError() );
			
		}
		else if (hd[res-WAIT_OBJECT_0] == m_hExit)
		{
			DebugOutF(filelog::log_error,"GetActiveConsoleSession failed,CWinRobotService stopped");
			hr = E_UNEXPECTED;
			//break;
		}
		else if(hd[res-WAIT_OBJECT_0] == m_hNewReg)
		{
			CAutoLock<CCrtSection> lock2(m_csLockSessions);
			for (SESSIONS::iterator it = m_sessions.begin();
				it != m_sessions.end();
				it++)
			{
				if(GetProcessId(hProcess) == GetProcessId(it->second->m_hProcess)){
					CRefObj< CReference_T<SESSION> >  sn = it->second;
					_ASSERT(sn->pSession);
					//! this will be a bug !
					// sometimes CreateChildProcess failed on the specified session with error 233,
					// then we CreateChildProcess on session 0 to make sure we can capture something,
					// so the sid will be not the specified session.
					if(it->first != sid){
						m_sessions.erase(it);
						m_sessions[sid] = sn;
					}
					if ( sn->IsOK() )
					{
						hr = sn->pSession->QueryInterface(__uuidof(IWinRobotSession),(void**)ppSession);

						if(SUCCEEDED(hr))
						{
							DebugOutF(filelog::log_info,"GetActiveConsoleSession %d ok",sid);
						}
						else
						{
							DebugOutF(filelog::log_error,"GetActiveConsoleSession %d failed with 0x%x ",sid,hr);
						}
						break;
					}
					else
					{
						DebugOutF(filelog::log_error,"GetActiveConsoleSession failed, process has end");
						hr = E_UNEXPECTED;
						break;
					}
				}
			}
			
		}
		else if(hd[res-WAIT_OBJECT_0] == hProcess)
		{
			DebugOutF(filelog::log_error,"GetActiveConsoleSession failed,process has end");
			hr = E_UNEXPECTED;
			//break;
		}else{
			DebugOutF(filelog::log_error,"GetActiveConsoleSession failed");
			hr = E_UNEXPECTED;
		}
	}
	CloseHandle(hProcess);
	return hr;// should never be here
}


// When the child process begin,it will call RegSession to register its IWinRobotSession
// interface
STDMETHODIMP CWinRobotService::RegSession(ULONG sid, ULONG processid,IUnknown* pUnk)
{
	TrackDebugOut;
	CComQIPtr<IWinRobotSession> pSession = pUnk;
	if(!pSession)
	{
		DebugOutF(filelog::log_error,"RegSession %d failed",sid);
		return E_INVALIDARG;
	}
	HANDLE hd = OpenProcess(PROCESS_ALL_ACCESS,FALSE,processid);
	if (hd == 0)
	{
		DebugOutF(filelog::log_error, "OpenProcess failed with %d",GetLastError() );
		return E_UNEXPECTED;
	}
	DebugOutF(filelog::log_info,"RegSession %d ok",sid);

	CAutoLock<CCrtSection> lock(m_csLockSessions);
	CRefObj< CReference_T<SESSION> > item =  new CReference_T<SESSION>();
	item->pSession = pSession;
	item->m_hProcess = hd;
	m_sessions[sid] = item;
	SetEvent(m_hNewReg);
	return S_OK;
}
template <class T>
BOOL QuerySessionInformationT(DWORD SessionId,WTS_INFO_CLASS WTSInfoClass,T& t)
{
	DWORD buflen = 0;
	LPTSTR	pv = NULL;
	t = T();
	if(!WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,SessionId,WTSInfoClass,&pv,&buflen))
	{
		return FALSE;
	}
	//if(sizeof(T) > buflen) return FALSE;
	t =  *(T*)pv;
	if(pv)WTSFreeMemory(pv);
	return TRUE;
}
HRESULT ConnectActiveSession()
{
	Kernel32Dll kernel32;
	WinstaDll winsta;
	DWORD activexid = kernel32.WTSGetActiveConsoleSessionId();
	INT connect_state;
	if(!QuerySessionInformationT(activexid,WTSConnectState,connect_state))
	{
		DebugOutF(filelog::log_warning,("WTSQuerySessionInformationW %d failed,error code = %d"),activexid,GetLastError());
	}
	else
	{
		if(connect_state == WTSDisconnected /*&&  dwSessionID != activexid*/)
		{

			if(!winsta.WinStationConnect(0,activexid,activexid,L"",TRUE)){
				DebugOutF(filelog::log_error,("Connect station %d to %d failed,error code = %d"),activexid,activexid,GetLastError());
				return E_FAIL;
			}else{
				LockWorkStation();
				DebugOutF(filelog::log_info,("Connect station %d to %d"),activexid,activexid);
			}
		}
	}
	return S_OK;
}
// create the child process with command line "-runas session -parent [parent process id]"
// the child process will run in the specified session
HANDLE CWinRobotService::CreateChildProcess(ULONG sid)
{
	TrackDebugOut;

	WCHAR szComd[MAX_PATH*2];
	WCHAR curpath[MAX_PATH];
	GetModuleFileNameW( 0, curpath, MAX_PATH );
	PathRemoveFileSpec(curpath);
	DWORD dwProcessId = 0;
	HANDLE hNewToken = NULL;
	HRESULT hr = S_OK;
	HWINSTA  hwinsta = NULL;
	HANDLE hToken= NULL;
	void* pEnvBlock = NULL;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi,sizeof(pi));
	DWORD errcode=0;

	try
	{
		DebugOutF(filelog::log_info,"attempt to CreateChildProcess %d",sid);
// 		HANDLE hCurProcess = 0;
// 
// 		DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(), GetCurrentProcess(), &hCurProcess, 
// 			0, TRUE, DUPLICATE_SAME_ACCESS);
#ifdef _WIN64
		swprintf(szComd,L"\"%s\\WinRobotHostx64.exe\" -runas session -parent %d",curpath,GetProcessId(GetCurrentProcess()));
#else
		swprintf(szComd,L"\"%s\\WinRobotHostx86.exe\" -runas session -parent %d",curpath,GetProcessId(GetCurrentProcess()));
#endif
		STARTUPINFOW si;
		ZeroMemory(&si, sizeof(STARTUPINFOW));
		si.cb= sizeof(STARTUPINFOW);

		DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS;

		si.lpDesktop = L"winsta0\\default";;

		INT  *pState = NULL;
		DWORD buflen = 0;
		ConnectActiveSession();

		if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&hToken)){
			DebugOutF(filelog::log_error,"OpenProcessToken failed , error code = %d",GetLastError());
			throw E_UNEXPECTED;
		}
		// Duplicate the token because we need modify it
		if(!DuplicateTokenEx(hToken,MAXIMUM_ALLOWED,0,SecurityAnonymous ,TokenPrimary ,&hNewToken)){
			errcode = GetLastError();
			DebugOutF(filelog::log_error,"DuplicateTokenEx failed , error code = %d",GetLastError());
			throw HRESULT_FROM_WIN32(errcode);
		}
		// change the token so that the process will be create in the new session
		if(!SetTokenInformation(hNewToken,TokenSessionId,&sid,sizeof(DWORD)))
		{
			errcode = GetLastError();
			DebugOutF(filelog::log_error,"SetTokenInformation failed , error code = %d",GetLastError());
			throw HRESULT_FROM_WIN32(errcode);
		}
 		if(!CreateEnvironmentBlock( &pEnvBlock, hToken, TRUE)){
 			DebugOutF(filelog::log_error,"CreateEnvironmentBlock Failed, error code = %d",GetLastError());
 		}else{
 			dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
 		}
		BOOL bCreate = FALSE;
		for(int i=0;i<5;i++)
		{
			if(!(bCreate = CreateProcessAsUserW(hNewToken,
				0,szComd,0,0,FALSE,dwCreationFlags ,pEnvBlock,curpath,&si,&pi )))
			{
				if(GetLastError() == 233){
					// change the token so that the process will be create in the new session
					sid = 0;
					if(!SetTokenInformation(hNewToken,TokenSessionId,&sid,sizeof(DWORD)))
					{
						errcode = GetLastError();
						DebugOutF(filelog::log_error,"SetTokenInformation failed , error code = %d",GetLastError());
						throw HRESULT_FROM_WIN32(errcode);
					}
					continue;
				}
				Sleep(300);
				continue;
			}
			else
			{
				break;
			}
		}
		if(bCreate)
		{
			DebugOutF(filelog::log_info,"CreateChildProcess on session %d ok",sid);
		}
		else
		{
			DebugOutF(filelog::log_error,"CreateProcessAsUser on session %d %s,%s,failed error code = %d"
				,sid,(char*)CW2A(szComd),(char*)CW2A(curpath),GetLastError());
		}
	}
	catch (HRESULT hr1)
	{
		hr = hr1;
	}

	if(hToken) CloseHandle(hToken);
	if(hNewToken)CloseHandle(hNewToken);
	if(pi.hThread){CloseHandle(pi.hThread);pi.hThread = 0;}
	if(pEnvBlock)DestroyEnvironmentBlock(pEnvBlock);
	return pi.hProcess;
}