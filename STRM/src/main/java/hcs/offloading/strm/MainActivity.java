package hcs.offloading.strm;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import org.json.JSONException;
import org.opencv.android.Utils;
import org.opencv.core.Mat;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

import hcs.offloading.strm.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements InferenceViewCallback {
    private ImageView mOutputView0;
    private ImageView mOutputView1;

    private Emulator mEmulator;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(ActivityMainBinding.inflate(getLayoutInflater()).getRoot());

        mOutputView0 = findViewById(R.id.outputView0);
        mOutputView1 = findViewById(R.id.outputView1);

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
    public void drawOutput0(long addr, List<BoundingBox> results) {
        Mat mat = new Mat(addr);
        Bitmap bitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bitmap);
        Bitmap outputBitmap = DrawUtil.drawBoxes(bitmap, results, false);
        mOutputView0.post(() -> mOutputView0.setImageBitmap(outputBitmap));
    }

    @Override
    public void drawOutput1(long addr, List<BoundingBox> results) {
        Mat mat = new Mat(addr);
        Bitmap bitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bitmap);
        Bitmap outputBitmap = DrawUtil.drawBoxes(bitmap, results, false);
        mOutputView1.post(() -> mOutputView1.setImageBitmap(outputBitmap));
    }

    private void saveBitmap(Bitmap bmp, String filepath) {
        try (FileOutputStream out = new FileOutputStream(filepath)) {
            bmp.compress(Bitmap.CompressFormat.PNG, 100, out);
            // PNG is a lossless format, the compression factor (100) is ignored
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
