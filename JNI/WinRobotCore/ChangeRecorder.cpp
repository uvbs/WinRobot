/*! \file $Id: ChangeRecorder.cpp 95 2011-10-11 16:06:14Z caoym $
*  \author caoyangmin
*  \brief CChangeRecorder
*/
#include "StdAfx.h"
#include "ChangeRecorder.h"
#include "debuglogger.h"
#include "WinRobotHook.h"

#ifdef _WIN64
#pragma comment(lib,"WinRobotHookx64.lib")
#else
#pragma comment(lib,"WinRobotHookx86.lib")
#endif


CChangeRecorder::CChangeRecorder(void)
{
	m_hBegin = CreateEvent(0,TRUE,FALSE,0);
}

CChangeRecorder::~CChangeRecorder(void)
{
	Stop();
	if(m_hBegin) CloseHandle(m_hBegin);
}
bool CChangeRecorder::Run()
{
	ResetEvent(m_hBegin);
	if(!CBaseThread::Run())
	{
		return false;
	}
	return WaitForBegin();
}
bool CChangeRecorder::Stop()
{
	if(IsRunning())
	{
		// post WM_QUIT to make thread exit.
		PostThreadMessage(m_dwThreadId,WM_QUIT,0,0);
	}
	return CBaseThread::Stop();
}
// we need a message loop to set windows hook
// so we start a thread
bool CChangeRecorder::ThreadLoop()
{

	MSG msg;
	BOOL bRet;
	// we must call PeekMessage , or SetWindowsHookEx will failed
	PeekMessage(&msg, 0, 0, 0,PM_NOREMOVE );

	// install hooks
	if(!InstallHook() )
	{
		DebugOutF(filelog::log_error,"InstallHook failed failed with %d",GetLastError() );
		SetEvent(m_hBegin);
		return false;
	}
	else
	{
		SetEvent(m_hBegin);
		DebugOutF(filelog::log_info,"InstallHook ok");
	}

	// if in x64,run a process to inject dll to x86 process
#ifdef _WIN64
	PROCESS_INFORMATION pi;
	if(!RunHookProcess(pi))
	{
		DebugOutF(filelog::log_error,"RunHookProcess failed");
	}
	
#endif
	// we must keep message loop running ,or WindowsHook will stop.
	while( (bRet = GetMessage( &msg, 0, 0, 0 )) != 0)
	{ 
		if (bRet != -1)
		{
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	}
	// uninstall hooks
	UninstallHook();
#ifdef _WIN64
	if (pi.dwThreadId)
	{
		PostThreadMessage(pi.dwThreadId,WM_QUIT,0,0);
	}
	if(pi.hThread)
	{
		CloseHandle(pi.hThread);
	}
	 
	if(pi.hProcess)
	{
		TerminateProcess(pi.hProcess,-1);
		CloseHandle(pi.hProcess);
	}
#endif
	return false;
}
bool CChangeRecorder::GetChangeList(CHANGES &changes)
{
	
	// get changes first
	ULONG count = ::GetChangesCount();

	RECT *pChanges = new RECT[count];
	// get changes list
	count = GetChanges(pChanges,count);

	for (ULONG i = 0; i !=count; i++)
	{
		changes.push_back(pChanges[i]);
	}
	delete []pChanges;
	return true;
}
bool CChangeRecorder::WaitForBegin()
{
	return  WaitForSingleObject(m_hBegin,-1) ==  WAIT_OBJECT_0;
}
#ifdef _WIN64
bool CChangeRecorder::RunHookProcess(PROCESS_INFORMATION &pi)
{
	TrackDebugOut;
	WCHAR szComd[MAX_PATH*2];
	WCHAR curpath[MAX_PATH];
	GetModuleFileNameW( 0, curpath, MAX_PATH );
	PathRemoveFileSpec(curpath);
	//_tcscat(curpath,_T("\\..\\x86"));
	DWORD dwProcessId = 0;
	HANDLE hNewToken = NULL;
	HRESULT hr = S_OK;
	HWINSTA  hwinsta = NULL;
	HANDLE hToken= NULL;
	void* pEnvBlock = NULL;
	ZeroMemory(&pi,sizeof(pi));
	DWORD errcode=0;

	try
	{
		DebugOutF(filelog::log_info,"attempt to RunHookProcess");
		HANDLE hCurProcess = 0;

		DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(), GetCurrentProcess(), &hCurProcess, 
			0, TRUE, DUPLICATE_SAME_ACCESS);
		swprintf(szComd,L"\"%s\\WinRobotHostx86.exe\" -runas hook -parent %d",curpath,hCurProcess);

		STARTUPINFOW si;
		ZeroMemory(&si, sizeof(STARTUPINFOW));
		si.cb= sizeof(STARTUPINFOW);

		DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS;

		si.lpDesktop = L"winsta0\\default";;

		INT  *pState = NULL;
		DWORD buflen = 0;

		if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&hToken)){
			DebugOutF(filelog::log_error,"OpenProcessToken failed , error code = %d",GetLastError());
			throw E_UNEXPECTED;
		}
		if(!CreateEnvironmentBlock( &pEnvBlock, hToken, TRUE)){
			DebugOutF(filelog::log_error,"CreateEnvironmentBlock Failed, error code = %d",GetLastError());
		}else{
			dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
		}

		if(!CreateProcess(0,szComd,0,0,TRUE,dwCreationFlags ,pEnvBlock,curpath,&si,&pi )){
			errcode = GetLastError();
			DebugOutF(filelog::log_error,"CreateProcessAsUser failed, error code = %d",GetLastError());
			throw HRESULT_FROM_WIN32(errcode);
		}
		DebugOutF(filelog::log_info,"RunHookProcess ok");
	}
	catch (HRESULT hr1)
	{
		hr = hr1;
	}

	if(hToken) CloseHandle(hToken);
	if(hNewToken)CloseHandle(hNewToken);
	if(pEnvBlock)DestroyEnvironmentBlock(pEnvBlock);
	return SUCCEEDED(hr);
}
#endif