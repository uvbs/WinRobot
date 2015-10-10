/*! \file $Id: SessionHost.h 33 2011-04-02 13:58:53Z caoym $
*  \author caoym
*  \brief CSessionHost
*/
#pragma once
#include "thread/BaseThread.h"
//! run WinRobotSession
/*!
	auto stop when parent process end
*/
class CSessionHost: public CBaseThread
{
public:
	CSessionHost(ULONG );
	~CSessionHost(void);
	
private:
	bool ThreadLoop();
	void OnExit();
	void OnBegin();

	HANDLE m_hParent;
};
