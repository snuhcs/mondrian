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

import java.io.IOException;

import hcs.offloading.edgedevice.config.Config;
import hcs.offloading.edgedevice.databinding.ActivityMainBinding;

@RequiresApi(api = Build.VERSION_CODES.P)
public class MainActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener, ViewCallback {
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
                    mInputView);
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
