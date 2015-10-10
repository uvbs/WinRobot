/*! \file $Id: MouseEvent.h 87 2011-09-28 13:52:41Z caoym $
*  \author caoym
*  \brief CMouseEvent
*/

#pragma once
#include "IInputEvent.h"

//! mouse event input item
class CMouseEvent:public IInputEvent
{
public:
	enum EVENT{
		bn_left_up=0x01<<1,
		bn_left_down=0x01<<2,
		bn_middle_up=0x01<<3,
		bn_middle_down=0x01<<4,
		bn_right_up=0x01<<5,
		bn_right_down=0x01<<6,
		mouse_move=0x01<<7,
		wheel=0x01<<8,
		//wheel_backward,
	};
	
	/*!
	 * \param event event type of mouse
	 * \param x x coordinate of mouse event,when event == wheel,this value is the number of\n
	 * "notches" to move the mouse wheel Negative values indicate movement up/away from the\n
	 * user, positive values indicate movement down/towards the user.
	 * \param y y coordinate of mouse event,when event == wheel,this value is the ignored.
	 */
	CMouseEvent(EVENT event,int x,int y);
	
	CMouseEvent(void);
	~CMouseEvent(void);
	void Input(); 
private:
	EVENT m_eEvent;
	int m_nX;
	int m_nY;
	
};
