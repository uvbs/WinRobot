/** @file $Id: BitmapFileBuffer.java 87 2011-09-28 13:52:41Z caoym $
 *  @author caoym
 *  @brief BitmapFileBuffer			
 */
package com.caoym;

import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferInt;
import java.awt.image.DirectColorModel;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;

/** 
 *  wrapper for handle bitmap file in memory
 * 
 * @author caoym
 */
public class BitmapFileBuffer {

    /** Bitmap file header
     * WORD    bfType;
     * DWORD   bfSize;
     * WORD    bfReserved1;
     * WORD    bfReserved2;
     * DWORD   bfOffBits;
     */
    public class BitmapFileHeader {

        public static final int LENGTH = 14;
        public static final short BM = 0x4D42;
        public short bfType;
        public int bfSize;
        public short bfReserved1;
        public short bfReserved2;
        public int bfOffBits;

        BitmapFileHeader(java.nio.ByteBuffer src) {
            bfType = src.getShort();
            bfSize = src.getInt();
            bfReserved1 = src.getShort();
            bfReserved2 = src.getShort();
            bfOffBits = src.getInt();
        }

        public boolean equals(BitmapFileHeader rh) {
            if (rh == this) {
                return true;
            }
            if (bfType != rh.bfType) {
                return false;
            }
            if (bfSize != rh.bfSize) {
                return false;
            }
            if (bfReserved1 != rh.bfReserved1) {
                return false;
            }
            if (bfReserved2 != rh.bfReserved2) {
                return false;
            }
            if (bfOffBits != rh.bfOffBits) {
                return false;
            }
            return true;
        }
    }

    /** 
     * Bitmap info header
     * DWORD      biSize;
     * LONG       biWidth;
     * LONG       biHeight;
     * WORD       biPlanes;
     * WORD       biBitCount;
     * DWORD      biCompression;
     * DWORD      biSizeImage;
     * LONG       biXPelsPerMeter;
     * LONG       biYPelsPerMeter;
     * DWORD      biClrUsed;
     * DWORD      biClrImportant;
     */
    public class BitmapInfoHeader {

        public static final int LENGTH = 40;
        public int biSize;
        public int biWidth;
        public int biHeight;
        public short biPlanes;
        public short biBitCount;
        public int biCompression;
        public int biSizeImage;
        public int biXPelsPerMeter;
        public int biYPelsPerMeter;
        public int biClrUsed;
        public int biClrImportant;

        BitmapInfoHeader(java.nio.ByteBuffer src) {
            biSize = src.getInt();
            biWidth = src.getInt();
            biHeight = src.getInt();
            biPlanes = src.getShort();
            biBitCount = src.getShort();
            biCompression = src.getInt();
            biSizeImage = src.getInt();
            biXPelsPerMeter = src.getInt();
            biYPelsPerMeter = src.getInt();
            biClrUsed = src.getInt();
            biClrImportant = src.getInt();
            if (biSize > LENGTH) // seek to the end of BitmapInfoHeader
            {
                src.position(src.position() + LENGTH - biSize);
            }
        }

        public boolean equals(BitmapInfoHeader rh) {
            if (rh == this) {
                return true;
            }
            if (biSize != rh.biSize) {
                return false;
            }
            if (biWidth != rh.biWidth) {
                return false;
            }
            if (biHeight != rh.biHeight) {
                return false;
            }
            if (biPlanes != rh.biPlanes) {
                return false;
            }
            if (biBitCount != rh.biBitCount) {
                return false;
            }
            if (biCompression != rh.biCompression) {
                return false;
            }
            if (biSizeImage != rh.biSizeImage) {
                return false;
            }
            if (biXPelsPerMeter != rh.biXPelsPerMeter) {
                return false;
            }
            if (biYPelsPerMeter != rh.biYPelsPerMeter) {
                return false;
            }
            if (biClrUsed != rh.biClrUsed) {
                return false;
            }
            if (biClrImportant != rh.biClrImportant) {
                return false;
            }

            return true;
        }
    }

    BitmapFileBuffer() {
    }

