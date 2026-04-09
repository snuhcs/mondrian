package hcs.offloading.mondrian;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.support.annotation.NonNull;
import android.util.Log;
import android.util.Pair;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.ImageView;
import android.widget.TextView;

import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

public class MondrianUI implements SurfaceHolder.Callback {
    private static final String TAG = MondrianUI.class.getName();

    private boolean isSurfaceSet = false;
    private final Size inputSize = new Size(640, 360);
    private final List<SurfaceView> inputViews;
    private final List<Mat> inputRgbMats = new ArrayList<>();
    private final List<Mat> inputResizedRgbMats = new ArrayList<>();
    private final List<byte[]> inputBuffers = new ArrayList<>();
    private final ImageView outputView;

    private final List<Pair<Integer, Long>> countTimes = new LinkedList<>();
    private final AtomicInteger frameCount = new AtomicInteger(0);
    private final TextView fpsView;
    private final TextView frameCountView;

    MondrianUI(List<SurfaceView> inputViews,
               ImageView outputView,
               TextView fpsView,
               TextView frameCountView) {
        this.inputViews = inputViews;
        this.outputView = outputView;
        this.fpsView = fpsView;
        this.frameCountView = frameCountView;
        for (SurfaceView inputView : inputViews) {
            inputView.getHolder().addCallback(this);
        }
        for (int i = 0; i < inputViews.size(); i++) {
            inputRgbMats.add(new Mat());
            inputResizedRgbMats.add(new Mat());
            inputBuffers.add(new byte[(int) (4 * inputSize.width * inputSize.height)]);
        }
    }

    @SuppressLint("DefaultLocale")
    public void onFrame(int vid, Mat yuvMat) {
        int count = frameCount.incrementAndGet();
        frameCountView.post(() -> frameCountView.setText(String.format("%04d", count)));
        if (count % 10 == 0) {
            updateFPS(count);
        }
        if (isSurfaceSet && vid < inputViews.size()) {
            Mat inputRgbMat = inputRgbMats.get(vid);
            Mat inputResizedRgbMat = inputResizedRgbMats.get(vid);
            byte[] inputBuffer = inputBuffers.get(vid);
            SurfaceView inputView = inputViews.get(vid);
            Imgproc.cvtColor(yuvMat, inputRgbMat, Imgproc.COLOR_YUV2RGBA_I420);
            Imgproc.resize(inputRgbMat, inputResizedRgbMat, inputSize, 0, 0, Imgproc.INTER_LINEAR);
            inputResizedRgbMat.get(0, 0, inputBuffer);
            JniRenderer.draw(inputView.getHolder(), inputBuffer, (int) inputSize.width, (int) inputSize.height);
        }
    }

    @SuppressLint("DefaultLocale")
    private void updateFPS(int frameCount) {
        long currTime = System.nanoTime();
        countTimes.add(new Pair<>(frameCount, currTime));
        if (countTimes.size() == 1) return;
        Pair<Integer, Long> first = countTimes.get(0);
        Pair<Integer, Long> last = countTimes.get(countTimes.size() - 1);
        double fps = 1e9 * (last.first - first.first) / (last.second - first.second);
        fpsView.post(() -> fpsView.setText(String.format("%02.1f", fps)));
        if (countTimes.size() > 50) {
            countTimes.remove(0);
        }
    }

    public void drawOutput(long rgbMatAddr, List<BoundingBox> results, long device) {
        Mat rgbMat = new Mat(rgbMatAddr);
        if (device == 1) {
            Imgproc.putText(rgbMat, "GPU", new Point(10, 80), 0, 3, new Scalar(0, 0, 255), 8);
        } else if (device == 2) {
            Imgproc.putText(rgbMat, "DSP", new Point(10, 80), 0, 3, new Scalar(255, 0, 0), 8);
        } else {
            assert (false);
        }
        Bitmap bitmap = Bitmap.createBitmap(rgbMat.cols(), rgbMat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(rgbMat, bitmap);
        Bitmap outputBitmap = ImageUtils.drawBoxes(bitmap, results);
        outputView.post(() -> {
            outputView.setImageBitmap(outputBitmap);
            rgbMat.release();
        });
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.d(TAG, "surface view created");
        for (SurfaceView inputView : inputViews) {
            inputView.getHolder().setFixedSize((int) inputSize.width, (int) inputSize.height);
        }
        isSurfaceSet = true;
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
    }
}
