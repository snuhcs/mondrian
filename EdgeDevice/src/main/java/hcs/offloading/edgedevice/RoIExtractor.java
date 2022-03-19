package hcs.offloading.edgedevice;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.util.Pair;

import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Size;
import org.opencv.core.TermCriteria;
import org.opencv.imgproc.Imgproc;
import org.opencv.video.Video;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;
import java.util.stream.Collectors;

import hcs.offloading.edgedevice.config.RoIExtractorConfig;
import hcs.offloading.edgedevice.datatypes.BoundingBox;
import hcs.offloading.edgedevice.datatypes.Frame;
import hcs.offloading.edgedevice.datatypes.InferenceRequest;
import hcs.offloading.edgedevice.datatypes.MockProfiles;
import hcs.offloading.edgedevice.datatypes.RoI;

public class RoIExtractor implements Runnable {
    private final String TAG;

    private static final int MAX_QUEUED_FRAMES = 2;

    private final boolean PACKING;
    private final int FULL_INFERENCE_INTERVAL;
    private final float MERGE_THRESHOLD;
    private final int MAX_OPTICAL_FLOW_INTERVAL;
    private final int ROI_PADDING;
    private final boolean NEED_OF_ROI;
    private final boolean NEED_PD_ROI;
    private final boolean NEED_ROI_MERGING;

    static {
        if (!OpenCVLoader.initDebug()) Log.e("OpenCV", "Unable to load OpenCV!");
        else Log.d("OpenCV", "OpenCV loaded Successfully");
    }

    public interface Callback {
        InferenceRequest tryMixingAndGetInferenceRequest(Pair<String, Integer> ipIndex, RoI roi);

        InferenceRequest getMixedFrameRequest();

        void enqueueInferenceRequest(InferenceRequest inferenceRequest);
    }

    private final Queue<Frame> mFrames = new LinkedList<>();
    private final Map<Integer, List<BoundingBox>> mResults = new HashMap<>();
    private int mLastQueriedIndex = -1;
    private int mCountFramesAfterFullInference = 0;

    private final Thread mRoIExtractorThread;
    private final Callback mCallback;

    private final MockProfiles mMockProfiles;


