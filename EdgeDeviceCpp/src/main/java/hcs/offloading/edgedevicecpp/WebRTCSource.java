package hcs.offloading.edgedevicecpp;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.Log;

import org.json.JSONException;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.webrtc.MediaStream;
import org.webrtc.PeerConnection;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoFrame;
import org.webrtc.VideoSink;
import org.webrtc.VideoTrack;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;

import hcs.offloading.network.webrtc.WebRTCManager;
import hcs.offloading.network.webrtc.YuvFrame;
import hcs.offloading.strmcpp.SpatioTemporalRoIMixer;
import hcs.offloading.strmcpp.BoundingBox;

public class WebRTCSource implements VideoSink, Runnable {
    private static final String TAG = WebRTCSource.class.getName();

    private final SurfaceViewRenderer mInputView;

    private final SpatioTemporalRoIMixer strm;

    private final Map<Integer, Bitmap> frames = new ConcurrentHashMap<>();
    private final LinkedBlockingQueue<Integer> frameIndices = new LinkedBlockingQueue<>();
    private final Thread drawThread;
    private final ResultCallback mResultCallback;
    private final float DRAW_CONFIDENCE;

    private final String mSourceIP;
    private MediaStream mMediaStream;
    private VideoTrack mVideoTrack;
    private final PeerConnection mPeerConnection;
    private final WebRTCManager mWebRTCManager;

    WebRTCSource(String sourceIP, SpatioTemporalRoIMixer strm, WebRTCManager webRTCManager,
                 SurfaceViewRenderer inputView, ResultCallback resultCallback, float drawConfidence) {
        mSourceIP = sourceIP;
        this.strm = strm;
        mWebRTCManager = webRTCManager;
        mInputView = inputView;
        mResultCallback = resultCallback;
        DRAW_CONFIDENCE = drawConfidence;

        mPeerConnection = mWebRTCManager.createPeerConnection(sourceIP);

        drawThread = new Thread(this);
    }

    @Override
    public void run() {
        try {
            while (true) {
                int frameIndex = frameIndices.take();
                List<BoundingBox> results = strm.getResults(mSourceIP, frameIndex);
                Bitmap bitmap = frames.remove(frameIndex);
                mResultCallback.log(mSourceIP, frameIndex, results);
                mResultCallback.drawObjectDetectionResult(drawBoxes(
                        bitmap, results, DRAW_CONFIDENCE));
                Thread.sleep(50);
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    public static Bitmap drawBoxes(Bitmap bitmap, List<BoundingBox> boxes, float drawConfidence) {
        final Canvas canvas = new Canvas(bitmap);
        final Paint paint = new Paint();
        paint.setColor(Color.HSVToColor(new float[]{120f, 1f, 1f}));
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(5.0f);
        for (BoundingBox box : boxes) {
            if (box.confidence >= drawConfidence) {
                canvas.drawRect(new Rect(box.left, box.top, box.right, box.bottom), paint);
            }
        }
        return bitmap;
    }

    void onAddStream(MediaStream mediaStream) {
        synchronized (mPeerConnection) {
            mMediaStream = mediaStream;
            mVideoTrack = mMediaStream.videoTracks.get(0);
            mVideoTrack.setEnabled(true);

            mVideoTrack.addSink(mInputView);
            mVideoTrack.addSink(this);
        }
    }

    public void close() {
        synchronized (mPeerConnection) {
            if (mMediaStream != null) {
                mVideoTrack.removeSink(this);
                mVideoTrack.removeSink(mInputView);
                mMediaStream.removeTrack(mVideoTrack);
                mPeerConnection.removeStream(mMediaStream);
            }
            if (!mPeerConnection.connectionState().equals(PeerConnection.PeerConnectionState.CLOSED)) {
                mPeerConnection.close();
            }
        }
        try {
            drawThread.interrupt();
            drawThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        Log.d(TAG, "closed");
    }

    @Override
    public void onFrame(VideoFrame videoFrame) {
        YuvFrame yuvFrame = new YuvFrame(videoFrame);
        Bitmap bitmap = yuvFrame.getBitmap();
        Mat mat = new Mat();
        Utils.bitmapToMat(bitmap, mat);
        int index = strm.enqueueImage(mSourceIP, mat.clone());
        frames.put(index, bitmap);
        try {
            frameIndices.put(index);
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        mat.release();
    }

    void handleSdpAndAnswer(String message) {
        try {
            mWebRTCManager.handleSdpMessage(mPeerConnection, message);
            mWebRTCManager.answer(mPeerConnection, mSourceIP);
        } catch (JSONException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    void handleIceMessage(String message) {
        try {
            mWebRTCManager.handleIceMessage(mPeerConnection, message);
        } catch (JSONException e) {
            Log.e(TAG, e.getMessage());
        }
    }
}
