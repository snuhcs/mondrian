package hcs.offloading.mondrian;

import android.graphics.ImageFormat;
import android.media.Image;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaExtractor;
import android.media.MediaFormat;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

import java.io.IOException;
import java.nio.ByteBuffer;

public class VideoLoader implements Runnable {
    private static final int MEDIACODEC_TIMEOUT_US = 1000;
    private static int numAllStreams = 0;

    private final Thread thread;
    private final Callback callback;
    private final int startVid;
    private final VideoConfig config;

    private final MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
    private final MediaExtractor extractor;
    private final MediaCodec decoder;
    private final byte[] yuvBytes;
    private final byte[] rowBytes;
    private final Mat yuvMat;

    public interface Callback {
        void onFrame(int vid, Mat yuvMat);
    }

    public static class VideoConfig {
        int numStreams;
        String path;
        int numFrames;
        int fps;
    }

    public VideoLoader(VideoConfig config, Callback callback) throws IOException {
        this.config = config;
        this.callback = callback;

        this.startVid = numAllStreams;
        numAllStreams += config.numStreams;

        extractor = new MediaExtractor();
        extractor.setDataSource(config.path);
        int videoTrackIndex = videoTrackIndexOf(extractor);
        assert videoTrackIndex != -1;

        MediaFormat format = extractor.getTrackFormat(videoTrackIndex);
        int width = format.getInteger(MediaFormat.KEY_WIDTH);
        int height = format.getInteger(MediaFormat.KEY_HEIGHT);
        int frameRate = format.getInteger(MediaFormat.KEY_FRAME_RATE);
        if (config.fps == 0) {
            config.fps = frameRate;
        }
        yuvBytes = new byte[width * height * 12 / 8]; // 12 bit for each YUV420 pixel
        rowBytes = new byte[width];
        yuvMat = new Mat(height + height / 2, width, CvType.CV_8UC1);

        extractor.selectTrack(videoTrackIndex);
        String mime = format.getString(MediaFormat.KEY_MIME);
        format.setInteger(MediaFormat.KEY_COLOR_FORMAT,
                MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible);
        decoder = MediaCodec.createDecoderByType(mime);
        decoder.configure(format, null, null, 0);
        decoder.start();

        thread = new Thread(this);
    }

    public void start() {
        thread.start();
    }

    private int videoTrackIndexOf(MediaExtractor extractor) {
        for (int trackIndex = 0; trackIndex < extractor.getTrackCount(); trackIndex++) {
            MediaFormat format = extractor.getTrackFormat(trackIndex);
            String mime = format.getString(MediaFormat.KEY_MIME);
            if (mime.startsWith("video/")) {
                return trackIndex;
            }
        }
        return -1;
    }

    public void close() {
        thread.interrupt();
        try {
            thread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        decoder.stop();
        decoder.release();
        extractor.release();
    }

    @Override
    public void run() {
        assert (Utils.schedSetAffinityLittle());
        boolean enqueueEnd = false;
        long intervalNs = (long) (1e9 / config.fps);
        int frameIndex = 0;
        long firstFrameFinishedTimeNs = -1;
        while (true) {
            if (!enqueueEnd) {
                enqueueEnd = enqueueStream(decoder, extractor);
            }

            int outputIndex = decoder.dequeueOutputBuffer(bufferInfo, MEDIACODEC_TIMEOUT_US);
            if (bufferInfo.flags == MediaCodec.BUFFER_FLAG_END_OF_STREAM) {
                break; // decoding end
            }
            if (outputIndex < 0) { // if no available output buffer
                continue;
            }

            Image image = decoder.getOutputImage(outputIndex);
            assert (image.getFormat() == ImageFormat.YUV_420_888);

            // https://gist.github.com/FWStelian/4c3dcd35960d6eabbe661c3448dd5539
            Image.Plane[] planes = image.getPlanes();
            int width = image.getWidth();
            int height = image.getHeight();
            int offset = 0;
            for (int i = 0; i < planes.length; i++) {
                ByteBuffer buffer = planes[i].getBuffer();
                int rowStride = planes[i].getRowStride();
                assert (rowStride <= rowBytes.length);
                int pixelStride = planes[i].getPixelStride();
                int w = (i == 0) ? width : width / 2;
                int h = (i == 0) ? height : height / 2;
                for (int row = 0; row < h; row++) {
                    int bytesPerPixel = ImageFormat.getBitsPerPixel(ImageFormat.YUV_420_888) / 8;
                    if (pixelStride == bytesPerPixel) {
                        int length = w * bytesPerPixel;
                        buffer.get(yuvBytes, offset, length);
                        // Advance buffer the remainder of the row stride, unless on the last row.
                        // Otherwise, this will throw an IllegalArgumentException because the buffer
                        // doesn't include the last padding.
                        if (h - row != 1) {
                            buffer.position(buffer.position() + rowStride - length);
                        }
                        offset += length;
                    } else {
                        // On the last row only read the width of the image minus the pixel stride
                        // plus one. Otherwise, this will throw a BufferUnderflowException because the
                        // buffer doesn't include the last padding.
                        if (h - row == 1) {
                            buffer.get(rowBytes, 0, width - pixelStride + 1);
                        } else {
                            buffer.get(rowBytes, 0, rowStride);
                        }
                        for (int col = 0; col < w; col++) {
                            yuvBytes[offset++] = rowBytes[col * pixelStride];
                        }
                    }
                }
            }

            yuvMat.put(0, 0, yuvBytes);
            decoder.releaseOutputBuffer(outputIndex, false);

            // Wait for interval
            if (config.fps > 0 && frameIndex >= 2) {
                assert firstFrameFinishedTimeNs != -1;
                long requiredNs = firstFrameFinishedTimeNs + (frameIndex - 1) * intervalNs;
                sleepFor(requiredNs - System.nanoTime());
            }

            for (int vid = startVid; vid < startVid + config.numStreams; vid++) {
                callback.onFrame(vid, yuvMat);
            }
            if (frameIndex == 1) {
                // Set start time = second frame finished time
                // Second frame waits for the first frame inference
                firstFrameFinishedTimeNs = System.nanoTime();
            }
            frameIndex++;
            if (frameIndex == config.numFrames) {
                break;
            }
        }
    }

    private static boolean enqueueStream(MediaCodec decoder, MediaExtractor extractor) {
        int inputIndex = decoder.dequeueInputBuffer(MEDIACODEC_TIMEOUT_US);
        if (inputIndex < 0) return false; // if no such buffer is currently available

        ByteBuffer inputBuffer = decoder.getInputBuffer(inputIndex);
        int sampleSize = extractor.readSampleData(inputBuffer, 0);
        if (sampleSize < 0) { // if end of stream
            decoder.queueInputBuffer(inputIndex, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
            return true;
        }

        // enqueue stream
        decoder.queueInputBuffer(inputIndex, 0, sampleSize, extractor.getSampleTime(), 0);
        extractor.advance();
        return false;
    }

    private void sleepFor(long sleepNs) {
        long sleepMs = sleepNs / (long) 1e6;
        if (sleepMs <= 0) return;
        try {
            Thread.sleep(sleepMs);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