    /**
     * load bitmap from memory,only support 32 color space currently
     * @param src bitmap data
     * @return return true if succeed
     */
    boolean Load(java.nio.ByteBuffer src) {
        if (src == null) {
            return false;
        }
        _data = null;
        src.position(0);
        src.order(java.nio.ByteOrder.LITTLE_ENDIAN);
        if (src.capacity() < BitmapFileHeader.LENGTH + BitmapInfoHeader.LENGTH) {
            return false;
        }
        BitmapFileHeader fh = new BitmapFileHeader(src);
        BitmapInfoHeader ih = new BitmapInfoHeader(src);
        if (fh.bfType != BitmapFileHeader.BM) {
            return false;
        }
        if (fh.bfSize > src.capacity()) {
            return false;
        }
        if (ih.biSize < BitmapInfoHeader.LENGTH) {
            return false;
        }
        if (fh.bfOffBits < BitmapFileHeader.LENGTH + BitmapInfoHeader.LENGTH) {
            return false;
        }
        if (fh.bfOffBits + ih.biSizeImage > fh.bfSize) {
            return false;
        }
        if ((ih.biWidth * ih.biHeight * ih.biBitCount + 7) / 8 > ih.biSizeImage) {
            return false;
        }
        // only support 32 color space currently
        if (ih.biBitCount != 32) {
            return false;
        }
        src.position(0);

        if (fileHeader == null) {
            fileHeader = fh;
        }
        if (infoHeader == null) {
            infoHeader = ih;
        }
        if (!fh.equals(fileHeader) || !ih.equals(infoHeader)) {
            fileHeader = fh;
            infoHeader = ih;
        }
        _src = src;
        if (getData() == null) {
            return false; //  copy to byte[]
        }
        return getData().length * 4 == infoHeader.biSizeImage;
    }
    private java.nio.ByteBuffer _src;
    /**
     * header of bitmap file
     */
    public BitmapFileHeader fileHeader;
    /**
     * header of bitmap info
     */
    public BitmapInfoHeader infoHeader;

    /**
     * get palette date.
     * @return return null if there is no palette,else return the palette data.
     */
    public int[] getPalette() {
        if (infoHeader.biClrUsed != 0) {
            int[] palette = new int[infoHeader.biClrUsed];
            _src.position(BitmapFileHeader.LENGTH + infoHeader.biSize);
            _src.asIntBuffer().get(palette, 0, infoHeader.biClrUsed);
            _src.position(0);
            return palette;
        }
        return null;
    }

    /**
     * Create BufferedImage from BitmapFileBuffer 
     * @param bWithAlphaChanle specify true to create BufferedImage with alpha channel,or false without alpha channel.
     * @return return null if failed
     */
    public BufferedImage GetBuffedImage(boolean bWithAlphaChannel) {
        BufferedImage image;
        DataBuffer buffer;
        WritableRaster raster;
        DirectColorModel screenCapCM;
        int[] bandmasks;
        if (bWithAlphaChannel) {
            bandmasks = new int[4];
            screenCapCM = new DirectColorModel(32,
                    /* red mask */ 0x00FF0000,
                    /* green mask */ 0x0000FF00,
                    /* blue mask */ 0x000000FF,
                    0xFF000000);
            bandmasks[3] = screenCapCM.getAlphaMask();
        } else {
            bandmasks = new int[3];
            screenCapCM = new DirectColorModel(24,
                    /* red mask */ 0x00FF0000,
                    /* green mask */ 0x0000FF00,
                    /* blue mask */ 0x000000FF);
        }

        buffer = new DataBufferInt(getData(), infoHeader.biSizeImage / 4, 0);
        bandmasks[0] = screenCapCM.getRedMask();
        bandmasks[1] = screenCapCM.getGreenMask();
        bandmasks[2] = screenCapCM.getBlueMask();

        raster = Raster.createPackedRaster(buffer, infoHeader.biWidth,
                Math.abs(infoHeader.biHeight), infoHeader.biWidth, bandmasks, null);
        image = new BufferedImage(screenCapCM, raster, false, null);
        return image;
    }

    /**
     * get DIB data
     * @return DIB data,the "int" value is used to specify an RGB color
     */
    public int[] getData() {

        if (_data == null) {
            _src.position(fileHeader.bfOffBits);
            _data = new int[infoHeader.biSizeImage / 4];
            _src.asIntBuffer().get(_data, 0, _data.length);
            _src.position(0);
        }

        return _data;
    }
    private int[] _data;
}
