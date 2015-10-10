/*! \file $Id: SetKeyboardLayoutEvent.h 87 2011-09-28 13:52:41Z caoym $
*  \author caoym
*  \brief CSetKeyboardLayoutEvent
*/
#pragma once
#include "IInputEvent.h"
#include "atlstr.h"
//! change keyboard layout
/*
 * Used to change keyboard layout of the input thread,the ActivateKeyboardLayout API will \n
 * change the keyboard layout of the call thread,so I inherit this class  from IInputEvent\n
 * to make sure ActivateKeyboardLayout will be called by input thread.
 */
class CSetKeyboardLayoutEvent:public IInputEvent
{
public:
	/*!
	 * \param szKLID [in] Pointer to the buffer that specifies the name 
	 * of the input locale identifier to load. This name is a string composed
	 * of the hexadecimal value of the Language Identifier (low word) and a 
	 * device identifier (high word). For example, U.S. English has a language 
	 * identifier of 0x0409, so the primary U.S. English layout is named "00000409". 
	 * Variants of U.S. English layout (such as the Dvorak layout) are named 
	 * "00010409", "00020409", and so on. 
	*/
	CSetKeyboardLayoutEvent(LPCTSTR szKLID);
	void Input();
	//! load the specified keyboard layout
	bool Load();
private:
	HKL m_hKL;
	ATL::CString m_szKLID;
};
