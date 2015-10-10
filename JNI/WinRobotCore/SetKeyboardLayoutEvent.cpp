/*! \file $Id: SetKeyboardLayoutEvent.cpp 38 2011-04-14 14:48:56Z caoym $
*  \author caoym
*  \brief CSetKeyboardLayoutEvent
*/
#include "StdAfx.h"
#include "SetKeyboardLayoutEvent.h"
#include "debuglogger.h"
CSetKeyboardLayoutEvent::CSetKeyboardLayoutEvent(LPCTSTR szKLID)
:m_szKLID(szKLID)
,m_hKL(0)
{
}
bool CSetKeyboardLayoutEvent::Load()
{
	if(m_hKL) return true;
	m_hKL = LoadKeyboardLayout(m_szKLID,0);
	if (m_hKL == 0)
	{
		DebugOutF(filelog::log_error,"LoadKeyboardLayout %s failed with %d",(char*)CT2A(m_szKLID),GetLastError()); 
		return false;
	}
	DebugOutF(filelog::log_info,"LoadKeyboardLayout %s ok",(char*)CT2A(m_szKLID)); 
	return true;
}
void CSetKeyboardLayoutEvent::Input()
{
	if(m_hKL)
	{
		if(0 == ActivateKeyboardLayout(m_hKL,0))
		{
			DebugOutF(filelog::log_error,"ActivateKeyboardLayout %s failed with %d",(char*)CT2A(m_szKLID),GetLastError()); 
		}
		else
		{
			DebugOutF(filelog::log_info,"ActivateKeyboardLayout %s ok",(char*)CT2A(m_szKLID)); 
		}
	}
}
