#ifndef __RCDEBUG_H__
#define __RCDEBUG_H__
#include <stdio.h>
#include "logger/flogger.h"

#include <time.h>

//#if defined(DEBUG) || defined(_DEBUG)
//#if defined(DEBUG) || defined(_DEBUG)
extern filelog g_log;
static filelog&GetLoggerInstance()
{
	return g_log;
}
static void InitLogEnv(HINSTANCE hint)
{
	// set the log file path: current process's path + "/log/"
	char temp[MAX_PATH*2]={0};
	GetModuleFileNameA( hint, temp, MAX_PATH );
	PathRemoveFileSpecA(temp);
	strcat(temp,"/log/");

	GetLoggerInstance().Path(temp);
	// name the log file : exe name + dll name + pid
	GetModuleFileNameA( 0, temp, MAX_PATH );
	std::string name = PathFindFileNameA(temp);
	if(hint)
	{
		GetModuleFileNameA( hint, temp, MAX_PATH );
		name += PathFindFileNameA(temp);
		_itoa_s(GetCurrentProcessId(),temp,MAX_PATH,10);
		name += temp;
	}
	char st[32] = "";
	time_t now = time(NULL);
	struct tm *tm = localtime(&(now));
	if (tm) strftime(st, sizeof(st),"%Y_%m_%d_%H_%M_%S", tm);
	name += st;
  	#if !defined(DEBUG) && !defined(_DEBUG)
	GetLoggerInstance().Flag(filelog::log_warning|filelog::log_error/*|filelog::toDebugOut*/|filelog::toFile);
	//GetLoggerInstance().Flag(-1);
  	#endif
	//GetLoggerInstance().Flag(filelog::log_warning|filelog::log_error|filelog::log_info|filelog::log_debug|filelog::toDebugOut|filelog::toFile);
	GetLoggerInstance().open( name.c_str() );
};


#define DebugOutF(...) GetLoggerInstance().write_log( __VA_ARGS__)
/**
 * Auto output debug logs 
 * AutoDebugTrackOut constructor output the string "__FILE__:__LINE__ {" and the destructor output "__FILE__:__LINE__}"
 */
struct AutoDebugTrackOut 
{
	AutoDebugTrackOut(const char* f,int l)
		:file(f?f:"")
		,line(l)
	{
		DebugOutF(filelog::log_debug,"%s:%d {",f,line);
	}
	~AutoDebugTrackOut()
	{
		DebugOutF(filelog::log_debug,"%s:%d }",file.c_str(),line);
	}
	std::string file;
	int line;
	
};
//#define TrackDebugOut  AutoDebugTrackOut __AutoDebugTrackOut__(__FILE__,__LINE__ );
#define TrackDebugOut

//  #else
//  #define DebugOutF(...) do{}while(0)
//  #define InitLogEnv(...) do{}while(0)
//  #endif

#if defined(DEBUG) || defined(_DEBUG)
	#ifndef RESULT
		#define RESULT(x) do{ HRESULT hr = x; if( FAILED(hr) )\
						  { DebugOutF(filelog::log_error,("return err:0x%x at %s,file: %s,line:%d"),\
						  hr,(#x),__FILE__,(DWORD)__LINE__); }}while(0)
	#endif
	#ifndef RESULT_RETURN
		#define RESULT_RETURN(x)    do{ HRESULT hr = x; if( FAILED(hr) )\
						  { DebugOutF(filelog::log_error,("return err:0x%x at %s,file: %s,line:%d"),\
						  hr,(#x),__FILE__,(DWORD)__LINE__); return hr;}}while(0)
	#endif
	#ifndef RESULT_THROW
		#define RESULT_THROW(x)    do{HRESULT hr = x; if( FAILED(hr) )\
						  { DebugOutF(filelog::log_error,("return err:0x%x at %s,file: %s,line:%d"),\
						  hr,(#x),__FILE__,(DWORD)__LINE__); throw hr;}}while(0)
	#endif
	#ifndef RETURN
	#define RETURN(x)  \
		do{ DebugOutF(filelog::log_error,("return error at %s,file: %s,line:%d"),\
			(#x),__FILE__,(DWORD)__LINE__); return x;} while(0)
	#endif

#else
	#ifndef RESULT
		#define RESULT(x)           do{ HRESULT hr = x; }while(0)
	#endif
	#ifndef RESULT_RETURN
		#define RESULT_RETURN(x)    do{HRESULT hr = x; if( FAILED(hr) ) { return hr; } }while(0)
	#endif
	#ifndef RESULT_THROW
		#define RESULT_THROW(x)    do{ HRESULT hr = x; if( FAILED(hr) ) { throw hr; } }while(0)
	#endif
	#ifndef RETURN
	#define RETURN(x) do{return x;}while(0)
	#endif
#endif

#endif //__RCDEBUG_H__

