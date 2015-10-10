// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WINROBOTHOOK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WINROBOTHOOK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WINROBOTHOOK_EXPORTS
#define WINROBOTHOOK_API __declspec(dllexport)
#else
#define WINROBOTHOOK_API __declspec(dllimport)
#endif



WINROBOTHOOK_API bool InstallHook();

WINROBOTHOOK_API bool UninstallHook();

WINROBOTHOOK_API ULONG GetChangesCount();

//! get changed rectangles of the screen 
/*!
	
*/
WINROBOTHOOK_API ULONG GetChanges(RECT*,ULONG size);


WINROBOTHOOK_API bool AppendDDrawOverlay(HDC hdc,LPCRECT rc);