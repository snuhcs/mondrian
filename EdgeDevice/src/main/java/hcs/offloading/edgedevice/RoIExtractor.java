package hcs.offloading.edgedevice;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;

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
    private final float OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD;
    private final float MERGE_THRESHOLD;
    private final int ROI_PADDING;
    private final boolean NEED_OF_ROI;
    private final boolean NEED_PD_ROI;
    private final boolean NEED_ROI_MERGING;

    static {
        if (!OpenCVLoader.initDebug()) Log.e("OpenCV", "Unable to load OpenCV!");
        else Log.d("OpenCV", "OpenCV loaded Successfully");
    }

    public interface Callback {
        InferenceRequest tryMixingAndGetInferenceRequest(Frame frame, List<RoI> rois);

        void enqueueInferenceRequest(InferenceRequest inferenceRequest);
    }

    private final Queue<Frame> mFrames = new LinkedList<>();
    private final Map<Integer, List<BoundingBox>> mResults = new HashMap<>();
    private int mLastQueriedIndex = -1;

    private final Thread mRoIExtractorThread;
    private final Callback mCallback;

    private final MockProfiles mMockProfiles;


    @RequiresApi(api = Build.VERSION_CODES.P)
    RoIExtractor(RoIExtractorConfig config, Callback callback, String ip) {
        TAG = ip;
        PACKING = config.PACKING;
        FULL_INFERENCE_INTERVAL = config.FULL_INFERENCE_INTERVAL;
        OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD = config.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD;
        MERGE_THRESHOLD = config.MERGE_THRESHOLD;
        ROI_PADDING = config.ROI_PADDING;
        NEED_OF_ROI = config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.COMBINED) ||
                config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.OF);
        NEED_PD_ROI = config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.COMBINED) ||
                config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.PD);
        NEED_ROI_MERGING = config.EXTRACTION_METHOD.equals(RoIExtractorConfig.Method.COMBINED);

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

    void enqueueResults(Map<Integer, List<BoundingBox>> results) {
        Log.v(TAG, "Start enqueueResults() : MIXED");
        synchronized (mResults) {
            for (Map.Entry<Integer, List<BoundingBox>> idxBoxes : results.entrySet()) {
                mResults.put(idxBoxes.getKey(), idxBoxes.getValue());
            }
            mResults.notifyAll();
        }
        Log.v(TAG, "End enqueueResults() : MIXED");
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    List<BoundingBox> getResults(int frameIndex) throws InterruptedException {
        Log.v(TAG, "Start getResults() : " + frameIndex);
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
            int mCountMixedFrameInference = FULL_INFERENCE_INTERVAL;
            boolean updateFrame = true;
            boolean useInferenceResults = false;
            Frame prevFrame = null;
            Frame currFrame = null;
            List<RoI> opticalFlowRoIs = null;
            while (true) {
                /* Cases
                 * 1. Full frame inference
                 *   1.1. When mixed frame inference count > FULL_INFERENCE_INTERVAL
                 * 2. Mixed frame inference
                 *   2.1. When mixed frame is full
                 *   2.2. When mixed frame count > MAX_OPTICAL_FLOW_INTERVAL
                 */
                if (updateFrame) {
                    if (prevFrame != null) {
                        prevFrame.bitmap.recycle();
                    }
                    prevFrame = currFrame != null ? currFrame.copy() : null;
                    currFrame = getFrame();
                }
                updateFrame = true;

                Log.v(TAG, "Prev: " + (prevFrame != null ? prevFrame.frameIndex : -1) + " Curr: " + currFrame.frameIndex);
                if (mCountMixedFrameInference >= FULL_INFERENCE_INTERVAL) {
                    mCallback.enqueueInferenceRequest(InferenceRequest.createFullFrameRequest(currFrame));
                    mCountMixedFrameInference = 0;
                    useInferenceResults = true;
                } else {
                    List<RoI> rois = new ArrayList<>();

                    if (NEED_OF_ROI) {
                        List<BoundingBox> prevResults;
                        if (useInferenceResults) {
                            prevResults = getResults(prevFrame.frameIndex).stream()
                                    .filter(box -> box.confidence > OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD)
                                    .collect(Collectors.toList());
                        } else {
                            prevResults = opticalFlowRoIs.stream()
                                    .map(roi -> new BoundingBox(roi.location, 1f, roi.labelName))
                                    .collect(Collectors.toList());
                        }
                        opticalFlowRoIs = createRoIWithInferenceResult(prevFrame, currFrame, prevResults);
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
                        mCallback.enqueueInferenceRequest(InferenceRequest.createPerRoIInferenceRequest(rois));
                    } else {
                        rois = resizeRoIs(rois);
                        InferenceRequest request = mCallback.tryMixingAndGetInferenceRequest(currFrame, rois);
                        if (request == null) {
                            useInferenceResults = false;
                        } else {
                            mCountMixedFrameInference++;
                            mCallback.enqueueInferenceRequest(request);
                            if (prevFrame.frameIndex != mLastQueriedIndex) {
                                updateFrame = false;
                            }
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
                        int newLeft = Math.min(roi0.location.left, roi1.location.left);
                        int newTop = Math.min(roi0.location.top, roi1.location.top);
                        int newRight = Math.max(roi0.location.right, roi1.location.right);
                        int newBottom = Math.max(roi0.location.bottom, roi1.location.bottom);
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

        List<Rect> boundingRects = boundingBoxes.stream()
                .map(bbx -> bbx.location)
                .collect(Collectors.toList());

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
        Mat f0_gray = new Mat();
        Mat f1_gray = new Mat();
        MatOfPoint2f p0 = new MatOfPoint2f();
        MatOfPoint2f p1 = new MatOfPoint2f();
        MatOfByte status = new MatOfByte();
        MatOfFloat err = new MatOfFloat();

        convertBitmapToGrayMat(f0, f0_gray);
        convertBitmapToGrayMat(f1, f1_gray);

        List<Point> centroids = boundingBoxes.stream().map(bbx -> new Point(bbx.centerX(), bbx.centerY())).collect(Collectors.toList());
        p0.fromList(centroids);
        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT + TermCriteria.EPS, 10, 0.03);
        Video.calcOpticalFlowPyrLK(f0_gray, f1_gray, p0, p1, status, err, new Size(15, 15), 2, criteria);

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

        f0_gray.release();
        f1_gray.release();
        p0.release();
        p1.release();
        status.release();
        err.release();
        return shifts;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private static List<RoI> createRoIsFromDiff(Frame prevFrame, Frame currFrame) {
        Mat img_0_gray = new Mat();
        Mat img_1_gray = new Mat();
        convertBitmapToGrayMat(prevFrame.bitmap, img_0_gray);
        convertBitmapToGrayMat(currFrame.bitmap, img_1_gray);

        Mat mat = img_1_gray.clone();
        calculateDiffAndThreshold(img_0_gray, img_1_gray, mat);
        cannyEdgeDetection(mat);

        ArrayList<MatOfPoint> contours = new ArrayList<>();
        Mat hierarchy = new Mat();
        Imgproc.findContours(mat, contours, hierarchy, Imgproc.RETR_EXTERNAL, Imgproc.CHAIN_APPROX_SIMPLE);
        List<Rect> locations = getBoxesFromContours(contours);
        List<RoI> rois = locations.stream()
                .map(location -> new RoI(currFrame, location, RoI.Type.PD, null))
                .collect(Collectors.toList());

        img_0_gray.release();
        img_1_gray.release();
        hierarchy.release();
        mat.release();
        for (MatOfPoint contour : contours) {
            contour.release();
        }
        return rois;
    }

    private static void convertBitmapToGrayMat(Bitmap original, Mat gray) {
        Mat originalMat = new Mat();
        Utils.bitmapToMat(original, originalMat);
        Imgproc.cvtColor(originalMat, gray, Imgproc.COLOR_BGR2GRAY);
        originalMat.release();
    }

    private static void calculateDiffAndThreshold(Mat frame0, Mat frame1, Mat diff) {
        Core.absdiff(frame0, frame1, diff);
        for (int i = 0; i < 3; i++) {
            Imgproc.dilate(diff, diff, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(3, 3)), new Point(0, 0), i + 1);
        }
        Imgproc.threshold(diff, diff, 30, 255, Imgproc.THRESH_BINARY);
    }

    private static void cannyEdgeDetection(Mat mat) {
        Imgproc.GaussianBlur(mat, mat, new Size(3, 3), 0);
        Imgproc.Canny(mat, mat, 120, 255, 3, true);
        Imgproc.dilate(mat, mat, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(5, 5)), new Point(0, 0), 1);
    }

    private static List<Rect> getBoxesFromContours(List<MatOfPoint> contours) {
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
            contour2f.release();
            approxCurve.release();
            points.release();
        }
        return boxes;
    }
}
