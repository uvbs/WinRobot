/*! \file $Id: OverlayBlend.cpp 111 2011-10-30 14:40:09Z caoym $
*  \author caoym
*  \brief CFileMapping			
*/
#include "StdAfx.h"
#include "OverlayBlend.h"
#include "debuglogger.h"
#include "YUV2RGB.h"
#include <ddraw.h>


#define PFGET_BPP(pf) (pf.dwFlags&DDPF_PALETTEINDEXED8?1:((pf.dwRGBBitCount+7)/8))
#define GET_BPP(desc) PFGET_BPP(desc.ddpfPixelFormat)

CComPtr<IDirectDrawSurface7> Conv2RGBSufface(IDirectDraw7*pDDraw,IDirectDrawSurface7*pSrc,const DDSURFACEDESC2 &desc)
{
	if(pSrc == 0) return 0;
	HRESULT hr = S_OK;
	DDSURFACEDESC2 ddsd_src = {0};
	ddsd_src.dwSize = sizeof(DDSURFACEDESC2);
	if(FAILED( pSrc->GetSurfaceDesc(&ddsd_src)) )
	{
		ddsd_src.dwSize = sizeof(DDSURFACEDESC);
		if(FAILED( pSrc->GetSurfaceDesc(&ddsd_src)) )
		{
			return 0;
		}
	}
 	DDSURFACEDESC2 ddsd_dest = {0};
 	ddsd_dest.dwSize = sizeof(DDSURFACEDESC2);
 	ddsd_dest.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH|DDSD_PIXELFORMAT;
 
 	ddsd_dest.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN /*|DDSCAPS_SYSTEMMEMORY*/;

 	ddsd_dest.dwHeight = ddsd_src.dwHeight;
 	ddsd_dest.dwWidth = ddsd_src.dwWidth;
 	ddsd_dest.ddpfPixelFormat = desc.ddpfPixelFormat;

	CComPtr<IDirectDrawSurface7> pDest;
	hr = pDDraw->CreateSurface(&ddsd_dest, &pDest, NULL);
	if(FAILED(hr))
	{
		OutputDebugStringA("CreateSurface failed");
		return 0;
	}
	
	RECT rc = {0,0,ddsd_src.dwWidth,ddsd_src.dwHeight};
	hr = pDest->BltFast(0,0,pSrc,0, DDBLTFAST_NOCOLORKEY);

// 	memset(&ddsd_dest,0,sizeof(ddsd_dest));
// 	ddsd_dest .dwSize = sizeof(ddsd_dest);
// 	pDest->Lock( NULL, &ddsd_dest, DDLOCK_READONLY, 0);
// 	pDest->Unlock(0);

	if(FAILED(hr))
	{
		DebugOutF(filelog::log_error,"Blt copy failed with 0x%x",hr);

		DDSURFACEDESC2	dest_desc={0},src_desc={0};
		src_desc.dwSize = sizeof(src_desc);
		dest_desc.dwSize = sizeof(dest_desc);

		hr = pDest->Lock(0,&dest_desc,DDLOCK_DONOTWAIT,0);
		if (FAILED(hr))
		{
			dest_desc.dwSize = sizeof(DDSURFACEDESC);
			hr = pDest->Lock(0,&dest_desc,DDLOCK_DONOTWAIT,0);
		}
		if(SUCCEEDED(hr))
		{
			hr = pSrc->Lock(0,&src_desc,DDLOCK_DONOTWAIT,0);
			if (FAILED(hr))
			{
				src_desc.dwSize = sizeof(DDSURFACEDESC);
				pSrc->Lock(0,&src_desc,DDLOCK_DONOTWAIT,0);
			}
			if(SUCCEEDED(hr))
			{
				if (src_desc.ddpfPixelFormat.dwFourCC == MAKEFOURCC('U','Y','V','Y') )
				{
					uyvy_to_rgb32(src_desc.dwWidth,ddsd_src.dwHeight,
						(const char *)src_desc.lpSurface,src_desc.lPitch,
						( char *)dest_desc.lpSurface,dest_desc.lPitch);
				}
				else if (src_desc.ddpfPixelFormat.dwFourCC == MAKEFOURCC('Y','U','Y','2') )
				{
					yuy2_to_rgb32(src_desc.dwWidth,ddsd_src.dwHeight,
						(const char *)src_desc.lpSurface,src_desc.lPitch,
						( char *)dest_desc.lpSurface,dest_desc.lPitch);
				}
				else
				{
					DebugOutF(filelog::log_error,"can't conv RGB -> FourCC 0x%x",ddsd_src.ddpfPixelFormat.dwFourCC);
				}
				pSrc->Unlock(0);
			}
			pDest->Unlock(0);
		}
		
	}
	return pDest;
}



