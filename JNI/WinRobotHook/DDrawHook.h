/*! \file $Id: DDrawHook.h 71 2011-07-11 16:03:56Z caoym $
*  \author caoym
*  \brief DirectDraw Hook
*/
#pragma once

//////////////////////////////////////////////////////////////////////////
//1.InstallDDrawHook with SetWindowsHookEx,so this dll will be load into every process
//2.InjectDDrawHook be called in DllMain when this dll is loaded,the memory of the API
//  calls(IDirectDrawSurface7::Flip,IDirectDrawSurface7::UpdateOverlay) will be overwrite,
//  inject our code.
//3.Every time those API is called(overwrite by ower code),we save the overlay surface 
//  pointer to memory.
//4.When we need to capture the screen,we broadcast CAPTURE_SCREEN message,each process 
//  receives this message save the overlay surface to named file mapping object.
//////////////////////////////////////////////////////////////////////////

//! Install DirectDraw hook
bool InstallDDrawHook(HINSTANCE inst);

//! Uninstall DirectDraw hook
bool UninstallDDrawHook();

//! Inject DirectDraw hook
/*
	called by DllMain in each hooked process
*/
bool InjectDDrawHook(void);

//! Uninject DirectDraw hook
/*
	called by DllMain in each hooked process
*/
void UninjectDDrawHook();


//! try to capture overlay to  named file mapping
/*
	called by hooked processes when receive "NotifyCapture"
*/
bool CaptureToSharedMemory();

//! wait for capture complete
/*
	because "Capture" is did in other processes,so we need to know
	when "Capture" is done.we use named Event object to do this.
*/
bool WaitForCaptureComplete();

//! Message that notify processes
extern const UINT CAPTURE_SCREEN;