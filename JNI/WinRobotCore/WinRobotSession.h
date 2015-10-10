/*! \file $Id: WinRobotSession.h 118 2011-11-15 16:22:30Z caoym $
*  \author caoym
*  \brief CWinRobotSession
*/
#pragma once
#include "resource.h"       
#include "WinRobotCore_i.h"
#include "InputThread.h"
#include "ScreenCapture.h"
class CCursorBuffer;
class CScreenBufferStream;
//! WinRobot session
/*!
 *	run in each Windows session to capture screen image
 *	and input mouse or keyboard event
 * \see IWinRobotSession
 */
class ATL_NO_VTABLE CWinRobotSession :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CWinRobotSession, &CLSID_WinRobotSession>,
	public IConnectionPointContainerImpl<CWinRobotSession>,
	public IDispatchImpl<IWinRobotSession, &IID_IWinRobotSession, &LIBID_WinRobotCoreLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CWinRobotSession()
		:m_bDesktopChanged(false)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_WINROBOTSESSION)


BEGIN_COM_MAP(CWinRobotSession)
	COM_INTERFACE_ENTRY(IWinRobotSession)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CWinRobotSession)
	
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

public:
	/** \see IWinRobotSession::CreateScreenCapture*/
	STDMETHOD(CreateScreenCapture)(LONG nX, LONG nY, LONG nWidth, LONG nHeight, IUnknown** ppBufferedImage);
	/** \see IWinRobotSession::Delay*/
	STDMETHOD(Delay)(ULONG ms);
	/** \see IWinRobotSession::get_AutoDelay*/
	STDMETHOD(get_AutoDelay)(ULONG* pVal);
	/** \see IWinRobotSession::AutoDelay*/
	STDMETHOD(put_AutoDelay)(ULONG newVal);
	/** \see IWinRobotSession::GetPixelColor*/
	STDMETHOD(GetPixelColor)(LONG x, LONG y, OLE_COLOR* color);
	/** \see IWinRobotSession::get_AutoWaitForIdle*/
	STDMETHOD(get_AutoWaitForIdle)(VARIANT_BOOL* pVal);
	/** \see IWinRobotSession::AutoWaitForIdle*/
	STDMETHOD(put_AutoWaitForIdle)(VARIANT_BOOL newVal);
	/** \see IWinRobotSession::WaitForIdle*/
	STDMETHOD(WaitForIdle)(void);
	/** \see IWinRobotSession::KeyPress*/
	STDMETHOD(KeyPress)(ULONG keycode);
	/** \see IWinRobotSession::KeyRelease*/
	STDMETHOD(KeyRelease)(ULONG keycode);
	/** \see IWinRobotSession::MouseMove*/
	STDMETHOD(MouseMove)(LONG x,LONG y);
	/** \see IWinRobotSession::MousePress*/
	STDMETHOD(MousePress)(ULONG buttons);
	/** \see IWinRobotSession::MouseRelease*/
	STDMETHOD(MouseRelease)(ULONG buttons);
	/** \see IWinRobotSession::MouseWheel*/
	STDMETHOD(MouseWheel)(LONG wheelAmt);
private:
	CScreenCapture m_screen_cap;
	CInputThread m_objInputThread;
	bool m_bDesktopChanged;
	//storage last captured cursor,to decide whether 
	//cursor is change sine the last capture
	CRefObj<CCursorBuffer> m_cursor_buffer;					
	CRefObj< CComObject<CScreenBufferStream > > m_cursor_stream;
public:
	/** \see IWinRobotSession::CaptureMouseCursor*/
	STDMETHOD(CaptureMouseCursor)(LONG* x, LONG* y, LONG* hotx, LONG* hoty,LONG* type, IUnknown** ppBufferedImage);
	/** \see IWinRobotSession::InputChar*/
	STDMETHOD(InputChar)(USHORT uchar);
	/** \see IWinRobotSession::SetKeyboardLayout*/
	STDMETHOD(SetKeyboardLayout)(BSTR KLID);
};

OBJECT_ENTRY_AUTO(__uuidof(WinRobotSession), CWinRobotSession)
