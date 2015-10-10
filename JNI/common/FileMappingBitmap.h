/*! \file $Id: FileMappingBitmap.h 94 2011-10-08 15:36:05Z caoym $
*  \author caoym
*  \brief CFileMappingBitmap
*/
#pragma once
#include "Reference/IReference.h"
#include "FileMapping.h"

inline int
CalculateLine(int width, int bitdepth) {
	return ((width * bitdepth) + 7) / 8;
}

inline int
CalculatePitch(int line) {
	return line + 3 & ~3;
}

inline int
CalculateUsedPaletteEntries(int bit_count) {
	if ((bit_count >= 1) && (bit_count <= 8))
		return 1 << bit_count;

	return 0;
}

/**
 * Create a bitmap on file mapping memory()
 */
class CFileMappingBitmap:public CReference
{
public:
	CFileMappingBitmap(void);
	virtual ~CFileMappingBitmap(void);
	/**
	 * Create the CFileMappingBitmap object
	 * \param w bitmap width, in pixels
	 * \param h bitmap height, in pixels
	 * \param bitscount Specifies the number of bits required to identify the color of a single pixel
	 * \param biClrUsed Specifies the number of color indexes in the color table that are actually \n
	 * used by the bitmap.
	 * \param pPalette palette,ignored when biClrUsed == 0
	 * \param szName Specifies the name of the file mapping,or set null to specify a random name.
	 * \return True if succeed.
	 */
	bool Create(int w,int h,int bitscount,
		int biClrUsed=0,RGBQUAD*pPalette = 0,LPCTSTR szName = 0 );
	/**
	 * Destroy the CFileMappingBitmap object
	 */
	void Destroy();
	/**
	 * Get the bitmap file header
	 */
	const BITMAPFILEHEADER * FileHeader()const{
		if(GetFilePointer() == 0) return 0;
		return (BITMAPFILEHEADER*)GetFilePointer();
	}
	/**
	 * Get the bitmap info header
	 */
	const BITMAPINFOHEADER * InfoHeader()const {
		if(FileHeader() == 0) return 0;
		return (BITMAPINFOHEADER*)&FileHeader()[1];
	}
	/**
	 * Get the bitmap palette
	 */
	const RGBQUAD*Palette()const {
		if(InfoHeader() == 0) return 0;
		return InfoHeader()->biClrUsed?(RGBQUAD *)&(InfoHeader())[1]:0;
	}
	/**
	 * Get the bitmap number of color indexes in the color table
	 */
	unsigned int GetClrUsed()const{
		if(InfoHeader() == 0) return 0;
		return InfoHeader()->biClrUsed;
	}
	/**
	 * Get the begin of the bitmap file
	 */
	const void *GetFilePointer()const {
		return ((CFileMappingBitmap*)this)->m_fileMapping.GetPointer();
	}
	/**
	 * Get the begin of the image
	 */
	const void *GetImagePointer()const {
		if(GetFilePointer() == 0) return 0;
		return (char*)GetFilePointer() + FileHeader()->bfOffBits;
	}
	/**
	 * Get the begin of the image
	 */
	void *GetImagePointer() {
		return (void *)static_cast<const CFileMappingBitmap*>(this)->GetImagePointer();
	}
	/**
	 * Get the image size
	 */
	unsigned int GetImageSize()const {
		if(InfoHeader() == 0) return 0;
		return InfoHeader()->biSizeImage;
	}
	/**
	 * Get the file size
	 */
	unsigned int GetFileSize()const {
		if(InfoHeader() == 0) return 0;
		return FileHeader()->bfSize;
	}
	/**
	 * Get file mapping name 
	 */
	LPCTSTR GetFileMappingName()const {
		return m_fileMapping.GetName();
	}
	/**
	 * Get file mapping HANDLE 
	 */
	HANDLE GetHandle(){return m_fileMapping.GetHandle();}
private:
	CFileMapping m_fileMapping;
};
 