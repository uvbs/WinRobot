/*! \file $Id: FunctionHook.h 57 2011-05-21 15:28:15Z caoym $
*  \author caoym
*  \brief CFunctionHook
*/
#pragma once
// #include "debuglogger.h"
//! hook API function
template<class CALL>
class CFunctionHook
{
public:
	CFunctionHook(void)
		:m_bInstall(false)
		,m_dwOriProtect(0)
	{
		memset(m_back,0,sizeof(m_back));
		memset(m_jump,0,sizeof(m_jump));
	}
	~CFunctionHook(void)
	{

	}
	//! install hook
	/**
	 *	replace the function with asm "jump" code
	*/
	bool Install(CALL pOri, CALL pNew)
	{
		if(m_bInstall)return true;
		//if(!Uninstall()) return false;

		if( pOri == 0 )return false;
		if(pOri == pNew) return false;
	//	DWORD dwOriProtect = 0;
		// make to writeable,so we can "jump"
		if ( !VirtualProtect( pOri, 8, PAGE_EXECUTE_READWRITE, &m_dwOriProtect))
		{
			OutputDebugStringA("VirtualProtect failed");
			return false;
		}

		m_jump[0] = 0xe9; // jump
		DWORD offset = (DWORD)((UINT_PTR)pNew - (UINT_PTR)pOri) - sizeof(m_jump);
		memcpy(&m_jump[1], &offset, sizeof(offset));
		if( memcmp(m_jump,pOri,sizeof(m_jump) )==0 )
		{
			VirtualProtect( pOri, 8, m_dwOriProtect, 0);
			return false;
		}
		memcpy(m_back, pOri, sizeof(m_back));
		memcpy(pOri, m_jump, sizeof(m_jump));
		m_pOri = pOri;
		m_bInstall = true;
		//VirtualProtect( pOri, 8, m_dwOriProtect, 0);
		return true;
	}
	void Restore()
	{
		if(m_bInstall)
		{
			memcpy(m_pOri, m_back, sizeof(m_back));
		}
	}
	void Reset()
	{
		if(m_bInstall)
		{
			memcpy(m_pOri, m_jump, sizeof(m_jump));
		}
	}
	//! uninstall hook
	bool Uninstall()
	{
		if(!m_bInstall) return true;
	
		DWORD dwOriProtect = 0;
		try
		{
// 			if ( !VirtualProtect( m_pOri ,8, PAGE_EXECUTE_READWRITE, &dwOriProtect))
// 			{
// 				OutputDebugStringA("VirtualProtect failed");
// 				return false;
// 			}
			memcpy(m_pOri, m_back, sizeof(m_back));
			VirtualProtect( m_pOri, 8, m_dwOriProtect, 0);

			memset(m_back,0,sizeof(m_back));
			memset(m_jump,0,sizeof(m_jump));
			m_bInstall = false;
		}
		catch (...)
		{
		}
		return true;
	}
	CALL& Call(){return m_pOri;}
public:
	bool m_bInstall;
	DWORD m_dwOriProtect;
	CALL m_pOri;
	unsigned char m_back[5];	
	unsigned char m_jump[5];
};
