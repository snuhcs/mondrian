package hcs.offloading.mondrian;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.util.Log;

import org.opencv.core.CvType;
import org.opencv.core.Mat;

import java.io.IOException;
import java.nio.ByteBuffer;

public class VideoLoader implements Runnable {
    private static final String TAG = VideoLoader.class.getName();
    private static final int MEDIACODEC_TIMEOUT_US = 1000;

    private final Thread thread;
    private final Callback callback;
    private final int startVid;
    private final int numStreams;
    private final int fps;
    public final long numFrames;

    private final MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
    private final MediaExtractor extractor;
    private final MediaCodec decoder;
    private final byte[] yuvBytes;
    private final Mat yuvMat;

    public interface Callback {
        void onFrame(int vid, Mat yuvMat);

        void onVideoEnd();
    }

    public VideoLoader(int startVid, int numStreams, String videoPath, int fps, Callback callback) throws IOException {
        this.startVid = startVid;
        this.numStreams = numStreams;
        this.fps = fps;
        this.callback = callback;

        extractor = new MediaExtractor();
        extractor.setDataSource(videoPath);
        int videoTrackIndex = videoTrackIndexOf(extractor);
        assert videoTrackIndex != -1;

        MediaFormat format = extractor.getTrackFormat(videoTrackIndex);
        int width = format.getInteger(MediaFormat.KEY_WIDTH);
        int height = format.getInteger(MediaFormat.KEY_HEIGHT);
        int frameRate = format.getInteger(MediaFormat.KEY_FRAME_RATE);
        long durationUs = format.getLong(MediaFormat.KEY_DURATION);
        numFrames = Math.round((double) (frameRate * durationUs) / 1000000.0);
        yuvBytes = new byte[width * height * 12 / 8]; // 12 bit for each YUV420 pixel
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
        assert(Utils.schedSetAffinityLittle());
        boolean enqueueEnd = false;
        long intervalNs = (long) (1e9 / fps);
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

            ByteBuffer outputBuffer = decoder.getOutputBuffer(outputIndex);
            outputBuffer.get(yuvBytes);
            yuvMat.put(0, 0, yuvBytes);
            decoder.releaseOutputBuffer(outputIndex, false);

            // Wait for interval
            if (fps > 0 && frameIndex >= 2) {
                assert firstFrameFinishedTimeNs != -1;
                long requiredNs = firstFrameFinishedTimeNs + (frameIndex - 1) * intervalNs;
                sleepFor(requiredNs - System.nanoTime());
            }

            for (int vid = startVid; vid < startVid + numStreams; vid++) {
                callback.onFrame(vid, yuvMat);
            }
            if (frameIndex == 1) {
                // Set start time = second frame finished time
                // Second frame waits for the first frame inference
                firstFrameFinishedTimeNs = System.nanoTime();
            }
            frameIndex++;
        }
        sleepFor(5000); // Wait 5s for processing end
        callback.onVideoEnd();
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
