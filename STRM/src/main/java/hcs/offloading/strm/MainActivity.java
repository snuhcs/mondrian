package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import org.json.JSONException;
import org.opencv.android.Utils;
import org.opencv.core.Mat;

import java.io.IOException;
import java.util.List;

import hcs.offloading.strm.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements InferenceViewCallback {
    private ImageView mOutputView;
    private ImageView mInferenceOutputView;

    private Emulator mEmulator;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(ActivityMainBinding.inflate(getLayoutInflater()).getRoot());

        mOutputView = findViewById(hcs.offloading.strm.R.id.outputView);
        mInferenceOutputView = findViewById(hcs.offloading.strm.R.id.inferenceOutputView);

        try {
            mEmulator = new Emulator(this);
        } catch (JSONException | IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mEmulator.close();
    }

    @Override
    public void drawInferenceResult(long addr, List<BoundingBox> results) {
        Mat mat = new Mat(addr);
        Bitmap bitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bitmap);
        mInferenceOutputView.post(() -> mInferenceOutputView.setImageBitmap(DrawUtil.drawBoxes(bitmap, results, false /* must be always false */)));
    }

    @Override
    public void drawObjectDetectionResult(long addr, List<BoundingBox> results) {
        Mat mat = new Mat(addr);
        Bitmap bitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bitmap);
        mOutputView.post(() -> mOutputView.setImageBitmap(DrawUtil.drawBoxes(bitmap, results, true)));
    }
}
