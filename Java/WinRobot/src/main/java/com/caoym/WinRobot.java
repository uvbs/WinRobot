/** @file $Id: WinRobot.java 95 2011-10-11 16:06:14Z caoym $
 *  @author caoym
 *  @brief WinRobot			
 */
package com.caoym;

import java.awt.AWTException;
import java.awt.Color;
import java.awt.GraphicsDevice;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * a implementation of java.awt.Robot under windows 
 * @author caoym
 *
 */
public class WinRobot extends Robot {

    /*  predefined cursor type   */
    public static final int CURSOR_APPSTARTIN = 0 ;/**< Standard arrow and small hourglass*/
    public static final int CURSOR_ARROW = 1;/**< 		Standard arrow*/
    public static final int CURSOR_CROSS = 2;/**< 		Crosshair*/
    public static final int CURSOR_HAND = 3;/**< 		Windows 98/Me; Windows 2000/XP: Hand*/
    public static final int CURSOR_HELP = 4;/**< 		Arrow and question mark*/
    public static final int CURSOR_IBEAM = 5;/**< 		I-beam*/
    public static final int CURSOR_ICON = 6;/**< 		Obsolete for applications marked version 4.0 or later.*/
    public static final int CURSOR_NO = 7;/**< 		Slashed circle*/
    public static final int CURSOR_SIZE = 8;/**< 		Obsolete for applications marked version 4.0 or later. Use CURSOR_SIZEALL.*/
    public static final int CURSOR_SIZEALL = 9;/**< 		Four-pointed arrow pointing north; south; east; and west*/
    public static final int CURSOR_SIZENESW = 10;/**< 		Double-pointed arrow pointing northeast and southwest*/
    public static final int CURSOR_SIZENS = 11;/**< 		Double-pointed arrow pointing north and south*/
    public static final int CURSOR_SIZENWSE = 12;/**< 		Double-pointed arrow pointing northwest and southeast*/
    public static final int CURSOR_SIZEWE = 13;/**< 		Double-pointed arrow pointing west and east*/
    public static final int CURSOR_UPARROW = 14;/**< 		Vertical arrow*/
    public static final int CURSOR_WAIT = 15;/**< 		Hourglass*/
    public static final int CURSOR_UNKNOWN = -1;/**< unknown cursor,not predefined  */
    public static final int JNI_LIB_VERSION = 121;

    public WinRobot() throws AWTException {
        super();
        /*initialize JNI object*/
        CreateJNIObj(null);
    }

    public WinRobot(GraphicsDevice device) throws AWTException {
        super();
        /*initialize JNI object*/
        if (device == null) {
            CreateJNIObj(null);
        } else {
            CreateJNIObj(device.getIDstring());
        }

    }

    /**  
     * export library to temp dir 
     * @throws Exception 
     */
    private static boolean exportLibFromJar(String name) {
        // 
        try {
            // read library from resource,this may fail if not run from a jar file
            InputStream in = WinRobot.class.getResourceAsStream("/" + name);
            if (in == null) {
                System.out.println("getResourceAsStream "
                        + " from /" + name + " failed");
            }
            if (in == null) {
                // read library from resource failed,so we try to load from file
                System.out.println("try to get " + name + " from file");
                File file = new File(name);
                in = new FileInputStream(file);
                if (in == null) {
                    System.out.println("get " + name + "from file failed");
                }
            }
            if (in == null) {
                return false;
            }

            String path = System.getProperty("java.io.tmpdir") + "WinRobot\\" + JNI_LIB_VERSION +"\\"+ name;
            path = path.replace('/', '\\');
            // make dirs
            File mkdir = new File(path.substring(0, path.lastIndexOf('\\')));
            mkdir.mkdirs();

            File fileOut = new File(path);
            System.out.println("Writing dll to: " + path);

            OutputStream out;

            out = new FileOutputStream(fileOut);

            byte buf[] = new byte[1024];
            int len;

            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
            out.close();
            in.close();

        } catch (Exception e) {
            System.out.println(e.toString());
            return false;
        }
        return true;
    }
    
