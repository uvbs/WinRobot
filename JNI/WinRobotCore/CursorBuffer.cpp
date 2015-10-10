/*! \file $Id: CursorBuffer.cpp 118 2011-11-15 16:22:30Z caoym $
*  \author caoym
*  \brief CCursorBuffer
*/
#include "StdAfx.h"
#include "CursorBuffer.h"
#include "debuglogger.h"
#include <algorithm>

CCursorBuffer::CCursorBuffer(void)
:m_hCursor(0)
{
	memset(&m_ptHotspot,0,sizeof(m_ptHotspot));
	memset(&m_ptPosition,0,sizeof(m_ptPosition));
}

CCursorBuffer::~CCursorBuffer(void)
{
	Destroy();
}
bool CCursorBuffer::Create()
{
	Destroy();
	m_hCursor = 0;
	CURSORINFO  cursorinfo;
	memset( &cursorinfo,0,sizeof(cursorinfo) );
	cursorinfo.cbSize = sizeof(CURSORINFO);

	ICONINFO iconinfo;
	memset(&iconinfo,0,sizeof(iconinfo));

	struct BITMAPINFO_X{
		BITMAPINFO bmi;
		RGBQUAD bmiColors[256];
	}mask_bmi;

	memset(&mask_bmi ,0,sizeof(mask_bmi));
	BITMAPINFO&bmi = mask_bmi.bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	HRESULT hr = S_OK;
	LONG w=0,h=0;
	HDC hdc = 0;
	HDC hTemp = 0;
	HDC hMemDC = 0;
	HBITMAP hBmp= 0;
	LPVOID pBuff= 0; 
	BYTE* pBitmasks = 0;
	try
	{
		hdc = GetDC(0);
		if (hdc ==0 )
		{
			DebugOutF(filelog::log_error,"GetDC failed with %d",GetLastError() );
			throw E_FAIL; 
		}
		// get cursor info
		if(!GetCursorInfo(&cursorinfo) )
		{
			DebugOutF(filelog::log_error,"GetCursorInfo failed with %d",GetLastError() );
			throw E_FAIL; 
		}
		m_hCursor = cursorinfo.hCursor;
		// get icon info
		if(!GetIconInfo(cursorinfo.hCursor,&iconinfo)) 
		{
			DebugOutF(filelog::log_error,"GetIconInfo failed with %d",GetLastError() );
			throw  E_FAIL;
		}
		// get hotspot
		m_ptHotspot.x = iconinfo.xHotspot;
		m_ptHotspot.y = iconinfo.yHotspot;
		// get position
		m_ptPosition = cursorinfo.ptScreenPos;
		LONG lWidth = 0,lHeight=0;
		// get icon width and height
		if(iconinfo.hbmColor == 0){ // Monochrome  cursor
			if(0 == GetDIBits(hdc,iconinfo.hbmMask,0,1,NULL,&bmi,DIB_RGB_COLORS)) 
			{
				DebugOutF(filelog::log_error,"GetDIBits failed with %d",GetLastError() );
				throw  E_FAIL;
			}
			lWidth = abs(bmi.bmiHeader.biWidth);
			lHeight = abs(bmi.bmiHeader.biHeight)/2;

		
		}else { // multicolor cursor
			if(0 == GetDIBits(hdc,iconinfo.hbmColor,0,1,NULL,&bmi,DIB_RGB_COLORS))
			{
				DebugOutF(filelog::log_error,"GetDIBits failed with %d",GetLastError() );
				throw  E_FAIL;
			}
			lWidth = abs(bmi.bmiHeader.biWidth);
			lHeight = abs(bmi.bmiHeader.biHeight);	
		}
		
		// create mem buffer
		hMemDC = CreateCompatibleDC(hdc);
		if (hMemDC == 0)
		{
			DebugOutF(filelog::log_error,"CreateCompatibleDC failed with %d",GetLastError() );
			throw  E_FAIL;
		}
		DWORD dwSize = lHeight*CalculatePitch(CalculateLine(lWidth,32));
		DWORD bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		
		//The bitmap bit values are aligned on doubleword boundaries, 
		//so dwOffset must be a multiple of the size of a DWORD
		bfOffBits = bfOffBits + 3 & ~3 ;
		DWORD dwFileSize = bfOffBits + dwSize;
		// save [bmp file header] + [bmp info header] + [bmp data] to the file mapping object
		if(!CFileMappingBitmap::Create(lWidth,lHeight,32))
		{
			DebugOutF(filelog::log_error,"create file mapping object failed");
			throw  E_FAIL;
		}
		
		hBmp = CreateDIBSection(hMemDC,(BITMAPINFO*)InfoHeader(),
			DIB_RGB_COLORS, (void**)&pBuff,
			GetHandle(), 
			FileHeader()->bfOffBits);
		if(hBmp == NULL){
			DebugOutF(filelog::log_error,("CreateDIBSection failed %d"),GetLastError() );
			throw E_FAIL;
		}
		SelectObject(hMemDC,hBmp);


		if(iconinfo.hbmColor == 0){ 
			// Monochrome  cursor
			// use AND and XOR bitmasks
			hTemp = CreateCompatibleDC(0);
			if(hTemp == 0)
			{
				DebugOutF(filelog::log_error,"CreateCompatibleDC failed with %d",GetLastError() );
				throw E_FAIL ;
			}
			SelectObject(hTemp,iconinfo.hbmMask);
			HBRUSH bWriteBrush = CreateSolidBrush(RGB(255,255,255));
			RECT rc = {0,0,lWidth,lHeight};
			FillRect(hMemDC,&rc ,bWriteBrush);
			DeleteObject(bWriteBrush);

   			if(!BitBlt(hMemDC,0,0,lWidth,lHeight,hTemp,0,0,SRCAND))
   			{
   				DebugOutF(filelog::log_error,"BitBlt failed with %d",GetLastError() );
   				throw E_FAIL ;
   			}
  			if(!BitBlt(hMemDC,0,0,lWidth,lHeight,hTemp,0,lHeight,SRCINVERT))
  			{
  				DebugOutF(filelog::log_error,"BitBlt failed with %d",GetLastError() );
  				throw E_FAIL ;
  			}
			pBitmasks = new BYTE[bmi.bmiHeader.biSizeImage];
			

			if(0 == GetDIBits(hTemp,iconinfo.hbmMask,0,abs(bmi.bmiHeader.biHeight),pBitmasks,&bmi,DIB_RGB_COLORS))
			{
				DebugOutF(filelog::log_error,"GetDIBits failed with %d",GetLastError() );
				throw  E_FAIL;
			}
			// set alpha channel
 			for (int h =0;h<lHeight;h++)
 			{
 				for (int w =0;w<lWidth;w++)
 				{
 					BYTE* pPixel = &((BYTE*)pBuff)[h*CalculatePitch(CalculateLine(lWidth,32)) + w*4];
					BYTE and = pBitmasks[(lHeight*2-1-h)*bmi.bmiHeader.biWidth/8 + w/8] & (0x80 >> (w%8));
					BYTE xor = pBitmasks[(lHeight-1-h)*bmi.bmiHeader.biWidth/8 + w/8] & (0x80 >> (w%8));
					if((and == 0) || (xor != 0))
 					{
 						pPixel[3] = 255;
 					}
					else
					{
						pPixel[3] = 0;
					}
 				}
 			}
//			HANDLE hf = INVALID_HANDLE_VALUE;
//			
// 			hf = CreateFile(_T("c:\\cursor.bmp"), 
// 			GENERIC_READ | GENERIC_WRITE, 
// 			(DWORD) 0, 
// 			NULL, 
// 			CREATE_ALWAYS, 
// 			FILE_ATTRIBUTE_NORMAL, 
// 			(HANDLE) NULL); 
// 
// 			DWORD cbWrite = 0;
// 			WriteFile(hf, m_fileMapping.GetPointer(), m_fileMapping.GetSize(), &cbWrite,  NULL) ;
// 			CloseHandle(hf);
		}
		else
		{
			// RGB cursor,just draw on DC
			HBRUSH bWriteBrush = CreateSolidBrush(RGB(255,255,255));
			RECT rc = {0,0,lWidth,lHeight};
			FillRect(hMemDC,&rc ,bWriteBrush);
			DeleteObject(bWriteBrush);
			if(!DrawIconEx(hMemDC,0,0,cursorinfo.hCursor,lWidth,lHeight,0,0,DI_NORMAL))
			{
				DebugOutF(filelog::log_error,"DrawIconEx failed with %d",GetLastError() );
				throw E_FAIL ;
			}
		}
		
	}
	catch (HRESULT hr1)
	{
		hr = hr1;
	}
	m_hCursor = cursorinfo.hCursor;
	if(pBitmasks){
		delete [] pBitmasks;
	}
 	if(cursorinfo.hCursor)
 	{
 		DeleteObject(cursorinfo.hCursor);
 	}
	if(iconinfo.hbmColor)
	{
		DeleteObject(iconinfo.hbmColor);
	}
	if (iconinfo.hbmMask)
	{
		DeleteObject(iconinfo.hbmMask);
	}
	if (hdc)
	{
		ReleaseDC(0,hdc);
	}
	if(hTemp)
	{
		DeleteDC(hTemp);
	}
	if(hMemDC)
	{
		DeleteDC(hMemDC);
	}
	if(hBmp){
		DeleteObject(hBmp);
	}
	if ( FAILED(hr) )
	{
		return false;
	}
	DebugOutF(filelog::log_debug,"CCursorBuffer::Create ok");
	return true;
}
CursorType CCursorBuffer::Type()const{

	//predefined cursors 
	static HCURSOR type_map[] = {
		LoadCursor(NULL,IDC_APPSTARTING),
		LoadCursor(NULL,IDC_ARROW),
		LoadCursor(NULL,IDC_CROSS),
		LoadCursor(NULL,IDC_HAND),
		LoadCursor(NULL,IDC_HELP),
		LoadCursor(NULL,IDC_IBEAM),
		LoadCursor(NULL,IDC_ICON),
		LoadCursor(NULL,IDC_NO),
		LoadCursor(NULL,IDC_SIZE),
		LoadCursor(NULL,IDC_SIZEALL),
		LoadCursor(NULL,IDC_SIZENESW),
		LoadCursor(NULL,IDC_SIZENS),
		LoadCursor(NULL,IDC_SIZENWSE),
		LoadCursor(NULL,IDC_SIZEWE),
		LoadCursor(NULL,IDC_UPARROW),
		LoadCursor(NULL,IDC_WAIT),
	}; 
	HCURSOR* pos = std::find(type_map,type_map+RTL_NUMBER_OF(type_map),m_hCursor);
	if(pos == (type_map+RTL_NUMBER_OF(type_map)) ) return CURSOR_UNKNOWN;
	return static_cast<CursorType>(pos - type_map);
}