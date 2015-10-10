/*! \file $Id: CharEvent.h 64 2011-06-15 14:01:00Z caoym $
*  \author caoym
*  \brief CCharEvent
*/
#pragma once
#include "IInputEvent.h"
//! input character
/*
	keyboard input as if the corresponding character was generated on the keyboard
*/
class CCharEvent:public IInputEvent
{
public:
	CCharEvent(wchar_t c);
	void Input();
private:
	wchar_t m_char;
};
