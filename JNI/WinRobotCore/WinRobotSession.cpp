/*! \file $Id: WinRobotSession.cpp 118 2011-11-15 16:22:30Z caoym $
*  \author caoym
*  \brief CWinRobotSession
*/

#include "stdafx.h"
#include "WinRobotSession.h"
#include "semqueue.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "ScreenCapture.h"
#include "CursorBuffer.h"
#include "SwitchInputDesktop.h"
#include "ScreenBufferStream.h"
#include "debuglogger.h"
#include "CharEvent.h"
#include "SetKeyboardLayoutEvent.h"
/**
 * Input multiple events
 * Sometimes we need input several events,for example: mousePress(BUTTON1_MASK |BUTTON2_MASK |BUTTON3_MASK ),
 * but we don't wan't "Delay()" will be called for every event,so we can use this class.
 */
class CInputEvents:public IInputEvent
{
public:
	void Push(IInputEvent*p)
	{
		m_events.push_back(p);
	}
	void Input()
	{
		for (std::list< CRefObj<IInputEvent> >::iterator it = m_events.begin();
			it != m_events.end();
			it++)
		{
			(*it)->Input();
		}
	}
private:
	std::list< CRefObj<IInputEvent> > m_events;
};
// CWinRobotSession
HRESULT CWinRobotSession::FinalConstruct()
{
	if(!m_objInputThread.Run()) return E_UNEXPECTED;
	//m_screen_cap.Reset();
	return S_OK;
}

void CWinRobotSession::FinalRelease()
{
	m_objInputThread.Stop();
}

STDMETHODIMP CWinRobotSession::CreateScreenCapture(LONG nX, LONG nY, LONG nWidth, LONG nHeight, IUnknown** ppBufferedImage)
{
	// set current thread desktop to input desktop
	if( SwitchInputDesktop() )
	{
		m_bDesktopChanged = true;
	}
	// if input desktop has changed,we must reset m_screen_cap 
	if(m_bDesktopChanged)
	{
		m_bDesktopChanged = false;
		m_screen_cap.Reset();
	}
	// now capture 
	RECT rc = {nX,nY,nX+nWidth,nY+nHeight};
	CComPtr<IScreenBufferStream> pSession ;
	HRESULT hr = m_screen_cap.Capture(rc,&pSession);
	if (FAILED(hr))
	{
		DebugOutF(filelog::log_error,"m_screen_cap.Capture failed with 0x%x",hr);
		return hr;
	}
	if(FAILED ( hr = pSession->QueryInterface(__uuidof(IUnknown),(void**)ppBufferedImage) ))
	{
		DebugOutF(filelog::log_error,"CreateScreenCapture failed with 0x%x",hr);
		return hr;
	}
	return hr;
}

STDMETHODIMP CWinRobotSession::Delay(ULONG ms)
{
	m_objInputThread.Delay(ms);

	return S_OK;
}

STDMETHODIMP CWinRobotSession::get_AutoDelay(ULONG* pVal)
{
	if(!pVal) return E_POINTER;
	*pVal = m_objInputThread.AutoDelay();
	return S_OK;
}

STDMETHODIMP CWinRobotSession::put_AutoDelay(ULONG newVal)
{
	m_objInputThread.AutoDelay(newVal);
	return S_OK;
}

STDMETHODIMP CWinRobotSession::GetPixelColor(LONG x, LONG y, OLE_COLOR* color)
{
	if(!color) return E_POINTER;
	*color = m_screen_cap.GetPixelColor(x,y);
	return S_OK;
}

