/*! \file $Id: JniObj_T.h 64 2011-06-15 14:01:00Z caoym $
*  \author caoym
*  \brief AttachJNIObj	GetJNIObj	DetachJNIObj	
*/
#pragma once
#include "JniException.h"
template<class T>
static T* AttachJNIObj(JNIEnv *env,jobject jthiz)
{
	jclass cls = env->GetObjectClass(jthiz);
	if (0 == cls)
	{
		JniException::Throw(env,"GetObjectClass failed");
		return 0;
	}
	jfieldID l_id = env->GetFieldID(cls, "__pJniObj", "J");
	if(l_id == 0) {
		JniException::Throw(env,"GetFieldID __pJniObj failed");
		return 0;
	}
	T*p = new T(); 
	env->SetLongField(jthiz,l_id,jlong(p) );
	return p;
}

template<class T>
static T* GetJNIObj(JNIEnv *env,jobject jthiz) 
{
	jclass cls = env->GetObjectClass(jthiz);

	if (0 == cls)
	{
		JniException::Throw(env,"GetObjectClass failed");
	}
	jfieldID l_id = env->GetFieldID(cls, "__pJniObj", "J");
	if(l_id == 0) JniException::Throw(env,"GetFieldID __pJniObj failed");
	jlong pointer = env->GetLongField(jthiz,l_id);
	if(pointer == 0)
		JniException::Throw(env,"__pJniObj == 0");
	return (T*)pointer;
}

template<class T>
static bool DetachJNIObj(JNIEnv *env,jobject jthiz)
{
	T*pThis = GetJNIObj<T>(env,jthiz);
	if(pThis)
	{
		jclass cls = env->GetObjectClass(jthiz);
		if (0 == cls)
		{
			return false;
		}
		jfieldID l_id = env->GetFieldID(cls, "__pJniObj", "J");
		if(l_id == 0) 
		{
			return false;
		}
		env->SetLongField(jthiz,l_id,0);

		delete pThis;
		return true;
	}
	return false;
}