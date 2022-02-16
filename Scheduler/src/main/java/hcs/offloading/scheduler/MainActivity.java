package hcs.offloading.scheduler;

import android.os.Build;
import android.os.Bundle;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;

import hcs.offloading.scheduler.databinding.ActivityMainBinding;

@RequiresApi(api = Build.VERSION_CODES.N)
public class MainActivity extends AppCompatActivity implements CompoundButton.OnCheckedChangeListener {
    private EditText mIpInput;
    private EditText mPortInput;

    private Scheduler mScheduler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(ActivityMainBinding.inflate(getLayoutInflater()).getRoot());
        mIpInput = findViewById(R.id.ipInput);
        mPortInput = findViewById(R.id.portInput);

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
                mScheduler = new Scheduler(getApplicationContext(), uri,
                        findViewById(R.id.sensorIpList), findViewById(R.id.edgeIpList));
            } else {
                mScheduler.close();
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        synchronized (this) {
            mScheduler.close();
        }
    }
}
