/*! \file $Id: OverlaySurfaceCache.cpp 111 2011-10-30 14:40:09Z caoym $
*  \author caoym
*  \brief COverlaySurfaceCache
*/
#include "StdAfx.h"
#include "OverlaySurfaceCache.h"
#include "FileMapping.h"
#include "ScreenBuffer.h"
#include "YUV2RGB.h"
#include "debuglogger.h"
#include "OverlayBlend.h"

const UINT CAPTURE_SCREEN	= RegisterWindowMessage(_T("C5181DFB-E5C9-4465-A192-53E2B9CBA2B4"));
/*#include <ddraw.h>*/
//CFileMapping g_filemapping;
HRESULT WINAPI EnumSurfacesCallback7(
									 LPDIRECTDRAWSURFACE7 lpDDSurface,
									 LPDDSURFACEDESC2 lpDDSurfaceDesc,
									 LPVOID lpContext
									 )
{
	//When you use the DDENUMSURFACES_DOESEXIST flag, an enumerated surface's reference count is incremented
	if(lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
	{
		*(IDirectDrawSurface7**)lpContext = lpDDSurface;
		return DDENUMRET_CANCEL;
	}
	lpDDSurface->Release();
	return DDENUMRET_OK ;
}
//! look for shared memory
CMutexLock g_mLock("BB7EE116-AA4D-432a-8723-786B74A7C3E8");
bool DumpSurface(IDirectDrawSurface7*pSurface,LPCTSTR szPath)
{
	return true;
	TCHAR path[MAX_PATH]={0};
	GetModuleFileName(0,path,MAX_PATH);
	if(pSurface == 0) return false;
	if(szPath == 0) return false;

	bool bRtn = true;
	DDSURFACEDESC2 ddsd_dest = {0};
	ddsd_dest.dwSize = sizeof(DDSURFACEDESC2);

	if(FAILED(pSurface->Lock(0,(DDSURFACEDESC2*)&ddsd_dest,DDLOCK_READONLY|DDLOCK_DONOTWAIT,0)))
	{
		ddsd_dest.dwSize = sizeof(DDSURFACEDESC);
		if(FAILED(pSurface->Lock(0,(DDSURFACEDESC2*)&ddsd_dest,DDLOCK_READONLY|DDLOCK_DONOTWAIT,0)))
		{
			return false;
		}
	}

	HANDLE hf = INVALID_HANDLE_VALUE;
	try
	{
		hf = CreateFile((std::basic_string<TCHAR>(path) + szPath).c_str(), 
			GENERIC_READ | GENERIC_WRITE, 
			(DWORD) 0, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			(HANDLE) NULL); 

		if (hf == INVALID_HANDLE_VALUE) 
		{ 
			throw false;
		}
		DWORD cbWrite = 0;
		if (!WriteFile(hf, &ddsd_dest, sizeof(ddsd_dest), &cbWrite,  NULL)) 
		{
			throw false;
		}

		if (!WriteFile(hf, ddsd_dest.lpSurface, ddsd_dest.lPitch*ddsd_dest.dwHeight, &cbWrite,  NULL)) 
		{
			throw false;
		}

	}
	catch(bool b1)
	{
		bRtn = b1;
	}
	pSurface->Unlock(0);
	CloseHandle(hf);
	return bRtn;
}
WORD GetBitsCount(DWORD fourcc)
{
	struct FOURCC_MAP
	{
		DWORD fourcc;
		WORD nBitsCount;
	};
	const static FOURCC_MAP fourcc_map[] = 
	{
		{ MAKEFOURCC('U','Y','V','Y'),16},
		{ MAKEFOURCC('Y','U','Y','2'),16},
	};

	for (int i=0;i<RTL_NUMBER_OF(fourcc_map);i++)
	{
		if(fourcc_map[i].fourcc == fourcc)
			return fourcc_map[i].nBitsCount;
	}
	return 0;
}

COverlaySurfaceCache::COverlaySurfaceCache(void)
{
}

COverlaySurfaceCache::~COverlaySurfaceCache(void)
{
}
bool COverlaySurfaceCache::IsOverlaySurface(IDirectDrawSurface7*pSuf)
{
	if( pSuf == 0 ) return false;
	DDSCAPS2 caps ={0};
	if(FAILED(pSuf->GetCaps(&caps)))
	{
		OutputDebugStringA("GetCaps failed");
		return false;
	}
	bool bOverlay = ( caps.dwCaps & DDSCAPS_OVERLAY ) && ( caps.dwCaps & DDSCAPS_VIDEOMEMORY ) ;
	return bOverlay;
}
// DWORD COverlaySurfaceCache::GetSharedMemorySize(OVERLAY_ITEM &item)
// {
// 	//DDSCAPS2 ddscaps = {0};
// 	//ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
// 
// 	CComPtr<IUnknown>pUnk = 0;
// 	HRESULT hr = item.pOverlay->GetDDInterface((void**)&pUnk);
// 	if (FAILED(hr))
// 	{
// 		OutputDebugStringA("IDirectDrawSurface7::GetDDInterface failed");
// 		return 0;
// 	}
// 	CComPtr<IDirectDraw7> pDDraw;
// 
// 	hr = pUnk->QueryInterface(IID_IDirectDraw7,(void**)&pDDraw);
// 
// 	if (FAILED(hr))
// 	{
// 		OutputDebugStringA("get IID_IDirectDraw7 failed");
// 		return 0;
// 	}
// 	CComPtr<IDirectDrawSurface7> pPrimary ;//= item.pPrimary;
//  	{
//  		DDSURFACEDESC2   ddsd; 
//  		ZeroMemory(&ddsd,   sizeof(ddsd)); 
//  		ddsd.dwSize   =   sizeof(ddsd); 
//  		ddsd.dwFlags   =   DDSD_CAPS   ; 
//  		ddsd.ddsCaps.dwCaps   =   DDSCAPS_PRIMARYSURFACE   ;   
//  		pDDraw->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL,
//  			&ddsd,   &pPrimary,   EnumSurfacesCallback7);
//  	}
// 	if(pPrimary == 0 )
// 	{
// 		OutputDebugStringA("get primary surface failed");
// 		return 0;
// 	}
// 	DDSURFACEDESC2 ddsd_src = {0};
// 	ddsd_src.dwSize = sizeof(DDSURFACEDESC2);
// 	if(FAILED( pPrimary->GetSurfaceDesc((DDSURFACEDESC2*)&ddsd_src)) )
// 	{
// 		ddsd_src.dwSize = sizeof(DDSURFACEDESC);
// 		if(FAILED( pPrimary->GetSurfaceDesc((DDSURFACEDESC2*)&ddsd_src)) )
// 		{
// 			OutputDebugStringA("IDirectDrawSurface7::GetSurfaceDesc failed");
// 			return 0;
// 		}
// 	}
// 
// 	DWORD rgnsize = 0;
// // 	CComPtr<IDirectDrawClipper> pClipper;
// // 	if(SUCCEEDED(item.pOverlay->GetClipper(&pClipper)))
// // 	{
// // 		pClipper->GetClipList(0,0,&rgnsize);
// // 	}
// 
// 	DWORD size;
// 	DWORD imagesize ; 
// // 	if(item.bEntireDest)
// // 	{
// // 		imagesize = ddsd_src.lPitch * ddsd_src.dwHeight;
// // 		size = FIELD_OFFSET(SURFACE_DATA,data) + (rgnsize?rgnsize-sizeof(RGNDATAHEADER):0) + imagesize;
// // 	}
// // 	else
// 	{
// 		imagesize = CalculatePitch(CalculateLine(item.rcDst.right - item.rcDst.left,ddsd_src.ddpfPixelFormat.dwRGBBitCount)) * ddsd_src.dwHeight;
// 		size = FIELD_OFFSET(SURFACE_DATA,data) + (rgnsize?rgnsize-sizeof(RGNDATAHEADER):0) + imagesize;
// 	}
// 
// 	return size;
// }
void COverlaySurfaceCache::AddSurface(OVERLAY_ITEM&item)
{
	// is overlay?
	CAutoLockEx<CCrtSection> lock(m_csLock);
	m_surfaces.clear();
	m_pDDraw = 0;
	if(item.pOverlay)
	{
		CComPtr<IUnknown>pUnk = 0;
		HRESULT hr = item.pOverlay->GetDDInterface((void**)&pUnk);
		if (FAILED(hr))
		{
			OutputDebugStringA("IDirectDrawSurface7::GetDDInterface failed");
			return ;
		}
		hr = pUnk->QueryInterface(IID_IDirectDraw7,(void**)&m_pDDraw);
		if (FAILED(hr))
		{
			OutputDebugStringA("QueryInterface IID_IDirectDraw7 failed");
			return ;
		}
	}
	m_surfaces[item.pOverlay] = item;

}
void COverlaySurfaceCache::Clean()
{
	CAutoLockEx<CCrtSection> lock(m_csLock);
	m_surfaces.clear();
	m_pDDraw = 0;
}
void COverlaySurfaceCache::DelSurface(IDirectDrawSurface7*p)
{
	CAutoLockEx<CCrtSection> lock(m_csLock);
	m_surfaces.erase(p);
}
bool COverlaySurfaceCache::CaptureToSharedMemory()
{
	//return true;
	CAutoLockEx<CCrtSection> lock(m_csLock);
	if(m_pDDraw == 0) return false;
	CComPtr<IDirectDraw7>  pDDraw = m_pDDraw;
	INTERFACES surfaces = m_surfaces;
	lock.UnLock();

	for (INTERFACES::iterator it = surfaces.begin();
		it != surfaces.end();
		)
	{
		INTERFACES::iterator temp = it++;
		OVERLAY_ITEM item = temp->second;
		HRESULT  hr;
		DDSCAPS2 caps ={0};
		if (FAILED(hr = item.pOverlay->GetCaps(&caps)))
		{
			OutputDebugStringA("IDirectDrawSurface7::GetCaps failed");
			//m_surfaces.erase(temp);
			continue;
		}
		bool bOverlay = ( caps.dwCaps & DDSCAPS_OVERLAY ) && ( caps.dwCaps & DDSCAPS_VIDEOMEMORY ) ;
		if(! bOverlay )
		{
			//m_surfaces.erase(temp);
			continue;
		}
		if (!(caps.dwCaps & DDSCAPS_VISIBLE) )
		{
			//m_surfaces.erase(temp);
			continue;
		}
		if(item.pOverlay->IsLost() != DD_OK )
		{
			continue;
		}
		if(!CopyToSharedMemory(pDDraw,item))
		{
			//m_surfaces.erase(temp);
			continue;
		}
		/*CComPtr<IDirectDrawSurface7>  pDest = CopyToOffscreenSurface(temp->m_T);
		if(pDest == 0)
		{
		m_surfaces.erase(temp);
		continue;
		}
		HDC hdc = 0;
		hr = pDest->GetDC(&hdc);
		if (FAILED(hr))
		{
		OutputDebugStringA("IDirectDrawSurface7::GetDC failed");
		m_surfaces.erase(temp);
		continue;
		}
		CScreenBuffer screenbuf;
		RECT rc = {0,0,65535,65535};
		if(!screenbuf.Create(hdc,rc,kChaptureOverlayShareMemory))
		{
		OutputDebugStringA("CScreenBuffer::Create failed");
		pDest->ReleaseDC(hdc);
		return false;
		}
		OutputDebugStringA("COverlaySurfaceCache::Capture ok");
		pDest->ReleaseDC(hdc);*/
		return true;
	}
	return false;
}


CComPtr<IDirectDrawSurface7> COverlaySurfaceCache::CopyToOffscreenSurface(IDirectDraw7*pDDraw,const OVERLAY_ITEM&item)
{
	/*
	 * 1) copy primary surface image to off-screen surface as background.
	 * 2) overlay surface 
	*/
	if(item.pOverlay == 0 ) return 0;
	if(pDDraw == 0)return 0;
	HRESULT hr = S_OK;
	CComPtr<IDirectDrawSurface7> pPrimary ;//= item.pPrimary;
 	{
 		DDSURFACEDESC2   ddsd; 
 		ZeroMemory(&ddsd,   sizeof(ddsd)); 
 		ddsd.dwSize   =   sizeof(ddsd); 
 		ddsd.dwFlags   =   DDSD_CAPS   ; 
 		ddsd.ddsCaps.dwCaps   =   DDSCAPS_PRIMARYSURFACE   ;   
 		pDDraw->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL,
 			&ddsd,   &pPrimary,   EnumSurfacesCallback7);
 	}
	if(pPrimary == 0 )
	{
		OutputDebugStringA("get primary surface failed");
		return 0;
	}
	if (!SUCCEEDED(pPrimary->IsLost()))
	{
		OutputDebugStringA("IsLost");
		return 0;
	}
	DDSURFACEDESC2 ddsd_src = {0};
	ddsd_src.dwSize = sizeof(DDSURFACEDESC2);
	if(FAILED( pPrimary->GetSurfaceDesc((DDSURFACEDESC2*)&ddsd_src)) )
	{
		ddsd_src.dwSize = sizeof(DDSURFACEDESC);
		if(FAILED( pPrimary->GetSurfaceDesc((DDSURFACEDESC2*)&ddsd_src)) )
		{
			OutputDebugStringA("IDirectDrawSurface7::GetSurfaceDesc failed");
			return 0;
		}
	}

	DDSURFACEDESC2 ddsd_overlay = {0};
	ddsd_overlay.dwSize = sizeof(DDSURFACEDESC2);
	
	if(FAILED( hr = item.pOverlay->GetSurfaceDesc((DDSURFACEDESC2*)&ddsd_overlay)) )
	{
		ddsd_overlay.dwSize = sizeof(DDSURFACEDESC);
		if(FAILED( hr = item.pOverlay->GetSurfaceDesc((DDSURFACEDESC2*)&ddsd_overlay)) )
		{
			DebugOutF(filelog::log_error,"IDirectDrawSurface7::GetSurfaceDesc failed with 0x%x",hr );
			return 0;
		}
	}

	DDSURFACEDESC2 ddsd_dest = {0};
	ddsd_dest.dwSize = sizeof(DDSURFACEDESC2);
	ddsd_dest.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;

	ddsd_dest.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY ;
//	if (ddsd_overlay.ddpfPixelFormat.dwFourCC == 0) //RGB
	{
		ddsd_dest.dwFlags |= DDSD_PIXELFORMAT;
		ddsd_dest.ddpfPixelFormat = ddsd_src.ddpfPixelFormat;
	}
 	
	
// 	if(item.bEntireDest)
// 	{
// 		ddsd_dest.dwHeight = ddsd_src.dwHeight;
// 		ddsd_dest.dwWidth = ddsd_src.dwWidth;
// 	}
// 	else
	{
		ddsd_dest.dwHeight = item.rcDst.bottom - item.rcDst.top;
		ddsd_dest.dwWidth = item.rcDst.right - item.rcDst.left;
	}

	CComPtr<IDirectDrawSurface7> pDest;
	hr = pDDraw->CreateSurface(&ddsd_dest, &pDest, NULL);
	if(FAILED(hr))
	{
		OutputDebugStringA("CreateSurface failed");
		return 0;
	}

	hr = pDest->BltFast(0,0,pPrimary,/*item.bEntireDest?0:*/(LPRECT)&item.rcDst, DDBLTFAST_NOCOLORKEY|DDBLTFAST_DONOTWAIT);
	if(FAILED(hr))
	{
		OutputDebugStringA("IDirectDrawSurface7::BltFast failed");
		return 0;
	}
	DDBLTFX fx = {0};
	fx.dwSize = sizeof(DDBLTFX);
	const DDOVERLAYFX &overlayfx = item.fx.dwSize?item.fx:m_fx;
	fx.ddckDestColorkey = overlayfx.dckDestColorkey;
	fx.ddckSrcColorkey = overlayfx.dckSrcColorkey;
	
	fx.dwAlphaEdgeBlendBitDepth = overlayfx.dwAlphaEdgeBlendBitDepth;
	fx.dwAlphaEdgeBlend = overlayfx.dwAlphaEdgeBlend; 
	fx.dwAlphaDestConstBitDepth = overlayfx.dwAlphaDestConstBitDepth;
	fx.lpDDSAlphaDest = overlayfx.lpDDSAlphaDest;
	fx.dwAlphaSrcConstBitDepth = overlayfx.dwAlphaSrcConstBitDepth;
	fx.lpDDSAlphaSrc = overlayfx.lpDDSAlphaSrc;

	if (overlayfx.dwDDFX & DDOVERFX_ARITHSTRETCHY)
	{
		fx.dwDDFX |= DDBLTFX_ARITHSTRETCHY;
	}
	if (overlayfx.dwDDFX & DDOVERFX_MIRRORLEFTRIGHT)
	{
		fx.dwDDFX |= DDBLTFX_MIRRORLEFTRIGHT;
	}
	if (overlayfx.dwDDFX & DDOVERFX_MIRRORUPDOWN)
	{
		fx.dwDDFX |= DDBLTFX_MIRRORUPDOWN;
	}

	DWORD flags = 0;

 	if(item.flags & DDOVER_DDFX)
 	{
 		flags |= DDBLT_DDFX;
 	}
	if(item.flags & DDOVER_KEYDESTOVERRIDE)
	{
		flags |= DDBLT_KEYDESTOVERRIDE;
		if ((item.flags & DDOVER_DDFX) == 0)
		{
			hr = pPrimary->GetColorKey(DDCKEY_DESTOVERLAY,&fx.ddckDestColorkey);
		}
		//
	}
	if(item.flags & DDOVER_KEYSRCOVERRIDE)
	{
		flags |= DDBLT_KEYSRCOVERRIDE;
		if ((item.flags & DDOVER_DDFX) == 0)
		{
			item.pOverlay->GetColorKey(DDCKEY_SRCOVERLAY,&fx.ddckSrcColorkey);
		}
	}

	if (item.flags & DDOVER_KEYDEST)
	{
		flags |= DDBLT_KEYDEST;
	}
	if (item.flags & DDOVER_KEYSRC)
	{
		flags |= DDBLT_KEYSRC;
	}
	
	DDCOLORKEY colorkey = {0};
	if(SUCCEEDED(pPrimary->GetColorKey(DDCKEY_DESTBLT,&colorkey)))
		pDest->SetColorKey(DDCKEY_DESTBLT,&colorkey);

	if(SUCCEEDED(pPrimary->GetColorKey(DDCKEY_DESTOVERLAY,&colorkey)))
		pDest->SetColorKey(DDCKEY_DESTOVERLAY,&colorkey);

	if(SUCCEEDED(pPrimary->GetColorKey(DDCKEY_SRCBLT,&colorkey)))
		pDest->SetColorKey(DDCKEY_SRCBLT,&colorkey);

	if(SUCCEEDED(pPrimary->GetColorKey(DDCKEY_SRCOVERLAY,&colorkey)))
		pDest->SetColorKey(DDCKEY_SRCOVERLAY,&colorkey);

//  	if(SUCCEEDED(item.pOverlay->GetColorKey(DDCKEY_DESTBLT,&colorkey)))
//  		pDest->SetColorKey(DDCKEY_DESTBLT,&colorkey);
//  
//  	if(SUCCEEDED(item.pOverlay->GetColorKey(DDCKEY_DESTOVERLAY,&colorkey)))
//  		pDest->SetColorKey(DDCKEY_DESTOVERLAY,&colorkey);
//  
//  	if(SUCCEEDED(item.pOverlay->GetColorKey(DDCKEY_SRCBLT,&colorkey)))
//  		pDest->SetColorKey(DDCKEY_SRCBLT,&colorkey);
//  
//  	if(SUCCEEDED(item.pOverlay->GetColorKey(DDCKEY_SRCOVERLAY,&colorkey)))
//  		pDest->SetColorKey(DDCKEY_SRCOVERLAY,&colorkey);
	DumpSurface(pDest,_T("DumpSurface3.temp"));
	hr = SimulateOverlayBlt(pDDraw,pDest,0,item.pOverlay,(LPRECT)&item.rcSrc, flags,&fx);

	if(FAILED(hr))
	{
		OutputDebugStringA("IDirectDrawSurface7::Blt DDOVER_KEYDESTOVERRIDE failed");
		return 0;
	}
	else
	{
		//OutputDebugStringA("CopyToOffscreenSurface ok");
	}
	return pDest;
}

