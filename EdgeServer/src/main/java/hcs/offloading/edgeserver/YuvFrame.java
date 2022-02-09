package hcs.offloading.edgeserver;

import android.graphics.Bitmap;
import android.graphics.Matrix;

import org.webrtc.VideoFrame;

import java.nio.ByteBuffer;
import java.nio.FloatBuffer;

import io.github.zncmn.libyuv.YuvConvert;
import io.github.zncmn.libyuv.YuvFormat;

// TODO: project should include org.webrtc:google-webrtc from Google
// TODO: project should include io.github.zncmn.libyuv:core:0.0.7 from Maven

public class YuvFrame {
    public static final int PROCESSING_NONE = 0x00;
    public static final int PROCESSING_CROP_TO_SQUARE = 0x01;
    // Converts from NV21 format to ARGB format
    private static final YuvConvert yuvConverter = new YuvConvert();
    private final Object planeLock = new Object();
    public int width;
    public int height;
    public byte[] nv21Buffer;
    public int rotationDegree;
    public long timestamp;


    /**
     * Creates a YuvFrame from the provided VideoFrame. Does no processing, and uses the current time as a timestamp.
     *
     * @param videoFrame Source VideoFrame.
     */
    @SuppressWarnings("unused")
    public YuvFrame(final VideoFrame videoFrame) {
        fromVideoFrame(videoFrame, PROCESSING_NONE, System.nanoTime());
    }


    /**
     * Creates a YuvFrame from the provided VideoFrame. Does any processing indicated, and uses the current time as a timestamp.
     *
     * @param videoFrame      Source VideoFrame.
     * @param processingFlags Processing flags, YuvFrame.PROCESSING_NONE for no processing.
     */
    @SuppressWarnings("unused")
    public YuvFrame(final VideoFrame videoFrame, final int processingFlags) {
        fromVideoFrame(videoFrame, processingFlags, System.nanoTime());
    }


    /**
     * Creates a YuvFrame from the provided VideoFrame. Does any processing indicated, and uses the given timestamp.
     *
     * @param videoFrame      Source VideoFrame.
     * @param processingFlags Processing flags, YuvFrame.PROCESSING_NONE for no processing.
     * @param timestamp       The timestamp to give the frame.
     */
    public YuvFrame(final VideoFrame videoFrame, final int processingFlags, final long timestamp) {
        fromVideoFrame(videoFrame, processingFlags, timestamp);
    }

