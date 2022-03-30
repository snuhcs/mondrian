package hcs.offloading.strm.datatypes;

import org.opencv.core.Mat;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;

public class Frame {
    public final Mat mat;
    public final String key;
    public final int frameIndex;

    /**
     * All RoIs / Optical Flow RoIs of the frame.
     * For all types of inference, final results will be saved.
     */
    private List<RoI> rois;
    private List<RoI> opticalFlowRoIs;

    /**
     * Results of the frame.
     * For all types of inference, final results will be saved.
     */
    private final List<BoundingBox> boxes = new ArrayList<>();

    public Frame(Mat mat, String key, int frameIndex) {
        this.mat = mat;
        this.key = key;
        this.frameIndex = frameIndex;
    }

    /**
     * Sort RoIs with given comparator
     *
     * @param roiPrioritizer Set priority of each roi.
     */
    public void sortRoIs(Comparator<RoI> roiPrioritizer) {
        rois = rois.stream().sorted(roiPrioritizer).collect(Collectors.toList());
    }

    public void setRoIs(List<RoI> rois) {
        this.rois = rois;
    }

    public List<RoI> getRoIs() {
        return rois;
    }

    public void setOpticalFlowRoIs(List<RoI> opticalFlowRoIs) {
        this.opticalFlowRoIs = opticalFlowRoIs;
    }

    public List<RoI> getOpticalFlowRoIs() {
        return opticalFlowRoIs;
    }

    public void addResult(BoundingBox box) {
        boxes.add(box);
    }

    public void addResults(List<BoundingBox> boxes) {
        this.boxes.addAll(boxes);
    }

    public List<BoundingBox> getResults() {
        return boxes;
    }
}