     static private void deleteFile(File file,File except) {
        if(file.equals(except)) return;
        
        if (file.exists()) {
            if (file.isFile()) {
                try {
                    file.delete();
                } catch (Exception e) {
                    System.out.println(e.toString());
                }
            } else if (file.isDirectory()) {
                File files[] = file.listFiles();
                for (int i = 0; i < files.length; i++) {
                   deleteFile(files[i],except);
                }
            }
            file.delete();
        } 
    }
    static private void ClearOldVersion(){
       deleteFile(new File(System.getProperty("java.io.tmpdir") + "WinRobot\\"),
               new File(System.getProperty("java.io.tmpdir") + "WinRobot\\"+JNI_LIB_VERSION));
    }
    /**
     * load JNIAdapter.dll
     */
    static {
        boolean bLoaded = false;

        //load from jar
        String libs[] = {"JNIAdapterx86.dll",
            "WinRobotCorex86.dll",
            "WinRobotHostx86.exe",
            "WinRobotHookx86.dll",
            "WinRobotHostPSx86.dll",
            "JNIAdapterx64.dll",
            "WinRobotCorex64.dll",
            "WinRobotHostx64.exe",
            "WinRobotHookx64.dll",
            "WinRobotHostPSx64.dll"};
        for (int i = 0; i < libs.length; i++) {
            exportLibFromJar(libs[i]);
        }
        ClearOldVersion();
        String path = new String();
        //try{
        path = System.getProperty("java.io.tmpdir") + "WinRobot\\" + JNI_LIB_VERSION;
        //} catch (Exception e) {
        //	System.out.println(e.toString());
        //}
		/*String path = getJarFolderByClassLoader(); 
        if(path.isEmpty()){
        path = getJarFolderByCodeSource();
        
        }*/
        //System.out.print(path);

        try {
            System.load(path + "\\JNIAdapterx64.dll");
            bLoaded = true;
        } catch (UnsatisfiedLinkError e) {
            System.out.print("load " + path + "\\JNIAdapterx64.dll" + " failed," + e.getMessage() + "\r\n");

        }
        if (!bLoaded) {
            try {
                System.load(path + "\\JNIAdapterx86.dll");
                bLoaded = true;
            } catch (UnsatisfiedLinkError e) {
                System.out.print("load " + path + "\\JNIAdapterx86.dll" + " failed," + e.getMessage() + "\r\n");
            }
        }
        if (!bLoaded) {
            try {
                System.loadLibrary("JNIAdapterx86");
                bLoaded = true;
            } catch (UnsatisfiedLinkError e) {
                System.out.print("load JNIAdapterx86 failed," + e.getMessage() + "\r\n");
            }
        }

        if (!bLoaded) {
            try {
                System.loadLibrary("JNIAdapterx64");
                bLoaded = true;
            } catch (UnsatisfiedLinkError e) {
                System.out.print("load JNIAdapterx64 failed," + e.getMessage() + "\r\n");
            }
        }
    }

    @Override
    protected void finalize() {
        /*terminate jni object*/
        DestroyJNIObj();
        try {
            super.finalize();
        } catch (Throwable e) {
            System.out.println(e.toString());
        }

    }

    /**
     * Creates an image containing pixels read from the screen. 
     * @param screenRect the rect to capture
     * @return screen image
     */
    public native WinFileMappingBuffer createScreenCaptureAsFileMapping(Rectangle screenRect);

    /**
     * Creates an image containing pixels read from the screen. 
     * @param screenRect the rect to capture
     * @return screen image
     */
    @Override
    public BufferedImage createScreenCapture(Rectangle screenRect) {
        if (screenRect == null) {
            return null;
        }
        WinFileMappingBuffer fm = createScreenCaptureAsFileMapping(screenRect);
        //return null;
        if (fm == null) {
            return null;
        }
        BufferedImage image = CreateBuffedImage(fm, false);
        fm.close();
        return image;
    }

    /**
     * Sleeps for the specified time. 
     */
    @Override
    public native void delay(int ms);

    /**
     * Returns the number of milliseconds this Robot sleeps after generating an event. 
     */
    @Override
    public native int getAutoDelay();

    /**
     * Returns the color of a pixel at the given screen coordinates. 
     */
    @Override
    public native Color getPixelColor(int x, int y);