bool COverlaySurfaceCache::CopyToSharedMemory(IDirectDraw7*pDDraw,const OVERLAY_ITEM& item)
{
	CAutoLockEx<CMutexLock> lock(g_mLock);
	
	if( !item.pOverlay ) return false;

	DumpSurface(item.pOverlay,_T("DumpSurface1.temp"));
	CComPtr<IDirectDrawSurface7> pTemp = CopyToOffscreenSurface(pDDraw,item);
	DumpSurface(pTemp,_T("DumpSurface2.temp"));
	if(!pTemp) 
	{
		OutputDebugStringA("CopyToOffscreenSurface failed");
		return false;
	}

	HRESULT hr=S_OK;
	DDSURFACEDESC2 ddsd_dest = {0};
	ddsd_dest.dwSize = sizeof(DDSURFACEDESC2);

	hr = pTemp->Lock(0,&ddsd_dest,DDLOCK_READONLY|DDLOCK_DONOTWAIT,0);
	if (FAILED(hr))
	{
		OutputDebugStringA("IDirectDrawSurface7::Lock failed");
		pTemp->Unlock(0);
		return false;
	}
	try
	{
		DWORD rgnsize = 0;
// 		CComPtr<IDirectDrawClipper> pClipper;
// 		if(SUCCEEDED(item.pOverlay->GetClipper(&pClipper)))
// 		{
// 			pClipper->GetClipList(0,0,&rgnsize);
// 		}

		DWORD size;
		DWORD imagesize ;


		imagesize = CalculatePitch(CalculateLine(ddsd_dest.dwWidth,ddsd_dest.ddpfPixelFormat.dwRGBBitCount)) * ddsd_dest.dwHeight;
		size = FIELD_OFFSET(SURFACE_DATA,data) + (rgnsize?rgnsize-sizeof(RGNDATAHEADER):0) + imagesize;


		CFileMapping mem;
		if(!mem.Open(kChaptureOverlayShareMemory,size,false))
		{
			// open failed ,maybe size is too small
			if(mem.Open(kChaptureOverlayShareMemory,sizeof(SURFACE_DATA),false))
			{
				SURFACE_DATA*pData = (SURFACE_DATA*)mem.GetPointer();
				pData->size = size;
				pData->desc = ddsd_dest;
				pData->modified = TRUE;
			}
			OutputDebugStringA("CFileMappingBitmap::Open failed");
			throw E_FAIL;
		}
		SURFACE_DATA*pData = (SURFACE_DATA*)mem.GetPointer();
		pData->size = size;
		pData->desc = ddsd_dest;
		pData->modified = TRUE;
// 		if(pClipper && rgnsize)
// 		{
// 			if(FAILED(pClipper->GetClipList(0,(LPRGNDATA)&pData->rdh,&rgnsize)))
// 			{
// 				OutputDebugStringA("IDirectDrawSurface7::GetClipList failed");
// 				throw E_FAIL;
// 			}
// 		}

// 		if(item.bEntireDest)
// 		{
// 			DDSURFACEDESC2 des = {0};
// 			des.dwSize = sizeof(DDSURFACEDESC2);
// 			if(FAILED(pPrimary->GetSurfaceDesc(&des)))
// 			{
// 				OutputDebugStringA("IDirectDrawSurface7::GetSurfaceDesc failed");
// 				throw E_FAIL;
// 			}
// 			pData->rcDst.left = pData->rcDst.top = 0;
// 			pData->rcDst.right = des.dwWidth;
// 			pData->rcDst.bottom = des.dwHeight;
// 		}
// 		else
			pData->rcDst = item.rcDst;

		pData->flags= item.flags; 
		pData->fx = item.fx;

		BYTE *pBegin = pData->data + (rgnsize?rgnsize-sizeof(RGNDATAHEADER):0);
		pData->desc.lPitch = CalculatePitch(CalculateLine(ddsd_dest.dwWidth,ddsd_dest.ddpfPixelFormat.dwRGBBitCount));
		//int left ;
		//left = CalculatePitch(CalculateLine(item.rcDst.left,ddsd_dest.ddpfPixelFormat.dwRGBBitCount));

		for (int i= 0;i != ddsd_dest.dwHeight;i++)
		{
			memcpy( pBegin + i *  pData->desc.lPitch ,
				(BYTE*)ddsd_dest.lpSurface + i*ddsd_dest.lPitch ,
				pData->desc.lPitch);
		}

// 		DebugOutF(filelog::log_info,"CopyToSharedMemory (%d,%d,%d,%d) -> (%d,%d,%d,%d) ok",
// 			item.rcSrc.left,
// 			item.rcSrc.top,
// 			item.rcSrc.right,
// 			item.rcSrc.bottom,
// 			item.rcDst.left,
// 			item.rcDst.top,
// 			item.rcDst.right,
// 			item.rcDst.bottom);
	}
	catch(HRESULT hr1)
	{
		hr = hr1;
	}
	pTemp->Unlock(0);
	return SUCCEEDED(hr);
}

