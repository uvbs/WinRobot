/*! \file $Id: KeyboardEvent.cpp 74 2011-07-18 16:25:42Z caoym $
*  \author caoym
*  \brief CKeyboardEvent
*/

#include "StdAfx.h"
#include "KeyboardEvent.h"
#include "debuglogger.h"
#include "atlsecurity.h"
BOOL IsWinNT();
BOOL SelectHDESK(HDESK new_desktop);
BOOL SelectDesktop(LPCTSTR name);
unsigned __stdcall SimulateCtrlAltDelThreadFn( void* pArguments );


CKeyboardEvent::CKeyboardEvent(int vkey,STATE state)
:m_nVKey(vkey)
,m_eState(state)
{
}

CKeyboardEvent::~CKeyboardEvent(void)
{
}
bool CKeyboardEvent::Input(int vk,STATE state)
{
	INPUT   input;
	ZeroMemory(&input,sizeof(INPUT));
	input.type		 = INPUT_KEYBOARD;
	input.ki.wVk	 = vk;  
	input.ki.wScan	 = MapVirtualKey(vk, 0);
	input.ki.dwFlags = (state ==down) ? 0:KEYEVENTF_KEYUP;  
	if(!SendInput(1,   &input,   sizeof(INPUT)))
	{
		DebugOutF(filelog::log_error,"SendInput %d failed with %d",vk,GetLastError());
		return false;
	}
	else
	{
		DebugOutF(filelog::log_info,"KeyboardEvent %d,%d input ok",vk,state);
	}
	return true;
}
void CKeyboardEvent::Input()
{
	if(!Input (m_nVKey ,m_eState))
	{
		return ;
	}
	
	// Simulate Ctrl + Alt + Del
	if (IsCtrlAltDel())
	{
		//release pressed key,because is SimulateCtrlAltDel switch desktop,
		//then the "key up event" will send to "winlogon" desktop

		if ((GetKeyState (VK_DELETE) & 0x8000) )	Input(VK_DELETE,up);
		if ((GetKeyState (VK_LMENU) & 0x8000) )		Input(VK_LMENU,up);
		if ((GetKeyState (VK_RMENU) & 0x8000) )		Input(VK_RMENU,up);
		if ((GetKeyState (VK_LCONTROL) & 0x8000) )	Input(VK_LCONTROL,up);
		if ((GetKeyState (VK_RCONTROL) & 0x8000) )	Input(VK_RCONTROL,up);

	
		SimulateCtrlAltDel();
	}
	return;
}
bool CKeyboardEvent::IsCtrlAltDel()
{
	if(m_eState != down) return false;

	if( m_nVKey == VK_DELETE &&
		((GetKeyState (VK_LMENU) & 0x8000)  || (GetKeyState (VK_RMENU) & 0x8000) )&&
		((GetKeyState (VK_LCONTROL) & 0x8000)  || (GetKeyState (VK_RCONTROL) & 0x8000) )
	)
	{
		return true;
	}
	return false;
}

// if os <= vista 
//		simulate CtrAltDel by posting a WM_HOTKEY message to all the windows on the Winlogon desktop.
// else if create a cad.exe process succeed
//		done
// else 
//		start Windows Task Manager