    /**
     * 
     * Returns whether this Robot automatically invokes waitForIdle after generating an event. 
     */
    @Override
    public native boolean isAutoWaitForIdle();

    /**
     * Presses a given key. 
     */
    @Override
    public native void keyPress(int keycode);

    /**
     * Releases a given key.
     */
    @Override
    public native void keyRelease(int keycode);

    /**
     * Moves mouse pointer to given screen coordinates. 
     */
    @Override
    public native void mouseMove(int x, int y);

    /**
     * Presses one or more mouse buttons. 
     */
    @Override
    public native void mousePress(int buttons);

    /**
     * Releases one or more mouse buttons. 
     */
    @Override
    public native void mouseRelease(int buttons);

    /**
     * Rotates the scroll wheel on wheel-equipped mice. 
     */
    @Override
    public native void mouseWheel(int wheelAmt);

    /**
     * Sets the number of milliseconds this Robot sleeps after generating an event. 
     */
    @Override
    public native void setAutoDelay(int ms);

    /**
     * Sets whether this Robot automatically invokes waitForIdle after generating an event.
     */
    @Override
    public native void setAutoWaitForIdle(boolean isOn);

    /**
     * Returns a string representation of this Robot. 
     */
    @Override
    public native String toString();

    /**
     * Waits until all events currently on the event queue have been processed. 
     */
    @Override
    public native void waitForIdle();

    /**
     *  capture mouse cursor to WinFileMappingBuffer
     * @param origin [out] point of the mouse cursor
     * @param hotspot [out] hotspot of the mouse cursor
     * @return mouse cursor
     */
    public native WinFileMappingBuffer captureMouseCursorAsFileMapping(Point origin, Point hotspot, int[] type);

    /**
     * capture mouse cursor
     * @param origin [out] point of the mouse cursor
     * @param hotspot [out] hotspot of the mouse cursor
     * @param type [out] return the type if cursor is predefined ,or reurn CURSOR_UNKNOWN
     * @return mouse cursor
     */
    public BufferedImage captureMouseCursor(java.awt.Point origin , java.awt.Point hotspot, int[] type) {
        WinFileMappingBuffer fm = captureMouseCursorAsFileMapping(origin, hotspot, type);
        if (fm == null) {
            return null;
        }
        if (m_last_cursor_buffer == fm && m_last_cursor_image != null) {
            return m_last_cursor_image;
        }
        if (m_last_cursor_buffer != null) {
            m_last_cursor_buffer.close();
        }
        m_last_cursor_buffer = fm;
        m_last_cursor_image = CreateBuffedImage(fm, true);

        return m_last_cursor_image;
    }

    /**
     * send character
     * 
     *  send the supplied character as keyboard input as if the
     *  corresponding character was generated on the keyboard 
     */
    public native void keyType(char c);

    /**
     * send CTRL-ALT-DEL 
     */
    public native void sendCtrlAltDel();

    /**
     * Change keyboard layout of the input thread
     * @param KLID KeyboardLayout id @see http://msdn.microsoft.com/en-us/library/ms646305(v=vs.85).aspx
     */
    public native void setKeyboardLayout(String KLID);

    /**
     * initialize jni object
     *  
     * create a jni object which implements native methods,and
     * then bind to the the java object.
     * @param device 
     */
    private native void CreateJNIObj(String device);

    /**
     * delete the binded jni object.
     */
    private native void DestroyJNIObj();
    /**
     * used by jni
     * 
     * save the jni object's pointer
     */
    @SuppressWarnings("unused")
    private long __pJniObj;

    /**
     * Create BufferedImage from WinFileMappingBuffer
     * @param fm 
     * @return
     */
    private BufferedImage CreateBuffedImage(WinFileMappingBuffer fm, boolean bWithAlphaChanle) {
        BitmapFileBuffer bitmap = new BitmapFileBuffer();
        if (!bitmap.Load(fm.getBuffer())) {
            return null;
        }
        return bitmap.GetBuffedImage(bWithAlphaChanle);
    }
    //storage last captured cursor,to decide whether 
    //cursor is change sine the last capture 
    private WinFileMappingBuffer m_last_cursor_buffer;
    private BufferedImage m_last_cursor_image;
}
