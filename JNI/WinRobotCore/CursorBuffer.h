/*! \file $Id: CursorBuffer.h 118 2011-11-15 16:22:30Z caoym $
*  \author caoym
*  \brief CCursorBuffer			
*/
#pragma once
#include "IBitmapBuffer.h"
#include "FileMappingBitmap.h"
#include "WinRobotCore_i.h"
//! mouse cursor buffer
class CCursorBuffer:public CFileMappingBitmap
{
public:
	
	CCursorBuffer(void);
	~CCursorBuffer(void);
	bool Create();
	//! Get position of current cursor
	const POINT& Position()const{return m_ptPosition;}
	//! Get Hotspot of current cursor
	const POINT& Hotspot()const{return m_ptHotspot;}
	//! Return the HCURSOR of last capture,only for compare,don't try to use it with any API.
	const HCURSOR Cursor()const { return m_hCursor;}
	//! the type of predefined cursor 
	CursorType Type()const;
private:
	POINT m_ptHotspot;
	POINT m_ptPosition;
	HCURSOR m_hCursor;	
	//WORD m_lBitsPPix;
};
