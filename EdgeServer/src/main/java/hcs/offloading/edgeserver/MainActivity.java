package hcs.offloading.edgeserver;

import android.os.Build;
import android.os.Bundle;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Switch;

import org.webrtc.EglBase;
import org.webrtc.SurfaceViewRenderer;

import hcs.offloading.edgeserver.databinding.ActivityMainBinding;

@RequiresApi(api = Build.VERSION_CODES.P)
public class MainActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener {
    private EditText mIpInput;
    private EditText mPortInput;
    private SurfaceViewRenderer mInputView;

    private EglBase mEglBase = EglBase.create();
    private EdgeServer mEdgeServer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(ActivityMainBinding.inflate(getLayoutInflater()).getRoot());
        mIpInput = findViewById(R.id.ipInput);
        mPortInput = findViewById(R.id.portInput);
        mInputView = findViewById(R.id.inputView);
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
                mEdgeServer = new EdgeServer(getApplicationContext(), mEglBase, uri,
                        mInputView,
                        new ImageView[]{findViewById(R.id.outputView0), findViewById(R.id.outputView1)},
                        findViewById(R.id.inferenceOutputView),
                        findViewById(R.id.FPS));
            } else {
                mEdgeServer.close();
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        synchronized (this) {
            mEdgeServer.close();
        }
    }
}
