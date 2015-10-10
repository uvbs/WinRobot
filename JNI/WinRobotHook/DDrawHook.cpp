/*! \file $Id: DDrawHook.cpp 78 2011-08-10 13:35:23Z caoym $
*  \author caoym
*  \brief DirectDraw Hook
*/


#include "StdAfx.h"
#define INITGUID
#include <cguid.h> 
#include <objbase.h>
#include <initguid.h>
#include <ddraw.h>

#include "DDrawHook.h"
#include "FunctionHook.h"
#include "WinSysDLL.h"
#include <tchar.h>
#include <atlbase.h>
#include "debuglogger.h"
#include "OverlaySurfaceCache.h"
#include "WinRobotHook.h"
#include "FileMapping.h"

//////////////////////////////////////////////////////////////////////////
//1.InstallDDrawHook with SetWindowsHookEx,so this dll will be load into every process
//2.InjectDDrawHook be called in DllMain when this dll is loaded,the memory of the API
//  calls(IDirectDrawSurface7::Flip,IDirectDrawSurface7::UpdateOverlay) will be modified,
//  inject the code we need,and we can get the overlay surface.
//3.Every time those API is called,we save the overlay surface pointer to memory.
//4.When we need to capture the screen,we broadcast CAPTURE_SCREEN message,each process 
//  receives this message save the overlay surface to named file mapping object.
//////////////////////////////////////////////////////////////////////////



typedef HRESULT (STDMETHODCALLTYPE *CALL_Blt)(LPDIRECTDRAWSURFACE7, LPRECT,LPDIRECTDRAWSURFACE7, LPRECT,DWORD, LPDDBLTFX) ;
typedef HRESULT (STDMETHODCALLTYPE *CALL_BltBatch)(LPDIRECTDRAWSURFACE7, LPDDBLTBATCH, DWORD, DWORD ) ;
typedef HRESULT (STDMETHODCALLTYPE *CALL_BltFast)(LPDIRECTDRAWSURFACE7, DWORD,DWORD,LPDIRECTDRAWSURFACE7, LPRECT,DWORD) ;
typedef HRESULT (STDMETHODCALLTYPE *CALL_Flip)(LPDIRECTDRAWSURFACE7, LPDIRECTDRAWSURFACE7, DWORD) ;
typedef HRESULT (STDMETHODCALLTYPE *CALL_UpdateOverlay)(LPDIRECTDRAWSURFACE7 , LPRECT, LPDIRECTDRAWSURFACE7,LPRECT,DWORD, LPDDOVERLAYFX);
typedef ULONG   (STDMETHODCALLTYPE *CALL_AddRef)(LPDIRECTDRAWSURFACE7);
typedef ULONG   (STDMETHODCALLTYPE *CALL_Release)(LPDIRECTDRAWSURFACE7);

//! Get overlay information from the memory of a DirectDrawSurface7 object
typedef struct _DirectDrawSurface7Data 
{
	BYTE offset0[4];

	struct {
		BYTE offset0 [100];
		struct
		{
			BYTE offset0 [12];
			struct  
			{
				BYTE offset0 [8];
				IDirectDrawSurface7*pPrimary;
			}*next;
			RECT rcOverlaySrc;
			RECT rcOverlayDest;
			BYTE offset1 [52];
			DWORD nOverlayDestFlags;
			BYTE offset2 [8];
			DDOVERLAYFX*pOverlayFx;	 
		}*next;
	}*next;



}DirectDrawSurface7Data;
COverlaySurfaceCache g_cache;