static HRESULT _Blt_ColorFill(
							  LPBYTE buf, int width, int height, int bpp, LONG lPitch, DWORD color
							  ) {
								  int x, y;
								  LPBYTE first;

								  /* Do first row */

#define COLORFILL_ROW(type) { \
	type *d = (type *) buf; \
	for (x = 0; x < width; x++) \
	d[x] = (type) color; \
	break; \
								  }

								  switch(bpp) {
	case 1: COLORFILL_ROW(BYTE)
	case 2: COLORFILL_ROW(WORD)
	case 3: { BYTE *d = (BYTE *) buf;
		for (x = 0; x < width; x++,d+=3) {
			d[0] = (color    ) & 0xFF;
			d[1] = (color>> 8) & 0xFF;
			d[2] = (color>>16) & 0xFF;
		}
		break;}
	case 4: COLORFILL_ROW(DWORD)
	default:
		DebugOutF(filelog::log_error,"Color fill not implemented for bpp %d!\n", bpp*8);
		return DDERR_UNSUPPORTED;
								  }

#undef COLORFILL_ROW

								  /* Now copy first row */
								  first = buf;
								  for (y = 1; y < height; y++) {
									  buf += lPitch;
									  memcpy(buf, first, width * bpp);
								  }
								  return DD_OK;
}




