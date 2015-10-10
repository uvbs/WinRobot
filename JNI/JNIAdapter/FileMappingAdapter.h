/*! \file $Id: FileMappingAdapter.h 100 2011-10-30 13:21:35Z caoym $
*  \author caoym
*  \brief CFileMappingAdapter			
*/
#pragma once
#include "FileMapping.h"
#include "jni.h"

//! adapter	for CFileMapping to JNI object
class CFileMappingAdapter
{
public:
	//! @see CFileMapping::open
	jboolean open(JNIEnv *, jstring name ,jint size);
	//! @see CFileMapping::close
	void close();
	//! get the shared memory as a java.nio.ByteBuffer
	jobject getBuffer(JNIEnv *);
private:
	CFileMapping m_file;

};
