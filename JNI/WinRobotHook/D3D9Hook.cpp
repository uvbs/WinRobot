#include "StdAfx.h"
#include "D3D9Hook.h"
#include "WinSysDLL.h"
#include <atlbase.h>
#include <d3d9types.h>
#include <d3d9.h>
#include <tchar.h>
#include "FunctionHook.h"
#include "ShareData.h"
#define VTABLE_QueryInterface 0
#define VTABLE_AddRef 1
#define VTABLE_Release 2

#define VTABLE_DX8_Reset 14
#define VTABLE_DX8_Present 15

#define VTABLE_DX9_SCPresent 3
#define VTABLE_DX9_Reset 16
#define VTABLE_DX9_Present 17



//! d3d9.dll
class D3D9DLL
	:public CDLLLib
{
public:
	D3D9DLL();
	IDirect3D9* Direct3DCreate(UINT);
	typedef IDirect3D9* (STDMETHODCALLTYPE *Direct3DCreate9_fn)(UINT);

	Direct3DCreate9_fn Direct3DCreate9FN;
};

D3D9DLL::D3D9DLL()
:CDLLLib(_T("d3d9.dll"))
,Direct3DCreate9FN(0)
{
	Direct3DCreate9FN =
		(Direct3DCreate9_fn)GetProcAddress(m_hLib,("Direct3DCreate9"));
}
IDirect3D9* D3D9DLL::Direct3DCreate(UINT ver)
{
	if(Direct3DCreate9FN) return Direct3DCreate9FN(ver);
	return 0;
}

typedef HRESULT (STDMETHODCALLTYPE *RESET_FUNC)(IDirect3DDevice9* pDevice, LPVOID);
typedef HRESULT (STDMETHODCALLTYPE *PRESENT_FUN)(IDirect3DDevice9* pDevice, const RECT*, const RECT*, HWND, LPVOID);
typedef HRESULT (STDMETHODCALLTYPE *SCPRESENT_FUN)(IDirect3DSwapChain9* pSwapChain, const RECT*,const RECT*, HWND, LPVOID, DWORD);

static HRESULT STDMETHODCALLTYPE HookReset(IDirect3DDevice9* , LPVOID);
static HRESULT STDMETHODCALLTYPE HookPresent(IDirect3DDevice9* , const RECT*, const RECT*, HWND, LPVOID);
static HRESULT STDMETHODCALLTYPE HookSCPresent(IDirect3DSwapChain9* , const RECT*,const RECT*, HWND, LPVOID, DWORD);


#pragma data_seg(".SHARED")

UINT_PTR g_offset_callIDirect3DDevice9Reset = 0;
UINT_PTR g_offset_callIDirect3DDevice9Present = 0;
UINT_PTR g_offset_callIDirect3DSwapChain9Present= 0;
bool g_bKeepHook = false;
bool g_bD3D9Init = false;
#pragma data_seg()
#pragma comment(linker, "/section:.SHARED,rws") 

static CFunctionHook<RESET_FUNC > g_hookIDirect3DDevice9Reset ;
static CFunctionHook<PRESENT_FUN> g_hookIDirect3DDevice9Present;
static CFunctionHook<SCPRESENT_FUN> g_hookIDirect3DSwapChain9Present  ;

RESET_FUNC g_callIDirect3DDevice9Reset = 0;
PRESENT_FUN g_callIDirect3DDevice9Present = 0;
SCPRESENT_FUN g_callIDirect3DSwapChain9Present= 0;

void UninjectD3D9Hook();

