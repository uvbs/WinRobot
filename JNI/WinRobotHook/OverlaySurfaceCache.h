/*! \file $Id: OverlaySurfaceCache.h 88 2011-09-30 14:26:46Z caoym $
*  \author caoym
*  \brief COverlaySurfaceCache
*/
#pragma once
#include "sync/cslock.h"

#include <map>
#include <ddraw.h>
extern const UINT CAPTURE_SCREEN;
static const TCHAR* kChaptureOverlayShareMemory = _T("FB4220CB-7C2D-49fd-8304-7AA5C38D627F");
static const TCHAR* kFlagsShareMemory = _T("{F7A60E4F-73CD-4947-8CE6-0FB1F2E4A8CB}");
/**
 * overlay surface data and infomation
 */
struct OVERLAY_ITEM
{
	CComPtr<IDirectDrawSurface7> pOverlay; /**< the overlay surface*/
	RECT rcSrc; /**< source of overlay surface*/
	RECT rcDst; /**< destination  of overlay surface*/
	DWORD flags; /**<flags that set by UpdateOverlay*/
	DDOVERLAYFX fx;/**<fx that set by UpdateOverlay*/

	OVERLAY_ITEM(IDirectDrawSurface7*pOverlay=0,
		LPCRECT rcSrc = 0,
		LPCRECT rcDst=0, 
		DWORD flags=0,
		LPDDOVERLAYFX fx=0 )
	{
		this->pOverlay = pOverlay;
//		this->pPrimary = pPrimary;
		this->flags = flags;
		if(fx)this->fx = *fx;
		if (rcSrc)
		{
			this->rcSrc = *rcSrc;
			//bEntireSrc = false;
		}
		else
		{
			this->rcSrc.left = this->rcSrc.top = 0;
			this->rcSrc.bottom = this->rcSrc.right = 0;
			//bEntireSrc = true;
		}
		if (rcDst)
		{
			this->rcDst = *rcDst;
			//bEntireDest = false;
		}
		else
		{
			this->rcDst.left = this->rcDst.top = 0;
			this->rcDst.bottom = this->rcDst.right = 0;
			//bEntireDest = true;
		}
	}
	
};

/**
 * used by SURFACE_DATA
 * information of a surface
 */
typedef struct _DDSURFACEDESC2_INFO
{
	_DDSURFACEDESC2_INFO(const DDSURFACEDESC2& info)
	{
		dwHeight = info.dwHeight;
		dwWidth= info.dwWidth;
		lPitch= info.lPitch;
		ddpfPixelFormat= info.ddpfPixelFormat;

	}
  // DWORD               dwSize;                 // size of the DDSURFACEDESC structure
  //  DWORD               dwFlags;                // determines what fields are valid
    DWORD               dwHeight;               // height of surface to be created
    DWORD               dwWidth;                // width of input surface
    union
    {
        LONG            lPitch;                 // distance to start of next line (return value only)
        DWORD           dwLinearSize;           // Formless late-allocated optimized surface size
    } DUMMYUNIONNAMEN(1);
    /* union
    {
        DWORD           dwBackBufferCount;      // number of back buffers requested
        DWORD           dwDepth;                // the depth if this is a volume texture 
    } DUMMYUNIONNAMEN(5);
    union
    {
        DWORD           dwMipMapCount;          // number of mip-map levels requestde
                                                // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
        DWORD           dwRefreshRate;          // refresh rate (used when display mode is described)
        DWORD           dwSrcVBHandle;          // The source used in VB::Optimize
    } DUMMYUNIONNAMEN(2);
    DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
    DWORD               dwReserved;             // reserved
    LPVOID              lpSurface;              // pointer to the associated surface memory
    union
    {
        DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
        DWORD           dwEmptyFaceColor;       // Physical color for empty cubemap faces
    } DUMMYUNIONNAMEN(3);
    DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
    DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
    DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use*/
    union
    {
        DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface
        DWORD           dwFVF;                  // vertex format description of vertex buffers
    } DUMMYUNIONNAMEN(4);
  /*  DDSCAPS2            ddsCaps;                // direct draw surface capabilities
    DWORD               dwTextureStage;         // stage in multitexture cascade*/
} DDSURFACEDESC2_INFO;
/**
 * used to storage date that capture from Overlay surface
 */
struct SURFACE_DATA
{
	DWORD			size;			/**< save of head + body*/
	BOOL			modified;		/**< whether the data was modified*/
	_DDSURFACEDESC2_INFO	desc;	/**< info if the surface*/
	
	RECT rcDst;						/**<destination of surface was be displayed*/
	DWORD flags;					/**<flags that set by UpdateOverlay*/
	DDOVERLAYFX fx;					/**<fx that set by UpdateOverlay*/
	RGNDATAHEADER	rdh;			/**<not used*/
	BYTE			data[1];		/**<surface data*/
};

/**
 * storage overlay surface as cache
 * so we can get overlay image from a surface any time if we want
 */
class COverlaySurfaceCache
{
public:
	COverlaySurfaceCache(void);
	~COverlaySurfaceCache(void);
	/**
	 * add a overlay surface 
	 */
	void AddSurface(OVERLAY_ITEM&);
	/**
	 * delete a overlay surface 
	 */
	void DelSurface(IDirectDrawSurface7*);
	/**
	 * capture overlay surface image to shared memory
	 */
	bool CaptureToSharedMemory();
	/**
	 * get overlay surface image from shared memory,and draw it it a DC.
	 */
	static bool CaptureFromSharedMemory(HDC hdc,LPCRECT src);
	/**
	 * decide whether is a overlay surface.
	 */
	static bool IsOverlaySurface(IDirectDrawSurface7*pSuf);
	//static DWORD GetSharedMemorySize(OVERLAY_ITEM &item);
	/** 
	 * because sometimes (why?) I can't get DDOVERLAYFX from OVERLAY_ITEM,
	 * so I need save it when UpdateOverlay called
	 */
	void SetOverlayFX(const DDOVERLAYFX* fx){if(fx)m_fx = *fx;};
	/**
	 * clean all overlay surfaces from cache
	 */
	void Clean();
private:
	CComPtr<IDirectDraw7> m_pDDraw;
	DDOVERLAYFX m_fx;
	CCrtSection m_csLock;
	typedef std::map<IDirectDrawSurface7*, OVERLAY_ITEM >  INTERFACES;
	INTERFACES m_surfaces;
	CComPtr<IDirectDrawSurface7> CopyToOffscreenSurface(IDirectDraw7*,const OVERLAY_ITEM&item);
	bool CopyToSharedMemory(IDirectDraw7*,const OVERLAY_ITEM&tiem);
};
