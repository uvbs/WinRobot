/*! \file $Id: SwitchInputDesktop.cpp 75 2011-07-31 14:54:01Z caoym $
*  \author caoym
*  \brief SwitchInputDesktop
*/
#include "StdAfx.h"
#include "SwitchInputDesktop.h"
#include "debuglogger.h"

// get the current input desktop name and compare with 
// the desktop the call thread attached ,if not equal,
// then switch to the input desktop
BOOL SwitchInputDesktop()
{
	
	
	HDESK threaddesk = GetThreadDesktop(GetCurrentThreadId());
	HDESK inputdesk  = OpenInputDesktop(0, DF_ALLOWOTHERACCOUNTHOOK,GENERIC_ALL);

	if (inputdesk == NULL)
	{
		DebugOutF(filelog::log_error,("OpenInputDesktop failed with %d"),GetLastError());
		return FALSE;
	}
	DWORD len=0;
	TCHAR szThread[MAX_PATH];
	TCHAR szInput[MAX_PATH];
	szThread[0] = _T('\0');
	szInput[0] = _T('\0');
	BOOL res = FALSE;
	try
	{
		// get desktop name
		if(!GetUserObjectInformation(threaddesk, UOI_NAME, szThread, MAX_PATH, &len)){
			DebugOutF(filelog::log_error,("GetUserObjectInformation failed with %d"),GetLastError());
			throw FALSE;
		}
		if (!GetUserObjectInformation(inputdesk, UOI_NAME, szInput, MAX_PATH, &len)) {
			DebugOutF(filelog::log_error,("GetUserObjectInformation failed with %d"),GetLastError());
			throw FALSE;
		}
		//compare,if not equal,then switch to the input desktop 
		if (_tcsicmp(szThread, szInput) != 0)
		{

			if(!SetThreadDesktop(inputdesk)){
				DebugOutF(filelog::log_error,("SetThreadDesktop %s failed with %d"),szInput,GetLastError());
				throw FALSE;
			}
			DebugOutF(filelog::log_info,("switch input desktop %s"),szInput);
			res = TRUE;

		}
	}
	catch(BOOL b1)
	{
		res = b1;
	}
	if(threaddesk)CloseDesktop(threaddesk);
	if(inputdesk)CloseDesktop(inputdesk);
	return res;
}