/*! \file $Id: WinRobotJNIAdapter.cpp 120 2011-12-08 15:11:09Z caoym $
*  \author caoym
*  \brief CWinRobotJNIAdapter			
*/
#include "StdAfx.h"
#include <sstream>
#include <shellapi.h>
#include "WinRobotJNIAdapter.h"
#include "debuglogger.h"
#include "JavaVKMap.h"
#include "JniException.h"
#include "FileMapping.h"
#include "JniObj_T.h" 
#include "Tlhelp32.h"
#pragma comment(lib, "shell32.lib")
CWinRobotJNIAdapter::CWinRobotJNIAdapter(void)
:m_cursor_obj(0)
{
	
}
CWinRobotJNIAdapter::~CWinRobotJNIAdapter(void)
{
	
}
bool CWinRobotJNIAdapter::AttachJNIObj(JNIEnv *env,jobject jthiz)
{
	CWinRobotJNIAdapter* pHostAdapter = 0;
	if(0 == (pHostAdapter = ::AttachJNIObj<CWinRobotJNIAdapter>(env,jthiz)))
	{
		return false;
	}
	HRESULT hr = pHostAdapter->m_pService.CoCreateInstance( __uuidof(ServiceHost),0,CLSCTX_LOCAL_SERVER );
	if( FAILED(hr) )
	{
		DebugOutF(filelog::log_error,"CoCreateInstance ServiceHost failed with 0x%x",hr);
		::DetachJNIObj<CWinRobotJNIAdapter>(env,jthiz);
		JniException::Throw(env,"CoCreateInstance ServiceHost failed with 0x%x",hr);
		return false;
	}
	return true;
}
bool CWinRobotJNIAdapter::DetachJNIObj(JNIEnv *env,jobject jthiz)
{
	CWinRobotJNIAdapter*pThis = GetJNIObj<CWinRobotJNIAdapter>(env,jthiz);
	if(pThis->m_cursor_obj){
		env->DeleteGlobalRef(pThis->m_cursor_obj);
		pThis->m_cursor_obj = 0;
	}
	return ::DetachJNIObj<CWinRobotJNIAdapter>(env,jthiz);
}
CWinRobotJNIAdapter* CWinRobotJNIAdapter::GetThis(JNIEnv *env,jobject jthiz) 
{
	return ::GetJNIObj<CWinRobotJNIAdapter>(env,jthiz);
}

bool IsInstalled(HINSTANCE hin,bool & bInstall) throw()
{
	bInstall = false;
	bool bOK = false;
	SC_HANDLE hSCM = 0;
	SC_HANDLE hService = 0;
	LPQUERY_SERVICE_CONFIG lpsc = 0;
	do
	{
		hSCM = ::OpenSCManager(NULL, NULL, GENERIC_READ);
		if(hSCM == 0) break;
		hService = ::OpenService(hSCM,_T("WinRobotHost"),GENERIC_READ );
		if( hService == 0){
			if( GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST )
			{
				bOK = true;
			}
			break;
		}
		DWORD dwBytesNeeded = 0;
		if(!QueryServiceConfig( 
			hService, 
			NULL, 
			0, 
			&dwBytesNeeded))
		{
			if( ERROR_INSUFFICIENT_BUFFER ==  GetLastError() )
			{
				
				lpsc = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LMEM_FIXED, dwBytesNeeded);
			}
			else
			{
				DebugOutF(filelog::log_error,"QueryServiceConfig failed (%d)", GetLastError());
				break;
			}
		}

		if( !QueryServiceConfig( 
			hService, 
			lpsc, 
			dwBytesNeeded, 
			&dwBytesNeeded) ) 
		{
			DebugOutF(filelog::log_error,"QueryServiceConfig failed (%d)", GetLastError());
			break;
		}
		// if is not same service exe file
		TCHAR path[MAX_PATH] = {0};
		GetModuleFileName(hin,path,MAX_PATH);
		PathRemoveFileSpec(path);
#if _WIN64
		PathAppend(path,_T("WinRobotHostx64.exe"));
#else
		PathAppend(path,_T("WinRobotHostx86.exe"));
#endif
		
		ATL::CString service_path;
		service_path.Format(_T("\"%s\" -runas service"),path);
		
		bOK = true;
		if( _tcsicmp(lpsc->lpBinaryPathName,service_path) != 0 )
		{
			break;
		}


		bInstall = true;
	}while(0);

	if(hService)::CloseServiceHandle(hService);
	if(hSCM)::CloseServiceHandle(hSCM);
	if(lpsc) LocalFree(lpsc);
	return bOK;
}
BOOL CALLBACK EnumProc( 
					   HWND hwnd, 
					   LPARAM lParam) 
{ 
	if(lParam) *(HWND*)lParam = hwnd;
	return TRUE; 
} 

