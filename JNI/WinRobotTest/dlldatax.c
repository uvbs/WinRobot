// dlldata.c 

#ifdef _MERGE_PROXYSTUB // (stub) DLL

#define REGISTER_PROXY_DLL //DllRegisterServer 

#define _WIN32_WINNT 0x0500	//
#define USE_STUBLESS_PROXY	//

#pragma comment(lib, "rpcns4.lib")
#pragma comment(lib, "rpcrt4.lib")

#define ENTRY_PREFIX	Prx

#include "dlldata.c"
#include "WinRobotTest_p.c"

#endif //_MERGE_PROXYSTUB
