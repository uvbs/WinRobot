/*! \file $Id: InputThread.h 64 2011-06-15 14:01:00Z caoym $
*  \author caoym
*  \brief CInputThread
*/
#pragma once
#include "thread/BaseThread.h"
#include "semqueue.h"
#include "reference/IReference.h"

class IInputEvent;

//!	mouse & keyboard input thread
class CInputThread:public CBaseThread
{
public:
	CInputThread(void);
	~CInputThread(void);
	//! begin thread
	bool Run();
	//! stop thread
	bool Stop();
	//! push a IInputEvent
	bool PushEvent(IInputEvent*);
	//! sleep specified time
	void Delay(int ms);
	//! get auto sleep time for each input event
	int AutoDelay()const {return m_nDelay;}
	//! set auto sleep time for each input event
	void AutoDelay(int ms) {m_nDelay = ms;}
	//! get whether sleep for each input event
	bool AutoWaitForIdle()const {return m_bAutoWaitForIdle; }
	//! set sleep time for each input event
	void AutoWaitForIdle(bool enable) {m_bAutoWaitForIdle = enable;}
	//! wait fot all input done
	bool WaitForIdle();

protected:
	void OnExit();
	void OnBegin();

	bool m_bClose;
	CCrtSection m_lock;
	HANDLE m_hDelayEvent;
	int m_nDelay;
	bool m_bAutoWaitForIdle;
	HANDLE m_hWaitForIdle;
	bool ThreadLoop();
	sem_queue< CRefObj<IInputEvent> > m_events;
};
