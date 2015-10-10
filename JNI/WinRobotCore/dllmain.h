// dllmain.h : Declaration of module class.
extern HINSTANCE g_hInstance;
class CWinRobotCoreModule : public CAtlDllModuleT< CWinRobotCoreModule >
{
public :
	DECLARE_LIBID(LIBID_WinRobotCoreLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WINROBOTCORE, "{D0E037C7-6FBC-4EC5-8879-AC769D778F0F}")
	//! replace %APP_NAME% to module file name
	virtual HRESULT AddCommonRGSReplacements(IRegistrarBase* pRegistrar) throw()
	{
		TCHAR szFilePath[MAX_PATH];
		TCHAR szBuffer[MAX_PATH];
		LPTSTR lpFilePart = NULL;
		::GetModuleFileName(g_hInstance, szFilePath, MAX_PATH);
		::GetFullPathName(szFilePath,MAX_PATH,szBuffer,&lpFilePart);
		if( lpFilePart == NULL ) return E_UNEXPECTED;

		HRESULT hr = pRegistrar->AddReplacement(L"APP_NAME",lpFilePart );
		if( FAILED(hr) ) return hr;
		return CAtlDllModuleT< CWinRobotCoreModule >::AddCommonRGSReplacements(pRegistrar);
	}
};

extern class CWinRobotCoreModule _AtlModule;
