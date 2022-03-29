package hcs.offloading.strm.datatypes;

import android.graphics.Bitmap;

import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;

public class Frame {
    public final Bitmap bitmap;
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
    private List<BoundingBox> boxes;

    public Frame(Bitmap bitmap, String key, int frameIndex) {
        this.bitmap = bitmap;
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

    public void setResults(List<BoundingBox> boxes) {
        synchronized (this) {
            this.boxes = boxes;
            notifyAll();
        }
    }

    public List<BoundingBox> getResults() {
        return boxes;
    }

    public void waitForResults() throws InterruptedException {
        synchronized (this) {
            while (boxes == null) {
                wait();
            }
        }
    }
}