//! Get OVERLAY_ITEM from a IDirectDrawSurface7 object.
bool GetOverlayItem(IDirectDrawSurface7*pOverlay,OVERLAY_ITEM& item)
{
	DirectDrawSurface7Data * pData = (DirectDrawSurface7Data *)(pOverlay);
	//item.bEntireDest = item.bEntireSrc = false;
	item.pOverlay = pOverlay;
	memset(&item.fx,0,sizeof(item.fx));
	if(pData == 0) return false;

	try
	{
		//item.pPrimary = pData->pPrimary;
		if (pData->next && pData->next->next)
		{
			item.flags = pData->next->next->nOverlayDestFlags;
			item.rcSrc = pData->next->next->rcOverlaySrc;
			item.rcDst = pData->next->next->rcOverlayDest;

			if(pData->next->next->pOverlayFx)
			{
				item.fx = *pData->next->next->pOverlayFx;
				g_cache.SetOverlayFX(pData->next->next->pOverlayFx);
			}

			//item.pPrimary = pData->next->next->next->pPrimary;
		}
	}
	catch (...)
	{
		return false;
	}
	// 	DWORD key =0;
	// 	DWORD offset = 0;
	// 	if(key)
	// 	{
	// 		BYTE*pBegin = (BYTE*)pOverlay;
	// 		try
	// 		{
	// 			while(1)
	// 			{
	// 				if(*(DWORD*)pBegin == key) break;
	// 				pBegin ++;
	// 			}
	// 		}
	// 		catch (...)
	// 		{
	// 		}
	// 		offset = (BYTE*)pOverlay -pBegin; 
	// 	}

	return true;

}


static LONG g_nOverlayCaptureTick = 0;

static CALL_Blt			g_CALL_Blt		= 0;
static CALL_BltBatch	g_CALL_BltBatch = 0;
static CALL_BltFast		g_CALL_BltFast	= 0;
static CALL_Flip		g_CALL_Flip		= 0;
static CALL_UpdateOverlay g_CALL_UpdateOverlay	= 0;
static CALL_AddRef g_CALL_AddRef	= 0;
static CALL_Release g_CALL_Release	= 0;
static CFunctionHook<CALL_Blt >		g_hook_Blt ;
static CFunctionHook<CALL_BltBatch>	g_hook_BltBatch;
static CFunctionHook<CALL_BltFast>	g_hook_BltFast ;
static CFunctionHook<CALL_Flip>		g_hook_Flip ;
static CFunctionHook<CALL_UpdateOverlay> g_hook_UpdateOverlay ;
static CFunctionHook<CALL_AddRef> g_hook_AddRef ;
static CFunctionHook<CALL_Release> g_hook_Release ;
#pragma data_seg(".SHARED")

UINT_PTR IDirectDrawSurface7_Flip_offset = 0;
UINT_PTR IDirectDrawSurface7_Blt_offset = 0;
UINT_PTR IDirectDrawSurface7_BltFast_offset = 0;
UINT_PTR IDirectDrawSurface7_BltBatch_offset = 0;
UINT_PTR IDirectDrawSurface7_UpdateOverlay_offset = 0;
UINT_PTR IDirectDrawSurface7_AddRef_offset = 0;
UINT_PTR IDirectDrawSurface7_Release_offset = 0;
HHOOK g_callwndproc = 0;
bool g_bDDrawInit = false;
bool g_bDDrawKeep = false;

#pragma data_seg()

bool g_bDDrawHookInit = false;
static bool g_bInjected = false;


void CacheSurface(LPDIRECTDRAWSURFACE7 pSuf);

HRESULT STDMETHODCALLTYPE HookBlt(LPDIRECTDRAWSURFACE7 pSuf, LPRECT rcdest,LPDIRECTDRAWSURFACE7 pSrc, LPRECT rcsrc,DWORD dw, LPDDBLTFX lpdblefx);
HRESULT STDMETHODCALLTYPE HookBltBatch(LPDIRECTDRAWSURFACE7 pSuf, LPDDBLTBATCH lpbb, DWORD dw1, DWORD dw2 );
HRESULT STDMETHODCALLTYPE HookBltFast(LPDIRECTDRAWSURFACE7 pSuf, DWORD dw1,DWORD dw2,LPDIRECTDRAWSURFACE7 pSrc, LPRECT rc,DWORD dw3);
HRESULT STDMETHODCALLTYPE HookFlip(LPDIRECTDRAWSURFACE7 pSuf, LPDIRECTDRAWSURFACE7 pSrc, DWORD dw);
HRESULT STDMETHODCALLTYPE HookUpdateOverlay(LPDIRECTDRAWSURFACE7 , LPRECT, LPDIRECTDRAWSURFACE7,LPRECT,DWORD, LPDDOVERLAYFX);
ULONG STDMETHODCALLTYPE HookAddRef(LPDIRECTDRAWSURFACE7);
ULONG STDMETHODCALLTYPE HookRelease(LPDIRECTDRAWSURFACE7);
//! ddraw.dll
class DDrawDLL
	:public CDLLLib
{
public:
	DDrawDLL();
	HRESULT DirectDrawCreateEx(GUID FAR * , LPVOID  *, REFIID  ,IUnknown FAR * );
	typedef HRESULT (WINAPI* DirectDrawCreateEx_fn)( GUID FAR * , LPVOID  *, REFIID  ,IUnknown FAR * );
	DirectDrawCreateEx_fn DirectDrawCreateExFN;
};