HWND GetMainWindow( DWORD dwOwnerPID ) 
{ 
	HWND hWnd = 0; 
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
	THREADENTRY32 te32; 

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if( hThreadSnap == INVALID_HANDLE_VALUE ) 
		return( 0 ); 

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32 ); 

	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if( !Thread32First( hThreadSnap, &te32 ) ) 
	{
		CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
		return( 0 );
	}

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do 
	{ 
		if( te32.th32OwnerProcessID == dwOwnerPID )
		{

			EnumThreadWindows(te32.th32ThreadID, EnumProc, (LPARAM)&hWnd); 
			if(hWnd) break;

		}
	} while( Thread32Next(hThreadSnap, &te32 ) ); 

	//  Don't forget to clean up the snapshot object.
	CloseHandle( hThreadSnap );
	return( hWnd );
}
bool HasServiceInstallRights()
{
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hSCM == 0) return false;
	CloseServiceHandle(hSCM);
	return true;
}

bool CWinRobotJNIAdapter::CheckInstall(HINSTANCE hin)
{
	bool bIsInstall = false;
	if(!IsInstalled(hin,bIsInstall))
	{
		return false;
	}
	if(bIsInstall){
		return true;
	}
	//step2.register service WinRobotHost.exe
	TCHAR curpath[MAX_PATH];
	GetModuleFileName( hin, curpath, MAX_PATH );
	PathRemoveFileSpec(curpath);
	ATL::CString file;
	file += "";
	file += curpath;
	file += "\\";
#if _WIN64
	file += "WinRobotHostx64.exe";
#else
	file += "WinRobotHostx86.exe";
#endif
	SHELLEXECUTEINFO sei = {0};
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	sei.lpFile = file;
	if(!HasServiceInstallRights())
	{
		sei.lpVerb = _T("runas");
	}
	sei.lpDirectory = NULL;
	sei.lpParameters = _T("-I");
	sei.hwnd = GetForegroundWindow();
	if(!ShellExecuteEx(&sei))
	{
		return false;
	}

	WaitForSingleObject(sei.hProcess,-1);

	if(sei.hProcess)CloseHandle(sei.hProcess);
	
	if(!IsInstalled(hin,bIsInstall)){
		return false;
	}
	return bIsInstall;

}