bool COverlaySurfaceCache::CaptureFromSharedMemory(HDC hdc,LPCRECT prcDest)
{
	
	if(prcDest ==0 ) return false;

	static CFileMapping shared_flags;
	//1.create filemapping object for sharing memory  
	if(!shared_flags.IsOpen())
	{
		if(!shared_flags.Open(kFlagsShareMemory,sizeof(DWORD),false ))
		{
			DebugOutF(filelog::log_error,"open shared_flags failed,could not open shared memory");
			return false;
		}
	}
	DWORD *pFlags = (DWORD *)shared_flags.GetPointer();
	if(*pFlags == 0) return false;
	TrackDebugOut;
	static LONG tick = GetTickCount();//make sure that message HWND_BROADCAST will be handle once.

	static CFileMapping mem;
	//1.create filemapping object for sharing memory  
	if(!mem.IsOpen())
	{
		if(!mem.Open(kChaptureOverlayShareMemory,sizeof(SURFACE_DATA),true ))
		{
			DebugOutF(filelog::log_error,"CaptureFromSharedMemory failed,could not open shared memory");
			return false;
		}
		//2.Notify processes which using DirectDaw overlay.
		SURFACE_DATA*pDate = static_cast<SURFACE_DATA*>( mem.GetPointer());
		pDate->modified = FALSE;
		SendMessageTimeout(HWND_BROADCAST,CAPTURE_SCREEN,++tick,0,SMTO_ABORTIFHUNG,10000,0);
		if(!pDate->modified)
		{
			DebugOutF(filelog::log_debug,"no overlay alive");
			return false;
		}
	}
	// resize shared memory if necessarily  
	int i = 0;
	for(i=0;i<3;++i)
	{
		SURFACE_DATA*pDate = static_cast<SURFACE_DATA*>( mem.GetPointer());
		if(pDate->size + sizeof(SURFACE_DATA ) > mem.GetSize() )
		{
			if(!mem.Open(kChaptureOverlayShareMemory,pDate->size + sizeof(SURFACE_DATA ),true ))
			{
				DebugOutF(filelog::log_error,"CaptureFromSharedMemory failed,could not open shared memory");
				return false;
			}
			pDate = static_cast<SURFACE_DATA*>( mem.GetPointer());
		}
		//Notify processes which using DirectDaw overlay.
		pDate->modified = FALSE;
		SendMessageTimeout(HWND_BROADCAST,CAPTURE_SCREEN,++tick,0,SMTO_ABORTIFHUNG,10000,0);
		if(!pDate->modified ) 
		{
			DebugOutF(filelog::log_debug,"no overlay alive");
			return false;
		}
		if(pDate->size + sizeof(SURFACE_DATA ) > mem.GetSize() ) continue;
		break;
	}
	/*return false;*/
	if(i == 3) return false;
	CAutoLockEx<CMutexLock> lock(g_mLock);
	//open and get the size of shared memory
	//CFileMapping mem;
// 	if(!mem.Open(kChaptureOverlayShareMemory,sizeof(SURFACE_DATA),false))
// 	{
// 		DebugOutF(filelog::log_error,"CaptureFromSharedMemory failed,could not open shared memory");
// 		return false;
// 	}
// 	SURFACE_DATA* pData = (SURFACE_DATA*)mem.GetPointer();
// 	DWORD size = pData->size;
// 	mem.Destroy();
// 	if(size < sizeof(SURFACE_DATA)) return false;
// 	// then get open again with the size
// 	if(!mem.Open(kChaptureOverlayShareMemory,size,false))
// 	{
// 		DebugOutF(filelog::log_error,"CaptureFromSharedMemory failed,could not open shared memory as size %d",size);
// 		return false;
// 	}

	SURFACE_DATA*pData = (SURFACE_DATA*)mem.GetPointer();
	// the area to be copy
	RECT dest ={0},src={0},intersect={0};
	if(IsRectEmpty(&pData->rcDst)) 
	{
		DebugOutF(filelog::log_debug,"CaptureFromSharedMemory dest rect is empty");
		return true;
	}
	IntersectRect(&intersect,prcDest,&pData->rcDst);
	if(IsRectEmpty(&intersect)) 
	{
		DebugOutF(filelog::log_debug,"CaptureFromSharedMemory dest rect is empty");
		return true;
	}

	HRGN hrgn = 0;
	HBITMAP hBmp = 0;
	HDC hMemDC = 0;

	dest.left = intersect.left - prcDest->left;
	dest.top = intersect.top - prcDest->top;
	int w = intersect.right-intersect.left;
	int h = intersect.bottom-intersect.top;
	dest.right = dest.left +w;
	dest.bottom = dest.top +h;

	src.left = (intersect.left - pData->rcDst.left)*pData->desc.dwWidth/(pData->rcDst.right-pData->rcDst.left);
	src.top = (intersect .top -pData->rcDst.top)*pData->desc.dwHeight/(pData->rcDst.bottom-pData->rcDst.top);
	w = w*pData->desc.dwWidth/(pData->rcDst.right-pData->rcDst.left);
	h = h*pData->desc.dwHeight/(pData->rcDst.bottom-pData->rcDst.top);
	src.right = src.left +w;
	src.bottom = src.top +h;


	bool bOK = true;
	try
	{

		if (pData->desc.ddpfPixelFormat.dwFlags & DDPF_FOURCC)
		{
			DebugOutF(filelog::log_error,"unsupport color space 0x%x",pData->desc.ddpfPixelFormat.dwFourCC);
			throw false;
		}
// 		if(pData->rdh.nCount )
// 		{
// 			hrgn = ExtCreateRegion(0,pData->rdh.nRgnSize,(RGNDATA*)&pData->rdh);
// 			if(prcDest) OffsetRgn(hrgn,-prcDest->left,-prcDest->top);
// 			SelectClipRgn(hdc,hrgn);
// 		}
		{
			BITMAPINFOHEADER infhead ;
			ZeroMemory(&infhead,sizeof(infhead));

			infhead.biSize = sizeof(BITMAPINFOHEADER);

			infhead.biBitCount = (WORD)pData->desc.ddpfPixelFormat.dwRGBBitCount;

			int imagesize =  pData->desc.lPitch* pData->desc.dwHeight;

			infhead.biPlanes	= 1;
			infhead.biHeight	= -LONG(pData->desc.dwHeight);
			infhead.biWidth		= pData->desc.dwWidth;
			infhead.biSizeImage	= imagesize ;
			infhead.biCompression = BI_RGB;
			hMemDC  = CreateCompatibleDC(hdc);

			if(hMemDC == NULL){
				DebugOutF(filelog::log_error,"CreateCompatibleDC failed with %d",GetLastError());
				throw false;
			}


			{
				hBmp = CreateDIBSection(hMemDC,
					(BITMAPINFO *)&infhead,
					DIB_RGB_COLORS, 0,mem.GetHandle(), FIELD_OFFSET(SURFACE_DATA,data) + (pData->rdh.nRgnSize?pData->rdh.nRgnSize-sizeof(RGNDATAHEADER):0));

				if(hBmp == NULL){
					DebugOutF(filelog::log_error,"CreateDIBSection failed with %d",GetLastError() );
					throw false;
				}
			}

			SelectObject(hMemDC,hBmp);

			if (((dest.right-dest.left) == (src.right-src.left)) &&
				((dest.bottom-dest.top) == (src.bottom-src.top))
				)
			{
				if(!BitBlt( hdc,
					dest.left,
					dest.top,
					dest.right-dest.left,
					dest.bottom-dest.top,
					hMemDC,
					src.left,
					src.top,
					SRCCOPY))
				{
					DebugOutF( filelog::log_error,"StretchBlt failed with %d",GetLastError() );
					throw false;
				}
			}
			else
			{
				if(!StretchBlt (hdc,
					dest.left,
					dest.top,
					dest.right-dest.left,
					dest.bottom-dest.top,
					hMemDC,
					src.left,
					src.top,
					src.right-src.left,
					src.bottom-src.top,
					SRCCOPY))
				{
					DebugOutF(filelog::log_error,"StretchBlt failed with %d",GetLastError());
					throw false;
				}
			}

// 			DebugOutF(filelog::log_info,"CaptureFromSharedMemory (w:%d,h:%d,FourCC:0x%x,RGBBitCount:%d,l:%d,t:%d,r:%d,b:%d)ok",
// 				pData->desc.dwWidth,
// 				pData->desc.dwHeight,
// 				pData->desc.ddpfPixelFormat.dwFourCC,
// 				pData->desc.ddpfPixelFormat.dwRGBBitCount,
// 				pData->rcDst.left,
// 				pData->rcDst.top,
// 				pData->rcDst.right,
// 				pData->rcDst.bottom
// 				);
			//return true;


		}

	}
	catch (bool b1)
	{
		bOK = b1;
	}

	if(hMemDC)DeleteDC(hMemDC);
	if(hBmp)DeleteObject(hBmp);
	if(hrgn)DeleteObject(hrgn);
	DebugOutF(filelog::log_debug,"CaptureFromSharedMemory ok");
	return bOK;
}