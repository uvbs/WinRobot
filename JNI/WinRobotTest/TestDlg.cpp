// TestDlg.cpp : CTestDlg 

#include "stdafx.h"
#include "TestDlg.h"
#include <atlstr.h>
#include "debuglogger.h"
#include "ddraw.h"
#include "FileMappingBitmap.h"

// CTestDlg
#include "FileMapping.h"
LRESULT CTestDlg::OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
// 	DWORD test = 'abcd';
// 	DebugOutF(filelog::log_error,"unsupport color space 0x%x",test);
	if (m_pService == 0)
	{
		MessageBox(_T("m_pService ==0 "));
		return 0;
	}
	HRESULT hr = S_OK;


 	CComPtr<IUnknown>pUnk;
 	hr = m_pService->GetActiveConsoleSession(&pUnk);
 
	if( FAILED(hr) || pUnk == 0)
	{
		MessageBox(_T("GetActiveConsoleSession failed"));
		return 0;
	}

 	CComQIPtr<IWinRobotSession> pSession = pUnk;
	if (!pSession)
	{
		MessageBox(_T("pSession ==0"));
		return 0;
	}
 
 	hr = pSession->MouseMove(1280/2,800/2);

	if( FAILED(hr) )
	{
		MessageBox(_T("MouseMove failed"));
		return 0;
	}
	return 0;
}

LRESULT CTestDlg::OnBnClickedButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_pService == 0)
	{
		MessageBox(_T("m_pService ==0 "));
		return 0;
	}
	HRESULT hr = S_OK;


	CComPtr<IUnknown>pUnk;
	hr = m_pService->GetActiveConsoleSession(&pUnk);

	if( FAILED(hr) ||  pUnk == 0)
	{
		MessageBox(_T("GetActiveConsoleSession failed"));
		return 0;
	}
	CComQIPtr<IWinRobotSession> pSession = pUnk;
	if (!pSession)
	{
		MessageBox(_T("pSession ==0"));
		return 0;
	}
	hr = pSession->KeyPress('A');
	hr = pSession->KeyRelease('A');
	return 0;
}

LRESULT CTestDlg::OnBnClickedButton3(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_pService == 0)
	{
		MessageBox(_T("m_pService ==0 "));
		return 0;
	}
	HRESULT hr = S_OK;

	CComPtr<IUnknown>pUnk;
	hr = m_pService->GetActiveConsoleSession(&pUnk);

	if( FAILED(hr) )
	{
		MessageBox(_T("GetActiveConsoleSession failed"));
		return 0;
	}

	CComQIPtr<IWinRobotSession> pSession = pUnk;

	if (!pSession)
	{
		MessageBox(_T("pSession ==0"));
		return 0;
	}
	DWORD start = GetTickCount();
	float count = 1.0;
	for (int i=0;i<count;i++)
	{
		pUnk = 0;
		if(FAILED(hr = pSession->CreateScreenCapture(-1280,0,1280*2,800,&pUnk)))
		{
			TCHAR buf[1024] = {0};
			_stprintf(buf,_T("CreateScreenCapture failed with 0x%x"),hr);
			MessageBox(buf);
			return 0;
		}

		CComQIPtr<IScreenBufferStream> pStream = pUnk;
		CComBSTR name;
		pStream->get_FileMappingName(&name);
		ULONG size = 0;
		pStream->get_Size(&size);
		CFileMapping fm;
		fm.Open(name,size,false);
	}
 	DWORD spend = GetTickCount()-start;
 	TCHAR buff[1034];
 	_stprintf(buff,_T("%f"),count*1000.0/spend);
 //	MessageBox(buff);
	//return 0;
	CComQIPtr<ISequentialStream> pStream = pUnk;

	_ASSERT(pStream);
	bool bRtn = true;
	HANDLE hf = INVALID_HANDLE_VALUE;
	try
	{
		hf = CreateFile(_T("c:\\test.bmp"), 
			GENERIC_READ | GENERIC_WRITE, 
			(DWORD) 0, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			(HANDLE) NULL); 

		if (hf == INVALID_HANDLE_VALUE) 
		{ 
			throw false;
		}
		char buf[1024];
		ULONG cbBuffer  = 0 ;
		
		while (pStream->Read(buf,sizeof(buf),&cbBuffer) == S_OK )
		{
			DWORD cbWrite = 0;
			if (!WriteFile(hf, buf, cbBuffer, &cbWrite,  NULL)) 
			{
				throw false;
			}
		}
	}
	catch(bool b1)
	{
		bRtn = b1;
	}
	CloseHandle(hf);
	//_ASSERT(bRtn);
	return 0;
}

