// WinRobotTest.cpp : WinMain 


#include "stdafx.h"
#include "resource.h"
#include "WinRobotTest.h"
#include "dlldatax.h"
#include "TestDlg.h"
#include "debuglogger.h"
class CWinRobotTestModule : public CAtlExeModuleT< CWinRobotTestModule >
{
public :
	DECLARE_LIBID(LIBID_WinRobotTestLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WINROBOTTEST, "{659DAF1C-93DD-4FB7-8C93-24AEC6AB646F}")
	HRESULT PreMessageLoop(int nShowCmd) throw()
	{

		HRESULT hr = S_OK;
		hr =  CAtlExeModuleT< CWinRobotTestModule >::PreMessageLoop(nShowCmd);
		if(FAILED(hr)) return hr;
		CTestDlg testdlg;
		testdlg.DoModal();
		return hr;
	}
};

CWinRobotTestModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	InitLogEnv(0);
    return _AtlModule.WinMain(nShowCmd);
}

