/*! \file $Id: WinRobotJNIAdapter.h 118 2011-11-15 16:22:30Z caoym $
*  \author caoym
*  \brief CWinRobotJNIAdapter			
*/
#pragma once
#include "jni.h"
#include <atlbase.h>
#ifdef _WIN64
#import "WinRobotCorex64.dll" auto_search,no_namespace
#import "WinRobotHostx64.exe" auto_search,no_namespace
#else
#import "WinRobotCorex86.dll" auto_search,no_namespace
#import "WinRobotHostx86.exe" auto_search,no_namespace
#endif

//! adapter for IWinRobotSession to JNI interface
class CWinRobotJNIAdapter
{
public:
	
	CWinRobotJNIAdapter(void);
	~CWinRobotJNIAdapter(void);
	static bool AttachJNIObj(JNIEnv *env,jobject jthiz);
	static bool DetachJNIObj(JNIEnv *env,jobject jthiz);
	static CWinRobotJNIAdapter* GetThis(JNIEnv *env,jobject jthiz);

	/**
	 * Sleeps for the specified time. 
	 */
	void delay(jint ms);
	 /**
     * Returns the number of milliseconds this Robot sleeps after generating an event. 
     */
	jint getAutoDelay();
	 /**
     * Returns the color of a pixel at the given screen coordinates. 
     */
	jobject getPixelColor(JNIEnv *env ,jint,jint);
	 /** 
     * Returns whether this Robot automatically invokes waitForIdle after generating an event. 
     */
	jboolean isAutoWaitForIdle();
	/**
     * Presses a given key. 
     */
	void keyPress(jint);
	/**
	 * Releases a given key.
	 */
	void keyRelease(jint);
	/**
	 * Moves mouse pointer to given screen coordinates. 
	 */
	void mouseMove(jint, jint);
	/**
	 * Presses one or more mouse buttons. 
	 */
	void mousePress(jint);
	/**
	 * Releases one or more mouse buttons. 
	 */
	void mouseRelease(jint);
	/**
	 * Rotates the scroll wheel on wheel-equipped mice. 
	 */
	void mouseWheel(jint);
	/**
	 * Sets the number of milliseconds this Robot sleeps after generating an event. 
	 */
	void setAutoDelay(jint);
	/**
	 * Sets whether this Robot automatically invokes waitForIdle after generating an event.
	 */
	void setAutoWaitForIdle(jboolean);
	/**
	 * Returns a string representation of this Robot. 
	 */
	jstring toString(JNIEnv *env);
	/**
	 * Waits until all events currently on the event queue have been processed. 
	 */
	void waitForIdle();
// 	/**
// 	 * Creates an image containing pixels read from the screen. 
// 	 * @param rc the rect to capture
// 	 * @return screen image
// 	 */
// 	jbyteArray createScreenCaptureAsArray(JNIEnv *env,jobject rc);
// 	/**
//      *  capture mouse cursor to byte[]
//      * @param origin [out] point of the mouse cursor
//      * @param hotspot[out] hotspot of the mouse cursor
//      * @return 
//      */
// 	jbyteArray captureMouseCursorAsArray(JNIEnv *, jobject origin, jobject hotspot);
	/**
     *  capture mouse cursor to WinFileMappingBuffer
     * @param origin [out] point of the mouse cursor
     * @param hotspot[out] hotspot of the mouse cursor
     * @return mouse cursor image
     */
	jobject captureMouseCursorAsFileMapping(JNIEnv *, jobject, jobject,jintArray pType);

	/**
	 * Creates an image containing pixels read from the screen. 
	 * @param rc the rect to capture
	 * @return screen image
	 */
	jobject createScreenCaptureAsFileMapping(JNIEnv *env,jobject rc);

	/**
   	 * send character
   	 * 
   	 *  send the supplied character as keyboard input as if the
     *  corresponding character was generated on the keyboard, 
     *  possibly through several key-presses and subsequent releases
   	 */
	void keyType(jchar);
	/**
   	 * send CTRL-ALT-DEL 
   	 */
	void sendCtrlAltDel();
	 /**
     * Set KeyboardLayout
     * @param KLID KeyboardLayout id @see http://msdn.microsoft.com/en-us/library/ms646305(v=vs.85).aspx
     */
	void setKeyboardLayout(JNIEnv *,jstring klid);

	//! check if service is install
	static bool CheckInstall(HINSTANCE hin);
private:
	jobject m_cursor_obj;
	CComQIPtr<IScreenBufferStream> m_pCursor ;
	CComPtr<IWinRobotService> m_pService;
	CComPtr<IWinRobotSession> GetActiveConsoleSession();
	static bool RegisterComDLL(HINSTANCE hin,LPCTSTR dll);
};
