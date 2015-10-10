/*! \file $Id: ServiceHost.h 95 2011-10-11 16:06:14Z caoym $
*  \author caoym
*  \brief CServiceHost
*/
#pragma once
#include "resource.h"      

#include "WinRobotHost_i.h"
#ifdef _WIN64
#import "WinRobotCorex64.dll" raw_interfaces_only, raw_native_types,auto_search,no_namespace
#else
#import "WinRobotCorex86.dll" raw_interfaces_only, raw_native_types,auto_search,no_namespace
#endif

//! host for WinRobotService
/*!
	run as a Singleton and provide interface IWinRobotService,
	to make sure that different caller can access the same WinRobotService object.
*/
class /*ATL_NO_VTABLE*/ CServiceHost :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CServiceHost, &CLSID_ServiceHost>,
	public IDispatchImpl<IServiceHost, &IID_IServiceHost, &LIBID_WinRobotHostLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CServiceHost()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SERVICEHOST)
DECLARE_CLASSFACTORY_SINGLETON(CServiceHost) // Singleton

BEGIN_COM_MAP(CServiceHost)
	COM_INTERFACE_ENTRY(IServiceHost)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_AGGREGATE(__uuidof(IWinRobotService),m_pWinRobotService.p)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return m_pWinRobotService.CoCreateInstance(__uuidof(WinRobotService));
	}

	void FinalRelease()
	{
	}

public:
	CComPtr<IWinRobotService> m_pWinRobotService;

};

OBJECT_ENTRY_AUTO(__uuidof(ServiceHost), CServiceHost)
