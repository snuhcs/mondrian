package hcs.offloading.edgeserver;

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
import java.util.NoSuchElementException;
import java.util.Queue;
import java.util.stream.Collectors;

import hcs.offloading.edgeserver.config.ExtractionMethod;
import hcs.offloading.edgeserver.config.RoIExtractorConfig;
import hcs.offloading.edgeserver.datatypes.BoundingBox;
import hcs.offloading.edgeserver.datatypes.Frame;
import hcs.offloading.edgeserver.datatypes.InferenceRequest;
import hcs.offloading.edgeserver.datatypes.RoI;

public class RoIExtractor implements Runnable {
    private static final String TAG = RoIExtractor.class.getName();

    private final int BATCH_SIZE;
    private final int MIXED_FRAME_SIZE;
    private final int AREA_THRESHOLD;
    private final int ROI_PADDING;
    private final ExtractionMethod EXTRACTION_METHOD;

    static {
        if (!OpenCVLoader.initDebug()) Log.e("OpenCV", "Unable to load OpenCV!");
        else Log.d("OpenCV", "OpenCV loaded Successfully");
    }

    public interface Callback {
        Pair<Bitmap, List<BoundingBox>> getFrameAndResults(String sourceIP, int frameIndex) throws InterruptedException;
        void enqueueInferenceRequest(InferenceRequest inferenceRequest);
    }

    private final Queue<Frame> mFrames = new LinkedList<>();

    private final Thread mRoIExtractorThread;
    private final Callback mCallback;

    @RequiresApi(api = Build.VERSION_CODES.P)
    RoIExtractor(RoIExtractorConfig config, Callback callback) {
        BATCH_SIZE = config.BATCH_SIZE;
        MIXED_FRAME_SIZE = config.MIXED_FRAME_SIZE;
        AREA_THRESHOLD = config.AREA_THRESHOLD;
        ROI_PADDING = config.ROI_PADDING;
        EXTRACTION_METHOD = config.EXTRACTION_METHOD;

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
        synchronized (mFrames) {
            mFrames.add(frame);
            if (mFrames.size() > BATCH_SIZE) {
                notifyAll();
            }
        }
    }

