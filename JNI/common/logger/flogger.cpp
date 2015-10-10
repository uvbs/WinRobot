/*! \file	$Id: flogger.cpp 98 2011-10-16 12:50:36Z caoym $
 *  \author caoym
 *  \brief	filelog        
 */
#include "stdafx.h"
#include "flogger.h"
#include <time.h>
#include <stdarg.h>
#include <io.h>
#include <direct.h>
#include <Windows.h>

filelog::filelog(void)
:m_fstream(0)
,m_nflag(-1)
,m_nfsize(0)
,m_nbuf(0)
,m_nmaxfsize(1024*1024*16)
,m_nOpenFailedCount(0)
{
	char path[MAX_PATH]={0};
	GetModuleFileNameA(0,path,MAX_PATH);
	PathRemoveFileSpecA(path);
	m_path=path;
}

filelog::~filelog(void)
{
	close();
}
bool  filelog::open(const char*name)
{
	CAutoLock<CMutexLock> lock(m_fileLock);
	m_name = name?name:"";
	return true;
}
bool  filelog::open_(const char*name)
{
	CAutoLock<CMutexLock> lock(m_fileLock);
	if(m_fstream) return false;
	m_nfsize = 0;

	_mkdir( m_path.c_str());

	m_name = name?name:"";
	std::string path = m_path +"/"+m_name +".log";

	if ( 0 == _access(path.c_str(),0) ) //exist,rename
	{
		char num[32] = ".log.1" ;
		std::string renamepath = m_path + "/" + m_name +num;

		for (int i=1; 0 == _access(renamepath.c_str(),0) ; i++ )
		{
			sprintf(num,".log.%d",i);
			renamepath = m_path + "/" + m_name +num;
		}
		rename(path.c_str(),renamepath.c_str());
	}
	m_fstream = fopen(path.c_str(),"w+b");
	
	return m_fstream != NULL;
}
bool  filelog::close()
{
	CAutoLock<CMutexLock> lock(m_fileLock);
	if(m_fstream)
	{
		fflush(m_fstream);
		fclose(m_fstream);
		m_fstream = NULL;
	}
	return true;
}

const static std::string cszInfo	= "- Info  -\t";
const static std::string cszWarning = "+Warning+\t";
const static std::string cszError	= "* Error *\t";
const static std::string cszDebug	= "= Debug =\t";
const static std::string cszUnknow	= "?unknown?\t";

inline const std::string  get_event_leave_str(int flag)
{
	switch(flag){
case filelog::log_info :
	return cszInfo;
	break;
case filelog::log_warning:
	return cszWarning;
	break;
case filelog::log_error:
	return cszError;
	break;
case filelog::log_debug:
	return cszDebug;
	break;
default:
	{
		char temp[16];
		sprintf(temp,"x%08x\t",flag);
		return temp;
	}
	}
}

bool filelog::write_log(int evt,const char* str,...)
{
	if(!(evt & m_nflag)) return true;
	char st[32] = "";
	time_t now = time(NULL);
	struct tm *tm = localtime(&(now));
	if (tm) strftime(st, sizeof(st),"%Y-%m-%d %H:%M:%S\t", tm);

	char *fmtstr = NULL;
	int len = 0;
	va_list   varg;   
	va_start(varg, str);
	len = _vscprintf( str, varg )+ 1;
	fmtstr = new char[len];
	memset(fmtstr,0,len);
	vsnprintf(fmtstr,len, str, varg);
	va_end(varg);
	{
		CAutoLock<CMutexLock> lock(m_bufLock);	
		m_buf[m_nbuf?0:1].append(st);
		m_buf[m_nbuf?0:1].append(get_event_leave_str(evt));
		m_buf[m_nbuf?0:1].append(fmtstr);
#ifdef WIN32
		m_buf[m_nbuf?0:1].append("\r\n");
#else
		m_buf[m_nbuf?0:1].append("\n");
#endif 
	}
	delete [] fmtstr;

	while (m_nOpenFailedCount <2)
	{
		CAutoLockEx<CMutexLock> lock(m_fileLock);
		if (!lock.Locked()) break;
		{
			if (m_nflag & toFile)
			{
				if(m_fstream == NULL)
				{
					open_(m_name.c_str());
				}
				if(m_fstream == NULL) break;
			}

			int index = 0; //
			{
				CAutoLock<CMutexLock> lock(m_bufLock);		
				if( m_buf[m_nbuf].empty() ) m_nbuf = m_nbuf?0:1;
				index = m_nbuf;
			}
			if( m_buf[index].empty() ) return true;
			if(m_nflag & toDebugOut)
				OutputDebugStringA(m_buf[index].c_str());
			if (m_nflag & toFile)
			{
				if(0 == fwrite(m_buf[index].c_str(),m_buf[index].size(),1,m_fstream))
				{
					m_nOpenFailedCount ++;
					close();
					open_(m_name.c_str());
					continue;
				}
				m_nOpenFailedCount = 0;
				fflush(m_fstream);
				m_nfsize += m_buf[index].size();
				
				if (m_nfsize >=m_nmaxfsize)
				{
					close();
					open_(m_name.c_str());
				}
			}
			m_buf[index].erase();
			CAutoLock<CMutexLock> lock(m_bufLock);
			m_nbuf = index?0:1;
		}
	}
	return true;

}