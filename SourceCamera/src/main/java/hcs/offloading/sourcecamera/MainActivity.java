package hcs.offloading.sourcecamera;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;

import org.json.JSONException;
import org.webrtc.EglBase;
import org.webrtc.SurfaceViewRenderer;

import java.io.IOException;

import hcs.offloading.sourcecamera.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener {
    private static final String TAG = MainActivity.class.getName();

    private static final String CONFIG_FILEPATH = "/data/local/tmp/sourcecamera.json";

    private EditText mIpInput;
    private EditText mPortInput;
    private SurfaceViewRenderer mInputView;

    private final EglBase mEglBase = EglBase.create();
    private SourceCamera mSourceCamera;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, 1);
        }

        setContentView(ActivityMainBinding.inflate(getLayoutInflater()).getRoot());
        mIpInput = findViewById(R.id.ipInput);
        mPortInput = findViewById(R.id.portInput);
        mInputView = findViewById(R.id.surfaceView);
        mInputView.setMirror(true);
        mInputView.init(mEglBase.getEglBaseContext(), null);

        Switch connectButton = findViewById(R.id.connectButton);
        connectButton.setOnCheckedChangeListener(this);
    }

    @RequiresApi(api = Build.VERSION_CODES.P)
    @Override
    public void onCheckedChanged(CompoundButton button, boolean isChecked) {
        synchronized (this) {
            if (isChecked) {
                String ip = mIpInput.getText().toString().trim();
                int port = Integer.parseInt(mPortInput.getText().toString());
                String uri = "tcp://" + ip + ":" + port;
                try {
                    mSourceCamera = new SourceCamera(
                            new Config(CONFIG_FILEPATH),
                            getApplicationContext(),
                            mEglBase,
                            uri,
                            mInputView
                    );
                } catch (IOException | JSONException e) {
                    Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
                    mSourceCamera = null;
                }
            } else {
                if (mSourceCamera != null) {
                    mSourceCamera.close();
                    mSourceCamera = null;
                }
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        synchronized (this) {
            if (mSourceCamera != null) {
                mSourceCamera.close();
                mSourceCamera = null;
            }
        }
    }
}
