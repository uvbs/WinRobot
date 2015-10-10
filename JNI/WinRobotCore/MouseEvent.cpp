/*! \file $Id: MouseEvent.cpp 111 2011-10-30 14:40:09Z caoym $
*  \author caoym
*  \brief CMouseEvent
*/
#include "StdAfx.h"
#include "MouseEvent.h"
#include "debuglogger.h"


inline LONG GetOpstLen(LONG len,LONG Max)
{
	if(Max == 0) return 0;
	if(len<0)len = 0;
	if(len>Max)len = Max;
	return len * 0xFFFF/ Max ;
}
CMouseEvent::CMouseEvent(EVENT event,int x,int y)
:m_eEvent(event)
,m_nX(x)
,m_nY(y)
{
}

CMouseEvent::~CMouseEvent(void)
{
}
void CMouseEvent::Input()
{
	INPUT   input;
	ZeroMemory(&input,sizeof(INPUT));
	input.type = INPUT_MOUSE;
	int cw = GetSystemMetrics(SM_CXSCREEN);
	int ch = GetSystemMetrics(SM_CYSCREEN);
	if(m_eEvent == mouse_move)
	{
		input.mi.dx = GetOpstLen(m_nX,cw);
		input.mi.dy = GetOpstLen(m_nY,ch);
	}
	else 
	{
		POINT pos;
		GetCursorPos(&pos);
		input.mi.dx = GetOpstLen(pos.x,cw);;
		input.mi.dy = GetOpstLen(pos.y,ch);
	}
	
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
	input.mi.time = 0;
	BOOL bInput = FALSE;
	switch(m_eEvent)
	{
	case mouse_move:
		input.mi.dwFlags |= MOUSEEVENTF_MOVE;
		SetCursorPos(m_nX,m_nY);
		bInput = SendInput(1,   &input,   sizeof(INPUT));
		break;
	case bn_left_down	:
		input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
		bInput = SendInput(1,   &input,   sizeof(INPUT));
		break;
	case bn_left_up	:	
		input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
		bInput = SendInput(1,   &input,   sizeof(INPUT));
		break;
	case bn_right_down:	
		input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
		bInput = SendInput(1,   &input,   sizeof(INPUT));
		break;
	case bn_right_up	:	
		input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
		bInput = SendInput(1,   &input,   sizeof(INPUT));
		break;
	case bn_middle_down	:	
		input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
		bInput = SendInput(1,   &input,   sizeof(INPUT));
		break;
	case bn_middle_up:	
		input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
		bInput = SendInput(1,   &input,   sizeof(INPUT));
		break;
	case wheel	:	
		input.mi.dwFlags |= MOUSEEVENTF_WHEEL;
		input.mi.mouseData = m_nX;
		bInput = SendInput(1,   &input,   sizeof(INPUT));
		break;
	default:
		DebugOutF(filelog::log_error,"unknown mouse event %d",m_eEvent);
		break;
	}
	if(!bInput){
		DebugOutF(filelog::log_error,"MouseEvent SendInput failed with %d",GetLastError());
	}
	else
	{
		DebugOutF(filelog::log_info,"MouseEvent %d,%d,%d input ok",m_eEvent,m_nX,m_nY);
	}
}