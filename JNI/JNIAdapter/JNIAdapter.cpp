// JNIAdapter.cpp
//

#include "stdafx.h"
#include "com_caoym_WinRobot.h"
#include "com_caoym_WinFileMappingBuffer.h"
#include "WinRobotJNIAdapter.h"
#include "FileMappingAdapter.h"
#include "debuglogger.h"
#include "JniObj_T.h"
//#include "mainpage.h"
#ifdef _MANAGED
#pragma managed(push, off)
#endif
HMODULE g_hModule;
LONG m_nChecked = 0;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		g_hModule = hModule;
		//Initialize logger
		InitLogEnv(hModule);
		// Initialize COM
		CoInitializeEx(0,COINIT_MULTITHREADED);
		
	}
	else if(ul_reason_for_call == DLL_PROCESS_DETACH )
	{
		//Uninstall COM
		//CoUninitialize();
	}
    return TRUE;
}
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_CreateJNIObj 
(JNIEnv *env, jobject jthiz)
{
  	if(!CWinRobotJNIAdapter::CheckInstall(g_hModule))
  	{
  		JniException::Throw(env,"CheckInstall failed with %d",GetLastError());
		return ;
  	}
	CWinRobotJNIAdapter::AttachJNIObj(env,jthiz);
}

JNIEXPORT void JNICALL Java_com_caoym_WinRobot_DestroyJNIObj 
(JNIEnv *env, jobject jthiz)
{
	CWinRobotJNIAdapter::DetachJNIObj(env,jthiz);
}

/*
* Class:     com_caoym_WinRobot
* Method:    delay
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_delay
(JNIEnv *env , jobject jthis, jint ms)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->delay(ms);
}

/*
* Class:     com_caoym_WinRobot
* Method:    getAutoDelay
* Signature: ()I
*/
JNIEXPORT jint JNICALL Java_com_caoym_WinRobot_getAutoDelay
(JNIEnv *env , jobject jthis)
{
	return CWinRobotJNIAdapter::GetThis(env,jthis)->getAutoDelay();
}

/*
* Class:     com_caoym_WinRobot
* Method:    getPixelColor
* Signature: (II)Ljava/awt/Color;
*/
JNIEXPORT jobject JNICALL Java_com_caoym_WinRobot_getPixelColor
(JNIEnv *env , jobject jthis, jint x, jint y)
{
	return CWinRobotJNIAdapter::GetThis(env,jthis)->getPixelColor(env,x,y);
}

/*
* Class:     com_caoym_WinRobot
* Method:    isAutoWaitForIdle
* Signature: ()Z
*/
JNIEXPORT jboolean JNICALL Java_com_caoym_WinRobot_isAutoWaitForIdle
(JNIEnv *env , jobject jthis)
{
	return CWinRobotJNIAdapter::GetThis(env,jthis)->isAutoWaitForIdle();
}

/*
* Class:     com_caoym_WinRobot
* Method:    keyPress
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_keyPress
(JNIEnv *env , jobject jthis, jint vk)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->keyPress(vk);
}


/*
* Class:     com_caoym_WinRobot
* Method:    keyRelease
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_keyRelease
(JNIEnv *env , jobject jthis, jint vk)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->keyRelease(vk);
}

/*
* Class:     com_caoym_WinRobot
* Method:    mouseMove
* Signature: (II)V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_mouseMove
(JNIEnv *env , jobject jthis, jint x, jint y)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->mouseMove(x,y);
}

/*
* Class:     com_caoym_WinRobot
* Method:    mousePress
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_mousePress
(JNIEnv *env , jobject jthis, jint bn)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->mousePress(bn);
}

/*
* Class:     com_caoym_WinRobot
* Method:    mouseRelease
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_mouseRelease
(JNIEnv *env , jobject jthis, jint bn)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->mouseRelease(bn);
}

/*
* Class:     com_caoym_WinRobot
* Method:    mouseWheel
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_mouseWheel
(JNIEnv *env , jobject jthis, jint wheelAmt)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->mouseWheel(wheelAmt);
}

/*
* Class:     com_caoym_WinRobot
* Method:    setAutoDelay
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_setAutoDelay
(JNIEnv *env , jobject jthis, jint newVal)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->setAutoDelay(newVal);
}

/*
* Class:     com_caoym_WinRobot
* Method:    setAutoWaitForIdle
* Signature: (Z)V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_setAutoWaitForIdle
(JNIEnv *env , jobject jthis, jboolean newVal)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->setAutoWaitForIdle(newVal);
}

/*
* Class:     com_caoym_WinRobot
* Method:    toString
* Signature: ()Ljava/lang/String;
*/
JNIEXPORT jstring JNICALL Java_com_caoym_WinRobot_toString
(JNIEnv *env , jobject jthis)
{
	return CWinRobotJNIAdapter::GetThis(env,jthis)->toString(env);
}