DDrawDLL::DDrawDLL()
:CDLLLib(_T("ddraw.dll"))
,DirectDrawCreateExFN(0)
{
	DirectDrawCreateExFN =
		(DirectDrawCreateEx_fn)GetProcAddress(m_hLib,("DirectDrawCreateEx"));
}
HRESULT DDrawDLL::DirectDrawCreateEx(GUID FAR *clsid , LPVOID  *pVoid, REFIID  iid,IUnknown FAR *pUk )
{
	if(DirectDrawCreateExFN) return DirectDrawCreateExFN(clsid,pVoid,iid ,pUk);
	return E_FAIL;
}

LRESULT CALLBACK DDDrawHookProc_CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION){

		CWPSTRUCT *cwpStruct = (CWPSTRUCT *) lParam;
		if(CAPTURE_SCREEN == cwpStruct->message)
		{
			// capture while the first time current process received the message 
			if(g_nOverlayCaptureTick != cwpStruct->wParam) 
			{
				//OutputDebugStringA("receive capture screen message");
				g_nOverlayCaptureTick = cwpStruct->wParam;
				CaptureToSharedMemory();
			}
			return TRUE;
		}

	}
	return CallNextHookEx(g_callwndproc, nCode, wParam, lParam) ;
}
CFileMapping g_shared_flags;
bool InstallDDrawHook(HINSTANCE inst)
{
	if(g_bDDrawHookInit) return true;

	if(!g_shared_flags.IsOpen())
	{
		if(!g_shared_flags.Open(kFlagsShareMemory,sizeof(DWORD),true ))
		{
			DebugOutF(filelog::log_error,"open g_shared_flags  failed,could not open shared memory");
			return false;
		}
		DWORD *pFlags = (DWORD *)g_shared_flags.GetPointer();
		*pFlags = 0;
	}
	if(!g_bDDrawInit) 
	{
		static DDrawDLL dll;
		CComPtr<IDirectDraw7> pDDraw;

		HRESULT hr = dll.DirectDrawCreateEx(NULL,(VOID**)&pDDraw,IID_IDirectDraw7,NULL);
		if ( FAILED(hr))
		{
			DebugOutF(filelog::log_error,"DirectDrawCreateEx failed with 0x%x",hr);
			return false;
		}
		hr = pDDraw->SetCooperativeLevel(NULL,DDSCL_NORMAL);
		if ( FAILED(hr))
		{
			DebugOutF(filelog::log_error,"SetCooperativeLevel failed with 0x%x",hr);
			return false;
		}
		DDSURFACEDESC2 ddsd;
		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		CComPtr<IDirectDrawSurface7> primary ;
		hr = pDDraw->CreateSurface(&ddsd, &primary, 0);
		if ( FAILED(hr))
		{
			DebugOutF(filelog::log_error,"CreateSurface failed with 0x%x",hr);
			return false;
		}
		UINT_PTR* pVTable = (UINT_PTR*)(*((UINT_PTR*)primary.p));
		IDirectDrawSurface7_Flip_offset		= UINT_PTR(pVTable[11]) - UINT_PTR(dll.m_hLib);
		IDirectDrawSurface7_Blt_offset		= UINT_PTR(pVTable[5]) - UINT_PTR(dll.m_hLib);
		IDirectDrawSurface7_BltFast_offset	= UINT_PTR(pVTable[7]) - UINT_PTR(dll.m_hLib);
		IDirectDrawSurface7_BltBatch_offset = UINT_PTR(pVTable[6]) - UINT_PTR(dll.m_hLib);
		IDirectDrawSurface7_UpdateOverlay_offset = UINT_PTR(pVTable[33]) - UINT_PTR(dll.m_hLib);
		IDirectDrawSurface7_AddRef_offset = UINT_PTR(pVTable[1]) - UINT_PTR(dll.m_hLib);
		IDirectDrawSurface7_Release_offset = UINT_PTR(pVTable[2]) - UINT_PTR(dll.m_hLib);
		g_bDDrawInit = true;
	}
	g_bDDrawKeep = true;

	g_callwndproc = SetWindowsHookEx(WH_CALLWNDPROC,&DDDrawHookProc_CallWndProc,inst,0);

	if(!g_callwndproc)
	{
		DebugOutF(filelog::log_error,"SetWindowsHookEx WH_CALLWNDPROC failed with %d",GetLastError());
	}
	else
	{
		DebugOutF(filelog::log_info,"SetWindowsHookEx WH_CALLWNDPROC ok");
	}
	g_bDDrawHookInit = true;
	DebugOutF(filelog::log_info,"InstallDDrawHook ok");
	return true;
}

