#pragma once
typedef BOOL (WINAPI * WinStationConnect_fn) (HANDLE,ULONG,ULONG,PCWSTR,ULONG);
typedef DWORD(WINAPI *WTSGetActiveConsoleSessionId_fn)();
/**
 * Load Windows dll.
 */
class CDLLLib
{
public:
	CDLLLib(LPCTSTR );
	~CDLLLib();
	BOOL LoadLib();
	HMODULE m_hLib;
};
/**
 * Load winsta.dll.
 */
class WinstaDll
	:public CDLLLib
{
public:
	WinstaDll();
	BOOL WinStationConnect(HANDLE hd,ULONG ul1,ULONG ul2,PCWSTR str,ULONG ul3);
private:
	WinStationConnect_fn WinStationConnectFN;
};

/**
 * Load kernel32.dll.
 */
class Kernel32Dll
	:public CDLLLib
{
public:
	Kernel32Dll();
	DWORD WTSGetActiveConsoleSessionId();
private:
	WTSGetActiveConsoleSessionId_fn WTSGetActiveConsoleSessionIdFN;

};
/**
 * Load wtsapi32.dll.
 */
class Wtsapi32DLL
	:public CDLLLib
{
public:
	Wtsapi32DLL();
	BOOL WTSQueryUserToken(ULONG SessionId, PHANDLE phToken);

	typedef BOOL (WINAPI *WTSQueryUserToken_fn)(ULONG SessionId, PHANDLE phToken);
	WTSQueryUserToken_fn WTSQueryUserTokenFN;
};
