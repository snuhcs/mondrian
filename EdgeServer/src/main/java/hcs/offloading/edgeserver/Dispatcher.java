package hcs.offloading.edgeserver;

import android.graphics.Bitmap;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;
import android.widget.ImageView;

import org.json.JSONException;
import org.webrtc.MediaStream;
import org.webrtc.PeerConnection;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoFrame;
import org.webrtc.VideoSink;
import org.webrtc.VideoTrack;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.stream.Collectors;

import hcs.offloading.edgeserver.config.DispatcherConfig;
import hcs.offloading.edgeserver.datatypes.BoundingBox;
import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.edgeserver.datatypes.FrameBatch;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.network.webrtc.WebRTCManager;
import hcs.offloading.network.webrtc.YuvFrame;

@RequiresApi(api = Build.VERSION_CODES.N)
public class Dispatcher implements Runnable, VideoSink {
    private static final String TAG = Dispatcher.class.getName();

    private final int BATCH_SIZE;
    private final int FULL_INFERENCE_INTERVAL;
    private final int NUM_FRAMES_ON_MEMORY;

    public interface Callback {
        void removeSourceIP(String ip);
        void enqueueFrameBatch(String ip, FrameBatch frameBatch);
        void enqueueInferenceRequest(InferenceRequest inferenceRequest);
    }

    private final Callback mCallback;

    private final SurfaceViewRenderer mInputView;
    private final ImageView mOutputView;

    private int mFrameIndex = 0;
    private final Thread mDispatchThread;
    private final Map<Integer, Frame> mFrames = new ConcurrentHashMap<>();
    private final Map<Integer, List<BoundingBox>> mResults = new HashMap<>();
    private final List<Integer> mFrameIndices = new ArrayList<>();
    private final LinkedBlockingQueue<Pair<List<Integer>, Integer>> mFrameIndicesAndLastIndexList = new LinkedBlockingQueue<>();

    private final String mSourceIP;
    private MediaStream mMediaStream;
    private VideoTrack mVideoTrack;
    private final PeerConnection mPeerConnection;
    private final WebRTCManager mWebRTCManager;

    Dispatcher(DispatcherConfig config, Callback callback, String sourceIP, WebRTCManager webRTCManager, SurfaceViewRenderer inputView, ImageView outputView) {
        BATCH_SIZE = config.BATCH_SIZE;
        FULL_INFERENCE_INTERVAL = config.FULL_INFERENCE_INTERVAL;
        NUM_FRAMES_ON_MEMORY = config.NUM_FRAMES_ON_MEMORY;

        mCallback = callback;

        mSourceIP = sourceIP;
        mWebRTCManager = webRTCManager;
        mInputView = inputView;
        mOutputView = outputView;

        mPeerConnection = mWebRTCManager.createPeerConnection(sourceIP);

        mDispatchThread = new Thread(this);
        mDispatchThread.start();
    }

    public void close() {
        if (mPeerConnection != null && !mPeerConnection.signalingState().equals(PeerConnection.SignalingState.CLOSED)) {
            synchronized (mPeerConnection) {
                if (mMediaStream != null) {
                    mVideoTrack.removeSink(this);
                    mVideoTrack.removeSink(mInputView);
                    mMediaStream.removeTrack(mVideoTrack);
                    mPeerConnection.removeStream(mMediaStream);
                }
            }
            mPeerConnection.close();
        }
        try {
            mDispatchThread.interrupt();
            mDispatchThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        mCallback.removeSourceIP(mSourceIP);
        Log.d(TAG, "closed");
    }

    @Override
    public void run() {
        try {
            while (true) {
                Pair<List<Integer>, Integer> frameIndicesAndLastIndex = mFrameIndicesAndLastIndexList.take();
                int lastIndex = frameIndicesAndLastIndex.second;
                List<BoundingBox> prevResults = getResult(lastIndex);
                mCallback.enqueueFrameBatch(mSourceIP, new FrameBatch(
                        frameIndicesAndLastIndex.first.stream().map(mFrames::get).collect(Collectors.toList()),
                        mFrames.get(lastIndex),
                        prevResults));
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    @Override
    public void onFrame(VideoFrame videoFrame) {
        YuvFrame yuvFrame = new YuvFrame(videoFrame);
        Bitmap bitmap = yuvFrame.getBitmap();

        Frame frame = new Frame(mSourceIP, mFrameIndex, bitmap);
        mFrames.put(mFrameIndex, frame);
        if (mFrameIndex > NUM_FRAMES_ON_MEMORY) {
            mFrames.remove(mFrameIndex - NUM_FRAMES_ON_MEMORY);
            removeResult(mFrameIndex - NUM_FRAMES_ON_MEMORY);
        }
        if (mFrameIndex % (FULL_INFERENCE_INTERVAL * BATCH_SIZE) == 0) {
            Log.d(TAG, "dispatch full frame " + mFrameIndex);
            mCallback.enqueueInferenceRequest(new InferenceRequest(frame));
        } else {
            mFrameIndices.add(mFrameIndex);
        }
        if ((mFrameIndex + 1) % BATCH_SIZE == 0) {
            Log.d(TAG, "dispatch mixed frame " + mFrameIndex);
            mFrameIndicesAndLastIndexList.add(new Pair<>(new ArrayList<>(mFrameIndices), mFrameIndices.get(0) - 1));
            mFrameIndices.clear();
        }
        mFrameIndex++;
    }

    private void removeResult(int frameIndex) {
        synchronized (mResults) {
            mResults.remove(frameIndex);
        }
    }

    private List<BoundingBox> getResult(int frameIndex) throws InterruptedException {
        synchronized (mResults) {
            while (!mResults.containsKey(frameIndex)) {
                mResults.wait();
            }
            return mResults.get(frameIndex);
        }
    }

    public void updateResult(int frameIndex, List<BoundingBox> boxes) {
        Log.d(TAG, "update result: " + frameIndex);
        synchronized (mResults) {
            mResults.put(frameIndex, boxes);
            mResults.notifyAll();
        }
        drawResult(frameIndex, boxes);
    }

    public void updateResult(Map<Integer, List<BoundingBox>> batchResult) {
        Log.d(TAG, "update results: " + batchResult.size());
        int maxFrameIndex = -1;
        List<BoundingBox> maxBoxes = null;
        synchronized (mResults) {
            for (Map.Entry<Integer, List<BoundingBox>> kv : batchResult.entrySet()) {
                int frameIndex = kv.getKey();
                List<BoundingBox> boxes = kv.getValue();
                if (!mResults.containsKey(frameIndex)) {
                    mResults.put(frameIndex, boxes);
                    if (maxFrameIndex < frameIndex) {
                        maxFrameIndex = frameIndex;
                        maxBoxes = boxes;
                    }
                }
            }
            mResults.notifyAll();
        }
        if (maxFrameIndex != -1) {
            drawResult(maxFrameIndex, maxBoxes);
        }
    }

    private void drawResult(int frameIndex, List<BoundingBox> boxes) {
        Frame frame = mFrames.get(frameIndex);
        assert frame != null;
        Bitmap bitmap = Utils.drawResult(frame.bitmap, boxes);
        mOutputView.post(() -> mOutputView.setImageBitmap(bitmap));
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

    void onAddStream(MediaStream mediaStream) {
        synchronized (mPeerConnection) {
            mMediaStream = mediaStream;
            mVideoTrack = mMediaStream.videoTracks.get(0);
            mVideoTrack.setEnabled(true);

            mVideoTrack.addSink(mInputView);
            mVideoTrack.addSink(this);
        }
    }
}
