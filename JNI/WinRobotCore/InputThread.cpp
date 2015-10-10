/*! \file $Id: InputThread.cpp 86 2011-09-18 15:30:23Z caoym $
*  \author caoym
*  \brief CInputThread
*/
#include "StdAfx.h"
#include "InputThread.h"
#include "IInputEvent.h"
#include <assert.h>
#include "SwitchInputDesktop.h"
#include "debuglogger.h"

CInputThread::CInputThread(void)
:m_nDelay(0)
,m_hDelayEvent(0)
,m_bAutoWaitForIdle(false)
,m_hWaitForIdle(0)
,m_bClose(false)
{
	m_hDelayEvent = CreateEvent(0,TRUE,FALSE,0);
	m_hWaitForIdle = CreateEvent(0,TRUE,FALSE,0);
}

CInputThread::~CInputThread(void)
{
	
	Stop();

	if (m_hDelayEvent)
	{
		CloseHandle(m_hDelayEvent);
	}
	if(m_hWaitForIdle)
	{
		CloseHandle(m_hWaitForIdle);
	}
}
bool CInputThread::WaitForIdle()
{
	return WaitForSingleObject(m_hWaitForIdle,-1) == WAIT_OBJECT_0;
}
bool CInputThread::Run()
{
	CAutoLock<CCrtSection> lock(m_lock);
	m_bClose = false;
	if(!m_events.open())
	{
		return false;
	}
	return CBaseThread::Run();
}
bool CInputThread::Stop()
{
	{
		CAutoLock<CCrtSection> lock(m_lock);
		m_bClose = false;
		if(m_hDelayEvent)
		{
			SetEvent(m_hDelayEvent);
		}
		if(m_hWaitForIdle)
		{
			SetEvent(m_hWaitForIdle);
		}
	}
	if(!m_events.close())
	{
		return false;
	}
	return CBaseThread::Stop();
}
bool CInputThread::PushEvent(IInputEvent*pEvent)
{
	CAutoLockEx<CCrtSection> lock(m_lock);
	if(m_bClose) return false;
	ResetEvent(m_hWaitForIdle);
	
	if(m_events.push(pEvent))
	{
		lock.UnLock();
		if(m_bAutoWaitForIdle) return WaitForIdle();
		return true;
	}
	return false;
}
void CInputThread::Delay(int ms)
{
	if(ms)
		WaitForSingleObject(m_hDelayEvent,ms);
}
bool CInputThread::ThreadLoop()
{
	
	CRefObj<IInputEvent> pEvent;
	if (m_events.pop(pEvent))
	{
		SwitchInputDesktop(); //try to switch to input desktop
		Delay(m_nDelay);
		pEvent->Input();
		if ( m_events.size() )
		{
			
		}
		else
		{
			SetEvent(m_hWaitForIdle);
		}
		return true;
	}
	return false;
}
void CInputThread::OnExit()
{
	DebugOutF(filelog::log_info,"CInputThread exit");
}
void CInputThread::OnBegin()
{
	DebugOutF(filelog::log_info,"CInputThread begin");
}