CComPtr<IWinRobotSession> CWinRobotJNIAdapter::GetActiveConsoleSession()
{
	if(!m_pService) throw _com_error(E_UNEXPECTED);
	CComQIPtr<IWinRobotSession> pQI = m_pService->GetActiveConsoleSession();
	if (!pQI)
	{
		throw _com_error(E_UNEXPECTED);
	}
	return pQI;
}
void CWinRobotJNIAdapter::delay(jint ms)
{
	try
	{
		GetActiveConsoleSession()->Delay(ms);
	}
	catch(_com_error)
	{

	}
	
	
}
jint CWinRobotJNIAdapter::getAutoDelay()
{
	try
	{
		return GetActiveConsoleSession()->GetAutoDelay();
	}
	catch(_com_error)
	{

	}
	return 0;
}
jobject CWinRobotJNIAdapter::getPixelColor(JNIEnv *env ,jint x,jint y)
{
	OLE_COLOR col = 0;
	try
	{
		col = GetActiveConsoleSession()->GetPixelColor(x,y);
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"GetPixelColor(%d,%d) failed with 0x%x",x,y,e.Error());
	}
	jclass jcls = env->FindClass("java/awt/Color");

	if (0 == jcls)
	{
		JniException::Throw(env,"FindClass java/awt/Color failed");
		return 0;
	}

	jmethodID ctor = env->GetMethodID(jcls, "<init>",
		"(III)V");

	if (0 == ctor)
	{
		JniException::Throw(env,"find java/awt/Color constructor failed");
		return 0;
	}
	return env->NewObject(jcls,ctor,GetRValue(col),GetGValue(col),GetBValue(col) );
}
jboolean CWinRobotJNIAdapter::isAutoWaitForIdle()
{
	try
	{
		return GetActiveConsoleSession()->GetAutoWaitForIdle()?JNI_TRUE:JNI_FALSE;
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"GetAutoWaitForIdle failed with 0x%x",e.Error());
	}
	return JNI_FALSE;
}
void CWinRobotJNIAdapter::keyPress(jint vk)
{	
	try
	{
		int wvk = 0;
		if(jvk2wvk(vk,wvk))
		{
			GetActiveConsoleSession()->KeyPress(wvk);
		}
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"KeyPress failed with 0x%x",e.Error());
	}
}
void CWinRobotJNIAdapter::keyRelease(jint vk)
{
	try
	{
		int wvk = 0;
		if(jvk2wvk(vk,wvk))
		{
			GetActiveConsoleSession()->KeyRelease(wvk);
		}
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"KeyRelease failed with 0x%x",e.Error());
	}
}
void CWinRobotJNIAdapter::mouseMove(jint x, jint y)
{
	try
	{
		GetActiveConsoleSession()->MouseMove(x,y);
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"MouseMove failed with 0x%x",e.Error());
	}
}
void CWinRobotJNIAdapter::mousePress(jint bn)
{
	try
	{
		GetActiveConsoleSession()->MousePress(bn);
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"MousePress failed with 0x%x",e.Error());
	}
}
void CWinRobotJNIAdapter::mouseRelease(jint bn)
{
	try
	{
		GetActiveConsoleSession()->MouseRelease(bn);
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"MouseRelease failed with 0x%x",e.Error());
	}
}
void CWinRobotJNIAdapter::mouseWheel(jint count)
{
	try
	{
		GetActiveConsoleSession()->MouseWheel(count);
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"MouseWheel failed with 0x%x",e.Error());
	}
}
void CWinRobotJNIAdapter::setAutoDelay(jint ms)
{
	try
	{
		GetActiveConsoleSession()->PutAutoDelay(ms);
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"PutAutoDelay failed with 0x%x",e.Error());
	}
}
void CWinRobotJNIAdapter::setAutoWaitForIdle(jboolean b)
{
	try
	{
		GetActiveConsoleSession()->PutAutoWaitForIdle(b?VARIANT_TRUE:VARIANT_FALSE);
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"PutAutoWaitForIdle failed with 0x%x",e.Error());
	}
}
jstring CWinRobotJNIAdapter::toString(JNIEnv *env)
{

	std::ostringstream oss;
	try
	{
		oss	<< "com.caoym.WinRobot"
			<< "[ "
			<< "autoDelay = "<<GetActiveConsoleSession()->GetAutoDelay()
			<<", autoWaitForIdle = " << GetActiveConsoleSession()->GetAutoWaitForIdle()
			<<" ]";
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"toString failed with 0x%x",e.Error());
	}
	return env->NewStringUTF(oss.str().c_str());
}
void CWinRobotJNIAdapter::waitForIdle()
{
	try
	{
		GetActiveConsoleSession()->WaitForIdle();
	}
	catch(_com_error e)
	{
		DebugOutF(filelog::log_error,"WaitForIdle failed with 0x%x",e.Error());
	}
}
jobject CWinRobotJNIAdapter::createScreenCaptureAsFileMapping(JNIEnv *env,jobject rc)
{
	jclass cls = env->GetObjectClass(rc);
	if (0 == cls)
	{
		JniException::Throw(env,"GetObjectClass failed");
		RETURN(0);
	}
	jfieldID x_id = env->GetFieldID(cls, "x", "I");
	if(x_id == 0) RETURN(0);
	jfieldID y_id = env->GetFieldID(cls, "y", "I");
	if(y_id == 0) RETURN(0);

	jfieldID w_id = env->GetFieldID(cls, "width", "I");
	if(w_id == 0) RETURN(0);
	jfieldID h_id = env->GetFieldID(cls, "height", "I");
	if(h_id == 0) RETURN(0);

	CComQIPtr<IScreenBufferStream> pBuf ;
	try
	{
		pBuf = GetActiveConsoleSession()->CreateScreenCapture(
			env->GetIntField(rc,x_id),
			env->GetIntField(rc,y_id),
			env->GetIntField(rc,w_id),
			env->GetIntField(rc,h_id));
	}
	catch(_com_error e)
	{ 
		DebugOutF(filelog::log_error,"CreateScreenCapture failed with 0x%x",e.Error());
	}
	if (!pBuf)
	{
		RETURN(0);
	}

	CComBSTR fmname ;
	HRESULT hr ;
	if(FAILED(hr = pBuf->get_FileMappingName(&fmname)))
	{
		RETURN(0);
	}
	
	ULONG szie = 0;
	pBuf->get_Size(&szie);
	
	jclass jcls = env->FindClass("com/caoym/WinFileMappingBuffer");

	if (0 == jcls)
	{
		JniException::Throw(env,"FindClass com/caoym/WinFileMappingBuffer failed");
		return 0;
	}
	jmethodID ctor = env->GetMethodID(jcls, "<init>",
		"(Ljava/lang/String;I)V");

	if (0 == ctor)
	{
		JniException::Throw(env,"find com/caoym/WinFileMappingBuffer constructor failed");
		return 0;
	}
	return env->NewObject(jcls,ctor,env->NewStringUTF(CW2A(fmname)),szie);
}
jobject CWinRobotJNIAdapter::captureMouseCursorAsFileMapping(JNIEnv *env, jobject ori, jobject hotspot,jintArray pType)
{
	

	CComQIPtr<IScreenBufferStream> pBuf ;
	LONG  x=0,y=0,hotspotx=0,hotspoty = 0,type= CURSOR_UNKNOWN;
	try
	{
		pBuf = GetActiveConsoleSession()->CaptureMouseCursor(&x,&y,&hotspotx,&hotspoty,&type);
	}
	catch(_com_error e)
	{ 
		DebugOutF(filelog::log_error,"CaptureMouseCursor failed with 0x%x",e.Error());
	}
	if (!pBuf)
	{
		RETURN(0);
	}
	if(ori)
	{
		jclass cls = env->GetObjectClass(ori);
		if (0 == cls)
		{
			RETURN(0);
		}
		jfieldID x_id = env->GetFieldID(cls, "x", "I");
		if(x_id == 0) RETURN(0);
		jfieldID y_id = env->GetFieldID(cls, "y", "I");
		if(y_id == 0) RETURN(0);
		if(ori)env->SetIntField(ori,x_id,x);
		if(ori)env->SetIntField(ori,y_id,y);
	}
	if(hotspot)
	{
		jclass cls = env->GetObjectClass(hotspot);
		if (0 == cls)
		{
			RETURN(0);
		}
		jfieldID x_id = env->GetFieldID(cls, "x", "I");
		if(x_id == 0) RETURN(0);
		jfieldID y_id = env->GetFieldID(cls, "y", "I");
		if(y_id == 0) RETURN(0);
		env->SetIntField(hotspot,x_id,hotspotx);
		env->SetIntField(hotspot,y_id,hotspoty);
	}
	if(pType){
		jint temp[] = {type};
		env->SetIntArrayRegion(pType,0,1,temp);
	}
	if(m_pCursor == pBuf && m_cursor_obj){
		return env->NewLocalRef(m_cursor_obj);
	}
	
	CComBSTR fmname ;
	HRESULT hr ;
	if(FAILED(hr = pBuf->get_FileMappingName(&fmname)))
	{
		RETURN(0);
	}
	ULONG szie = 0;
	pBuf->get_Size(&szie);

	jclass jcls = env->FindClass("com/caoym/WinFileMappingBuffer");

	if (0 == jcls)
	{
		JniException::Throw(env,"FindClass com/caoym/WinFileMappingBuffer failed");
		return 0;
	}
	jmethodID ctor = env->GetMethodID(jcls, "<init>",
		"(Ljava/lang/String;I)V");

	if (0 == ctor)
	{
		JniException::Throw(env,"find com/caoym/WinFileMappingBuffer constructor failed");
		return 0;
	}

	
	jobject obj = env->NewObject(jcls,ctor,env->NewStringUTF(CW2A(fmname)),szie);

	if(m_cursor_obj)env->DeleteGlobalRef(m_cursor_obj);
	m_cursor_obj = env->NewGlobalRef(obj);
	m_pCursor = pBuf;
	return obj;
}
void CWinRobotJNIAdapter::keyType(jchar c)
{
	try
	{
		GetActiveConsoleSession()->InputChar(c);
	}
	catch(_com_error e)
	{ 
		DebugOutF(filelog::log_error,"InputChar failed with 0x%x",e.Error());
	}
}
void CWinRobotJNIAdapter::sendCtrlAltDel()
{
	try
	{
 		GetActiveConsoleSession()->KeyPress(VK_LCONTROL);
 		GetActiveConsoleSession()->KeyPress(VK_MENU);
 		GetActiveConsoleSession()->KeyPress(VK_DELETE);
 		GetActiveConsoleSession()->KeyRelease(VK_LCONTROL);
 		GetActiveConsoleSession()->KeyRelease(VK_MENU);
 		GetActiveConsoleSession()->KeyRelease(VK_DELETE);

		/*GetActiveConsoleSession()->KeyPress(VK_LCONTROL);
		GetActiveConsoleSession()->KeyPress(VK_LSHIFT);
		GetActiveConsoleSession()->KeyPress(VK_ESCAPE);
		GetActiveConsoleSession()->KeyRelease(VK_ESCAPE);
		GetActiveConsoleSession()->KeyRelease(VK_LSHIFT);
		GetActiveConsoleSession()->KeyRelease(VK_LCONTROL);*/
	}
	catch(_com_error e)
	{ 
		DebugOutF(filelog::log_error,"sendCtrlAltDel failed with 0x%x",e.Error());
	}
}
void CWinRobotJNIAdapter::setKeyboardLayout(JNIEnv *env,jstring klid)
{
	const jchar * wklid = env->GetStringChars(klid, 0);
	try
	{
		GetActiveConsoleSession()->SetKeyboardLayout((wchar_t*)wklid);
	}
	catch(_com_error)
	{ 

	}
	env->ReleaseStringChars(klid, wklid);
}