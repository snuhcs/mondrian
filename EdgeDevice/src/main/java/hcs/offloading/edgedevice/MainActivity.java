package hcs.offloading.edgedevice;

import android.graphics.Bitmap;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;

import org.json.JSONException;
import org.webrtc.SurfaceViewRenderer;

import java.io.FileWriter;
import java.io.IOException;
import java.util.stream.Collectors;

import hcs.offloading.edgedevice.config.Config;
import hcs.offloading.edgedevice.databinding.ActivityMainBinding;
import hcs.offloading.strm.datatypes.BoundingBox;
import hcs.offloading.strm.datatypes.Frame;

@RequiresApi(api = Build.VERSION_CODES.P)
public class MainActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener, ResultCallback {
    private static final String TAG = MainActivity.class.getName();

    private static final String CONFIG_FILEPATH = "/data/local/tmp/edgedevice.json";

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
            mEdgeDevice = new EdgeDevice(
                    mConfig,
                    getApplicationContext(),
                    uri,
                    mInputView,
                    this);
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
    public void log(Frame frame) {
        if (mLogWriter == null) {
            return;
        }
        long timeStamp = System.nanoTime() - mStartTime;
        try {
            mLogWriter.write(frame.key + "," + frame.frameIndex + "," + timeStamp + "," + frame.getResults().stream()
                    .map(box -> box.location.left + "," + box.location.top + "," + box.location.right + "," + box.location.bottom + "," + box.confidence + "," + box.labelName)
                    .collect(Collectors.joining(",")) + "\n");
            mLogWriter.flush();
        } catch (IOException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    @Override
    public void drawInferenceResult(Bitmap bitmap) {
        mInferenceOutputView.post(() -> mInferenceOutputView.setImageBitmap(bitmap));
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
