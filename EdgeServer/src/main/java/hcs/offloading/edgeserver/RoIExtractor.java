package hcs.offloading.edgeserver;

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
import org.opencv.core.Scalar;
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

import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.edgeserver.datatypes.FrameBatch;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.edgeserver.datatypes.RoI;

public class RoIExtractor implements Runnable {
    private static final String TAG = RoIExtractor.class.getName();

    private static final int IDLE_WAIT_MS = 1000;
    private static final int AREA_THRESHOLD = 10000;
    private static final int ROI_PADDING = 0;

    static {
        if (!OpenCVLoader.initDebug()) Log.e("OpenCV", "Unable to load OpenCV!");
        else Log.d("OpenCV", "OpenCV loaded Successfully");
    }

    private final InferenceEngine mInferenceEngine;
    private final Map<String, Queue<FrameBatch>> mFrameBatches = new HashMap<>();

    private final Thread mRoIExtractorThread;

    @RequiresApi(api = Build.VERSION_CODES.P)
    RoIExtractor(InferenceEngine inferenceEngine) {
        mInferenceEngine = inferenceEngine;
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

    public void putFrameBatch(String ip, FrameBatch frameBatch) {
        synchronized (this) {
            if (!mFrameBatches.containsKey(ip)) {
                mFrameBatches.put(ip, new LinkedList<>());
            }
            mFrameBatches.get(ip).add(frameBatch);
            notifyAll();
        }
    }

    public void removeSource(String ip) {
        synchronized (this) {
            mFrameBatches.remove(ip);
            notifyAll();
        }
    }

    private Map<String, FrameBatch> getFrameBatchMap() throws InterruptedException {
        Map<String, FrameBatch> frameBatchMap = new HashMap<>();
        synchronized (this) {
            Set<String> IPs = mFrameBatches.keySet();
            for (String ip : IPs) {
                while (mFrameBatches.containsKey(ip) && mFrameBatches.get(ip).isEmpty()) {
                    wait();
                }
                if (mFrameBatches.containsKey(ip)) {
                    frameBatchMap.put(ip, mFrameBatches.get(ip).poll());
                }
            }
        }
        return frameBatchMap;
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    @Override
    public void run() {
        try {
            while (true) {
                if (mFrameBatches.size() == 0) {
                    Log.d(TAG, "wait for enqueue");
                    Thread.sleep(IDLE_WAIT_MS);
                    continue;
                }

                long startTime, endTime;
                List<RoI> rois = new ArrayList<>();
                Map<String, List<Integer>> mixedFrameIndices = new HashMap<>();

                Map<String, FrameBatch> frameBatchMap = getFrameBatchMap();
                for (Map.Entry<String, FrameBatch> kv : frameBatchMap.entrySet()) {
                    FrameBatch frameBatch = kv.getValue();
                    startTime = System.nanoTime();
                    rois.addAll(getRoIs(frameBatch));
                    mixedFrameIndices.put(kv.getKey(), frameBatch.frames.stream().map(frame -> frame.frameIndex).collect(Collectors.toList()));
                    endTime = System.nanoTime();
                    Log.v(TAG, "RoI extraction time: " + (endTime - startTime) / 1000000.0f);
                }

                startTime = System.nanoTime();
                resizeRoIs(rois);
                rois = rois.stream().sorted((lhs, rhs) -> Integer.compare(rhs.getFrameIndex(), lhs.getFrameIndex())).collect(Collectors.toList());
                Bitmap mixedFrame = PatchMixer.packRoIs(rois);
                endTime = System.nanoTime();
                Log.v(TAG, "RoI packing time: " + (endTime - startTime) / 1000000.0f);

                mInferenceEngine.enqueueRequest(new InferenceRequest(Frame.createMixedFrame(mixedFrame), rois, mixedFrameIndices));
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    public List<RoI> getRoIs(FrameBatch frameBatch) {
        List<RoI> rois = new ArrayList<>();
        Frame prevFrame = frameBatch.prevFrame;
        List<Rect> prevResults = frameBatch.prevResults.stream().map(box -> box.location).collect(Collectors.toList());
        for (Frame currentFrame : frameBatch.frames) {
            Bitmap prevBitmap = prevFrame.bitmap;
            Bitmap currBitmap = currentFrame.bitmap;
            if (prevBitmap.getWidth() != currBitmap.getWidth() || prevBitmap.getHeight() != currBitmap.getHeight()) {
                prevBitmap = Bitmap.createScaledBitmap(prevBitmap, currBitmap.getWidth(), currBitmap.getHeight(), false);
            }
            List<Rect> currentRects = createRoIWithInferenceResult(prevBitmap, currBitmap, prevResults);
            List<RoI> opticalFlowRoIs = currentRects.stream()
                    .map(rect -> new RoI(currentFrame, rect))
                    .collect(Collectors.toList());
            List<RoI> diffRois = createRoIsFromDiff(prevBitmap, currBitmap).stream()
                    .map(rect -> new RoI(currentFrame, rect))
                    .collect(Collectors.toList());
            rois.addAll(opticalFlowRoIs);
            rois.addAll(diffRois);
            prevResults = opticalFlowRoIs.stream().map(roi -> roi.position).collect(Collectors.toList());
            prevFrame = currentFrame;
        }
        return rois;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    public List<Rect> createRoIWithInferenceResult(Bitmap f0, Bitmap f1, List<Rect> boundingBoxes) {
        final int width = f1.getWidth();
        final int height = f1.getHeight();

        List<Rect> shiftedBoxes = new ArrayList<>();
        if (!boundingBoxes.isEmpty()) {
            int[][] shifts = getOpticalFlowForBoundingBoxes(f0, f1, boundingBoxes);
            for (int boxIndex = 0; boxIndex < boundingBoxes.size(); boxIndex++) {
                int[] shift = shifts[boxIndex];
                Rect bbx = boundingBoxes.get(boxIndex);
                int newLeft = bbx.left + shift[0] - ROI_PADDING;
                int newTop = bbx.top + shift[1] - ROI_PADDING;
                int newRight = bbx.right + shift[0] + ROI_PADDING;
                int newBottom = bbx.bottom + shift[1] + ROI_PADDING;
                if (newLeft < 0) {
                    newLeft = 0;
                }
                if (newTop < 0) {
                    newTop = 0;
                }
                if (newRight > width) {
                    newRight = width;
                }
                if (newBottom > height) {
                    newBottom = height;
                }
                shiftedBoxes.add(new Rect(newLeft, newTop, newRight, newBottom));
            }
        }
        return shiftedBoxes;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    public int[][] getOpticalFlowForBoundingBoxes(Bitmap f0, Bitmap f1, List<Rect> boundingBoxes) {
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

    private void resizeRoIs(List<RoI> rois) {
        for (RoI roi : rois) {
            roi.resizeRoI(AREA_THRESHOLD);
        }
    }

    /* Returns the grayscale Mat of a bitmap image. */
    public Mat convertBitmapToGrayMat(Bitmap original) {
        Mat originalMat = new Mat();
        Utils.bitmapToMat(original, originalMat);
        Mat grayMat = new Mat();
        Imgproc.cvtColor(originalMat, grayMat, Imgproc.COLOR_BGR2GRAY);
        return grayMat;
    }

    public List<Rect> createRoIsFromDiff(Bitmap f0, Bitmap f1) {
        Mat img_0_gray = convertBitmapToGrayMat(f0);
        Mat img_1_gray = convertBitmapToGrayMat(f1);

        Mat diff = calculateDiffAndThreshold(img_0_gray, img_1_gray);
        Mat dilated = cannyEdgeDetection(diff);

        ArrayList<MatOfPoint> contours = new ArrayList<>();
        Mat hierarchy = new Mat();
        Imgproc.findContours(dilated, contours, hierarchy, Imgproc.RETR_EXTERNAL, Imgproc.CHAIN_APPROX_SIMPLE);

        return getROIBitmapsFromContours(contours, f1);
    }

    /* Accepts two frames, calculates their difference and returns the threshold Mat. */
    public Mat calculateDiffAndThreshold(Mat frame0, Mat frame1) {
        Mat diff = frame1.clone();
        Core.absdiff(frame0, frame1, diff);
        for (int i = 0; i < 3; i++) {
            Imgproc.dilate(diff, diff, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(3, 3)), new Point(0, 0), i + 1);
        }
        Imgproc.threshold(diff, diff, 30, 255, Imgproc.THRESH_BINARY);
        return diff;
    }

    /* Runs Canny Edge Detection on grayscale difference of two frames. Dilates the result before returning. */
    public Mat cannyEdgeDetection(Mat diff) {
        Imgproc.GaussianBlur(diff, diff, new Size(3, 3), 0);
        Imgproc.Canny(diff, diff, 120, 255, 3, true);
        Imgproc.dilate(diff, diff, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(5, 5)), new Point(0, 0), 1);
        return diff;
    }

    /* Returns a list of Bitmaps of the ROIs. Also draws bounding boxes on the later frame and shows them on ImageView.*/
    public List<Rect> getROIBitmapsFromContours(ArrayList<MatOfPoint> contours, Bitmap laterFrame) {
        //Matrix to draw bounding boxes on.
        Mat laterMat = new Mat();
        Utils.bitmapToMat(laterFrame, laterMat);
        //We need a clean image to crop RoI images from.
        Mat laterMatCopyForROIExtraction = new Mat();
        Utils.bitmapToMat(laterFrame, laterMatCopyForROIExtraction);

        List<Rect> roiList = new ArrayList<>();
        for (MatOfPoint contour : contours) {
            MatOfPoint2f contour2f = new MatOfPoint2f(contour.toArray());
            double approxDistance = Imgproc.arcLength(contour2f, true) * 0.02;
            MatOfPoint2f approxCurve = new MatOfPoint2f();
            Imgproc.approxPolyDP(contour2f, approxCurve, approxDistance, true);
            MatOfPoint points = new MatOfPoint(approxCurve.toArray());

            org.opencv.core.Rect rect = Imgproc.boundingRect(points);
            Rect location = new Rect(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
            if (rect.height * rect.width < 10000) {
                continue;
            }
            Imgproc.rectangle(laterMat, rect.tl(), rect.br(), new Scalar(0, 255, 0), 3);

            Mat croppedROI = new Mat(laterMatCopyForROIExtraction, rect);
            Bitmap croppedROIBitmap = Bitmap.createBitmap(rect.width, rect.height, Bitmap.Config.RGB_565);
            Utils.matToBitmap(croppedROI, croppedROIBitmap);
            roiList.add(location);
        }

        return roiList;
    }
}