bool UninstallDDrawHook()
{
	if(g_bDDrawHookInit){
		g_bDDrawHookInit = false;
		if(!UnhookWindowsHookEx(g_callwndproc))
		{
			DebugOutF(filelog::log_error,"UnhookWindowsHookEx WH_CALLWNDPROC failed with %d",GetLastError() );
		}
		else
		{
			g_callwndproc = 0;
		}
	}
	return true;
}
//! Inject DirectDraw hook
/*
called by DllMain in each hooked process
*/
bool InjectDDrawHook(void)
{
	//hook the following CALL
	//1.IDirectDrawSurface7::Filp
	//2.IDirectDrawSurface7::Blt
	//3.IDirectDrawSurface7::BltFast
	//4.IDirectDrawSurface7::BltBatch
	if(g_bInjected) return true;
	if(!g_bDDrawInit) return false;
	OutputDebugStringA("InjectDDrawHook...");

	static DDrawDLL dll;
	if (!dll.m_hLib)
	{
		OutputDebugStringA("load ddraw.dll failed");
		return false;
	}

	g_CALL_Blt		= (CALL_Blt)((UINT_PTR)dll.m_hLib + (UINT_PTR)IDirectDrawSurface7_Blt_offset);
	g_CALL_BltBatch = (CALL_BltBatch)((UINT_PTR)dll.m_hLib + (UINT_PTR)IDirectDrawSurface7_BltBatch_offset);
	g_CALL_BltFast	= (CALL_BltFast)((UINT_PTR)dll.m_hLib + (UINT_PTR)IDirectDrawSurface7_BltFast_offset);
	g_CALL_Flip		= (CALL_Flip)((UINT_PTR)dll.m_hLib + (UINT_PTR)IDirectDrawSurface7_Flip_offset);
	g_CALL_UpdateOverlay = (CALL_UpdateOverlay)((UINT_PTR)dll.m_hLib + (UINT_PTR)IDirectDrawSurface7_UpdateOverlay_offset);
	g_CALL_AddRef = (CALL_AddRef)((UINT_PTR)dll.m_hLib + (UINT_PTR)IDirectDrawSurface7_AddRef_offset);
	g_CALL_Release = (CALL_Release)((UINT_PTR)dll.m_hLib + (UINT_PTR)IDirectDrawSurface7_Release_offset);
	// 	if(g_hook_Blt.Install(g_CALL_Blt,&HookBlt))
	// 	{
	// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::Blt ok");
	// 	}
	// 	else
	// 	{
	// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::Blt failed");
	// 		UninjectDDrawHook();
	// 		return false;
	// 	}
	// 
	// 	if(g_hook_BltFast.Install(g_CALL_BltFast,&HookBltFast))
	// 	{
	// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::BltFast ok");
	// 	}
	// 	else
	// 	{
	// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::BltFast failed");
	// 		UninjectDDrawHook();
	// 		return false;
	// 	}
	// 
	// 	if(g_hook_BltBatch.Install(g_CALL_BltBatch,&HookBltBatch))
	// 	{
	// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::BltBatch ok");
	// 	}
	// 	else
	// 	{
	// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::BltBatch failed");
	// 		UninjectDDrawHook();
	// 		return false;
	// 	}

// 	if(g_hook_AddRef.Install(g_CALL_AddRef,&HookAddRef))
// 	{
// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::AddRef ok");
// 	}
// 	else
// 	{
// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::AddRef failed");
// 		UninjectDDrawHook();
// 		return false;
// 	}
// 
// 	if(g_hook_Release.Install(g_CALL_Release,&HookRelease))
// 	{
// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::Release ok");
// 	}
// 	else
// 	{
// 		OutputDebugStringA("hook ddraw IDirectDrawSurface7::Release failed");
// 		UninjectDDrawHook();
// 		return false;
// 	}

	if(g_hook_Flip.Install(g_CALL_Flip,&HookFlip))
	{
		OutputDebugStringA("hook ddraw IDirectDrawSurface7::Flip ok");
	}
	else
	{
		OutputDebugStringA("hook ddraw IDirectDrawSurface7::Flip failed");
		UninjectDDrawHook();
		return false;
	}
	if(g_hook_UpdateOverlay.Install(g_CALL_UpdateOverlay,&HookUpdateOverlay))
	{
		OutputDebugStringA("hook ddraw IDirectDrawSurface7::UpdateOverlay ok");
	}
	else
	{
		OutputDebugStringA("hook ddraw IDirectDrawSurface7::UpdateOverlay failed");
		UninjectDDrawHook();
		return false;
	}
	g_bInjected = true;
	OutputDebugStringA("InjectDDrawHook ok");
	return true;
}


