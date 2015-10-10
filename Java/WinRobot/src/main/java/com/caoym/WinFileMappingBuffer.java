/** @file $Id: WinFileMappingBuffer.java 87 2011-09-28 13:52:41Z caoym $
 *  @author caoym
 *  @brief WinFileMappingBuffer			
 */
package com.caoym;

/**
 * wrap the use of Windows FileMapping
 * Use "Windows FileMapping"(CreateFileMapping) to share memory between processes,\n
 * from JNI process to java process,to avoid copying memory.
 */
public final class WinFileMappingBuffer {

    WinFileMappingBuffer(String name, int size) {
        CreateJNIObj();
        open(name, size);
    }

    /**
     * open specified Windows file mapping object
     * @param name the name of the file mapping object which to be opened
     * @param size the size of the mapped memory
     * @return
     */
    public native boolean open(String name, int size);
    public native void close();
    /**
     * return a java.nio.ByteBuffer which contain the mapped memory.it is
     * created by NewDirectByteBuffer in JNI,with no memory copy,and it will
     * be Inaccessible after WinFileMappingBuffer destroyed.
     * @return
     */
    public native java.nio.ByteBuffer getBuffer();

    @Override
    protected void finalize() {
        //terminate jni object
        DestroyJNIObj();
        try {
            super.finalize();
        } catch (Throwable e) {
            e.printStackTrace();
        }

    }

    /** initialize JNI object
     * create a JNI object which implements native methods,and
     * then bind to the the java object.
     * */
    private native void CreateJNIObj();

    /**
     * delete the binded JNI object.
     */
    private native void DestroyJNIObj();
    /** used by JNI
     * save the JNI object's pointer
     * */
    @SuppressWarnings("unused")
    private long __pJniObj;
}
