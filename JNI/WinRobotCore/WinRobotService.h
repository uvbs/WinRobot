/*! \file $Id: WinRobotService.h 94 2011-10-08 15:36:05Z caoym $
*  \author caoym
*  \brief CWinRobotService
*/

#pragma once
#include "resource.h"     
#include "WinRobotCore_i.h"
#include "sync/cslock.h"
#include <map>
#include "reference/IReference.h"

//! manager WinRobot sessions
/*
	run as a Windows service,while GetActiveConsoleSession is called
	,create a child process on active console session(if the child 
	process specified does not exist)
*/
class /*ATL_NO_VTABLE*/ CWinRobotService :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CWinRobotService, &CLSID_WinRobotService>,
	public IConnectionPointContainerImpl<CWinRobotService>,
	public IDispatchImpl<IWinRobotService, &IID_IWinRobotService, &LIBID_WinRobotCoreLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CWinRobotService()
		:m_hNewReg(0)
		,m_hExit(0)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_WINROBOTSERVICE)


BEGIN_COM_MAP(CWinRobotService)
	COM_INTERFACE_ENTRY(IWinRobotService)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CWinRobotService)
	
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

public:
	//! receive the session currently attached to the physical console 
	STDMETHOD(GetActiveConsoleSession)(IUnknown** ppSession);

	//! register WinRobot session
	/*!
		called by WinRobotSession when it starts.
		\param sid session id
		\param pSession point to IWinRobotSession
		\param processid id of the WinRobotSession process
	*/
	STDMETHOD(RegSession)(ULONG sid, ULONG processid,IUnknown* pSession);

private:
	struct SESSION 
	{
		HANDLE m_hProcess;
		CComPtr<IWinRobotSession> pSession;
		SESSION(HANDLE hd = 0)
			:m_hProcess(hd){

		}
		~SESSION(){
			if(m_hProcess){
				CloseHandle(m_hProcess);
			}
		}
		bool IsOK()
		{
			if(m_hProcess == 0) return false;
			return WaitForSingleObject(m_hProcess,0) == WAIT_TIMEOUT;
		}
	};
	typedef std::map<ULONG , CRefObj< CReference_T<SESSION> > > SESSIONS;

	CCrtSection m_csLock; // lock for GetActiveConsoleSession
	CCrtSection m_csLockSessions; //lock for m_sessions
	SESSIONS m_sessions;
	HANDLE m_hNewReg; //signed when new session registered
	HANDLE m_hExit;// signed when stop

	HANDLE CreateChildProcess(ULONG sid);
};

OBJECT_ENTRY_AUTO(__uuidof(WinRobotService), CWinRobotService)