// we save address of Direct3D9 API to shared memory
bool InstallD3D9Hook()
{
	if(g_bD3D9Init) return true;

	static D3D9DLL dll;
	CComPtr<IDirect3D9> pD3D9 = dll.Direct3DCreate(D3D_SDK_VERSION);
	if ( !pD3D9)
	{
		OutputDebugStringA("Direct3DCreate failed");
		return false;
	}
	D3DDISPLAYMODE mode;
	HRESULT hr;
	if (FAILED(hr = pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode )))
	{
		OutputDebugStringA("GetAdapterDisplayMode failed");
		return false;
	}
	CComPtr<IDirect3DDevice9>pDevice;
	D3DPRESENT_PARAMETERS params; 
	ZeroMemory( &params, sizeof(params));
	params.Windowed = true;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.BackBufferFormat = mode.Format;

	if (FAILED( hr = pD3D9->CreateDevice( 
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		GetDesktopWindow(),
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
		&params,
		&pDevice)))
	{
		OutputDebugStringA("CreateDevice failed");
		return false;
	}
	UINT_PTR* pVTable = (UINT_PTR*)(*((UINT_PTR*)pDevice.p));
	g_offset_callIDirect3DDevice9Reset = pVTable[VTABLE_DX9_Reset]- UINT_PTR(dll.m_hLib);
	g_offset_callIDirect3DDevice9Present = pVTable[VTABLE_DX9_Present]- UINT_PTR(dll.m_hLib);

	CComPtr<IDirect3DSwapChain9> pSC;
	if (FAILED(hr = pDevice->GetSwapChain( 0, &pSC ) ))
	{
		OutputDebugStringA("GetSwapChain failed");
		return false;
	}
	pVTable = (UINT_PTR*)(*((UINT_PTR*)pSC.p));

	g_offset_callIDirect3DSwapChain9Present = pVTable[VTABLE_DX9_SCPresent] - UINT_PTR(dll.m_hLib);
	g_bKeepHook = true;
	g_bD3D9Init = true;
	OutputDebugStringA("InitializeD3D9Hook ok");
	return true;
}
bool UninstallD3D9Hook()
{
	g_bKeepHook = false;
	return true;
}
bool InjectD3D9Hook(void)
{
	//hook the following CALL
	//1.IDirect3DDevice9::Reset
	//2.IDirect3DDevice9::Present
	//3.IDirect3DSwapChain9::SCPresent
	OutputDebugStringA("InjectD3D9Hook...");
	if(!g_bD3D9Init){
		OutputDebugStringA("InjectD3D9Hook failed,g_bD3D9Init == false");
		return false;
	}
	static D3D9DLL dll;
	g_callIDirect3DDevice9Reset = (RESET_FUNC)((UINT_PTR)dll.m_hLib + (UINT_PTR)g_offset_callIDirect3DDevice9Reset);
	g_callIDirect3DDevice9Present = (PRESENT_FUN)((UINT_PTR)dll.m_hLib + (UINT_PTR)g_offset_callIDirect3DDevice9Present);
	g_callIDirect3DSwapChain9Present = (SCPRESENT_FUN)((UINT_PTR)dll.m_hLib + (UINT_PTR)g_offset_callIDirect3DSwapChain9Present);

	if(g_hookIDirect3DDevice9Reset.Install(g_callIDirect3DDevice9Reset,&HookReset))
	{
		OutputDebugStringA("hook d3d9 IDirect3DDevice9::Reset ok");
	}
	else
	{
		OutputDebugStringA("hook d3d9 IDirect3DDevice9::Reset failed");
		UninjectD3D9Hook();
		return false;
	}

	if(g_hookIDirect3DDevice9Present.Install(g_callIDirect3DDevice9Present,&HookPresent))
	{
		OutputDebugStringA("hook d3d9 IDirect3DDevice9::Present ok");
	}
	else
	{
		OutputDebugStringA("hook d3d9 IDirect3DDevice9::Present failed");
		UninjectD3D9Hook();
		return false;
	}
	if(g_hookIDirect3DSwapChain9Present.Install(g_callIDirect3DSwapChain9Present,&HookSCPresent))
	{
		OutputDebugStringA("hook d3d9 IDirect3DSwapChain9::SCPresent ok");
	}
	else
	{
		OutputDebugStringA("hook d3d9 IDirect3DSwapChain9::SCPresent failed");
		UninjectD3D9Hook();
		return false;
	}
	OutputDebugStringA("InjectD3D9Hook ok");
	return true;
}
void UninjectD3D9Hook()
{
	g_hookIDirect3DDevice9Reset.Uninstall();
	g_hookIDirect3DDevice9Present.Uninstall();
	g_hookIDirect3DSwapChain9Present.Uninstall();
	
}
HRESULT STDMETHODCALLTYPE HookReset(IDirect3DDevice9*pDevice , LPVOID pUnk)
{
	OutputDebugStringA("HookReset");
	g_hookIDirect3DDevice9Reset.Uninstall();
	HRESULT hr= g_callIDirect3DDevice9Reset(pDevice,pUnk);
	if(g_bKeepHook)g_hookIDirect3DDevice9Reset.Install(g_callIDirect3DDevice9Reset,&HookReset);
	return hr;
}
HRESULT STDMETHODCALLTYPE HookPresent(IDirect3DDevice9*pDevice , const RECT*pDest, const RECT*pSrc, HWND hWnd, LPVOID pVoid)
{
	OutputDebugStringA("HookPresent");
	g_hookIDirect3DDevice9Present.Uninstall();

	// capture


	HRESULT hr= g_callIDirect3DDevice9Present(pDevice,pDest,pSrc,hWnd,pVoid);
	if(g_bKeepHook)g_hookIDirect3DDevice9Present.Install(g_callIDirect3DDevice9Present,&HookPresent);
	return hr;
}
HRESULT STDMETHODCALLTYPE HookSCPresent(IDirect3DSwapChain9* pSC, const RECT*pDest,const RECT*pSrc, HWND hWnd, LPVOID pVoid, DWORD dw)
{
	OutputDebugStringA("HookSCPresent");
	g_hookIDirect3DSwapChain9Present.Uninstall();
	HRESULT hr=  g_callIDirect3DSwapChain9Present(pSC,pDest,pSrc,hWnd,pVoid,dw);
	if(g_bKeepHook)g_hookIDirect3DSwapChain9Present.Install(g_callIDirect3DSwapChain9Present,&HookSCPresent);
	return hr;
}