bool CKeyboardEvent::SimulateCtrlAltDel()
{
	DebugOutF(filelog::log_error,"SimulateCtrlAltDel");
	OSVERSIONINFO osversioninfo;

	osversioninfo.dwOSVersionInfoSize = sizeof(osversioninfo);
	GetVersionEx(&osversioninfo);

	if (osversioninfo.dwMajorVersion >=6) // >vista
	{
		// if OS >= vista ,we need a cad.exe to simulate CtrlAtlDel
		CAccessToken token;
		if(!token.GetProcessToken(TOKEN_ALL_ACCESS))
		{
			DebugOutF(filelog::log_error,"GetProcessToken with %d",GetLastError());
			return false;
		}

		WCHAR szComd[MAX_PATH];
		WCHAR curpath[MAX_PATH];
		GetModuleFileNameW( 0, curpath, MAX_PATH );
		PathRemoveFileSpecW(curpath);
		swprintf(szComd,L"\"%s\\cad.exe\"",curpath);

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );
		if(!CreateProcess(0,szComd,0,0,0,NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW ,0,0,&si,&pi )){
			DebugOutF(filelog::log_error,("CreateProcess failed, error code = %d"),GetLastError());
			Input(VK_LCONTROL,down);
			Input(VK_LSHIFT,down);
			Input(VK_ESCAPE,down);
			Input(VK_ESCAPE,up);
			Input(VK_LSHIFT,up);
			Input(VK_LCONTROL,up);
			return true;
		}
		else
		{
			if(WaitForSingleObject(pi.hThread,5000) == WAIT_TIMEOUT)
			{
				TerminateProcess(pi.hProcess,0);
			}
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
			return true;
		}
		
	}
	
	unsigned threadID;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, &SimulateCtrlAltDelThreadFn, 0, 0, &threadID );
	CloseHandle(hThread);
	return TRUE;
}
BOOL SelectDesktop(LPCTSTR name)
{
	// Are we running on NT?
	if (IsWinNT())
	{
		HDESK desktop;
		if (name != NULL)
		{

			// Attempt to open the named desktop
			desktop = OpenDesktop(name, 0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
		}
		else
		{

			// No, so open the input desktop
			desktop = OpenInputDesktop(0, FALSE,
				DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
				DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
		}

		// Did we succeed?
		if (desktop == NULL) {
			DebugOutF(filelog::log_error,("OpenInputDesktop/OpenDesktop failed with %d"),GetLastError());
			DWORD err = GetLastError();
			return FALSE;
		}

		// Switch to the new desktop
		if (!SelectHDESK(desktop)) {
			// Failed to enter the new desktop, so free it!
			CloseDesktop(desktop);

			return FALSE;
		}

		// We successfully switched desktops!
		return TRUE;
	}

	return (name == NULL);
}
unsigned __stdcall SimulateCtrlAltDelThreadFn( void* pArguments )
{
	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

	if(old_desktop == NULL ){
		DebugOutF(filelog::log_error,"GetThreadDesktop with %d",GetLastError());
		return FALSE;
	}
	if (!SelectDesktop(_T("Winlogon")))
	{
		DebugOutF(filelog::log_error,("SelectDesktop(\"Winlogon\") with %d"),GetLastError());
		return FALSE;
	}

	// Winlogon uses hotkeys to trap Ctrl-Alt-Del...
	if(!PostMessage(HWND_BROADCAST, WM_HOTKEY, 0, MAKELONG(MOD_ALT | MOD_CONTROL, VK_DELETE)))
		DebugOutF(filelog::log_error,"PostMessage(HWND_BROADCAST) with %d",GetLastError());

	// Switch back to our original desktop
	if (old_desktop != NULL)
		SelectHDESK(old_desktop);
	
	return 0;

}
BOOL SelectHDESK(HDESK new_desktop)
{
	if (IsWinNT())
	{
		HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

		if(old_desktop == NULL )
			DebugOutF(filelog::log_error,("GetThreadDesktop with %d"),GetLastError());
		DWORD dummy;
		TCHAR new_name[256];

		if (!GetUserObjectInformation(new_desktop, UOI_NAME, new_name, 256, &dummy)) {
			DebugOutF(filelog::log_error,("GetUserObjectInformation with %d"),GetLastError());
			return FALSE;
		}

		// Switch the desktop
		if(!SetThreadDesktop(new_desktop)) {
			DebugOutF(filelog::log_error,("SetThreadDesktop with %d"),GetLastError());
			return FALSE;
		}

		// Switched successfully - destroy the old desktop
		CloseDesktop(old_desktop);

		return TRUE;

	}

	return TRUE;
}
BOOL IsWinNT()
{
	return TRUE;
	static BOOL bFirstCall = TRUE;
	static OSVERSIONINFO osversioninfo;

	if(bFirstCall)
	{
		osversioninfo.dwOSVersionInfoSize = sizeof(osversioninfo);
		// Get the current OS version
		if (!GetVersionEx(&osversioninfo)) return FALSE;
		bFirstCall = FALSE;
	}
	return (osversioninfo.dwPlatformId == VER_PLATFORM_WIN32_NT);
}