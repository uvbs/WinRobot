/*! \file $Id: ScreenCapture.cpp 94 2011-10-08 15:36:05Z caoym $
*  \author caoym
*  \brief CScreenCapture			
*/
#include "StdAfx.h"
#include "ScreenCapture.h"
#include "debuglogger.h"
#include "SwitchInputDesktop.h"
#include "debuglogger.h"
#include "ScreenBufferStream.h"
#include "ScreenBuffer.h"
// #include "DDScreenBuffer.h"
#include "WinRobotHook.h"

CScreenCapture::CScreenCapture(LPCTSTR szDevice)
:m_hDev(0)
,m_szDevice(szDevice?szDevice:_T("DISPLAY"))
{
}

CScreenCapture::~CScreenCapture(void)
{
	if (m_hDev)
	{
		DeleteDC(m_hDev);
	}
}
// HRGN CScreenCapture::GetChangesRgn()
// {
// 	
// 	// get changes
// 	CChangeRecorder::CHANGES changes;
// 	if(!m_recoder.GetChangeList(changes))
// 	{
// 		DebugOutF(filelog::log_error,"GetChangeList failed");
// 		return 0;
// 	}
// 	HRGN hrgn = CreateRectRgn(0,0,0,0);
// 	if (hrgn == 0)
// 	{
// 		DebugOutF(filelog::log_error,"CreateRectRgn failed with %d",GetLastError() );
// 		return 0;
// 	}
// 	HRGN hTemp = CreateRectRgn(0,0,0,0);
// 	if (hTemp == 0)
// 	{
// 		DebugOutF(filelog::log_error,"CreateRectRgn failed with %d",GetLastError() );
// 		DeleteObject(hrgn);
// 		return 0;
// 	}
// 	RECT maxrc =  GetDCRect(m_hDev);
// 
// 	for(CChangeRecorder::CHANGES::iterator it =changes.begin();
// 		it != changes.end();
// 		it++)
// 	{
// 		RECT rc = {0} ;
// 		IntersectRect(&rc,&*it,&maxrc);
// 		SetRectRgn(hTemp,rc.left,rc.top,rc.right,rc.bottom);
// 		CombineRgn(hrgn,hrgn,hTemp,RGN_OR);
// 	}
// 	RECT box;
// 	GetRgnBox(hrgn,&box);
// 	//DebugOutF(filelog::log_error,"%d,%d,%d,%d",box.left,box,box.right,box.bottom);
// 	DeleteObject(hTemp);
// 	return hrgn;
// }
HRESULT CScreenCapture::Capture(const RECT&rc,IScreenBufferStream**ppStream)
{
	TrackDebugOut;
	CAutoLock<CCrtSection>	lock(m_csLock);
	if (NeedToReset())
	{
		TrackDebugOut;
		if(!Reset())
		{
			DebugOutF(filelog::log_error,"CScreenCapture::Reset failed");
			return E_FAIL;
		}
	}
	{
		TrackDebugOut;
	}

 	CRefObj<CScreenBuffer> pBuf = new CScreenBuffer();
 	if(!pBuf->Create(m_hDev,rc))
 	{
 		DebugOutF(filelog::log_error,"CScreenCapture::Capture failed");
		Reset();
 		return E_FAIL ;
 	}
	
	RECT rcscreen =GetDCRect(m_hDev);
	RECT rcdest={0};
	IntersectRect(&rcdest,&rcscreen,&rc);

	// capture DirectDraw overlay image
	AppendDDrawOverlay(pBuf->GetDC(),&rcdest);

	CComObject<CScreenBufferStream>* pScreen;
	HRESULT hr = CComObject<CScreenBufferStream>::CreateInstance(&pScreen);
	if(	FAILED(hr)	)
	{
		DebugOutF(filelog::log_error,"CreateInstance CScreenBufferStream failed with 0x%x",hr);
		return hr ;
	}
	pScreen->AddRef();
	pScreen->SetBuffer(pBuf);

	DebugOutF(filelog::log_info,("capture screen (%d,%d,%d,%d) ok"),rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top);
	*ppStream = pScreen;
	return S_OK;
}

OLE_COLOR CScreenCapture::GetPixelColor(int x,int y)
{
	
	CAutoLock<CCrtSection>	lock(m_csLock);
	if (NeedToReset())
	{
		if(!Reset())
		{
			DebugOutF(filelog::log_error,"CScreenCapture::Reset failed");
			return E_FAIL;
		}
	}
	return GetPixel(m_hDev,x,y);
}
bool CScreenCapture::NeedToReset()
{
	if (m_hDev == 0) return true;
	return false;
}
bool CScreenCapture::Reset()
{
	TrackDebugOut;
	if (m_hDev)
	{
		DeleteDC(m_hDev);
		m_hDev = 0;
	}
	m_recoder.Stop();
	m_recoder.Run();
	//send a message to hooked windows to make the hook work
	PostMessage(HWND_BROADCAST,WM_NCHITTEST ,0,0);
	// make all windows redraw
	//InvalidateRect(0,0,TRUE);
	//ChangeDisplaySettingsEx(0,0,0,0,0);

	m_hDev = CreateDC(m_szDevice.c_str(), NULL, NULL, NULL);
	if (m_hDev == 0)
	{
		DebugOutF(filelog::log_error,"CreateDC failed with %d",GetLastError() );
		return false;
	}
//
	
	return true;
}