LRESULT CTestDlg::OnBnClickedButton4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//Sleep(-1);
	if (m_pService == 0)
	{
		MessageBox(_T("m_pService ==0 "));
		return 0;
	}
	HRESULT hr = S_OK;

	CComPtr<IUnknown>pUnk;
	hr = m_pService->GetActiveConsoleSession(&pUnk);

	if( FAILED(hr) )
	{
		MessageBox(_T("GetActiveConsoleSession failed"));
		return 0;
	}

	CComQIPtr<IWinRobotSession> pSession = pUnk;

	if (!pSession)
	{
		MessageBox(_T("pSession ==0"));
		return 0;
	}

	pUnk = 0;
	LONG x,y,hx,hy;
	if(FAILED(hr = pSession->CaptureMouseCursor( &x,&y,&hx,&hy,0,&pUnk)))
	{
		MessageBox(_T("CaptureMouseCursor failed"));
		return 0;
	}
	CComQIPtr<ISequentialStream> pStream = pUnk;

	_ASSERT(pStream);
	bool bRtn = true;
	HANDLE hf = INVALID_HANDLE_VALUE;
	try
	{
		hf = CreateFile(_T("c:\\cur.bmp"), 
			GENERIC_READ | GENERIC_WRITE, 
			(DWORD) 0, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			(HANDLE) NULL); 

		if (hf == INVALID_HANDLE_VALUE) 
		{ 
			throw FALSE;
		}
		char buf[1024];
		ULONG cbBuffer  = 0 ;
		while (pStream->Read(buf,sizeof(buf),&cbBuffer) == S_OK )
		{
			DWORD cbWrite = 0;
			if (!WriteFile(hf, buf, cbBuffer, &cbWrite,  NULL)) 
			{
				throw false;
			}
		}
	}
	catch(bool b1)
	{
		bRtn = b1;
	}
	CloseHandle(hf);
	//_ASSERT(bRtn);
	return 0;
}

LRESULT CTestDlg::OnBnClickedButton5(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool bRtn = true;

	HANDLE hf = INVALID_HANDLE_VALUE;
	HANDLE hf2 = INVALID_HANDLE_VALUE;
	try
	{
		hf = CreateFile(_T("E:\\DumpSurface2.temp"), 
			GENERIC_READ , 
			(DWORD) 0, 
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			(HANDLE) NULL); 

		if (hf == INVALID_HANDLE_VALUE) 
		{ 
			throw FALSE;
		}
		DDSURFACEDESC2 ddsd;
		DWORD cbRead = 0;
		ReadFile(hf,&ddsd,sizeof(ddsd),&cbRead,0);
		CFileMappingBitmap file;
		file.Create(ddsd.dwWidth,ddsd.dwHeight,ddsd.ddpfPixelFormat.dwRGBBitCount,0,0,_T("1268-1-74EF6127C-0846-4c63-8EAE-6C2DF379A618"));
		
		BYTE*pPointer = (BYTE*)file.GetImagePointer();
		
		int pitch = CalculatePitch(CalculateLine(ddsd.dwWidth,ddsd.ddpfPixelFormat.dwRGBBitCount));
		while ( ReadFile(hf,pPointer,pitch,&cbRead,0) )
		{
			SetFilePointer(hf,ddsd.lPitch - pitch,0,FILE_CURRENT );
			pPointer += pitch;
		}
		
		hf2 = CreateFile(_T("E:\\DumpSurface2.bmp"), 
			GENERIC_READ | GENERIC_WRITE, 
			(DWORD) 0, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			(HANDLE) NULL); 

		WriteFile(hf2,file.GetFilePointer(),file.GetFileSize(),&cbRead,0);

		
	}
	catch(bool b1)
	{
		bRtn = b1;
	}
	CloseHandle(hf);
	CloseHandle(hf2);
	return 0;
}
