#pragma once
#include "FileMappingBitmap.h"
#include "FunctionHook.h"



//! screen capture use DirectX9
class CDX9ScreenCapture
{
public:
	CDX9ScreenCapture(void);
	~CDX9ScreenCapture(void);
	CRefObj<CFileMappingBitmap> Capture(const RECT&rc);
	bool Create();
	void Destroy();
private:
	
	
};
