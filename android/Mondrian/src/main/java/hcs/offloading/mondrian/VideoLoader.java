package hcs.offloading.mondrian;

import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

import java.io.IOException;
import java.nio.ByteBuffer;

public class VideoLoader implements Runnable {
    private static final String TAG = VideoLoader.class.getName();
    private static final int MEDIACODEC_TIMEOUT_US = 1000;

    private final Thread thread;
    private final Callback callback;
    private final int vid;
    private final int fps;

    private final MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
    private final MediaExtractor extractor;
    private final MediaCodec decoder;
    private final byte[] yuvBytes;
    private final Mat yuvMat;

    public interface Callback {
        void onFrame(int vid, Mat yuvMat);
    }

    public VideoLoader(int vid, String videoPath, int fps, Callback callback) throws IOException {
        this.vid = vid;
        this.fps = fps;
        this.callback = callback;

        extractor = new MediaExtractor();
        extractor.setDataSource(videoPath);
        int videoTrackIndex = videoTrackIndexOf(extractor);
        assert videoTrackIndex != -1;

        MediaFormat format = extractor.getTrackFormat(videoTrackIndex);
        int width = format.getInteger(MediaFormat.KEY_WIDTH);
        int height = format.getInteger(MediaFormat.KEY_HEIGHT);
        yuvBytes = new byte[width * height * 12 / 8]; // 12 bit for each YUV420 pixel
        yuvMat = new Mat(height + height / 2, width, CvType.CV_8UC1);

        extractor.selectTrack(videoTrackIndex);
        String mime = format.getString(MediaFormat.KEY_MIME);
        decoder = MediaCodec.createDecoderByType(mime);
        decoder.configure(format, null, null, 0);
        decoder.start();

        thread = new Thread(this);
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
        long intervalNs = (long) (1e9 / fps);
        boolean enqueueEnd = false;
        long prevNs = System.nanoTime();
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

            ByteBuffer outputBuffer = decoder.getOutputBuffer(outputIndex);
            outputBuffer.get(yuvBytes);
            yuvMat.put(0, 0, yuvBytes);
            decoder.releaseOutputBuffer(outputIndex, false);

            if (fps > 0) { // Wait for interval
                long elapsedNs = System.nanoTime() - prevNs;
                sleepFor(intervalNs - elapsedNs);
            }

            callback.onFrame(vid, yuvMat);
            prevNs = System.nanoTime();
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
