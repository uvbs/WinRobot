/*! \file $Id: IInputEvent.h 87 2011-09-28 13:52:41Z caoym $
*  \author caoym
*  \brief IInputEvent
*/

#pragma once
#include "reference/IReference.h"

//! basic input event
/*
 * Indicates event objects which are used by CInputThread to input mouse or keyboard event.
 */
class IInputEvent:public CReference
{
public:
	virtual ~IInputEvent(){}
	//! Input the event,called by CInputThread
	virtual void Input() = 0;
};