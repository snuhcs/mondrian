package hcs.offloading.edgedevice;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Switch;

import org.json.JSONException;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.webrtc.SurfaceViewRenderer;

import java.io.IOException;
import java.util.List;

import hcs.offloading.edgedevice.config.Config;
import hcs.offloading.edgedevice.databinding.ActivityMainBinding;
import hcs.offloading.strmcpp.BoundingBox;
import hcs.offloading.strmcpp.InferenceViewCallback;

public class MainActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener, InferenceViewCallback {
    private static final String TAG = MainActivity.class.getName();

    private static final String CONFIG_FILEPATH = "/data/local/tmp/strmcpp.json";

    private EditText mIpInput;
    private EditText mPortInput;
    private SurfaceViewRenderer mInputView;
    private ImageView mOutputView;
    private ImageView mInferenceOutputView;

    private Config mConfig;
    private EdgeDevice mEdgeDevice;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(ActivityMainBinding.inflate(getLayoutInflater()).getRoot());
        mIpInput = findViewById(R.id.ipInput);
        mPortInput = findViewById(R.id.portInput);
        mInputView = findViewById(R.id.inputView);
        mInputView.setMirror(true);
        mOutputView = findViewById(R.id.outputView);
        mInferenceOutputView = findViewById(R.id.inferenceOutputView);

        try {
            mConfig = new Config(CONFIG_FILEPATH);

            Switch connectButton = findViewById(R.id.connectButton);

            if (!mConfig.USE_LOCAL_VIDEO) {
                connectButton.setOnCheckedChangeListener(this);
            } else {
                connectButton.setVisibility(View.INVISIBLE);
                startEdgeDevice();
            }
        } catch (IOException | JSONException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton button, boolean isChecked) {
        synchronized (this) {
            if (isChecked) {
                startEdgeDevice();
            } else {
                if (mEdgeDevice != null) {
                    mEdgeDevice.close();
                }
            }
        }
    }

    private void startEdgeDevice() {
        String ip = mIpInput.getText().toString().trim();
        int port = Integer.parseInt(mPortInput.getText().toString());
        String uri = "tcp://" + ip + ":" + port;
        try {
            mEdgeDevice = new EdgeDevice(mConfig, getApplicationContext(), uri,
                    mInputView, this);
        } catch (IllegalArgumentException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
            mEdgeDevice = null;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        synchronized (this) {
            if (mEdgeDevice != null) {
                mEdgeDevice.close();
            }
        }
    }

    @Override
    public void drawInferenceResult(long addr, List<BoundingBox> results) {
        Mat mat = new Mat(addr);
        Bitmap bitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bitmap);
        mInferenceOutputView.post(() -> mInferenceOutputView.setImageBitmap(
                DrawUtil.drawBoxes(bitmap, results, mConfig.DRAW_CONFIDENCE, false /* must be always false */)));
    }

    @Override
    public void drawObjectDetectionResult(long addr, List<BoundingBox> results) {
        Mat mat = new Mat(addr);
        Bitmap bitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bitmap);
        mOutputView.post(() -> mOutputView.setImageBitmap(
                DrawUtil.drawBoxes(bitmap, results, mConfig.DRAW_CONFIDENCE, true)));
    }
}
