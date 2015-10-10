/*! \file $Id: HookHost.h 76 2011-08-02 14:56:09Z caoym $
*  \author caoym
*  \brief CHookHost
*/
#pragma once
//! run host process
/*!
	we use SetWindowsHookEx to inject a DLL into another process,to capture overlay image.
	A 32-bit DLL cannot be injected into a 64-bit process, and a 64-bit DLL cannot be 
	injected into a 32-bit process. So if we run in 64-bit platform,we must run another 
	32-bit process to inject 32-bit dll to 32-bit processes.
*/
class CHookHost
{
public:
	CHookHost(ULONG id);
	~CHookHost(void);
	bool Run();
private:
	HANDLE m_hParent;
};
