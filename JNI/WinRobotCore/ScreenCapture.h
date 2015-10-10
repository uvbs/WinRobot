/*! \file $Id: ScreenCapture.h 94 2011-10-08 15:36:05Z caoym $
*  \author caoym
*  \brief CScreenCapture			
*/
#pragma once
#include "sync/cslock.h"
#include "reference/IReference.h"
#include "DX9ScreenCapture.h"
#include "ChangeRecorder.h"
#include "ScreenBuffer.h"

//class CDDScreenBuffer;
struct IScreenBufferStream;
//! capture screen 
class CScreenCapture
{
public:
	CScreenCapture(LPCTSTR szDevice = 0);
	~CScreenCapture(void);
	HRESULT Capture(const RECT&rc,IScreenBufferStream**ppStream);
	OLE_COLOR GetPixelColor(int x,int y);
	bool Reset();
private:
	std::basic_string<TCHAR> m_szDevice;
	CChangeRecorder m_recoder;
	CCrtSection m_csLock;
	HDC m_hDev;
	//CDX9ScreenCapture m_pCapImpl;
	bool NeedToReset();
	
	//CRefObj<CScreenBuffer> m_pScreenBuf;

	//HRGN GetChangesRgn();
};
