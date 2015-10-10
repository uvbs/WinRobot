// TestDlg.h : CTestDlg 

#pragma once

#include "resource.h"       // 

#include <atlhost.h>
#ifdef _WIN64
#import "WinRobotCorex64.dll" raw_interfaces_only, raw_native_types,auto_search,no_namespace
#import "WinRobotHostx64.exe" auto_search,no_namespace
#else
#import "WinRobotCorex86.dll" raw_interfaces_only, raw_native_types,auto_search,no_namespace
#import "WinRobotHostx86.exe" auto_search,no_namespace
#endif

static BOOL CALLBACK MonitorEnumProc_(
							  HMONITOR hMonitor,  // handle to display monitor
							  HDC hdcMonitor,     // handle to monitor DC
							  LPRECT lprcMonitor, // monitor intersection rectangle
							  LPARAM dwData       // data
							  )
{
	MONITORINFOEX  info;
	info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor,&info);
	return TRUE;
}


// CTestDlg

class CTestDlg : 
	public CAxDialogImpl<CTestDlg>
{
public:
	CTestDlg()
	{
		EnumDisplayMonitors(0,0,MonitorEnumProc_,0);
		DISPLAY_DEVICE device = {sizeof(DISPLAY_DEVICE)};
		int i=0;
		while (EnumDisplayDevices (0,i,&device,EDD_GET_DEVICE_INTERFACE_NAME))
		{
			i++;
		}

		HDC hDev = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
		POINT pt;
		GetDCOrgEx(hDev,&pt);
		DEVMODE mode = {sizeof(DEVMODE)};
		EnumDisplaySettings(
			_T("DISPLAY"),  // display device
			ENUM_CURRENT_SETTINGS,          // graphics mode
			&mode      // graphics mode settings
			);

		return;

	}

	~CTestDlg()
	{
	}

	enum { IDD = IDD_TESTDLG };

BEGIN_MSG_MAP(CTestDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedButton1)
	COMMAND_HANDLER(IDC_BUTTON2, BN_CLICKED, OnBnClickedButton2)
	COMMAND_HANDLER(IDC_BUTTON3, BN_CLICKED, OnBnClickedButton3)
	COMMAND_HANDLER(IDC_BUTTON4, BN_CLICKED, OnBnClickedButton4)
	COMMAND_HANDLER(IDC_BUTTON5, BN_CLICKED, OnBnClickedButton5)
	CHAIN_MSG_MAP(CAxDialogImpl<CTestDlg>)
END_MSG_MAP()
	CComPtr<IWinRobotService> m_pService;

//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CTestDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		bHandled = TRUE;

		HRESULT hr = S_OK;

		CComPtr<IWinRobotService> test;
		if(FAILED (test.CoCreateInstance(__uuidof(WinRobotService))))
		{
			MessageBox(_T("CoCreateInstance WinRobotService failed"));
		}
		hr = m_pService.CoCreateInstance(	__uuidof(ServiceHost) );
		if( FAILED(hr) )
		{
			MessageBox(_T("CoCreateInstance Service failed"));
			return 1;
		}
		if(m_pService == 0)
		{
			MessageBox(_T("m_pService == 0"));
			return 1;
		}
		return 1;  // 
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
	LRESULT OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton3(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton5(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


