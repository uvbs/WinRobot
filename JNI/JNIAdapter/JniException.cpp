/*! \file	$Id: JniException.cpp 33 2011-04-02 13:58:53Z caoym $
 *  \author caoym
 *  \brief	JniException        
 */
#include "StdAfx.h"
#include "JniException.h"

JniException::JniException(void)
{
}
JniException::~JniException(void)
{
}
void JniException::Throw(JNIEnv *env,char* format,...)
{
	jclass cls = (env)->FindClass("java/awt/AWTException"); 
	if (cls == 0)   
	{   
		return;   
	} 

	char *fmtstr = NULL;
	int len = 0;
	va_list   varg;   
	va_start(varg, format);
	len = _vscprintf( format, varg )+ 1;
	fmtstr = new char[len];
	memset(fmtstr,0,len);
	vsnprintf(fmtstr,len, format, varg);
	va_end(varg);

	(env)->ThrowNew( cls, fmtstr);

	delete [] fmtstr;
}