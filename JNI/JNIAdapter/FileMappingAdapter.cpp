#include "StdAfx.h"
#include "FileMappingAdapter.h"
#include <atlbase.h>
#include <atlconv.h>
#include "debuglogger.h"

jboolean CFileMappingAdapter::open(JNIEnv *env, jstring jstr,jint size)
{
	
	const char* name = env->GetStringUTFChars(jstr,0);
	bool b1 = m_file.Open(CA2T(name),size,false);
	env->ReleaseStringUTFChars(jstr,name);
	return !!b1; 
}
void CFileMappingAdapter::close()
{
	m_file.Destroy();
}
jobject CFileMappingAdapter::getBuffer(JNIEnv * env)
{
	
	jobject obj = env->NewDirectByteBuffer(m_file.GetPointer() ,m_file.GetSize()); 
	
	return obj;
}