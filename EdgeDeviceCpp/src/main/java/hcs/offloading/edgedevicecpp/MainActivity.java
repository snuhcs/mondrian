package hcs.offloading.edgedevicecpp;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;

import org.json.JSONException;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.webrtc.SurfaceViewRenderer;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.stream.Collectors;

import hcs.offloading.edgedevicecpp.config.Config;
import hcs.offloading.edgedevicecpp.databinding.ActivityMainBinding;
import hcs.offloading.strmcpp.BoundingBox;
import hcs.offloading.strmcpp.InferenceViewCallback;

public class MainActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener, ResultCallback, InferenceViewCallback {
    private static final String TAG = MainActivity.class.getName();

    private static final String CONFIG_FILEPATH = "/data/local/tmp/edgedevicecpp.json";

    private EditText mIpInput;
    private EditText mPortInput;
    private SurfaceViewRenderer mInputView;
    private ImageView mOutputView;
    private ImageView mInferenceOutputView;
    private TextView mFpsView;

    private Config mConfig;
    private EdgeDevice mEdgeDevice;
    private FileWriter mLogWriter;
    private final long mStartTime = System.nanoTime();

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
        mFpsView = findViewById(R.id.FPS);

        try {
            mConfig = new Config(CONFIG_FILEPATH);

            if (mConfig.LOG_PATH != null) {
                File file = new File(mConfig.LOG_PATH);
                if (file.exists()) {
                    if (file.delete()) {
                        Log.d(TAG, mConfig.LOG_PATH + " deleted");
                    } else {
                        Log.e(TAG, mConfig.LOG_PATH + " deletion failed");
                    }
                }
                mLogWriter = new FileWriter(mConfig.LOG_PATH);
            }

            Switch connectButton = findViewById(R.id.connectButton);

            if (!mConfig.sourceConfig.USE_LOCAL_VIDEO) {
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
                    mInputView, this, this);
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
        if (mLogWriter != null) {
            try {
                mLogWriter.flush();
                mLogWriter.close();
            } catch (IOException e) {
                Log.e(TAG, e.getMessage());
            }
        }
    }

    @Override
    public void log(String key, int frameIndex, List<BoundingBox> results) {
        if (mLogWriter == null) {
            return;
        }
        long timeStamp = System.nanoTime() - mStartTime;
        try {
            mLogWriter.write(key + "," + frameIndex + "," + timeStamp + "," + results.stream()
                    .map(box -> box.left + "," + box.top + "," + box.right + "," + box.bottom + "," + box.confidence + "," + box.labelName)
                    .collect(Collectors.joining(",")) + "\n");
            mLogWriter.flush();
        } catch (IOException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    @Override
    public void drawInferenceResult(long addr, List<BoundingBox> results) {
        Mat mat = new Mat(addr);
        Bitmap bitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bitmap);
        mInferenceOutputView.post(() -> mInferenceOutputView.setImageBitmap(
                DrawUtil.drawBoxes(bitmap, results, 0f)));
    }

    @Override
    public void drawObjectDetectionResult(Bitmap bitmap) {
        mOutputView.post(() -> mOutputView.setImageBitmap(bitmap));
    }

    @Override
    public void drawFPS(String fpsStr) {
        mFpsView.post(() -> mFpsView.setText(fpsStr));
    }
}
