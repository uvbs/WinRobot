/*! \file $Id: ScreenBufferStream.cpp 75 2011-07-31 14:54:01Z caoym $
*  \author caoym
*  \brief CScreenBufferStream			
*/

#include "stdafx.h"
#include "ScreenBufferStream.h"
#include "FileMappingBitmap.h"
#include "debuglogger.h"
// CScreenBufferStream

HRESULT STDMETHODCALLTYPE CScreenBufferStream::Read( 
	/* [length_is][size_is][out] */ void *pv,
	/* [in] */ ULONG cb,
	/* [out] */ ULONG *pcbRead) 
{
	

	if (!pv) return E_POINTER;
	if(pcbRead)*pcbRead = 0;
	
	ULONG cbRead = min(cb,m_pBuf->GetFileSize() - m_nPos);
	if (cbRead == 0)
	{
		return S_FALSE;
	}
	memcpy(pv,(char*)m_pBuf->GetFilePointer() + m_nPos,cbRead);
	m_nPos += cbRead ;
	if(pcbRead)*pcbRead = cbRead;
	return S_OK;


}
STDMETHODIMP CScreenBufferStream::get_Size(ULONG* pVal)
{
	if(pVal == 0) return E_POINTER;
	*pVal = m_pBuf->GetFileSize(); 
	return S_OK;
}

STDMETHODIMP CScreenBufferStream::get_FileMappingName(BSTR* pVal)
{
	if (pVal == 0) return E_POINTER;
	*pVal = CComBSTR(m_pBuf->GetFileMappingName()).Detach();
	return S_OK;
}
