
/*! \file	$Id: cslock.h 87 2011-09-28 13:52:41Z caoym $
 *  \author caoym
 *  \brief	CAutoLock CAutoLockEx CMutexLock(for Windows) CCrtSection(for Windows/Linux)
 *          
 */
#ifndef __CCSLOCK_H__
#define __CCSLOCK_H__

#ifndef WIN32
	#include <pthread.h>
	#include <sys/types.h>
	#include <errno.h>
	#include "interlock.h"
#else
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <atlsecurity.h>
#endif


#ifdef WIN32
// Windows compability
	typedef HANDLE pthread_t;
	typedef HANDLE pthread_mutex_t;
	typedef HANDLE pthread_cond_t;
	typedef DWORD pthread_key_t;
#endif

/**
 * The CAutoLock constructor locks the T object, and the destructor unlocks it. 
 */
template <class T> 
class CAutoLock
{
public:
	CAutoLock(T& lock);
	~CAutoLock();
private:
	T& m_rLock;
};

template <class T>
CAutoLock<T>::CAutoLock(T& lock)
:m_rLock(lock)
{
	m_rLock.Lock();
}

template <class T>
CAutoLock<T>::~CAutoLock()
{
	m_rLock.Unlock();
}
/**
 * The CAutoLockEx constructor locks the T object, and the destructor unlocks it. 
 * 
 */
template <class T> 
class CAutoLockEx
{
public:
	CAutoLockEx(T& lock,bool block = true,bool bManu = false);
	~CAutoLockEx();
	bool Locked(){return m_bLocked;}
	bool TryLock(){m_bLocked = m_rLock.TryLock();return m_bLocked;}
	void UnLock(){if(m_bLocked){ m_bLocked=false; m_rLock.Unlock();}}
private:
	T& m_rLock;
	bool m_bLocked;
};

template <class T>
CAutoLockEx<T>::CAutoLockEx(T& lock,bool block ,bool bManu )
:m_rLock(lock)
,m_bLocked(false)
{
	if(!bManu)
	{
		if (block == true)
		{
			m_rLock.Lock();
			m_bLocked = true;
		}
		else
		{
			m_bLocked = m_rLock.TryLock();
		}
	}
}

template <class T>
CAutoLockEx<T>::~CAutoLockEx()
{
	if(m_bLocked)
		m_rLock.Unlock();
}

/**
 * This class is a wrapper for a mutex object.
 * 
 */
class CMutexLock
{
public:
	CMutexLock(const char* name = 0)
	{
	#ifdef WIN32
		if(name)
		{
			SID* pSid = 0;
			ConvertStringSidToSid(_T("S-1-1-0"),(LPVOID*)&pSid);

			ATL::CSid ace(pSid);
			ATL::CDacl dacl;
			dacl.AddAllowedAce(ace,0x10000000);

			ATL::CSecurityDesc sd;
			sd.SetDacl(dacl,false);

			ATL::CSecurityAttributes security(sd,false);
			m_hMutex  = CreateMutexA(&security, FALSE, name); 
			LocalFree(pSid);
		}
		else
		{
			m_hMutex  = CreateMutexA(NULL, FALSE, name); 
		}
	#else
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init(&m_hMutex, &attr);
	#endif

	}
	~CMutexLock()
	{
		#ifdef WIN32
			CloseHandle(m_hMutex);
		#else
			pthread_mutex_destroy(&m_hMutex);
		#endif
		
		
	}
	bool TryLock() 
	{
		#ifdef WIN32
			return WaitForSingleObject(m_hMutex,0) == WAIT_OBJECT_0 ;
		#else
			return pthread_mutex_trylock(&m_hMutex) != EBUSY;
		#endif
	}
	long Lock()
	{
	#ifdef WIN32
		if(WaitForSingleObject(m_hMutex,INFINITE) == WAIT_OBJECT_0) 
			return 1;
		return 0;
	#else
		pthread_mutex_lock(&m_hMutex);
		return 1;
	#endif

	}
	long Unlock()
	{
		

	#ifdef WIN32
		ReleaseMutex(m_hMutex);
	#else
		pthread_mutex_unlock(&m_hMutex);
	#endif
		
		return 1;
	}

private:

	pthread_mutex_t  m_hMutex;            // Alias name of the mutex to be protected

};
typedef CMutexLock CMutexLockEx;


#ifdef WIN32
/**
 * This class is a wrapper for a critical section
 */
class CCrtSection
{
public:
	CCrtSection()
	{
		InitializeCriticalSection(&m_csLock);
	}
	~CCrtSection(){
		DeleteCriticalSection(&m_csLock);
	}
	long Lock()
	{
		EnterCriticalSection(&m_csLock);
		return m_csLock.LockCount;
	}
	long Unlock()
	{
		LeaveCriticalSection(&m_csLock);
		return m_csLock.LockCount;
	}
	bool TryLock()
	{
		return TryEnterCriticalSection(&m_csLock) != 0;
	}
private:
	CRITICAL_SECTION m_csLock;
};
typedef CCrtSection CCrtSectionEx;
#else
typedef CMutexLock CCrtSection;
#endif //WIN32

typedef  CAutoLock<CCrtSection> CCSLock;


#endif //__CCSLOCK_H__