void UninjectDDrawHook()
{
	if(!g_bInjected) return;
	g_hook_Blt.Uninstall();
	g_hook_BltBatch.Uninstall();
	g_hook_BltFast.Uninstall();
	g_hook_Flip.Uninstall();
	g_hook_UpdateOverlay.Uninstall();
	g_cache.Clean();
	g_bInjected = false;
}

HRESULT STDMETHODCALLTYPE HookBlt(LPDIRECTDRAWSURFACE7 pSuf, LPRECT rcdest,LPDIRECTDRAWSURFACE7 pSrc, LPRECT rcsrc,DWORD dw, LPDDBLTFX lpdblefx)
{
	g_hook_Blt.Restore();
	HRESULT hr= g_CALL_Blt(pSuf,rcdest,pSrc,rcsrc,dw,lpdblefx);

	if (COverlaySurfaceCache::IsOverlaySurface(pSuf))
	{
		OVERLAY_ITEM item;
		if(GetOverlayItem(pSuf,item))
		{
			if (item.flags & DDOVER_SHOW)
			{
				g_cache.AddSurface(item);
			}
		}
	}

	if(g_bDDrawKeep)g_hook_Blt.Reset();
	return hr;
}
HRESULT STDMETHODCALLTYPE HookBltBatch(LPDIRECTDRAWSURFACE7 pSuf, LPDDBLTBATCH lpbb, DWORD dw1, DWORD dw2 )
{
	g_hook_BltBatch.Restore();
	HRESULT hr=  g_CALL_BltBatch(pSuf,lpbb,dw1,dw2);
	if (COverlaySurfaceCache::IsOverlaySurface(pSuf))
	{
		OVERLAY_ITEM item;
		if(GetOverlayItem(pSuf,item))
		{
			if (item.flags & DDOVER_SHOW)
			{
				g_cache.AddSurface(item);
			}
		}
	}
	if(g_bDDrawKeep)g_hook_BltBatch.Reset();
	return hr;
}
HRESULT STDMETHODCALLTYPE HookBltFast(LPDIRECTDRAWSURFACE7 pSuf, DWORD dw1,DWORD dw2,LPDIRECTDRAWSURFACE7 pSrc, LPRECT rc,DWORD dw3)
{
	g_hook_BltFast.Restore();
	HRESULT hr=  g_CALL_BltFast(pSuf,dw1,dw2,pSrc,rc,dw3);
	if (COverlaySurfaceCache::IsOverlaySurface(pSuf))
	{
		OVERLAY_ITEM item;
		if(GetOverlayItem(pSuf,item))
		{
			if (item.flags & DDOVER_SHOW)
			{
				g_cache.AddSurface(item);
			}
		}
	}
	if(g_bDDrawKeep)g_hook_BltFast.Reset();
	return hr;
}
HRESULT STDMETHODCALLTYPE HookFlip(LPDIRECTDRAWSURFACE7 pSuf, LPDIRECTDRAWSURFACE7 pSrc, DWORD dw)
{
	g_hook_Flip.Restore();
	HRESULT hr=  g_CALL_Flip(pSuf,pSrc,dw);
	if (COverlaySurfaceCache::IsOverlaySurface(pSuf))
	{
		OVERLAY_ITEM item;
		if(GetOverlayItem(pSuf,item))
		{
			if (item.flags & DDOVER_SHOW)
			{
				g_cache.AddSurface(item);
			}
		}
	}
	if(g_bDDrawKeep)g_hook_Flip.Reset();
	return hr;
}