    private List<Frame> getFrameBatch() throws InterruptedException {
        List<Frame> frames = new ArrayList<>(BATCH_SIZE);
        synchronized (mFrames) {
            while (mFrames.size() < BATCH_SIZE) {
                wait();
            }
            for (int i = 0; i < BATCH_SIZE; i++) {
                frames.add(mFrames.poll());
            }
        }
        return frames;
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    @Override
    public void run() {
        try {
            long startTime, endTime;
            while (true) {
                List<Frame> frames = getFrameBatch();

                startTime = System.nanoTime();
                List<RoI> rois = getRoIsFromMultipleSourceFrames(frames);
                endTime = System.nanoTime();
                Log.v(TAG, "RoI extraction time (us): " + (endTime - startTime) / 1e3);

                List<RoI> resizedRoIs = resize(rois);
                List<RoI> sortedRoIs = sortByPriority(resizedRoIs);

                startTime = System.nanoTime();
                List<RoI> packedRoIs = PatchMixer.packRoIs(sortedRoIs, MIXED_FRAME_SIZE);
                Bitmap mixedFrame = PatchMixer.getMixedFrame(packedRoIs, MIXED_FRAME_SIZE);
                endTime = System.nanoTime();
                Log.v(TAG, "RoI packing time (us): " + (endTime - startTime) / 1e3);

                mCallback.enqueueInferenceRequest(InferenceRequest.createMixedFrameRequest(Frame.createMixedFrame(mixedFrame), frames, rois));
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private List<RoI> getRoIsFromMultipleSourceFrames(List<Frame> frames) throws InterruptedException {
        List<RoI> rois = new ArrayList<>();
        Map<String, List<Frame>> framesPerStream = groupBySourceStream(frames);
        for (Map.Entry<String, List<Frame>> kv : framesPerStream.entrySet()) {
            String sourceIP = kv.getKey();
            List<Frame> sameSourceFrames = kv.getValue();
            int minIndex = sameSourceFrames.stream().map(frame -> frame.frameIndex).min(Integer::compare).orElseThrow(NoSuchElementException::new);
            int prevLastIndex = minIndex - 1;
            Pair<Bitmap, List<BoundingBox>> prevFrameAndResults = mCallback.getFrameAndResults(sourceIP, prevLastIndex);
            if (prevFrameAndResults != null) {
                rois.addAll(getRoIs(sameSourceFrames, prevFrameAndResults.first, prevFrameAndResults.second));
            }
        }
        return rois;
    }

    private static Map<String, List<Frame>> groupBySourceStream(List<Frame> frames) {
        Map<String, List<Frame>> framesPerStream = new HashMap<>();
        for (Frame frame : frames) {
            if (!framesPerStream.containsKey(frame.sourceIP)) {
                framesPerStream.put(frame.sourceIP, new ArrayList<>());
            }
            framesPerStream.get(frame.sourceIP).add(frame);
        }
        return framesPerStream;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private List<RoI> resize(List<RoI> rois) {
        return rois.stream().map(roi -> roi.resize(AREA_THRESHOLD)).collect(Collectors.toList());
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private static List<RoI> sortByPriority(List<RoI> rois) {
        return rois.stream().sorted((lhs, rhs) -> Integer.compare(rhs.getFrameIndex(), lhs.getFrameIndex())).collect(Collectors.toList());
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private List<RoI> getRoIs(List<Frame> frames, Bitmap prevBitmap, List<BoundingBox> prevBoxes) {
        List<RoI> rois = new ArrayList<>();
        List<RoI> opticalFlowRoIs = new ArrayList<>();
        List<RoI> pixelDiffRoIs = new ArrayList<>();
        List<Rect> prevResults = prevBoxes.stream().map(box -> box.location).collect(Collectors.toList());
        // add other methods below if needed
        for (Frame currentFrame : frames) {
            Bitmap currBitmap = currentFrame.bitmap;
            if (prevBitmap.getWidth() != currBitmap.getWidth() || prevBitmap.getHeight() != currBitmap.getHeight()) {
                prevBitmap = Bitmap.createScaledBitmap(prevBitmap, currBitmap.getWidth(), currBitmap.getHeight(), false);
            }
            if (EXTRACTION_METHOD.equals(ExtractionMethod.COMBINED) || EXTRACTION_METHOD.equals(ExtractionMethod.OF)) {
                List<Rect> currentRects = createRoIWithInferenceResult(prevBitmap, currBitmap, prevResults);
                List<RoI> tempRoIs = currentRects.stream()
                        .map(rect -> new RoI(currentFrame, rect))
                        .collect(Collectors.toList());
                opticalFlowRoIs.addAll(tempRoIs);
                prevResults = tempRoIs.stream().map(roi -> roi.position).collect(Collectors.toList());
            }
            if (EXTRACTION_METHOD.equals(ExtractionMethod.COMBINED) || EXTRACTION_METHOD.equals(ExtractionMethod.PD)) {
                List<RoI> tempRoIs = createRoIsFromDiff(prevBitmap, currBitmap).stream()
                        .map(rect -> new RoI(currentFrame, rect))
                        .collect(Collectors.toList());
                pixelDiffRoIs.addAll(tempRoIs);
            }
            prevBitmap = currBitmap;
        }
        rois.addAll(opticalFlowRoIs);
        rois.addAll(pixelDiffRoIs);
        return rois;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    private List<Rect> createRoIWithInferenceResult(Bitmap f0, Bitmap f1, List<Rect> boundingBoxes) {
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

    private static List<Rect> createRoIsFromDiff(Bitmap f0, Bitmap f1) {
        Mat img_0_gray = convertBitmapToGrayMat(f0);
        Mat img_1_gray = convertBitmapToGrayMat(f1);

        Mat diff = calculateDiffAndThreshold(img_0_gray, img_1_gray);
        Mat dilated = cannyEdgeDetection(diff);

        ArrayList<MatOfPoint> contours = new ArrayList<>();
        Mat hierarchy = new Mat();
        Imgproc.findContours(dilated, contours, hierarchy, Imgproc.RETR_EXTERNAL, Imgproc.CHAIN_APPROX_SIMPLE);

        return getRoIBitmapsFromContours(contours, f1);
    }

    /* Returns the grayscale Mat of a bitmap image. */
    private static Mat convertBitmapToGrayMat(Bitmap original) {
        Mat originalMat = new Mat();
        Utils.bitmapToMat(original, originalMat);
        Mat grayMat = new Mat();
        Imgproc.cvtColor(originalMat, grayMat, Imgproc.COLOR_BGR2GRAY);
        return grayMat;
    }

    /* Accepts two frames, calculates their difference and returns the threshold Mat. */
    private static Mat calculateDiffAndThreshold(Mat frame0, Mat frame1) {
        Mat diff = frame1.clone();
        Core.absdiff(frame0, frame1, diff);
        for (int i = 0; i < 3; i++) {
            Imgproc.dilate(diff, diff, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(3, 3)), new Point(0, 0), i + 1);
        }
        Imgproc.threshold(diff, diff, 30, 255, Imgproc.THRESH_BINARY);
        return diff;
    }

    /* Runs Canny Edge Detection on grayscale difference of two frames. Dilates the result before returning. */
    private static Mat cannyEdgeDetection(Mat diff) {
        Imgproc.GaussianBlur(diff, diff, new Size(3, 3), 0);
        Imgproc.Canny(diff, diff, 120, 255, 3, true);
        Imgproc.dilate(diff, diff, Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(5, 5)), new Point(0, 0), 1);
        return diff;
    }

    /* Returns a list of Bitmaps of the ROIs. Also draws bounding boxes on the later frame and shows them on ImageView.*/
    private static List<Rect> getRoIBitmapsFromContours(ArrayList<MatOfPoint> contours, Bitmap laterFrame) {
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