STDMETHODIMP CWinRobotSession::get_AutoWaitForIdle(VARIANT_BOOL* pVal)
{
	if(!pVal) return E_POINTER;
	*pVal = m_objInputThread.AutoWaitForIdle()?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CWinRobotSession::put_AutoWaitForIdle(VARIANT_BOOL newVal)
{
	m_objInputThread.AutoWaitForIdle(!!newVal);
	return S_OK;
}

STDMETHODIMP CWinRobotSession::WaitForIdle(void)
{
	return m_objInputThread.WaitForIdle()?S_OK:E_FAIL;
}

STDMETHODIMP CWinRobotSession::KeyPress(ULONG keycode)
{
	return m_objInputThread.PushEvent(new CKeyboardEvent(keycode,CKeyboardEvent::down) ) ?S_OK:E_FAIL;
}

STDMETHODIMP CWinRobotSession::KeyRelease(ULONG keycode)
{
	return m_objInputThread.PushEvent(new CKeyboardEvent(keycode,CKeyboardEvent::up) )?S_OK:E_FAIL;
}

STDMETHODIMP CWinRobotSession::MouseMove(LONG x,LONG y)
{
	return m_objInputThread.PushEvent(new CMouseEvent(CMouseEvent::mouse_move,x,y) )?S_OK:E_FAIL;
}

STDMETHODIMP CWinRobotSession::MousePress(ULONG buttons)
{
	CRefObj<CInputEvents> events = new CInputEvents();
	
	if (buttons & BUTTON1_MASK){
		events->Push(new CMouseEvent( CMouseEvent::bn_left_down,0,0) );
	}
	if(buttons & BUTTON2_MASK){
		events->Push(new CMouseEvent( CMouseEvent::bn_middle_down,0,0) );
	}
	if(buttons & BUTTON3_MASK){
		events->Push(new CMouseEvent( CMouseEvent::bn_right_down,0,0));
		
	}
	m_objInputThread.PushEvent(events);
	return S_OK;
}

STDMETHODIMP CWinRobotSession::MouseRelease(ULONG buttons)
{
	CRefObj<CInputEvents> events = new CInputEvents();

	if (buttons & BUTTON1_MASK){
		events->Push(new CMouseEvent( CMouseEvent::bn_left_up,0,0) );
	}
	if(buttons & BUTTON2_MASK){
		events->Push(new CMouseEvent( CMouseEvent::bn_middle_up,0,0) );
	}
	if(buttons & BUTTON3_MASK){
		events->Push(new CMouseEvent( CMouseEvent::bn_right_up,0,0));
	}
	m_objInputThread.PushEvent(events);
	return S_OK;
}

STDMETHODIMP CWinRobotSession::MouseWheel(LONG wheelAmt)
{
//	CMouseEvent::EVENT me ;
// 	if (wheelAmt >0 )
// 	{
// 		me = CMouseEvent::wheel_forward;
// 	}
// 	else
// 	{
// 		me = CMouseEvent::wheel_backward;
// 	}
// 	wheelAmt = wheelAmt%256;
// 	for (LONG i =0; i<wheelAmt ;i++)
// 	{
		m_objInputThread.PushEvent(new CMouseEvent( CMouseEvent::wheel,wheelAmt*WHEEL_DELTA,0) );
// 	}
	return S_OK;
}

STDMETHODIMP CWinRobotSession::CaptureMouseCursor(LONG* x, 
												  LONG* y, 
												  LONG* hotx, 
												  LONG* hoty, 
												  LONG* type,
												  IUnknown** ppBufferedImage)
{
	if( SwitchInputDesktop() )
	{
		m_bDesktopChanged = true;
	}
	if (x)*x = 0;
	if (y)*y = 0;
	if (hotx)*hotx = 0;
	if (hoty)*hoty = 0;
	if (type)*type = CURSOR_UNKNOWN;

	// get cursor info
	CURSORINFO  cursorinfo;
	memset( &cursorinfo,0,sizeof(cursorinfo) );
	cursorinfo.cbSize = sizeof(CURSORINFO);
	if(!GetCursorInfo(&cursorinfo) )
	{
		DebugOutF(filelog::log_error,"GetCursorInfo failed with %d",GetLastError() );
		return E_FAIL; 
	}
	DeleteObject(cursorinfo.hCursor);

	// if cursor doesn't changed since last call,we just return the last one
	if(	m_cursor_buffer &&
		(m_cursor_buffer->Cursor() == cursorinfo.hCursor )&& 
		m_cursor_stream)
	{
		if (x)*x = cursorinfo.ptScreenPos.x;
		if (y)*y = cursorinfo.ptScreenPos.y;
		if (hotx)*hotx = m_cursor_buffer->Hotspot().x;
		if (hoty)*hoty = m_cursor_buffer->Hotspot().y;
		if (type)*type = m_cursor_buffer->Type();
		return m_cursor_stream->QueryInterface(__uuidof(IUnknown),(void**)ppBufferedImage);
	}
	
	CRefObj<CCursorBuffer> bmp = new CCursorBuffer();
	if(!bmp->Create())
	{
		DebugOutF(filelog::log_error,"Create CCursorBuffer failed");
		return E_FAIL ;
	}
	CComPtr<CComObject<CScreenBufferStream> > pStream ;
	HRESULT hr = CComObject<CScreenBufferStream>::CreateInstance(&pStream);
	if(	FAILED(hr)	)
	{
		DebugOutF(filelog::log_error,"CreateInstance CScreenBufferStream failed with 0x%x",hr);
		return hr ;
	}
	pStream.p->AddRef(); // +1,then ref count == 1 
	pStream->SetBuffer(bmp);

	if (x)*x = bmp->Position().x;
	if (y)*y = bmp->Position().y;
	if (hotx)*hotx = bmp->Hotspot().x;
	if (hoty)*hoty = bmp->Hotspot().y;
	if (type)*type = bmp->Type();

	DebugOutF(filelog::log_info,("capture mouse cursor ok"));
	m_cursor_buffer = bmp;
	m_cursor_stream = pStream;
	return pStream->QueryInterface(__uuidof(IUnknown),(void**)ppBufferedImage);
}

STDMETHODIMP CWinRobotSession::InputChar(USHORT uchar)
{
	m_objInputThread.PushEvent(new CCharEvent(uchar));
	return S_OK;
}

STDMETHODIMP CWinRobotSession::SetKeyboardLayout(BSTR KLID)
{
	CRefObj<CSetKeyboardLayoutEvent> p = new CSetKeyboardLayoutEvent(KLID);
	if (!p->Load())
	{
		return E_FAIL;
	}
	m_objInputThread.PushEvent(p);
	return S_OK;
}
