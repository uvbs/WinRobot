/*! \file $Id: ScreenBuffer.cpp 75 2011-07-31 14:54:01Z caoym $
*  \author caoym
*  \brief CScreenBuffer			
*/
#include "StdAfx.h"
#include "ScreenBuffer.h"
#include "debuglogger.h"

CScreenBuffer::CScreenBuffer(void)
:m_pBuff(0)
,m_hBmp(0)
,m_hMemDC(0)
{
	
}

CScreenBuffer::~CScreenBuffer(void)
{
	Destroy();
}
bool CScreenBuffer::Create(HDC hDev,const RECT & rc,LPCTSTR szName )
{
	TrackDebugOut;
	Destroy();
	m_hMemDC = CreateCompatibleDC(hDev);
	if(m_hMemDC == NULL){
		DebugOutF(filelog::log_error,("CreateCompatibleDC failed with %d"),GetLastError() );
		return false;
	}
	RECT rcscreen = GetDCRect(hDev);
	RECT rcdest;
	IntersectRect(&rcdest,&rcscreen,&rc);

	LONG lWidth		= rcdest.right - rcdest.left;
	LONG lHeight	= rcdest.bottom - rcdest.top; 
	LONG lBitsPPix	= 32;//GetDeviceCaps(hDev,BITSPIXEL);
	LONG dwImageSize = lHeight*CalculatePitch(CalculateLine(lWidth,lBitsPPix));


	// save [bmp file header] + [bmp info header] + [bmp data] to the file mapping object
	//DWORD filesize = 0;
	LONG biClrUsed = 0;
	RGBQUAD rgbquad[256];
	if (lBitsPPix < 16)
	{
		TrackDebugOut;
		biClrUsed = GetDIBColorTable(hDev,0,256,rgbquad);
	}
	
	if(!CFileMappingBitmap::Create(lWidth,lHeight,lBitsPPix,biClrUsed,rgbquad,szName))
	{
		return false;
	}
	
	m_hBmp = CreateDIBSection(m_hMemDC,(BITMAPINFO*)InfoHeader(),DIB_RGB_COLORS, (void**)&m_pBuff, GetHandle(), FileHeader()->bfOffBits);
	if(m_hBmp == NULL){
		DebugOutF(filelog::log_error,("CreateDIBSection failed %d"),GetLastError() );
		return false;
	}
	SelectObject(m_hMemDC,m_hBmp);
	//HDC hdc = GetDC(0);
	BitBlt(m_hMemDC,0,0,rcdest.right-rcdest.left,rcdest.bottom-rcdest.top,hDev,rcdest.left,rcdest.top,SRCCOPY|CAPTUREBLT);
	//ReleaseDC(0,hdc);
	return true;
}
void CScreenBuffer::Destroy()
{
	
	if(m_hMemDC)
	{
		DeleteDC(m_hMemDC);
	}
	if(m_hBmp){
		DeleteObject(m_hBmp);
	}
	CFileMappingBitmap::Destroy();
}

BOOL CALLBACK  MyDCDisplayMonitorsEnumProc( HMONITOR hMonitor,  // handle to display monitor
							HDC hdcMonitor,     // handle to monitor DC
							LPRECT lprcMonitor, // monitor intersection rectangle
							LPARAM dwData       // data
)
{
	if(dwData == 0) return FALSE;
	LPRECT dest = (LPRECT)dwData;
	RECT temp = *dest;
	UnionRect(dest,&temp,lprcMonitor);
	return TRUE;
}
RECT GetDCRect( HDC hdc)
{
	
	RECT rc = {0};
	EnumDisplayMonitors(hdc, NULL, MyDCDisplayMonitorsEnumProc, (LPARAM)&rc);
	return rc;
}