/*
* Class:     com_caoym_WinRobot
* Method:    waitForIdle
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_waitForIdle
(JNIEnv *env , jobject jthis)
{
	CWinRobotJNIAdapter::GetThis(env,jthis)->waitForIdle();
}

/*
 * Class:     com_caoym_WinRobot
 * Method:    keyType
 * Signature: (C)V
 */
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_keyType
  (JNIEnv *env, jobject jthis, jchar c)
{
	return CWinRobotJNIAdapter::GetThis(env,jthis)->keyType(c);
}

/*
 * Class:     com_caoym_WinRobot
 * Method:    sendCtrlAltDel
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_sendCtrlAltDel
  (JNIEnv *env, jobject jthis)
{
	return CWinRobotJNIAdapter::GetThis(env,jthis)->sendCtrlAltDel();
}
/*
 * Class:     com_caoym_WinRobot
 * Method:    setKeyboardLayout
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_caoym_WinRobot_setKeyboardLayout
  (JNIEnv *env, jobject jthis, jstring klid)
{
	return CWinRobotJNIAdapter::GetThis(env,jthis)->setKeyboardLayout(env,klid);
}
/*
 * Class:     com_caoym_WinFileMappingBuffer
 * Method:    open
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_caoym_WinFileMappingBuffer_open
  (JNIEnv *env, jobject jthiz, jstring jstr,jint size)
{
	return GetJNIObj<CFileMappingAdapter>(env,jthiz)->open(env,jstr,size);
}
/*
 * Class:     com_caoym_WinFileMappingBuffer
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_caoym_WinFileMappingBuffer_close
  (JNIEnv *env, jobject jthiz)
{
	return GetJNIObj<CFileMappingAdapter>(env,jthiz)->close();
}
/*
 * Class:     com_caoym_WinFileMappingBuffer
 * Method:    getBuffer
 * Signature: ()Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_com_caoym_WinFileMappingBuffer_getBuffer
  (JNIEnv *env, jobject jthiz)
{
	return GetJNIObj<CFileMappingAdapter>(env,jthiz)->getBuffer(env);
}

/*
 * Class:     com_caoym_WinFileMappingBuffer
 * Method:    CreateJNIObj
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_caoym_WinFileMappingBuffer_CreateJNIObj
 (JNIEnv *env, jobject jthiz)
{
	AttachJNIObj<CFileMappingAdapter>(env,jthiz);
}

/*
 * Class:     com_caoym_WinFileMappingBuffer
 * Method:    DestroyJNIObj
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_caoym_WinFileMappingBuffer_DestroyJNIObj
  (JNIEnv *env, jobject jthiz)
{
	DetachJNIObj<CFileMappingAdapter>(env,jthiz);
}
/*
 * Class:     com_caoym_WinRobot
 * Method:    createScreenCaptureAsFileMapping
 * Signature: (Ljava/awt/Rectangle;)Lcom/caoym/WinFileMappingBuffer;
 */
JNIEXPORT jobject JNICALL Java_com_caoym_WinRobot_createScreenCaptureAsFileMapping
  (JNIEnv *env, jobject jthiz, jobject rc)
{
	return GetJNIObj<CWinRobotJNIAdapter>(env,jthiz)->createScreenCaptureAsFileMapping(env,rc);
}
/*
 * Class:     com_caoym_WinRobot
 * Method:    captureMouseCursorAsFileMapping
 * Signature: (Ljava/awt/Point;Ljava/awt/Point;)Lcom/caoym/WinFileMappingBuffer;
 */
JNIEXPORT jobject JNICALL Java_com_caoym_WinRobot_captureMouseCursorAsFileMapping
  (JNIEnv *env, jobject jthiz, jobject ori, jobject hotspot,jintArray pType)
{
	return GetJNIObj<CWinRobotJNIAdapter>(env,jthiz)->captureMouseCursorAsFileMapping(env,ori,hotspot,pType); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

