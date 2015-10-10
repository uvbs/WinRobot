/*! \file $Id: ScreenBufferStream.h 64 2011-06-15 14:01:00Z caoym $
*  \author caoym
*  \brief CScreenBufferStream			
*/
#pragma once
#include "resource.h"       
#include "WinRobotCore_i.h"
#include "IBitmapBuffer.h"

class CFileMappingBitmap;

//! ScreenBufferStream
/*!
	implement ISequentialStream interface
*/
class ATL_NO_VTABLE CScreenBufferStream :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CScreenBufferStream, &CLSID_ScreenBufferStream>,
	public IDispatchImpl<IScreenBufferStream, &IID_IScreenBufferStream, &LIBID_WinRobotCoreLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISequentialStream
{
public:
	CScreenBufferStream()
		:m_nPos(0)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SCREENBUFFERSTREAM)


BEGIN_COM_MAP(CScreenBufferStream)
	COM_INTERFACE_ENTRY(IScreenBufferStream)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISequentialStream)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	bool SetBuffer(CFileMappingBitmap*pBuf)
	{
		if(pBuf == 0) return false;
		m_pBuf = pBuf;

		return true;
	}
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read( 
		/* [length_is][size_is][out] */ void *pv,
		/* [in] */ ULONG cb,
		/* [out] */ ULONG *pcbRead) ;

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write( 
		/* [size_is][in] */ const void *pv,
		/* [in] */ ULONG cb,
		/* [out] */ ULONG *pcbWritten)
	{
		return E_NOTIMPL;
	}
	//CScreenBuffer& Buffer(){return m_buffer;}
public:
	ULONG m_nPos;
	CRefObj<CFileMappingBitmap> m_pBuf;
	STDMETHOD(get_Size)(ULONG* pVal);
	STDMETHOD(get_FileMappingName)(BSTR* pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(ScreenBufferStream), CScreenBufferStream)
