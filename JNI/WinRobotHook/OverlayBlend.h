/*! \file $Id: OverlayBlend.h 73 2011-07-12 16:50:08Z caoym $
*  \author caoym
*  \brief OverlayBlend			
*/
#pragma once
#include <ddraw.h>
//bool OverlayBlend(HDC hDest,const RECT&  rcDest,HDC hSrc,const RECT&  rcSrc,DWORD flag,const DDOVERLAYFX& fx);

//! I simulate overlay bitblt because Bit to off-screen surface dose not support all color-key flags
HRESULT SimulateOverlayBlt(IDirectDraw7*pDDraw,LPDIRECTDRAWSURFACE7 iface, LPRECT rdst,
							  LPDIRECTDRAWSURFACE7 src, LPRECT rsrc,
							  DWORD dwFlags, LPDDBLTFX lpbltfx);