/*! \file $Id: KeyboardEvent.h 64 2011-06-15 14:01:00Z caoym $
*  \author caoym
*  \brief CKeyboardEvent
*/

#pragma once
#include "IInputEvent.h"

//! input keyboard event
class CKeyboardEvent:public IInputEvent
{
public:
	enum STATE{
		up, //!< key up
		down //!< key down
	};
	CKeyboardEvent(int vkey,STATE state);
	~CKeyboardEvent(void);
	void Input();
private:
	int m_nVKey;
	STATE m_eState;
	bool IsCtrlAltDel();
	static bool SimulateCtrlAltDel();
	static bool Input(int vk,STATE state);
};
