/*! \file $Id: ChangeRecorder.h 75 2011-07-31 14:54:01Z caoym $
*  \author caoyangmin
*  \brief CChangeRecorder
*/
#pragma once
#include "FileMapping.h"
#include <list>
#include "thread/BaseThread.h"

//! record the image changes of the screen
class CChangeRecorder:public CBaseThread
{
public:
	CChangeRecorder(void);
	~CChangeRecorder(void);
	typedef std::list<RECT> CHANGES;
	//! get and clean changes
	bool GetChangeList(CHANGES&);
	//! stop recording
	bool Run();
	bool Stop();
private:
	HANDLE m_hBegin;
	bool ThreadLoop();
	bool WaitForBegin();
#ifdef _WIN64
	bool RunHookProcess(PROCESS_INFORMATION &pi); 
#endif
};