HRESULT STDMETHODCALLTYPE HookUpdateOverlay(LPDIRECTDRAWSURFACE7 pOverlay, 
											LPRECT rcSrc, 
											LPDIRECTDRAWSURFACE7 pPrimary,
											LPRECT rcDst,
											DWORD flags, 
											LPDDOVERLAYFX fx)
{
	g_hook_UpdateOverlay.Restore();
	HRESULT hr=  g_CALL_UpdateOverlay(pOverlay,rcSrc,pPrimary,rcDst,flags,fx);

	if(g_bDDrawKeep)g_hook_UpdateOverlay.Reset();

	static CFileMapping shared_flags;
	if(!shared_flags.IsOpen())
	{
		shared_flags.Open(kFlagsShareMemory,sizeof(DWORD),false );
	}
	if(flags & DDOVER_SHOW)
	{

		OVERLAY_ITEM item;
		if(GetOverlayItem(pOverlay,item))
		{
			g_cache.SetOverlayFX(fx);
			g_cache.AddSurface(item);
		}
		if(shared_flags.IsOpen())
		{
			DWORD *pFlags = (DWORD *)shared_flags.GetPointer();
			*pFlags = 1;
		}
	}
	else
	{
		g_cache.DelSurface(pOverlay);

		if(shared_flags.IsOpen())
		{
			DWORD *pFlags = (DWORD *)shared_flags.GetPointer();
			*pFlags = 0;
		}
	}
	return hr;
}
ULONG STDMETHODCALLTYPE HookAddRef(LPDIRECTDRAWSURFACE7 pOverlay)
{
	g_hook_AddRef.Restore();
	ULONG count = g_CALL_AddRef(pOverlay);

	if(g_bDDrawKeep)g_hook_AddRef.Reset();
	return count;
}
ULONG STDMETHODCALLTYPE HookRelease(LPDIRECTDRAWSURFACE7 pOverlay)
{
	g_hook_Release.Restore();
	ULONG count = g_CALL_Release(pOverlay);
	if(g_bDDrawKeep)g_hook_Release.Reset();
	if(count == 0)
	{
		g_cache.DelSurface(pOverlay);
	}
	return count;
}
bool CaptureToSharedMemory()
{
	return g_cache.CaptureToSharedMemory();
}

WINROBOTHOOK_API bool AppendDDrawOverlay(HDC hdc,LPCRECT rc)
{
	return g_cache.CaptureFromSharedMemory(hdc,rc);

}