    @RequiresApi(api = Build.VERSION_CODES.P)
    RoIExtractor(RoIExtractorConfig config, Callback callback, String ip) {
        TAG = ip;
        PACKING = config.PACKING;
        FULL_INFERENCE_INTERVAL = config.FULL_INFERENCE_INTERVAL;
        MERGE_THRESHOLD = config.MERGE_THRESHOLD;
        MAX_OPTICAL_FLOW_INTERVAL = config.MAX_OPTICAL_FLOW_INTERVAL;
        ROI_PADDING = config.ROI_PADDING;
        NEED_OF_ROI = config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.COMBINED) ||
                config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.OF);
        NEED_PD_ROI = config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.COMBINED) ||
                config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.PD);
        NEED_ROI_MERGING = config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.COMBINED);
        assert !(!PACKING && NEED_OF_ROI); // if no packing, no optical flow

        mMockProfiles = new MockProfiles(config.PERSON_THRESHOLD, config.CLASS_AGNOSTIC_THRESHOLD);

        mCallback = callback;

        mRoIExtractorThread = new Thread(this);
        mRoIExtractorThread.start();
    }

    public void close() {
        try {
            mRoIExtractorThread.interrupt();
            mRoIExtractorThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage());
        }
        Log.d(TAG, "closed");
    }

    public void enqueueFrame(Frame frame) {
        Log.v(TAG, "Start enqueueFrame() : " + frame.frameIndex);
        try {
            synchronized (mFrames) {
                while (mFrames.size() > MAX_QUEUED_FRAMES) {
                    mFrames.wait();
                }
                mFrames.add(frame);
                mFrames.notifyAll();
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
        Log.v(TAG, "End enqueueFrame() : " + frame.frameIndex);
    }

    private Frame getFrame() throws InterruptedException {
        Log.v(TAG, "Start getFrame()");
        mCountFramesAfterFullInference++;
        Frame frame;
        synchronized (mFrames) {
            while (mFrames.size() <= 0) {
                mFrames.wait();
            }
            frame = mFrames.poll();
            mFrames.notifyAll();
        }
        Log.v(TAG, "End getFrame() : " + frame.frameIndex);
        return frame;
    }

    void enqueueResults(int frameIndex, List<BoundingBox> results) {
        Log.v(TAG, "Start enqueueResults() : " + frameIndex);
        synchronized (mResults) {
            mResults.put(frameIndex, results);
            mResults.notifyAll();
        }
        Log.v(TAG, "End enqueueResults() : " + frameIndex);
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    List<BoundingBox> getResults(int frameIndex) throws InterruptedException {
        mLastQueriedIndex = frameIndex;
        List<BoundingBox> results;
        synchronized (mResults) {
            while (!mResults.containsKey(frameIndex)) {
                mResults.wait();
            }
            results = mResults.get(frameIndex);
            Set<Integer> removeIndices = mResults.keySet().stream()
                    .filter(index -> index <= frameIndex).collect(Collectors.toSet());
            for (int removeIndex : removeIndices) {
                if (removeIndex <= frameIndex) {
                    mResults.remove(removeIndex);
                }
            }
            mResults.notifyAll();
        }
        Log.v(TAG, "End getResults() : " + frameIndex);
        return results;
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    @Override
    public void run() {
        try {
            int mCountMixedInference = FULL_INFERENCE_INTERVAL;
            List<BoundingBox> prevResult = null;
            Frame prevFrame = null;
            Frame currFrame = getFrame();
            while (true) {
                Log.v(TAG, "Prev: " + (prevFrame != null ? prevFrame.frameIndex : -1) + " Curr: " + currFrame.frameIndex);
                if (mCountMixedInference >= FULL_INFERENCE_INTERVAL) {
                    mCountMixedInference = 0;
                    mCountFramesAfterFullInference = 0;
                    mCallback.enqueueInferenceRequest(InferenceRequest.createFullFrameRequest(currFrame));
                    prevFrame = currFrame;
                    currFrame = getFrame();
                    if (NEED_OF_ROI) {
                        Log.v(TAG, "Start getResults() FULL : " + prevFrame.frameIndex);
                        prevResult = getResults(prevFrame.frameIndex);
                    }
                    continue;
                }

                if (mCountFramesAfterFullInference > MAX_OPTICAL_FLOW_INTERVAL) {
                    mCountMixedInference = 0;
                    mCountFramesAfterFullInference = 0;
                    InferenceRequest request = mCallback.getMixedFrameRequest();
                    mCallback.enqueueInferenceRequest(request);
                    mCallback.enqueueInferenceRequest(InferenceRequest.createFullFrameRequest(currFrame));
                    prevFrame = currFrame;
                    currFrame = getFrame();
                    if (NEED_OF_ROI) {
                        Log.v(TAG, "Start getResults() FULL with Max OF : " + prevFrame.frameIndex);
                        prevResult = getResults(prevFrame.frameIndex);
                    }
                    continue;
                }

                List<RoI> rois = new ArrayList<>();
                List<RoI> opticalFlowRoIs = null;
                if (NEED_OF_ROI) {
                    opticalFlowRoIs = createRoIWithInferenceResult(prevFrame, currFrame, prevResult);
                    rois.addAll(opticalFlowRoIs);
                }
                if (NEED_PD_ROI) {
                    List<RoI> pixelDiffRoIs = createRoIsFromDiff(prevFrame, currFrame);
                    rois.addAll(pixelDiffRoIs);
                }
                if (NEED_ROI_MERGING) {
                    mergeSingleFrameRoIs(rois);
                }

                if (!PACKING) {
                    mCallback.enqueueInferenceRequest(InferenceRequest.createSingleRoIFrameRequest(rois));
                    prevFrame = currFrame;
                    currFrame = getFrame();
                    continue;
                }

                rois = resizeRoIs(rois);
                InferenceRequest request = null;
                for (RoI roi : rois) {
                    request = mCallback.tryMixingAndGetInferenceRequest(
                            new Pair<>(currFrame.sourceIP, currFrame.frameIndex), roi);
                    if (request != null) {
                        break;
                    }
                }
                if (request == null) {
                    prevFrame = currFrame;
                    currFrame = getFrame();
                    if (NEED_OF_ROI) {
                        prevResult = opticalFlowRoIs.stream()
                                .map(roi -> new BoundingBox(roi.location, 0f, roi.labelName))
                                .collect(Collectors.toList());
                    }
                } else {
                    mCountMixedInference++;
                    mCountFramesAfterFullInference = 0;
                    mCallback.enqueueInferenceRequest(request);
                    if (NEED_OF_ROI) {
                        if (prevFrame.frameIndex != mLastQueriedIndex) {
                            Log.v(TAG, "Start getResults() MIXED : " + prevFrame.frameIndex);
                            prevResult = getResults(prevFrame.frameIndex);
                        } else {
                            prevFrame = currFrame;
                            currFrame = getFrame();
                        }
                    }
                }
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private List<RoI> resizeRoIs(List<RoI> rois) {
        return rois.stream().map(roi -> roi.resize(mMockProfiles.getProfile(roi.labelName))).collect(Collectors.toList());
    }

    public void mergeSingleFrameRoIs(List<RoI> rois) {
        if (rois.size() == 0) {
            return;
        }
        Frame frame = rois.get(0).frame;

        while (true) {
            RoI originalRoI0 = null;
            RoI originalRoI1 = null;
            RoI mergedRoI = null;
            for (int i = 0; i < rois.size(); i++) {
                for (int j = i + 1; j < rois.size(); j++) {
                    RoI roi0 = rois.get(i);
                    RoI roi1 = rois.get(j);
                    float intersection = hcs.offloading.edgedevice.Utils.box_intersection(roi0.location, roi1.location);
                    if (intersection / roi0.getArea() > MERGE_THRESHOLD || intersection / roi1.getArea() > MERGE_THRESHOLD) {
                        int newTop = Math.min(roi0.location.top, roi1.location.top);
                        int newBottom = Math.max(roi0.location.bottom, roi1.location.bottom);
                        int newRight = Math.max(roi0.location.right, roi1.location.right);
                        int newLeft = Math.min(roi0.location.left, roi1.location.left);
                        if (newLeft >= newRight || newTop >= newBottom) {
                            continue;
                        }
                        originalRoI0 = roi0;
                        originalRoI1 = roi1;
                        Rect newLocation = new Rect(newLeft, newTop, newRight, newBottom);
                        RoI.Type roiType = RoI.Type.PD;
                        String roiLabel = null;
                        if (roi0.type.equals(RoI.Type.OF) || roi1.type.equals(RoI.Type.OF)) {
                            roiType = RoI.Type.OF;
                            if (roi0.labelName != null && roi0.labelName.equals(roi1.labelName)) {
                                roiLabel = roi0.labelName;
                            }
                        }
                        mergedRoI = new RoI(frame, newLocation, roiType, roiLabel);
                        break;
                    }
                }
                if (mergedRoI != null) {
                    break;
                }
            }
            if (mergedRoI == null) {
                break;
            }
            rois.remove(originalRoI0);
            rois.remove(originalRoI1);
            rois.add(mergedRoI);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private List<RoI> createRoIWithInferenceResult(Frame prevFrame, Frame currFrame, List<BoundingBox> boundingBoxes) {
        final int width = currFrame.bitmap.getWidth();
        final int height = currFrame.bitmap.getHeight();

        List<Rect> boundingRects = boundingBoxes.stream().map(bbx -> bbx.location).collect(Collectors.toList());

        List<RoI> opticalFlowRoIs = new ArrayList<>();
        if (!boundingBoxes.isEmpty()) {
            int[][] shifts = getOpticalFlowForBoundingBoxes(prevFrame.bitmap, currFrame.bitmap, boundingRects);
            for (int boxIndex = 0; boxIndex < boundingBoxes.size(); boxIndex++) {
                int[] shift = shifts[boxIndex];
                BoundingBox box = boundingBoxes.get(boxIndex);
                Rect loc = box.location;
                int newLeft = Math.max(0, loc.left + shift[0] - ROI_PADDING);
                int newTop = Math.max(0, loc.top + shift[1] - ROI_PADDING);
                int newRight = Math.min(width, loc.right + shift[0] + ROI_PADDING);
                int newBottom = Math.min(height, loc.bottom + shift[1] + ROI_PADDING);
                if (newLeft < newRight && newTop < newBottom) {
                    opticalFlowRoIs.add(new RoI(
                            currFrame, new Rect(newLeft, newTop, newRight, newBottom), RoI.Type.OF, box.labelName));
                }
            }
        }
        return opticalFlowRoIs;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private int[][] getOpticalFlowForBoundingBoxes(Bitmap f0, Bitmap f1, List<Rect> boundingBoxes) {
        List<Point> centroids = boundingBoxes.stream().map(bbx -> new Point(bbx.centerX(), bbx.centerY())).collect(Collectors.toList());
        MatOfPoint2f p0 = new MatOfPoint2f(), p1 = new MatOfPoint2f();
        p0.fromList(centroids);
        Mat f1_gray = convertBitmapToGrayMat(f0), f2_gray = convertBitmapToGrayMat(f1);

        MatOfByte status = new MatOfByte();
        MatOfFloat err = new MatOfFloat();
        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT + TermCriteria.EPS, 10, 0.03);
        Video.calcOpticalFlowPyrLK(f1_gray, f2_gray, p0, p1, status, err, new Size(15, 15), 2, criteria);

        byte[] StatusArr = status.toArray();
        Point[] p1Arr = p1.toArray();

        int[][] shifts = new int[centroids.size()][2];
        for (int pointIdx = 0; pointIdx < centroids.size(); pointIdx++) {
            if (StatusArr[pointIdx] == 1) {
                shifts[pointIdx][0] = (int) ((float) p1Arr[pointIdx].x - (float) centroids.get(pointIdx).x);
                shifts[pointIdx][1] = (int) ((float) p1Arr[pointIdx].y - (float) centroids.get(pointIdx).y);
            } else {
                shifts[pointIdx][0] = 0;
                shifts[pointIdx][1] = 0;
            }
        }

        return shifts;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private static List<RoI> createRoIsFromDiff(Frame prevFrame, Frame currFrame) {
        Mat img_0_gray = convertBitmapToGrayMat(prevFrame.bitmap);
        Mat img_1_gray = convertBitmapToGrayMat(currFrame.bitmap);

        Mat diff = calculateDiffAndThreshold(img_0_gray, img_1_gray);
        Mat dilated = cannyEdgeDetection(diff);

        ArrayList<MatOfPoint> contours = new ArrayList<>();
        Mat hierarchy = new Mat();
        Imgproc.findContours(dilated, contours, hierarchy, Imgproc.RETR_EXTERNAL, Imgproc.CHAIN_APPROX_SIMPLE);

        List<Rect> locations = getBoxesFromContours(contours, currFrame.bitmap);
        return locations.stream()
                .map(location -> new RoI(currFrame, location, RoI.Type.PD, null))
                .collect(Collectors.toList());
    }

    private static Mat convertBitmapToGrayMat(Bitmap original) {
        Mat originalMat = new Mat();
        Utils.bitmapToMat(original, originalMat);
        Mat grayMat = new Mat();
        Imgproc.cvtColor(originalMat, grayMat, Imgproc.COLOR_BGR2GRAY);
        originalMat.release();
        return grayMat;
    }

    private static Mat calculateDiffAndThreshold(Mat frame0, Mat frame1) {
        Mat diff = frame1.clone();
        Core.absdiff(frame0, frame1, diff);
        for (int i = 0; i < 3; i++) {
            Imgproc.dilate(diff, diff, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(3, 3)), new Point(0, 0), i + 1);
        }
        Imgproc.threshold(diff, diff, 30, 255, Imgproc.THRESH_BINARY);
        return diff;
    }

    private static Mat cannyEdgeDetection(Mat diff) {
        Imgproc.GaussianBlur(diff, diff, new Size(3, 3), 0);
        Imgproc.Canny(diff, diff, 120, 255, 3, true);
        Imgproc.dilate(diff, diff, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(5, 5)), new Point(0, 0), 1);
        return diff;
    }

    private static List<Rect> getBoxesFromContours(List<MatOfPoint> contours, Bitmap frame) {
        List<Rect> boxes = new ArrayList<>();
        for (MatOfPoint contour : contours) {
            MatOfPoint2f contour2f = new MatOfPoint2f(contour.toArray());
            MatOfPoint2f approxCurve = new MatOfPoint2f();
            double approxDistance = Imgproc.arcLength(contour2f, true) * 0.02;
            Imgproc.approxPolyDP(contour2f, approxCurve, approxDistance, true);
            MatOfPoint points = new MatOfPoint(approxCurve.toArray());
            org.opencv.core.Rect rect = Imgproc.boundingRect(points);
            if (rect.height * rect.width < 10000) {
                continue;
            }
            Rect location = new Rect(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
            if (location.left < location.right && location.top < location.bottom) {
                boxes.add(location);
            }
        }
        return boxes;
    }
}