    public float[] getYFloat() {
        byte[] bytes = new byte[width * height];
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bytes[y * width + x] = nv21Buffer[y * width + x];
            }
        }
        FloatBuffer fb = ByteBuffer.wrap(bytes).asFloatBuffer();
        float[] yPlane = new float[width * height / 4];
        fb.get(yPlane);

        return yPlane;
    }


    /**
     * Replaces the data in this YuvFrame with the data from the provided frame. Will create new byte arrays to hold pixel data if necessary,
     * or will reuse existing arrays if they're already the correct size.
     *
     * @param videoFrame      Source VideoFrame.
     * @param processingFlags Processing flags, YuvFrame.PROCESSING_NONE for no processing.
     * @param timestamp       The timestamp to give the frame.
     */
    public void fromVideoFrame(final VideoFrame videoFrame, final int processingFlags, final long timestamp) {
        if (videoFrame == null) {
            return;
        }

        synchronized (planeLock) {
            try {
                // Save timestamp
                this.timestamp = timestamp;

                // Copy rotation information
                rotationDegree = videoFrame.getRotation();  // Just save rotation info for now, doing actual rotation can wait until per-pixel processing.

                // Copy the pixel data, processing as requested.
                if (PROCESSING_CROP_TO_SQUARE == (processingFlags & PROCESSING_CROP_TO_SQUARE)) {
                    copyPlanesCropped(videoFrame.getBuffer());
                } else {
                    copyPlanes(videoFrame.getBuffer());
                }
            } catch (Throwable t) {
                dispose();
            }
        }
    }


    public void dispose() {
        nv21Buffer = null;
    }


    public boolean hasData() {
        return nv21Buffer != null;
    }


    /**
     * Copy the Y, V, and U planes from the source I420Buffer.
     * Sets width and height.
     *
     * @param videoFrameBuffer Source frame buffer.
     */
    private void copyPlanes(final VideoFrame.Buffer videoFrameBuffer) {
        VideoFrame.I420Buffer i420Buffer = null;

        if (videoFrameBuffer != null) {
            i420Buffer = videoFrameBuffer.toI420();
        }

        if (i420Buffer == null) {
            return;
        }

        synchronized (planeLock) {
            // Set the width and height of the frame.
            width = i420Buffer.getWidth();
            height = i420Buffer.getHeight();

            // Calculate sizes needed to convert to NV21 buffer format
            final int size = width * height;
            final int chromaStride = width;
            final int chromaWidth = (width + 1) / 2;
            final int chromaHeight = (height + 1) / 2;
            final int nv21Size = size + chromaStride * chromaHeight;

            if (nv21Buffer == null || nv21Buffer.length != nv21Size) {
                nv21Buffer = new byte[nv21Size];
            }

            final byte[] yPlane = getByteArrayFromByteBuffer(i420Buffer.getDataY());
            final byte[] uPlane = getByteArrayFromByteBuffer(i420Buffer.getDataU());
            final byte[] vPlane = getByteArrayFromByteBuffer(i420Buffer.getDataV());
            final int yStride = i420Buffer.getStrideY();
            final int uStride = i420Buffer.getStrideU();
            final int vStride = i420Buffer.getStrideV();

            // Populate a buffer in NV21 format because that's what the converter wants
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    nv21Buffer[y * width + x] = yPlane[y * yStride + x];
                }
            }

            for (int y = 0; y < chromaHeight; y++) {
                for (int x = 0; x < chromaWidth; x++) {
                    // Swapping U and V values here because it makes the image the right color

                    // Store V
                    nv21Buffer[size + y * chromaStride + 2 * x + 1] = uPlane[y * uStride + x];

                    // Store U
                    nv21Buffer[size + y * chromaStride + 2 * x] = vPlane[y * vStride + x];
                }
            }
        }
    }

    private byte[] getByteArrayFromByteBuffer(ByteBuffer byteBuffer) {
        byte[] bytesArray = new byte[byteBuffer.remaining()];
        byteBuffer.get(bytesArray, 0, bytesArray.length);
        return bytesArray;
    }

    /**
     * Copy the Y, V, and U planes from the source I420Buffer, cropping them to square.
     * Sets width and height.
     *
     * @param videoFrameBuffer Source frame buffer.
     */
    private void copyPlanesCropped(final VideoFrame.Buffer videoFrameBuffer) {
        if (videoFrameBuffer == null) {
            return;
        }

        synchronized (planeLock) {
            // Verify that the dimensions of the I420Frame are appropriate for cropping
            // If improper dimensions are found, default back to copying the entire frame.
            final int width = videoFrameBuffer.getWidth();
            final int height = videoFrameBuffer.getHeight();

            if (width > height) {
                final VideoFrame.Buffer croppedVideoFrameBuffer = videoFrameBuffer.cropAndScale((width - height) / 2, 0, height, height, height, height);

                copyPlanes(croppedVideoFrameBuffer);

                croppedVideoFrameBuffer.release();
            } else {
                final VideoFrame.Buffer croppedVideoFrameBuffer = videoFrameBuffer.cropAndScale(0, (height - width) / 2, width, width, width, width);

                copyPlanes(croppedVideoFrameBuffer);

                croppedVideoFrameBuffer.release();
            }
        }
    }


    /**
     * Converts this YUV frame to an ARGB_8888 Bitmap. Applies stored rotation.
     *
     * @return A new Bitmap containing the converted frame.
     */
    public Bitmap getBitmap() {
        if (nv21Buffer == null) {
            return null;
        }

        // Calculate the size of the frame
        final int size = width * height;

        // Allocate an array to hold the ARGB pixel data
        final byte[] argbBytes = new byte[size * 4];

        // Use the converter (based on WebRTC source) to change to ARGB format
        yuvConverter.toARGB(nv21Buffer, argbBytes, width, height, YuvFormat.NV21);

        // Construct a Bitmap based on the new pixel data
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.copyPixelsFromBuffer(ByteBuffer.wrap(argbBytes));

        boolean filter = false;
        // If necessary, generate a rotated version of the Bitmap
        if (rotationDegree == 90 || rotationDegree == -270) {
            final Matrix m = new Matrix();
            m.postRotate(90);
            return Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), m, filter);
        } else if (rotationDegree == 180 || rotationDegree == -180) {
            final Matrix m = new Matrix();
            m.postRotate(180);

            return Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), m, filter);
        } else if (rotationDegree == 270 || rotationDegree == -90) {
            final Matrix m = new Matrix();
            m.postRotate(270);

            Bitmap bitmap1 = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), m, filter);
            return bitmap1;
        } else {
            // Don't rotate, just return the Bitmap
            return bitmap;
        }
    }
}
