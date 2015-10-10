/*! \file $Id: IBitmapBuffer.h 94 2011-10-08 15:36:05Z caoym $
*  \author caoym
*  \brief IBitmapBuffer			
*/
#pragma once

#include "reference/IReference.h"

//! basic bitmap buffer
class IBitmapBuffer:public CReference
{
public:
	virtual ~IBitmapBuffer(void){};
	virtual const BITMAPINFOHEADER & Header()const = 0;
	virtual const RGBQUAD*Palette()const  = 0;
	virtual unsigned int GetClrUsed()const = 0;
	virtual const void *GetPointer()const  = 0;
	virtual unsigned int GetImageSize()const  = 0;
	virtual unsigned int GetFileSize()const  = 0;
	virtual LPCTSTR GetFileMappingName()const = 0;
};
