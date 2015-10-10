/*! \file	$Id: flogger.h 38 2011-04-14 14:48:56Z caoym $
 *  \author caoym
 *  \brief	filelog
 *          
 */
#pragma once
#ifndef __FLOGGER_H__
#define __FLOGGER_H__

#include <stdio.h>
#include <string>
#include "sync/cslock.h"

//! log to file
class filelog
{
public:
	filelog(void);
	virtual	~filelog(void);
	
	bool  open(const char* name);
	bool  close();
	bool write_log(int evt,const char* str,...);
	
	enum FLAG
	{
		log_info = 1,
		log_warning =2,
		log_error = 4,
		log_debug = 8
	};
	enum FILTER
	{
		toFile = 1<<8,
		toDebugOut = 1<<9
	};
	void Flag(unsigned int flag){m_nflag = flag;}
	void Path(const char* path){m_path = path;}
	void MaxSize(size_t size){m_nmaxfsize = size; }

private:
	bool  open_(const char* name);
	int m_nOpenFailedCount;
	
	size_t m_nfsize;
	size_t m_nmaxfsize;
	std::string m_path;
	std::string m_name;
	unsigned long m_nflag;
	//enum {max_file_size = 16*1024*1024 };

	int m_nbuf;	
	std::string m_buf[2];
	CMutexLock m_bufLock;
	CMutexLock m_fileLock;
	FILE *m_fstream;
};

#endif // __FLOGGER_H__