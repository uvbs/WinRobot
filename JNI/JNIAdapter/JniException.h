/*! \file	$Id: JniException.h 33 2011-04-02 13:58:53Z caoym $
 *  \author caoym
 *  \brief	JniException        
 */
#pragma once
#include "jni.h"
//! throw java exception
class JniException
{
public:
	JniException(void);
	~JniException(void);
	static void Throw(JNIEnv *env,char* format,...);
};
