/*! \file $Id: FileMapping.cpp 77 2011-08-08 16:13:48Z caoym $
*  \author caoym
*  \brief CFileMapping			
*/
#include "StdAfx.h"
#include "FileMapping.h"
#include <atlsecurity.h>
#include "debuglogger.h"
//#include "Singleton_T.h"
#include <map>
#include <sstream>
volatile LONG CFileMapping::s_nRandonNameTick  = 0;

//! store unused FileMapping handles
/*
* store unused FileMapping handles
*/
class CFileMappingCache
{
public:
	CFileMappingCache()
	{
		
	}
	~CFileMappingCache()
	{
		
		for (CACHES::iterator it = m_caches.begin();
			it!=m_caches.end();it++)
		{
			CloseHandle(it->second.handle);
		}
	}
	bool AddCache(HANDLE hd,DWORD size,const std::basic_string<TCHAR> &  name)
	{
		
		CAutoLockEx<CCrtSection> lock(m_csLock);
		// to more caches,delete the more unusual one
		if(m_caches.size() >= 16)
		{
			CloseHandle(m_caches.begin()->second.handle);
			m_caches.erase(m_caches.begin());
			//OutputDebugStringA("erase cache ok");
		}
		m_caches.insert(std::make_pair(1,CACHE(hd,size,name)));

		return true;
	}
	HANDLE GetCache(DWORD size,std::basic_string<TCHAR> & name )
	{
		
		CAutoLockEx<CCrtSection> lock(m_csLock);

		for (CACHES::iterator it = m_caches.begin();
			it!=m_caches.end();it++)
		{
			if (it->second.size == size)
			{
				name = it->second.name;
				HANDLE hMutexDup = 0;
				// return duplicated handle
				DuplicateHandle(GetCurrentProcess(), 
					it->second.handle, 
					GetCurrentProcess(),
					&hMutexDup, 
					0,
					FALSE,
					DUPLICATE_SAME_ACCESS);

				m_caches.insert(std::make_pair(it->first+1,it->second));
				m_caches.erase(it);
				//OutputDebugStringA("GetCache ok");
				return hMutexDup;
			}
		}
		return 0;
	}
private:
	 
	struct CACHE
	{
		CACHE(HANDLE a_hd,DWORD a_size,const std::basic_string<TCHAR>&a_name)
			:handle(a_hd)
			,size(a_size)
			,name(a_name)
		{
			
		}
		HANDLE handle;
		DWORD size;
		std::basic_string<TCHAR> name;
	};
	// <used times,item>
	typedef std::multimap<int,CACHE> CACHES;
	CACHES m_caches;
	CCrtSection m_csLock;
};
CFileMappingCache g_fileMappingCache;
CFileMapping::CFileMapping()
:m_hMapFile(0)
,m_pData(0)
,m_nSize(0)
,m_bCreate(false)
,m_bCached(false)
,m_bUseCache(false)
{
	
}
bool CFileMapping::Open(LPCTSTR name,DWORD size,bool bCreate,DWORD flProtect/*=PAGE_READWRITE*/)
{
	
	Destroy();
	
	if (bCreate)
	{
		

		// if create a unnamed object,try to find a unused from cache
		if(name == 0)
		{
			m_bUseCache = true;
			if(m_hMapFile = g_fileMappingCache.GetCache(size,m_szName))
			{
				DebugOutF(filelog::log_debug,"hit the target in CFileMappingCache");
				m_bCached = true;
			}
		}
		if(!m_hMapFile)
		{
			if (name)
			{
				m_szName = name;
			}
			else
			{
				m_szName = GetRandomName( );
			}
			// allow "everyone" to access this object
			SID* pSid = 0;
			
			HRESULT hr = S_OK;
			try
			{	
				if(!ConvertStringSidToSid(_T("S-1-1-0"),(LPVOID*)&pSid))
				{
					DebugOutF(filelog::log_error,"ConvertStringSidToSid failed %d",GetLastError() );
					AtlThrowLastWin32(); 
				}
				CSid ace( pSid);
				if( !ace.IsValid() )
				{
					DebugOutF(filelog::log_error,"Create SID \"EveryOne\" failed with %d",GetLastError() );
					AtlThrowLastWin32(); 
				}
				CDacl dacl;
				if(!dacl.AddAllowedAce(ace,0x10000000))
				{
					DebugOutF(filelog::log_error,"AddAllowedAce failed with %d",GetLastError() );
					AtlThrowLastWin32(); 
				}
				CSecurityDesc sd;
				
				sd.SetDacl(dacl,false);
				CSecurityAttributes security(sd,false);
				m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, &security, PAGE_READWRITE,0, size, m_szName.c_str());
			}
			catch (CAtlException e)
			{
				hr = e.m_hr;
				
			}
			if(pSid)
			{
				LocalFree(pSid);
				pSid = 0;
			}
			if(FAILED(hr))
			{
				DebugOutF(filelog::log_error,"catch CAtlException 0x%x",hr );
				return false;
			}

			if (m_hMapFile == 0)
			{
				DebugOutF(filelog::log_error,"CreateFileMapping %s failed with %d",(const char*)CT2A(m_szName.c_str()),GetLastError());
				return false;
			}
			else
			{
				DebugOutF(filelog::log_debug,"CreateFileMapping %s ok",(const char*)CT2A(m_szName.c_str()),GetLastError());
			}
		}
	}
	else
	{
		
		m_szName  = name?name:_T("");
		m_hMapFile = OpenFileMapping( FILE_MAP_WRITE|FILE_MAP_READ,FALSE,m_szName.c_str());
		if (m_hMapFile == 0)
		{
			DebugOutF(filelog::log_error,"OpenFileMapping %s failed tith %d",(const char*)CT2A(m_szName.c_str()),GetLastError());
			return false;
		}
		else
		{
			DebugOutF(filelog::log_debug,"OpenFileMapping %s ok",(const char*)CT2A(m_szName.c_str()),GetLastError());
		}
	}
	m_pData = MapViewOfFile(m_hMapFile, FILE_MAP_WRITE,0, 0, size);
	if (m_pData == 0)
	{
		DebugOutF(filelog::log_error,"MapViewOfFile failed tith %d",GetLastError());
		return false;
	}
	m_bCreate = bCreate;
	m_nSize = size;
	//m_bUseCache = bUseCache;
	return true;
}
CFileMapping::~CFileMapping(void)
{
	Destroy();
}
void CFileMapping::Destroy()
{

	if (m_pData)
	{
		UnmapViewOfFile(m_pData);
		m_pData = 0;
	}
	// I need cache it ?
	if(m_bCreate && m_hMapFile && !m_bCached && m_bUseCache)
	{
		// add to cache
		if(!g_fileMappingCache.AddCache(m_hMapFile,m_nSize,m_szName))
		{
			CloseHandle(m_hMapFile);	
		}
		m_hMapFile = 0;
	}
	
	if(m_hMapFile)
	{
		CloseHandle(m_hMapFile);
		m_hMapFile = 0;
	}
	m_nSize = 0;
}
std::basic_string<TCHAR> CFileMapping::GetRandomName()
{
	

	::InterlockedIncrement(&s_nRandonNameTick);

	std::basic_ostringstream<TCHAR, std::char_traits<TCHAR>,
		std::allocator<TCHAR> >  oss;
	oss << _T("Global\\") << GetCurrentProcessId() << _T("-") << s_nRandonNameTick << _T("-74EF6127C-0846-4c63-8EAE-6C2DF379A618");
	
	return oss.str();
}