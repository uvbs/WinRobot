/*! \file	$Id: semqueue.h 94 2011-10-08 15:36:05Z caoym $
 *  \author caoym
 *  \brief	sem_queue:semaphore queue
 *          
 */
#ifndef __SQMQUEUE_H__
#define __SQMQUEUE_H__
#pragma once

#include "sync/cslock.h"
#include <list>
#ifdef WIN32

#else
	#include <semaphore.h>
	#include <time.h> 
	#include <sys/time.h>
#endif

#ifdef __arm__
	#ifndef SEM_VALUE_MAX
	#define SEM_VALUE_MAX   0x3FFFFFFF
	#endif
#endif
/**
 * Waitable&ThreadedSafe queue
 */
template<class T>
class sem_queue
{
public:
	typedef typename std::list<T> QUEUE;

	sem_queue():m_bOpen(false),m_nSize(0)
	{
		//open();
	}
	~sem_queue()
	{
		close();
		
	}
	/**
	 * result code
	 */
	enum RES_CODE
	{
		OK = 0, /**< succeeded*/
		TIMEOUT=1,/**< timout*/
		FAILED = -1,/**< error*/
	};
	/**
	 * push a object of T to the queue.
	 * if bForce == false,the call will be blocked until the queue has free space,
	 * or the object will be pushed immediately.
	 */
	bool push(const T&t,bool bForce=false)
	{
		if(!m_bOpen) return false;
		if(!bForce){

#ifdef WIN32
			HANDLE hd[] = {m_semin ,m_hExit};
			if(WAIT_OBJECT_0 != WaitForMultipleObjects(2,hd,FALSE,-1)) return false;
#else
			while( -1 == sem_wait(&m_semin) )
			{
				if(errno != EINTR) return false; 
			}
#endif
		}

		{
			CAutoLock<CCrtSection> lock(m_csLock);
			if(!m_bOpen) 
			{
#ifndef WIN32
				sem_post(&m_semin);
#endif
				return false;
			}
			m_queue.push_back(t);
			m_nSize++;
		}
	#ifdef WIN32
		return TRUE == ReleaseSemaphore(m_semout,1,0);
	#else
		return 0 ==sem_post(&m_semout);
	#endif
	}
	/**
	 * pop a object of T from the queue.
	 * the call will not be blocked if the queue has more then one item
	 */
	bool peek(T& t)
	{
		if(!m_bOpen) return false;
#ifdef WIN32
		HANDLE hd[] = {m_semout ,m_hExit};
		if(WAIT_OBJECT_0 != WaitForMultipleObjects(2,hd,FALSE,0)) return false;
#else
		if( -1 == sem_trywait(&m_semout) )return false;
#endif
		{
			CAutoLock<CCrtSection> lock(m_csLock);
			if(m_nSize==0) return false;
			t = m_queue.front();
			m_queue.pop_front();
			m_nSize--;
		}
#ifdef WIN32
		return TRUE == ReleaseSemaphore(m_semin,1,0);
#else
		return 0 ==sem_post(&m_semin);
#endif
		return true;
	}
	/**
	 * pop a object of T from the queue.
	 * the call will be blocked until the queue has more then one item
	 */
	bool pop(T& t)
	{
		if(!m_bOpen) return false;
#ifdef WIN32
		HANDLE hd[] = {m_semout ,m_hExit};
		if(WAIT_OBJECT_0 != WaitForMultipleObjects(2,hd,FALSE,-1)) return false;
#else
		while( -1 == sem_wait(&m_semout) )
		{
			if(errno != EINTR) return false; 
		}
#endif
		{
			CAutoLock<CCrtSection> lock(m_csLock);
			if(!m_bOpen || !m_nSize) 
			{
#ifndef WIN32
				sem_post(&m_semout);
#endif
				return false;
			}
			t = m_queue.front();
			m_queue.pop_front();
			m_nSize--;
		}
#ifdef WIN32
		return TRUE == ReleaseSemaphore(m_semin,1,0);
#else
		return 0 ==sem_post(&m_semin);
#endif
		
	}

	/**
	 * close the queue
	 * all blocked pop and push will return immediately
	 */
	bool close()
	{
		CAutoLock<CCrtSection> lock(m_csLock);
		if(!m_bOpen) return false;
		m_bOpen = false;
#ifdef WIN32
		SetEvent(m_hExit);
#else
		if(-1 == sem_post(&m_semout)) return false;
		if(-1 == sem_post(&m_semin)) return false;
		
#endif
		T t;
		while (peek(t))
		{
		}

#ifdef WIN32
		if(m_semin)CloseHandle(m_semin);
		if(m_semout)CloseHandle(m_semout);
		if(m_hExit)CloseHandle(m_hExit);
#else
		sem_destroy(&m_semin) ;
		sem_destroy(&m_semout) ;
#endif
		return m_bOpen;
	}
	/**
	 * open the queue
	 * \param iMax max size of the queue
	 */
	bool open(int iMax=0)
	{
		CAutoLock<CCrtSection> lock(m_csLock);
		if(m_bOpen) return false;
#ifdef WIN32
		m_semin = CreateSemaphore(0,iMax?iMax:MAXLONG,MAXLONG,0);
		m_semout = CreateSemaphore(0,0,MAXLONG,0);
		m_hExit = CreateEvent(0,1,0,0);
#else
		sem_init(&m_semin, 0, iMax?iMax:SEM_VALUE_MAX) ;
		sem_init(&m_semout, 0, 0) ;
#endif

#ifdef WIN32
		while(WAIT_OBJECT_0 == WaitForSingleObject( m_semout, 0L))
		{}
		ResetEvent(m_hExit);
#else
		while(sem_trywait(&m_semout) == 0){};
#endif
		m_bOpen = true;
		return m_bOpen;
	}
	/**
	 * current queue size
	 */
	size_t size() const
	{
		/*CAutoLock<CCrtSection> lock(m_csLock);*/
		return m_nSize;
	}
	/**
	 * whether queue is empty
	 */
	bool empty()const 
	{
		return m_nSize==0;
	}
private:
	CCrtSection m_csLock;
	size_t m_nSize;
	QUEUE m_queue;
#ifdef WIN32
	HANDLE m_semin;
	HANDLE m_semout;
	HANDLE m_hExit;
#else
	sem_t m_semin;
	sem_t m_semout;
#endif
	//int m_iMax;
	bool m_bOpen;
};
#endif //__SQMQUEUE_H__
