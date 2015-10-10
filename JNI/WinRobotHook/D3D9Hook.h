/*! \file $Id: D3D9Hook.h 56 2011-05-08 04:31:30Z caoym $
*  \author caoym
*  \brief DirectX 9 API Hook
*/
#pragma once

//! install D3D hook
bool InstallD3D9Hook();


//! Uninstall D3D hook
bool UninstallD3D9Hook(void);

//! Inject D3D hook
/*
	called by DllMain in each hooked process
*/
bool InjectD3D9Hook(void);

