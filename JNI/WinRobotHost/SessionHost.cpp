/*! \file $Id: SessionHost.cpp 121 2012-03-26 15:34:59Z caoym $
*  \author caoym
*  \brief CSessionHost
*/
#include "StdAfx.h"
#include "SessionHost.h"
#include "debuglogger.h"
#include "ServiceHost.h"
#include <atlsecurity.h>
#include "debuglogger.h"
CSessionHost::CSessionHost(ULONG id)
{
	m_hParent = (HANDLE)OpenProcess(PROCESS_ALL_ACCESS,FALSE,id);
	if(m_hParent == 0)
	{
		DebugOutF(filelog::log_error, "OpenProcess failed with %d",GetLastError() );
	}
}

CSessionHost::~CSessionHost(void)
{
	if(m_hParent)
	{
		CloseHandle(m_hParent);
	}
}
// run until parent process exit
bool CSessionHost::ThreadLoop()
{
	if(m_hParent == 0) 
	{
		return false;
	}
	CoInitializeEx(0,COINIT_MULTITHREADED);
	{
		CComQIPtr<IWinRobotService>pService;
		HRESULT hr = pService.CoCreateInstance(__uuidof(ServiceHost));
		if ( FAILED(hr) )
		{
			DebugOutF(filelog::log_error,"CoCreateInstance Service failed with 0x%x",hr);
			return false;
		}
		CComPtr<IWinRobotSession>pSession;
		hr = pSession.CoCreateInstance(__uuidof(WinRobotSession));
		if ( FAILED(hr) )
		{
			DebugOutF(filelog::log_error,"CoCreateInstance WinRobotSession failed with 0x%x",hr);
			return false;
		}
		CAccessToken token;
		if(!token.GetProcessToken(TOKEN_ALL_ACCESS))
		{
			DebugOutF(filelog::log_error,"GetProcessToken failed with %d",GetLastError());
			return false;
		}
		DWORD sid = 0;
		if(!token.GetTerminalServicesSessionId(&sid))
		{
			DebugOutF(filelog::log_error,"GetTerminalServicesSessionId failed with %d",GetLastError());
			return false;
		}

		pService->RegSession(sid,GetCurrentProcessId(),pSession);

		WaitForSingleObject(m_hParent,-1);
	}
	//CoUninitialize();
	return false;
}
void CSessionHost::OnExit()
{
	DebugOutF(filelog::log_info,"CSessionHost exit");
}
void CSessionHost::OnBegin()
{
	DebugOutF(filelog::log_info,"CSessionHost begin");
}