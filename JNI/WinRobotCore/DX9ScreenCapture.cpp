#include "StdAfx.h"
#include "DX9ScreenCapture.h"
#include "WinSysDLL.h"

CDX9ScreenCapture::CDX9ScreenCapture(void)
{
	
}

CDX9ScreenCapture::~CDX9ScreenCapture(void)
{
	Destroy();
}
bool CDX9ScreenCapture::Create()
{
	return false;
	
}
void CDX9ScreenCapture::Destroy()
{
	
}
CRefObj<CFileMappingBitmap> CDX9ScreenCapture::Capture(const RECT&rc)
{
	
// 		RECT rcMax = {0,0,abs(m_desc.Width),abs(m_desc.Height)};
// 		RECT rcNew = {0,0,0,0};
// 		IntersectRect(&rcNew,&rc,&rcMax);ca0yangm
		return false;
}

