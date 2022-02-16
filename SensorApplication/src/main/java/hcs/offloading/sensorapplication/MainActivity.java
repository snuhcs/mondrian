package hcs.offloading.sensorapplication;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;

import org.webrtc.EglBase;
import org.webrtc.SurfaceViewRenderer;

import hcs.offloading.sensorapplication.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener {
    private EditText mIpInput;
    private EditText mPortInput;
    private SurfaceViewRenderer mInputView;

    private final EglBase mEglBase = EglBase.create();
    private SensorApplication mSensorApplication;

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

    @Override
    public void onCheckedChanged(CompoundButton button, boolean isChecked) {
        synchronized (this) {
            if (isChecked) {
                String ip = mIpInput.getText().toString().trim();
                int port = Integer.parseInt(mPortInput.getText().toString());
                String uri = "tcp://" + ip + ":" + port;
                mSensorApplication = new SensorApplication(getApplicationContext(), mEglBase, uri, mInputView);
            } else{
                mSensorApplication.close();
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        synchronized (this) {
            mSensorApplication.close();
        }
    }
}
