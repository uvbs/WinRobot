/*! \file $Id: BaseThread.h 33 2011-04-02 13:58:53Z caoym $
*  \author caoyangmin
*  \brief CBaseThread
*/

#if !defined(AFX_BASETHREAD_H__CB0E1EB6_F3C0_4663_888F_C66226684788__INCLUDED_)
#define AFX_BASETHREAD_H__CB0E1EB6_F3C0_4663_888F_C66226684788__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef WIN32
	#include <pthread.h>
	#include <time.h>
#else
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <process.h>
#endif

//! basic thread implement
class CBaseThread
{
	
public:
	CBaseThread():m_hThread(NULL)
		,m_bRunning(false)
		,m_bWaitStop(false)
	{
	}
	virtual ~CBaseThread()
	{
	}
	//! start thread
	virtual bool Run()
	{
		if(m_bRunning) return true;
		m_bRunning = true;
		m_bWaitStop = false;
#ifdef WIN32
		m_hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc, this, 0, (unsigned*)&m_dwThreadId );
		if (m_hThread == NULL) return false;

#else
		if (0 != pthread_create(&m_hThread, NULL, &ThreadFunc, this))
		{
			m_hThread = 0;
			return false;
		}
#endif
		return m_bRunning;
	}

	//! stop thread
	virtual bool Stop()
	{
		if(!m_bRunning) return true;

		WaitForStop();
#ifdef WIN32
		CloseHandle(m_hThread);
		m_hThread = NULL;
#else
		m_hThread = 0;
#endif
		m_bRunning = false;
		return true;
	}

	//! thread is running
	bool IsRunning()const {return m_bRunning;}

	//! wait for thread end
	bool WaitThreadEnd(unsigned long timeout = -1)
	{
		if(!m_bRunning) return true;

#ifdef WIN32
		return WaitForSingleObject(m_hThread,timeout ) == WAIT_OBJECT_0;
#else
		pthread_join(m_hThread, NULL);
#endif

		return true;
	}
#ifdef WIN32
	DWORD m_dwThreadId;
	HANDLE m_hThread;
#else
	pthread_t m_hThread;
#endif	
protected:
	//! thread loop
	/*!
		return false then thread will be end
	*/
	virtual bool ThreadLoop() = 0;
	//! wait for thread end
	/*!
		set stop flag,then wait for thread end
	*/
	virtual bool WaitForStop(unsigned long timeout = -1)
	{
		if(!m_bRunning) return true;
		m_bWaitStop = true;

#ifdef WIN32
		return WaitForSingleObject(m_hThread,timeout ) == WAIT_OBJECT_0;
#else
		pthread_join(m_hThread, NULL);
#endif

		return true;
	}

	//! called when thread end
	virtual void OnExit(){}

	//! called when thread begin
	virtual void OnBegin(){}

	volatile bool m_bRunning;
	volatile bool m_bWaitStop;

#ifdef WIN32
	static unsigned __stdcall ThreadFunc( void* pArguments )
#else
	static void* ThreadFunc( void* pArguments )
#endif	

	{
		CBaseThread*pThis = (CBaseThread*)pArguments;
		unsigned exetcode = 0;
		
		pThis->OnBegin();
		while(!pThis->m_bWaitStop){
			if(!pThis->ThreadLoop()){
				exetcode = 0;
				break;
			}
		}
	
		pThis->OnExit();
#ifdef WIN32
		
		return exetcode;
#else
		return NULL;
#endif

	} 
	
};
#endif // !defined(AFX_BASETHREAD_H__CB0E1EB6_F3C0_4663_888F_C66226684788__INCLUDED_)