HRESULT SimulateOverlayBlt(IDirectDraw7*pDDraw,LPDIRECTDRAWSURFACE7 iface, LPRECT rdst,
							  LPDIRECTDRAWSURFACE7 src, LPRECT rsrc,
							  DWORD dwFlags, LPDDBLTFX lpbltfx)
{

	RECT		xdst,xsrc;
	DDSURFACEDESC2	ddesc ={0},sdesc={0};

	HRESULT		ret = DD_OK;
	int bpp, srcheight, srcwidth, dstheight, dstwidth, width;
	int x, y;
	LPBYTE dbuf, sbuf;
	CComPtr<IDirectDrawSurface7>pConv2RGB = 0;
	/* First, check if the possible override function handles this case */
	if(iface->Blt( rdst, src, rsrc, dwFlags, lpbltfx) == DD_OK) 
	{
		//DebugOutF(filelog::log_info,"override function handles this case");
		return DD_OK;
	}

	sdesc.dwSize = sizeof(sdesc);
	ddesc.dwSize = sizeof(ddesc);
	HRESULT hr;
	if(FAILED(hr = iface->GetSurfaceDesc(&ddesc)))
	{
		ddesc.dwSize = sizeof(DDSURFACEDESC);
		if(FAILED(hr = iface->GetSurfaceDesc(&ddesc)))
		{
			return hr;
		}
	}
	if(FAILED(hr = src->GetSurfaceDesc(&sdesc)))
	{
		sdesc.dwSize = sizeof(DDSURFACEDESC);
		if(FAILED(hr = src->GetSurfaceDesc(&sdesc)))
		{
			return hr;
		}
	}

	if ((memcpy(&sdesc.ddpfPixelFormat,&ddesc.ddpfPixelFormat,sizeof(ddesc.ddpfPixelFormat) ) !=0 )/* ||
 		(sdesc.dwWidth != ddesc.dwWidth) ||
 		(sdesc.dwHeight != ddesc.dwHeight) */
		) {
			//convert to RGB
			pConv2RGB = Conv2RGBSufface(pDDraw,src,ddesc);
			src = pConv2RGB;
			//src->GetSurfaceDesc(&sdesc);

// 			if(iface->Blt( rdst, src, rsrc, dwFlags, lpbltfx) == DD_OK) 
// 			{
// 				//DebugOutF(filelog::log_info,"override function handles this case");
// 				return DD_OK;
// 			}
	}
	
// 	if (sdesc.ddpfPixelFormat != ddesc.ddpfPixelFormat) {
// 			//convert to RGB
// 			pConv2RGB = Conv2RGBSufface(src,ddesc.ddpfPixelFormat);
// 			src = pConv2RGB;
// 			src->GetSurfaceDesc(&sdesc);
// 	}


	if (src == iface) {
		if(FAILED(hr = IDirectDrawSurface7_Lock(iface, NULL, (DDSURFACEDESC2*)&ddesc, DDLOCK_DONOTWAIT, 0)))
		{
			ddesc.dwSize = sizeof(DDSURFACEDESC);
			if(FAILED(hr = IDirectDrawSurface7_Lock(iface, NULL, (DDSURFACEDESC2*)&ddesc, DDLOCK_DONOTWAIT, 0)))
			{
				goto error;
			}
		}
		sdesc = ddesc;
	} else {
		if (src) 
		{
			if(FAILED(hr = IDirectDrawSurface7_Lock(src, NULL, (DDSURFACEDESC2*)&sdesc, DDLOCK_READONLY|DDLOCK_DONOTWAIT, 0)))
			{
				sdesc.dwSize = sizeof(DDSURFACEDESC);
				if(FAILED(hr = IDirectDrawSurface7_Lock(src, NULL, (DDSURFACEDESC2*)&sdesc, DDLOCK_READONLY|DDLOCK_DONOTWAIT, 0)))
				{
					goto error;
				}
			}
		}
		if(FAILED(hr = IDirectDrawSurface7_Lock(iface,NULL,(DDSURFACEDESC2*)&ddesc,DDLOCK_WRITEONLY|DDLOCK_DONOTWAIT,0)))
		{
			ddesc.dwSize = sizeof(DDSURFACEDESC);
			if(FAILED(hr = IDirectDrawSurface7_Lock(iface,NULL,(DDSURFACEDESC2*)&ddesc,DDLOCK_WRITEONLY|DDLOCK_DONOTWAIT,0)))
			{
				goto error;
			}
		}
	}

	if (!lpbltfx || !(lpbltfx->dwDDFX)) dwFlags &= ~DDBLT_DDFX;

	if ((sdesc.ddpfPixelFormat.dwFlags & DDPF_FOURCC) &&
		(ddesc.ddpfPixelFormat.dwFlags & DDPF_FOURCC)) {
			if (sdesc.ddpfPixelFormat.dwFourCC != sdesc.ddpfPixelFormat.dwFourCC) {
				DebugOutF(filelog::log_error,"FOURCC->FOURCC copy only supported for the same type of surface");
				ret = DDERR_INVALIDPIXELFORMAT;
				goto release;
			}
			DebugOutF(filelog::log_warning,"FOURCC->FOURCC copy without colorkey");
			memcpy(ddesc.lpSurface, sdesc.lpSurface, ddesc.dwLinearSize);
			goto release;
	}

	if ((sdesc.ddpfPixelFormat.dwFlags & DDPF_FOURCC) &&
		(!(ddesc.ddpfPixelFormat.dwFlags & DDPF_FOURCC))) {
			//convert to RGB
			goto release;
	}

	if (rdst) {
		memcpy(&xdst,rdst,sizeof(xdst));
	} else {
		xdst.top	= 0;
		xdst.bottom	= ddesc.dwHeight;
		xdst.left	= 0;
		xdst.right	= ddesc.dwWidth;
	}

	if (rsrc) {
		memcpy(&xsrc,rsrc,sizeof(xsrc));
	} else {
		if (src) {
			xsrc.top	= 0;
			xsrc.bottom	= sdesc.dwHeight;
			xsrc.left	= 0;
			xsrc.right	= sdesc.dwWidth;
		} else {
			memset(&xsrc,0,sizeof(xsrc));
		}
	}

	/* First check for the validity of source / destination rectangles. This was
	verified using a test application + by MSDN.
	*/
	if ((src != NULL) &&
		((xsrc.bottom > (LONG)sdesc.dwHeight) || (xsrc.bottom < 0) ||
		(xsrc.top > (LONG)sdesc.dwHeight) || (xsrc.top < 0) ||
		(xsrc.left > (LONG)sdesc.dwWidth) || (xsrc.left < 0) ||
		(xsrc.right > (LONG)sdesc.dwWidth) || (xsrc.right < 0) ||
		(xsrc.right < xsrc.left) || (xsrc.bottom < xsrc.top))) {
			DebugOutF(filelog::log_error,"Application gave us bad source rectangle for Blt");
			ret = DDERR_INVALIDRECT;
			goto release;
	}
	/* For the Destination rect, it can be out of bounds on the condition that a clipper
	is set for the given surface.
	*/
	//     if ((This->clipper == NULL) &&
	// 	((xdst.bottom > ddesc.dwHeight) || (xdst.bottom < 0) ||
	// 	 (xdst.top > ddesc.dwHeight) || (xdst.top < 0) ||
	// 	 (xdst.left > ddesc.dwWidth) || (xdst.left < 0) ||
	// 	 (xdst.right > ddesc.dwWidth) || (xdst.right < 0) ||
	// 	 (xdst.right < xdst.left) || (xdst.bottom < xdst.top))) {
	//         WARN("Application gave us bad destination rectangle for Blt without a clipper set.\n");
	// 	ret = DDERR_INVALIDRECT;
	// 	goto release;
	//     }

	/* Now handle negative values in the rectangles. Warning: only supported for now
	in the 'simple' cases (ie not in any stretching / rotation cases).

	First, the case where nothing is to be done.
	*/
	if (((xdst.bottom <= 0) || (xdst.right <= 0) || (xdst.top >= (int) ddesc.dwHeight) || (xdst.left >= (int) ddesc.dwWidth)) ||
		((src != NULL) &&
		((xsrc.bottom <= 0) || (xsrc.right <= 0) || (xsrc.top >= (int) sdesc.dwHeight) || (xsrc.left >= (int) sdesc.dwWidth))))
	{
		DebugOutF(filelog::log_error,"Nothing to be done !");
		goto release;
	}

	/* The easy case : the source-less blits.... */
	if (src == NULL) {
		RECT full_rect;
		RECT temp_rect; /* No idea if intersect rect can be the same as one of the source rect */

		full_rect.left   = 0;
		full_rect.top    = 0;
		full_rect.right  = ddesc.dwWidth;
		full_rect.bottom = ddesc.dwHeight;
		IntersectRect(&temp_rect, &full_rect, &xdst);
		xdst = temp_rect;
	} else {
		/* Only handle clipping on the destination rectangle */
		int clip_horiz = (xdst.left < 0) || (xdst.right  > (int) ddesc.dwWidth );
		int clip_vert  = (xdst.top  < 0) || (xdst.bottom > (int) ddesc.dwHeight);
		if (clip_vert || clip_horiz) {
			/* Now check if this is a special case or not... */
			if ((((xdst.bottom - xdst.top ) != (xsrc.bottom - xsrc.top )) && clip_vert ) ||
				(((xdst.right  - xdst.left) != (xsrc.right  - xsrc.left)) && clip_horiz) ||
				(dwFlags & DDBLT_DDFX)) {
					DebugOutF(filelog::log_error,"Out of screen rectangle in special case. Not handled right now");
					goto release;
			}

			if (clip_horiz) {
				if (xdst.left < 0) { xsrc.left -= xdst.left; xdst.left = 0; }
				if (xdst.right > (LONG)ddesc.dwWidth) { xsrc.right -= (xdst.right - (int) ddesc.dwWidth); xdst.right = (int) ddesc.dwWidth; }
			}
			if (clip_vert) {
				if (xdst.top < 0) { xsrc.top -= xdst.top; xdst.top = 0; }
				if (xdst.bottom > (LONG)ddesc.dwHeight) { xsrc.bottom -= (xdst.bottom - (int) ddesc.dwHeight); xdst.bottom = (int) ddesc.dwHeight; }
			}
			/* And check if after clipping something is still to be done... */
			if ((xdst.bottom <= 0) || (xdst.right <= 0) || (xdst.top >= (int) ddesc.dwHeight) || (xdst.left >= (int) ddesc.dwWidth) ||
				(xsrc.bottom <= 0) || (xsrc.right <= 0) || (xsrc.top >= (int) sdesc.dwHeight) || (xsrc.left >= (int) sdesc.dwWidth)) {
					DebugOutF(filelog::log_error,"Nothing to be done after clipping !\n");
					goto release;
			}
		}
	}

	bpp = GET_BPP(ddesc);
	srcheight = xsrc.bottom - xsrc.top;
	srcwidth = xsrc.right - xsrc.left;
	dstheight = xdst.bottom - xdst.top;
	dstwidth = xdst.right - xdst.left;
	width = (xdst.right - xdst.left) * bpp;


	dbuf = (BYTE*)ddesc.lpSurface+(xdst.top*ddesc.lPitch)+(xdst.left*bpp);

	//     if (dwFlags & DDBLT_WAIT) {
	// 	static BOOL displayed = FALSE;
	// 	if (!displayed)
	// 	    DebugOutF(filelog::log_error,"Can't handle DDBLT_WAIT flag right now");
	// 		displayed = TRUE;
	// 		dwFlags &= ~DDBLT_WAIT;
	//     }
	//     if (dwFlags & DDBLT_ASYNC) {
	// 	static BOOL displayed = FALSE;
	// 	if (!displayed)
	// 	    FIXME("Can't handle DDBLT_ASYNC flag right now.\n");
	// 	displayed = TRUE;
	// 	dwFlags &= ~DDBLT_ASYNC;
	//     }
	//     if (dwFlags & DDBLT_DONOTWAIT) {
	// 	/* DDBLT_DONOTWAIT appeared in DX7 */
	// 	static BOOL displayed = FALSE;
	// 	if (!displayed)
	// 	    FIXME("Can't handle DDBLT_DONOTWAIT flag right now.\n");
	// 	displayed = TRUE;
	// 	dwFlags &= ~DDBLT_DONOTWAIT;
	//     }

	/* First, all the 'source-less' blits */
	if (dwFlags & DDBLT_COLORFILL) {
		ret = _Blt_ColorFill(dbuf, dstwidth, dstheight, bpp,
			ddesc.lPitch, lpbltfx->dwFillColor);
		dwFlags &= ~DDBLT_COLORFILL;
	}

	if (dwFlags & DDBLT_DEPTHFILL)

		if (dwFlags & DDBLT_ROP) {
			/* Catch some degenerate cases here */
			switch(lpbltfx->dwROP) {
	case BLACKNESS:
		ret = _Blt_ColorFill(dbuf,dstwidth,dstheight,bpp,ddesc.lPitch,0);
		break;
	case 0xAA0029: /* No-op */
		break;
	case WHITENESS:
		ret = _Blt_ColorFill(dbuf,dstwidth,dstheight,bpp,ddesc.lPitch,~0);
		break;
	case SRCCOPY: /* well, we do that below ? */
		break;
	default:
		DebugOutF(filelog::log_error,"Unsupported raster op: %08lx  Pattern: %p", lpbltfx->dwROP, lpbltfx->lpDDSPattern);
		goto error;
			}
			dwFlags &= ~DDBLT_ROP;
		}
		if (dwFlags & DDBLT_DDROPS) {
			//FIXME("\tDdraw Raster Ops: %08lx  Pattern: %p\n", lpbltfx->dwDDROP, lpbltfx->u5.lpDDSPattern);
		}
		/* Now the 'with source' blits */
		if (src) {
			LPBYTE sbase;
			int sx, xinc, sy, yinc;

			if (!dstwidth || !dstheight) /* hmm... stupid program ? */
				goto release;
			sbase = (BYTE*)sdesc.lpSurface+(xsrc.top*sdesc.lPitch)+xsrc.left*bpp;
			xinc = (srcwidth << 16) / dstwidth;
			yinc = (srcheight << 16) / dstheight;

			if (!dwFlags) {
				/* No effects, we can cheat here */
				if (dstwidth == srcwidth) {
					if (dstheight == srcheight) {
						/* No stretching in either direction. This needs to be as
						* fast as possible */
						sbuf = sbase;

						/* check for overlapping surfaces */
						if (src != iface || xdst.top < xsrc.top ||
							xdst.right <= xsrc.left || xsrc.right <= xdst.left)
						{
							/* no overlap, or dst above src, so copy from top downwards */
							for (y = 0; y < dstheight; y++)
							{
								memcpy(dbuf, sbuf, width);
								sbuf += sdesc.lPitch;
								dbuf += ddesc.lPitch;
							}
						}
						else if (xdst.top > xsrc.top)  /* copy from bottom upwards */
						{
							sbuf += (sdesc.lPitch*dstheight);
							dbuf += (ddesc.lPitch*dstheight);
							for (y = 0; y < dstheight; y++)
							{
								sbuf -= sdesc.lPitch;
								dbuf -= ddesc.lPitch;
								memcpy(dbuf, sbuf, width);
							}
						}
						else /* src and dst overlapping on the same line, use memmove */
						{
							for (y = 0; y < dstheight; y++)
							{
								memmove(dbuf, sbuf, width);
								sbuf += sdesc.lPitch;
								dbuf += ddesc.lPitch;
							}
						}
					} else {
						/* Stretching in Y direction only */
						for (y = sy = 0; y < dstheight; y++, sy += yinc) {
							sbuf = sbase + (sy >> 16) * sdesc.lPitch;
							memcpy(dbuf, sbuf, width);
							dbuf += ddesc.lPitch;
						}
					}
				} else {
					/* Stretching in X direction */
					int last_sy = -1;
					for (y = sy = 0; y < dstheight; y++, sy += yinc) {
						sbuf = sbase + (sy >> 16) * sdesc.lPitch;

						if ((sy >> 16) == (last_sy >> 16)) {
							/* this sourcerow is the same as last sourcerow -
							* copy already stretched row
							*/
							memcpy(dbuf, dbuf - ddesc.lPitch, width);
						} else {
#define STRETCH_ROW(type) { \
	type *s = (type *) sbuf, *d = (type *) dbuf; \
	for (x = sx = 0; x < dstwidth; x++, sx += xinc) \
	d[x] = s[sx >> 16]; \
	break; }

							switch(bpp) {
			case 1: STRETCH_ROW(BYTE)
			case 2: STRETCH_ROW(WORD)
			case 4: STRETCH_ROW(DWORD)
			case 3: {
				LPBYTE s,d = dbuf;
				for (x = sx = 0; x < dstwidth; x++, sx+= xinc) {
					DWORD pixel;

					s = sbuf+3*(sx>>16);
					pixel = s[0]|(s[1]<<8)|(s[2]<<16);
					d[0] = (pixel    )&0xff;
					d[1] = (pixel>> 8)&0xff;
					d[2] = (pixel>>16)&0xff;
					d+=3;
				}
				break;
					}
			default:
				DebugOutF(filelog::log_error,"Stretched blit not implemented for bpp %d!\n", bpp*8);
				ret = DDERR_UNSUPPORTED;
				goto error;
							}
#undef STRETCH_ROW
						}
						dbuf += ddesc.lPitch;
						last_sy = sy;
					}
				}
			} else {
				LONG dstyinc = ddesc.lPitch, dstxinc = bpp;
				DWORD keylow = 0xFFFFFFFF, keyhigh = 0, keymask = 0xFFFFFFFF;
				DWORD keyflag = 0;
				if (dwFlags & (DDBLT_KEYSRC | DDBLT_KEYDEST | DDBLT_KEYSRCOVERRIDE | DDBLT_KEYDESTOVERRIDE)) {

					if (dwFlags & DDBLT_KEYSRC) {
						keyflag = DDBLT_KEYSRC;
						DDCOLORKEY key = {0};
						src->GetColorKey(DDCKEY_SRCOVERLAY,&key);
						keylow  = key.dwColorSpaceLowValue;
						keyhigh = key.dwColorSpaceHighValue;
					} else if (dwFlags & DDBLT_KEYDEST){
						keyflag = DDBLT_KEYDEST;
						DDCOLORKEY key = {0};
						iface->GetColorKey(DDCKEY_DESTOVERLAY,&key);
						keylow  = key.dwColorSpaceLowValue;
						keyhigh = key.dwColorSpaceHighValue;
					} else if (dwFlags & DDBLT_KEYSRCOVERRIDE) {
						keyflag = DDBLT_KEYSRCOVERRIDE;
						keylow  = lpbltfx->ddckSrcColorkey.dwColorSpaceLowValue;
						keyhigh = lpbltfx->ddckSrcColorkey.dwColorSpaceHighValue;
					} else {
						keyflag = DDBLT_KEYDESTOVERRIDE;
						keylow  = lpbltfx->ddckDestColorkey.dwColorSpaceLowValue;
						keyhigh = lpbltfx->ddckDestColorkey.dwColorSpaceHighValue;
					}
					keymask = sdesc.ddpfPixelFormat.dwRBitMask | sdesc.ddpfPixelFormat.dwGBitMask | sdesc.ddpfPixelFormat.dwBBitMask;
					dwFlags &= ~(DDBLT_KEYSRC | DDBLT_KEYDEST | DDBLT_KEYSRCOVERRIDE | DDBLT_KEYDESTOVERRIDE);
				}

				if (dwFlags & DDBLT_DDFX)  {
					LPBYTE dTopLeft, dTopRight, dBottomLeft, dBottomRight, tmp;
					LONG tmpxy;
					dTopLeft     = dbuf;
					dTopRight    = dbuf+((dstwidth-1)*bpp);
					dBottomLeft  = dTopLeft+((dstheight-1)*ddesc.lPitch);
					dBottomRight = dBottomLeft+((dstwidth-1)*bpp);

					if (lpbltfx->dwDDFX & DDBLTFX_ARITHSTRETCHY){
						/* I don't think we need to do anything about this flag */
						// WARN("dwflags=DDBLT_DDFX nothing done for DDBLTFX_ARITHSTRETCHY\n");
					}
					if (lpbltfx->dwDDFX & DDBLTFX_MIRRORLEFTRIGHT) {
						tmp          = dTopRight;
						dTopRight    = dTopLeft;
						dTopLeft     = tmp;
						tmp          = dBottomRight;
						dBottomRight = dBottomLeft;
						dBottomLeft  = tmp;
						dstxinc = dstxinc *-1;
					}
					if (lpbltfx->dwDDFX & DDBLTFX_MIRRORUPDOWN) {
						tmp          = dTopLeft;
						dTopLeft     = dBottomLeft;
						dBottomLeft  = tmp;
						tmp          = dTopRight;
						dTopRight    = dBottomRight;
						dBottomRight = tmp;
						dstyinc = dstyinc *-1;
					}
					if (lpbltfx->dwDDFX & DDBLTFX_NOTEARING) {
						/* I don't think we need to do anything about this flag */
						// WARN("dwflags=DDBLT_DDFX nothing done for DDBLTFX_NOTEARING\n");
					}
					if (lpbltfx->dwDDFX & DDBLTFX_ROTATE180) {
						tmp          = dBottomRight;
						dBottomRight = dTopLeft;
						dTopLeft     = tmp;
						tmp          = dBottomLeft;
						dBottomLeft  = dTopRight;
						dTopRight    = tmp;
						dstxinc = dstxinc * -1;
						dstyinc = dstyinc * -1;
					}
					if (lpbltfx->dwDDFX & DDBLTFX_ROTATE270) {
						tmp          = dTopLeft;
						dTopLeft     = dBottomLeft;
						dBottomLeft  = dBottomRight;
						dBottomRight = dTopRight;
						dTopRight    = tmp;
						tmpxy   = dstxinc;
						dstxinc = dstyinc;
						dstyinc = tmpxy;
						dstxinc = dstxinc * -1;
					}
					if (lpbltfx->dwDDFX & DDBLTFX_ROTATE90) {
						tmp          = dTopLeft;
						dTopLeft     = dTopRight;
						dTopRight    = dBottomRight;
						dBottomRight = dBottomLeft;
						dBottomLeft  = tmp;
						tmpxy   = dstxinc;
						dstxinc = dstyinc;
						dstyinc = tmpxy;
						dstyinc = dstyinc * -1;
					}
					if (lpbltfx->dwDDFX & DDBLTFX_ZBUFFERBASEDEST) {
						/* I don't think we need to do anything about this flag */
						//WARN("dwflags=DDBLT_DDFX nothing done for DDBLTFX_ZBUFFERBASEDEST\n");
					}
					dbuf = dTopLeft;
					dwFlags &= ~(DDBLT_DDFX);
				}

#define COPY_COLORKEY_FX(type) { \
	type *s, *d = (type *) dbuf, *dx, tmp; \
	for (y = sy = 0; y < dstheight; y++, sy += yinc) { \
	s = (type*)(sbase + (sy >> 16) * sdesc.lPitch); \
	dx = d; \
	for (x = sx = 0; x < dstwidth; x++, sx += xinc) { \
		tmp = s[sx >> 16]; \
		switch(keyflag)\
		{\
		case DDBLT_KEYSRC:\
		case DDBLT_KEYSRCOVERRIDE:\
			 if ((tmp & keymask) >= keylow && (tmp & keymask) <= keyhigh) \
			 {\
				dx[0] = tmp; \
			 }\
			  break;\
			break;\
		case DDBLT_KEYDEST:\
		case DDBLT_KEYDESTOVERRIDE:\
			if ((dx[0] & keymask) >= keylow && (dx[0] & keymask) <= keyhigh) \
			{\
				dx[0] = tmp; \
			}\
			break;\
		default:\
			dx[0] = tmp; \
			break;\
		}\
		dx = (type*)(((LPBYTE)dx)+dstxinc); \
	} \
	d = (type*)(((LPBYTE)d)+dstyinc); \
	} \
	break; }

	switch (bpp) {
		case 1: COPY_COLORKEY_FX(BYTE)
		case 2: COPY_COLORKEY_FX(WORD)
		case 4: COPY_COLORKEY_FX(DWORD)
		case 3: {LPBYTE s,d = dbuf, dx;
			for (y = sy = 0; y < dstheight; y++, sy += yinc) {
				sbuf = sbase + (sy >> 16) * sdesc.lPitch;
				dx = d;
				for (x = sx = 0; x < dstwidth; x++, sx+= xinc) {
					DWORD pixel,srcpixel;
					s = sbuf+3*(sx>>16);
					srcpixel = dx[0]|(dx[1]<<8)|(dx[2]<<16);
					pixel = s[0]|(s[1]<<8)|(s[2]<<16);
					switch(keyflag)
					{
						case DDBLT_KEYSRC:
						case DDBLT_KEYSRCOVERRIDE:
						if ((pixel & keymask) >= keylow && (pixel & keymask) <= keyhigh){ 
							dx[0] = (pixel    )&0xff;
							dx[1] = (pixel>> 8)&0xff;
							dx[2] = (pixel>>16)&0xff;
						}
						break;
						case DDBLT_KEYDEST:
						case DDBLT_KEYDESTOVERRIDE:
						if ((srcpixel & keymask) >= keylow && (srcpixel & keymask) <= keyhigh){ 
							dx[0] = (pixel    )&0xff;
							dx[1] = (pixel>> 8)&0xff;
							dx[2] = (pixel>>16)&0xff;
						}
						break;
					}
					dx+= dstxinc;
				}
				d += dstyinc;
			}
			break;}
		default:
			DebugOutF(filelog::log_error,"%s color-keyed blit not implemented for bpp %d!\n",
				(dwFlags & DDBLT_KEYSRC) ? "Source" : "Destination", bpp*8);
			ret = DDERR_UNSUPPORTED;
			goto error;
#undef COPY_COLORKEY_FX

			
				}
// 	DebugOutF(filelog::log_info,"COPY_COLORKEY_FX bpp:%d,w:%d,h:%d,keyflag:0x%x,key:0x%x,0x%x ok",
// 		bpp,dstwidth,dstheight,keyflag,keylow,keyhigh);
			}
		}

error:
		

release:
		IDirectDrawSurface7_Unlock(iface,NULL);
		if (src && src != iface) IDirectDrawSurface7_Unlock(src,NULL);
		return ret;
}

