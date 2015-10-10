/*! \file $Id: FileMapping.h 77 2011-08-08 16:13:48Z caoym $
*  \author caoym
*  \brief CFileMapping			
*/
#pragma once
#include <string>
//! warpper for Windows file mapping
class CFileMapping
{
public:
	CFileMapping();
	virtual ~CFileMapping(void);
	//! open or create a named file mapping object
	/*!
	 * \param name The name of the file mapping object. 
	 * If this parameter is NULL, and bCreate is true,the file mapping object is 
	 * created with a random name.
	 * \param size The size of the file mapping object. 	
	 * \param bCreate If this parameter is true,and the parameter name matches the
	 * name of an existing mapping object, the function open the existing object,
	 * else a new existing mapping object will be created if succeeded.if bCreate 
	 * is false,the function will try to open a existing object.
	 * \return If the function succeeds, the return is true.
	*/
	bool Open(LPCTSTR name,DWORD size,bool bCreate,DWORD flProtect=PAGE_READWRITE);
	//! Get the HANDLE of the file mapping object.
	HANDLE GetHandle(){return m_hMapFile;}
	//! Get pointer of the shared memory.
	LPVOID GetPointer() {return m_pData;}
	//! Get the size of the shared memory.
	DWORD GetSize()const {return m_nSize;}
	//! Get the name of the file mapping object.
	LPCTSTR GetName()const {return m_szName.c_str();}
	//! destroy
	void Destroy();
	//! Decide is open. 
	bool IsOpen()const {return m_hMapFile !=0;}
private:
	static std::basic_string<TCHAR> GetRandomName();
	std::basic_string<TCHAR> m_szName;
	static volatile LONG s_nRandonNameTick;

	HANDLE m_hMapFile;
	LPVOID m_pData;
	DWORD m_nSize ;
	// create or open
	bool m_bCreate;
	// is cached
	bool m_bCached;
	// use cache or not
	bool m_bUseCache;
};
