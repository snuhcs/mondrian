package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.graphics.Rect;
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
import java.util.List;
import java.util.stream.Collectors;

import hcs.offloading.strm.config.RoIExtractorConfig;
import hcs.offloading.strm.datatypes.BoundingBox;
import hcs.offloading.strm.datatypes.Frame;
import hcs.offloading.strm.datatypes.RoI;

public class RoIExtractor {
    private static final String TAG = RoIExtractor.class.getName();

    static {
        if (!OpenCVLoader.initDebug()) Log.e("OpenCV", "Unable to load OpenCV!");
        else Log.d("OpenCV", "OpenCV loaded Successfully");
    }

    private final RoIExtractorConfig mConfig;
    private final Size mTargetSize;

    public RoIExtractor(RoIExtractorConfig config) {
        mConfig = config;
        mTargetSize = new Size(mConfig.EXTRACTION_RESIZE_WIDTH, mConfig.EXTRACTION_RESIZE_HEIGHT);
    }

    public boolean useOpticalFlowRoIs() {
        return mConfig.OF_ROI;
    }

    public void process(Pair<Pair<Frame, Frame>, List<BoundingBox>> item) {
        Frame prevFrame = item.first.first;
        Frame currFrame = item.first.second;

        List<RoI> rois = new ArrayList<>();
        if (mConfig.OF_ROI) {
            List<BoundingBox> prevResults = item.second.stream()
                    .filter(box -> box.confidence > mConfig.OPTICAL_FLOW_ROI_CONFIDENCE_THRESHOLD)
                    .collect(Collectors.toList());
            List<RoI> opticalFlowRoIs = getOpticalFlowRoIs(prevFrame, currFrame, prevResults, mTargetSize);
            currFrame.setOpticalFlowRoIs(opticalFlowRoIs);
            rois.addAll(opticalFlowRoIs);
        }
        if (mConfig.PD_ROI) {
            rois.addAll(getPixelDiffRoIs(prevFrame, currFrame, mTargetSize));
        }
        if (mConfig.MERGE_ROI) {
            rois = mergeSingleFrameRoIs(currFrame, rois, mConfig.MERGE_THRESHOLD);
        }
        currFrame.setRoIs(rois);
    }

    private static List<RoI> mergeSingleFrameRoIs(Frame frame, List<RoI> rois, float mergeThreshold) {
        List<RoI> mergedRoIs = new ArrayList<>(rois);
        while (true) {
            RoI originalRoI0 = null;
            RoI originalRoI1 = null;
            RoI mergedRoI = null;
            for (int i = 0; i < mergedRoIs.size(); i++) {
                for (int j = i + 1; j < mergedRoIs.size(); j++) {
                    RoI roi0 = mergedRoIs.get(i);
                    RoI roi1 = mergedRoIs.get(j);
                    float intersection = STRMUtils.box_intersection(roi0.location, roi1.location);
                    if (intersection / roi0.getArea() < mergeThreshold
                            && intersection / roi1.getArea() < mergeThreshold) {
                        continue;
                    }
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
                    RoI.Type roiType = roi0.type.equals(RoI.Type.OF) || roi1.type.equals(RoI.Type.OF)
                            ? RoI.Type.OF
                            : RoI.Type.PD;
                    String roiLabel = roi0.labelName == null || roi1.labelName == null
                            || !roi0.labelName.equals(roi1.labelName)
                            ? null : roi0.labelName;
                    mergedRoI = new RoI(frame, newLocation, roiType, roiLabel,
                            Math.min(Math.max(roi0.location.width(), roi0.location.height()),
                                    Math.max(roi1.location.width(), roi1.location.height())));
                    break;
                }
                if (mergedRoI != null) {
                    break;
                }
            }
            if (mergedRoI == null) {
                break;
            }
            mergedRoIs.remove(originalRoI0);
            mergedRoIs.remove(originalRoI1);
            mergedRoIs.add(mergedRoI);
        }
        return mergedRoIs;
    }

    private static List<RoI> getOpticalFlowRoIs(
            Frame prevFrame, Frame currFrame, List<BoundingBox> boundingBoxes, Size targetSize) {
        final int width = currFrame.bitmap.getWidth();
        final int height = currFrame.bitmap.getHeight();

        List<Rect> boundingRects = boundingBoxes.stream()
                .map(bbx -> bbx.location)
                .collect(Collectors.toList());

        List<RoI> opticalFlowRoIs = new ArrayList<>();
        if (!boundingBoxes.isEmpty()) {
            int[][] shifts = getBoundingBoxShifts(
                    prevFrame.bitmap, currFrame.bitmap, boundingRects, targetSize);
            for (int boxIndex = 0; boxIndex < boundingBoxes.size(); boxIndex++) {
                int[] shift = shifts[boxIndex];
                BoundingBox box = boundingBoxes.get(boxIndex);
                Rect loc = box.location;
                int newLeft = Math.max(0, loc.left + shift[0]);
                int newTop = Math.max(0, loc.top + shift[1]);
                int newRight = Math.min(width, loc.right + shift[0]);
                int newBottom = Math.min(height, loc.bottom + shift[1]);
                if (newLeft < newRight && newTop < newBottom) {
                    opticalFlowRoIs.add(new RoI(
                            currFrame, new Rect(newLeft, newTop, newRight, newBottom),
                            RoI.Type.OF, box.labelName));
                }
            }
        }
        return opticalFlowRoIs;
    }

