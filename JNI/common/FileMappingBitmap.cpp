#include "StdAfx.h"
#include "FileMappingBitmap.h"
#include "debuglogger.h"

CFileMappingBitmap::CFileMappingBitmap(void)
{
}

CFileMappingBitmap::~CFileMappingBitmap(void)
{
}
bool CFileMappingBitmap::Create(int w,int h,int bitscount,
			int biClrUsed,RGBQUAD*pPalette,LPCTSTR szName /*= 0*/ )
{
	
	Destroy();
	int imagesize = h*CalculatePitch(CalculateLine(w,bitscount));
	
	int bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + biClrUsed*sizeof(RGBQUAD);
	
	//The bitmap bit values are aligned on double word boundaries, 
	//so dwOffset must be a multiple of the size of a DWORD
	bfOffBits = bfOffBits + 3 & ~3 ;

	int filesize = imagesize + bfOffBits;
	if(!m_fileMapping.Open(szName,filesize,true))
	{
		DebugOutF(filelog::log_error,"create file mapping object failed");
		return  false;
	}
	BITMAPFILEHEADER& fhead = *(BITMAPFILEHEADER*)m_fileMapping.GetPointer();
	BITMAPINFOHEADER& infhead = *(BITMAPINFOHEADER*)&(&fhead)[1];
	ZeroMemory(&infhead,sizeof(infhead));
	ZeroMemory(&fhead,sizeof(fhead));

	infhead.biSize		= sizeof(BITMAPINFOHEADER);
	infhead.biBitCount	= bitscount;
	infhead.biPlanes	= 1;
	infhead.biHeight	= -	h;
	infhead.biWidth		= w;
	infhead.biSizeImage	= imagesize ;
	infhead.biCompression = BI_RGB;

	fhead.bfType = 0x4d42;
	fhead.bfSize = filesize; 
	fhead.bfOffBits =  bfOffBits ;
	if(biClrUsed)
	{
		memcpy( &(&infhead)[1],pPalette,sizeof(RGBQUAD)*biClrUsed );
	}
	return true;
}
void CFileMappingBitmap::Destroy()
{
	
	m_fileMapping.Destroy();
}