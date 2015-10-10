/*! \file $Id: CharEvent.cpp 64 2011-06-15 14:01:00Z caoym $
*  \author caoym
*  \brief CCharEvent
*/
#include "StdAfx.h"
#include "CharEvent.h"
#include "debuglogger.h"
//! auto clean and resume key state
struct CleanKeyState 
{
	CleanKeyState(BYTE * vks,int count)
	{
		if(!GetKeyboardState(m_oristate))
		{
			DebugOutF(filelog::log_error,"GetKeyboardState failed with %d",GetLastError());
		}
		memcpy(m_newstate,m_oristate,sizeof(m_oristate));
		for (int i = 0;i< count;i++)
		{
			m_newstate[ vks[i] ] = 0;
		}
		if(!SetKeyboardState(m_newstate))
		{
			DebugOutF(filelog::log_error,"SetKeyboardState failed with %d",GetLastError());
		}	
		
	}

	~CleanKeyState()
	{
		if(!SetKeyboardState(m_oristate))
		{
			DebugOutF(filelog::log_error,"SetKeyboardState failed with %d",GetLastError());
		}	
	}
private:
	BYTE m_oristate[256];
	BYTE m_newstate[256];
};

CCharEvent::CCharEvent(wchar_t c)
:m_char(c)
{
}
// input a unicode char 
void CCharEvent::Input()
{
	BYTE toclean[] = {VK_SHIFT,VK_CONTROL,VK_MENU};
	CleanKeyState keystate(toclean,RTL_NUMBER_OF(toclean)); // clean key state while inputing,resume key state

	INPUT   input[2];
	ZeroMemory(input,sizeof(input));

	input[0].type = input[1].type	= INPUT_KEYBOARD;
	input[0].ki.wVk	= input[1].ki.wVk	= 0;  // specifies KEYEVENTF_UNICODE
	input[0].ki.wScan	= input[1].ki.wScan	 = m_char;
	input[0].ki.dwFlags	= KEYEVENTF_UNICODE;
	input[1].ki.dwFlags = KEYEVENTF_UNICODE|KEYEVENTF_KEYUP;  

	if(!SendInput(1,   &input[0],   sizeof(INPUT)))
	{
		DebugOutF(filelog::log_error,"SendInput failed with %d",GetLastError());
	}
	else if(!SendInput(1,   &input[1],   sizeof(INPUT)))
	{
		DebugOutF(filelog::log_error,"SendInput failed with %d",GetLastError());
	}
	else
	{
		DebugOutF(filelog::log_info,"CCharEvent::Input %c ok",m_char);
	}

}