    private static int[][] getBoundingBoxShifts(
            Bitmap prevImage, Bitmap currImage, List<Rect> boundingBoxes, Size targetSize) {
        Mat prevMat = new Mat();
        Mat currMat = new Mat();
        MatOfPoint2f p0 = new MatOfPoint2f();
        MatOfPoint2f p1 = new MatOfPoint2f();
        MatOfByte status = new MatOfByte();
        MatOfFloat err = new MatOfFloat();

        Utils.bitmapToMat(prevImage, prevMat);
        Utils.bitmapToMat(currImage, currMat);

        Imgproc.cvtColor(prevMat, prevMat, Imgproc.COLOR_BGR2GRAY);
        Imgproc.cvtColor(currMat, currMat, Imgproc.COLOR_BGR2GRAY);

        Imgproc.resize(prevMat, prevMat, targetSize);
        Imgproc.resize(currMat, currMat, targetSize);

        List<Point> centroids = boundingBoxes.stream()
                .map(bbx -> new Point(
                        (float) bbx.centerX() * targetSize.width / currImage.getWidth(),
                        (float) bbx.centerY() * targetSize.height / currImage.getHeight()))
                .collect(Collectors.toList());
        p0.fromList(centroids);
        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT + TermCriteria.EPS, 10, 0.03);
        Video.calcOpticalFlowPyrLK(prevMat, currMat, p0, p1, status, err, new Size(15, 15), 2, criteria);

        byte[] StatusArr = status.toArray();
        Point[] p1Arr = p1.toArray();

        int[][] shifts = new int[centroids.size()][2];
        for (int pointIdx = 0; pointIdx < centroids.size(); pointIdx++) {
            if (StatusArr[pointIdx] == 1) {
                shifts[pointIdx][0] = (int) ((p1Arr[pointIdx].x - centroids.get(pointIdx).x)
                        * currImage.getWidth() / targetSize.width);
                shifts[pointIdx][1] = (int) ((p1Arr[pointIdx].y - centroids.get(pointIdx).y)
                        * currImage.getHeight() / targetSize.height);
            } else {
                shifts[pointIdx][0] = 0;
                shifts[pointIdx][1] = 0;
            }
        }

        prevMat.release();
        currMat.release();
        p0.release();
        p1.release();
        status.release();
        err.release();
        return shifts;
    }

    private static List<RoI> getPixelDiffRoIs(Frame prevFrame, Frame currFrame, Size targetSize) {
        Mat prevMat = new Mat();
        Mat currMat = new Mat();

        Utils.bitmapToMat(prevFrame.bitmap, prevMat);
        Utils.bitmapToMat(currFrame.bitmap, currMat);

        Imgproc.cvtColor(prevMat, prevMat, Imgproc.COLOR_BGR2GRAY);
        Imgproc.cvtColor(currMat, currMat, Imgproc.COLOR_BGR2GRAY);

        Imgproc.resize(prevMat, prevMat, targetSize);
        Imgproc.resize(currMat, currMat, targetSize);

        Mat mat = new Mat();
        calculateDiffAndThreshold(prevMat, currMat, mat);
        cannyEdgeDetection(mat);

        ArrayList<MatOfPoint> contours = new ArrayList<>();
        Mat hierarchy = new Mat();
        Imgproc.findContours(mat, contours, hierarchy, Imgproc.RETR_EXTERNAL, Imgproc.CHAIN_APPROX_SIMPLE);
        List<Rect> locations = getBoxesFromContours(contours,
                currFrame.bitmap.getWidth(), currFrame.bitmap.getHeight(),
                (int) targetSize.width, (int) targetSize.height);
        List<RoI> rois = locations.stream()
                .map(location -> new RoI(currFrame, location, RoI.Type.PD, null))
                .collect(Collectors.toList());

        prevMat.release();
        currMat.release();
        hierarchy.release();
        mat.release();
        for (MatOfPoint contour : contours) {
            contour.release();
        }
        return rois;
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

    private static List<Rect> getBoxesFromContours(
            List<MatOfPoint> contours, int originalWidth, int originalHeight, int resizedWidth, int resizedHeight) {
        List<Rect> boxes = new ArrayList<>();
        for (MatOfPoint contour : contours) {
            MatOfPoint2f contour2f = new MatOfPoint2f(contour.toArray());
            MatOfPoint2f approxCurve = new MatOfPoint2f();
            double approxDistance = Imgproc.arcLength(contour2f, true) * 0.02;
            Imgproc.approxPolyDP(contour2f, approxCurve, approxDistance, true);
            MatOfPoint points = new MatOfPoint(approxCurve.toArray());
            org.opencv.core.Rect rect = Imgproc.boundingRect(points);
            if (rect.width * rect.height
                    < 10000 * (resizedWidth * resizedHeight) / (originalWidth * originalHeight)) {
                continue;
            }
            Rect location = new Rect(
                    rect.x * originalWidth / resizedWidth,
                    rect.y * originalHeight / resizedHeight,
                    rect.x + rect.width * originalWidth / resizedWidth,
                    rect.y + rect.height * originalHeight / resizedHeight
            );
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
