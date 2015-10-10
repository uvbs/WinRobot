#include "StdAfx.h"
#include "WinSysDLL.h"
#include "TCHAR.h"
//#include "WinAuth.h"
CDLLLib::CDLLLib(LPCTSTR szLib)
:m_hLib(NULL)
{
	m_hLib = LoadLibrary(szLib);
}
CDLLLib::~CDLLLib()
{
	if(m_hLib)
		FreeLibrary(m_hLib);
}
// WinstaDll
WinstaDll::WinstaDll()
	:WinStationConnectFN(0)
	,CDLLLib(_T("winsta.dll"))
{
	WinStationConnectFN	= 
		(WinStationConnect_fn)GetProcAddress(m_hLib,("WinStationConnectW"));
}
BOOL WinstaDll::WinStationConnect(HANDLE hd,ULONG ul1,ULONG ul2,PCWSTR str,ULONG ul3)
{
	if(WinStationConnectFN == NULL )return FALSE;
	return WinStationConnectFN(hd,ul1,ul2,str,ul3);
}
// Kernel32Dll
Kernel32Dll::Kernel32Dll()
:CDLLLib(_T("Kernel32.dll"))
,WTSGetActiveConsoleSessionIdFN(0)
{
	WTSGetActiveConsoleSessionIdFN	= 
		(WTSGetActiveConsoleSessionId_fn)GetProcAddress(m_hLib,("WTSGetActiveConsoleSessionId"));
}
DWORD Kernel32Dll::WTSGetActiveConsoleSessionId()
{
	if(WTSGetActiveConsoleSessionIdFN == NULL )return 0;
	//if(IsWinXP()) return 0;
	return WTSGetActiveConsoleSessionIdFN();
}
// Wtsapi32DLL

Wtsapi32DLL::Wtsapi32DLL()
:CDLLLib(_T("Wtsapi32.dll"))
,WTSQueryUserTokenFN(0)
{
	WTSQueryUserTokenFN =
		(WTSQueryUserToken_fn)GetProcAddress(m_hLib,("WTSQueryUserToken"));
}
BOOL Wtsapi32DLL::WTSQueryUserToken(ULONG SessionId, PHANDLE phToken)
{
	if(WTSQueryUserTokenFN) return WTSQueryUserTokenFN(SessionId,phToken);
	return FALSE;
}

