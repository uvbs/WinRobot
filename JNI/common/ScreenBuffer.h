/*! \file $Id: ScreenBuffer.h 66 2011-06-20 14:08:41Z caoym $
*  \author caoym
*  \brief CScreenBuffer			
*/
#pragma once
#include "FileMappingBitmap.h"
#include "FileMapping.h"
//! screen buffer
class CScreenBuffer:public CFileMappingBitmap
{
public:
	CScreenBuffer();
	//! create buffer and get image from screen
	/*!
	 *	\param hDev DC of the screen
	 *	\param specify the rect of the screen where to  capture
	 *	\param szName set the name of the screen buffer,if szName is 0,a random  name will be specified.
	 *	\return true if succeed 
	*/
	bool Create(HDC hDev,const RECT & rc,LPCTSTR szName = 0);
	//! destroy it
	void Destroy();
	~CScreenBuffer(void);
	//! Get the DC that be created in memory and used to copy image from screen DC
	HDC GetDC(){return m_hMemDC;}
private:
	//CFileMapping m_fileMapping;
	LPVOID m_pBuff; 
	HDC m_hMemDC;
	HBITMAP m_hBmp;
};
RECT GetDCRect( HDC hdc);