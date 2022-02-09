package hcs.offloading.scheduler;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Switch;

import androidx.appcompat.app.AppCompatActivity;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.concurrent.atomic.AtomicBoolean;

import hcs.offloading.network.Header;
import hcs.offloading.network.Message;
import hcs.offloading.network.NetworkManager;
import hcs.offloading.network.Packet;
import hcs.offloading.network.PacketHandler;
import hcs.offloading.network.Topic;
import hcs.offloading.scheduler.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements Runnable {
    private static final String TAG = MainActivity.class.getName();
    // Configuration
    private static final int SCHEDULE_INTERVAL_MS = 5000; // 5s
    private final Map<String, RadioButton> mSensorIPtoButton = new TreeMap<>();
    private final Map<String, RadioButton> mEdgeIPtoButton = new TreeMap<>();
    private final AtomicBoolean mbRunScheduler = new AtomicBoolean(false);
    // UI Elements
    private ActivityMainBinding mBinding;
    private EditText mIpInput;
    private EditText mPortInput;
    private Switch mConnectButton;
    private RadioGroup mSensorIPList;
    private RadioGroup mEdgeIPList;
    private final PacketHandler registerTopicHandler = packet -> {
        if (packet.header.equals(Header.REGISTER)) {
            String ip = packet.srcIp;
            String device = packet.message;
            if (device.equals(Message.DEVICE_SENSOR)) {
                synchronized (mSensorIPtoButton) {
                    if (!mSensorIPtoButton.containsKey(ip)) {
                        RadioButton button = new RadioButton(this);
                        button.setId(View.generateViewId());
                        button.setText(ip);
                        mSensorIPtoButton.put(ip, button);
                        mSensorIPList.post(() -> mSensorIPList.addView(button));
                    }
                }
                Log.d(TAG, "Sensor registered : " + ip);
            } else if (device.equals(Message.DEVICE_EDGE)) {
                synchronized (mEdgeIPtoButton) {
                    if (!mEdgeIPtoButton.containsKey(ip)) {
                        RadioButton button = new RadioButton(this);
                        button.setId(View.generateViewId());
                        button.setText(ip);
                        mEdgeIPtoButton.put(ip, button);
                        mEdgeIPList.post(() -> mEdgeIPList.addView(button));
                    }
                }
                Log.d(TAG, "Edge registered : " + ip);
            } else {
                Log.e(TAG, "Wrong Device Registration : " + device);
            }
        }
    };
    private final PacketHandler unregisterTopicHandler = packet -> {
        if (packet.header.equals(Header.UNREGISTER)) {
            String ip = packet.srcIp;
            if (packet.message.equals(Message.DEVICE_SENSOR)) {
                synchronized (mSensorIPtoButton) {
                    if (mSensorIPtoButton.containsKey(ip)) {
                        RadioButton button = mSensorIPtoButton.get(ip);
                        mSensorIPList.post(() -> mSensorIPList.removeView(button));
                        mSensorIPtoButton.remove(ip);
                    }
                }
                Log.d(TAG, "Sensor unregistered : " + ip);
            } else if (packet.message.equals(Message.DEVICE_EDGE)) {
                synchronized (mEdgeIPtoButton) {
                    if (mEdgeIPtoButton.containsKey(ip)) {
                        RadioButton button = mEdgeIPtoButton.get(ip);
                        mEdgeIPList.post(() -> mEdgeIPList.removeView(button));
                        mEdgeIPtoButton.remove(ip);
                    }
                }
                Log.d(TAG, "Edge unregistered : " + ip);
            } else {
                Log.e(TAG, "Wrong Device Unregistration : " + packet.message);
            }
        }
    };
    // Network Manager
    private NetworkManager mNetworkManager;
    // Scheduler
    private Thread mSchedulerThread;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // UI Elements
        mBinding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(mBinding.getRoot());
        mIpInput = findViewById(R.id.ip_input);
        mPortInput = findViewById(R.id.port_input);
        mConnectButton = findViewById(R.id.connect_button);
        mSensorIPList = findViewById(R.id.sensor_ip_list);
        mEdgeIPList = findViewById(R.id.edge_ip_list);

        // Initialize NetworkManager
        mNetworkManager = new NetworkManager(new HashMap<String, PacketHandler>() {{
            put(Topic.REGISTER, registerTopicHandler);
            put(Topic.UNREGISTER, unregisterTopicHandler);
        }});

        mConnectButton.setOnCheckedChangeListener((compoundButton, b) -> {
            if (b) {
                startConnection();
            } else {
                stopConnection();
            }
        });
    }

    private void startConnection() {
        // Start NetworkManager
        String brokerIP = mIpInput.getText().toString().trim();
        int brokerPort = Integer.parseInt(mPortInput.getText().toString().trim());
        String uri = "tcp://" + brokerIP + ":" + brokerPort;
        mNetworkManager.start(getApplicationContext(), getLocalIPAddress(), uri);
        mNetworkManager.connect(() -> {
            mNetworkManager.subscribe(Topic.REGISTER, null);
            mNetworkManager.subscribe(Topic.UNREGISTER, null);
        });

        // Start Scheduling Thread
        mbRunScheduler.set(true);
        if (mSchedulerThread == null) {
            mSchedulerThread = new Thread(this);
            mSchedulerThread.start();
        }
    }

    private void stopConnection() {
        // Stop Scheduling Thread
        mbRunScheduler.set(false);
        if (mSchedulerThread != null) {
            try {
                mSchedulerThread.interrupt();
                mSchedulerThread.join();
            } catch (InterruptedException ignored) {
            }
            mSchedulerThread = null;
        }

        // Stop NetworkManager
        mNetworkManager.stop();

        // Clear memory
        mEdgeIPtoButton.clear();
        mSensorIPtoButton.clear();
        mEdgeIPList.removeAllViews();
        mSensorIPList.removeAllViews();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopConnection();
    }

    @Override
    public void run() {
        while (mbRunScheduler.get()) {
            Set<String> edgeIPs;
            synchronized (mEdgeIPtoButton) {
                edgeIPs = new TreeSet<>(mEdgeIPtoButton.keySet());
            }
            int selectedButtonId = mEdgeIPList.getCheckedRadioButtonId();
            String edgeIP;
            if (selectedButtonId != -1) {  // Manual select
                mEdgeIPList.post(() -> mEdgeIPList.clearCheck());
                RadioButton button = findViewById(selectedButtonId);
                edgeIP = (String) button.getText();
                if (!edgeIPs.contains(edgeIP)) {
                    edgeIP = Scheduler.schedule(edgeIPs);
                }
            } else {  // Random selection
                edgeIP = Scheduler.schedule(edgeIPs);
            }
            mNetworkManager.publish(Topic.SCHEDULE, Packet.ALL, Header.SCHEDULE, edgeIP, null);
            try {
                Thread.sleep(SCHEDULE_INTERVAL_MS);
            } catch (InterruptedException e) {
                Log.d(TAG, "mSchedulerThread interrupted");
            }
        }
    }

    private String getLocalIPAddress() {
        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        int ip = wifiManager.getConnectionInfo().getIpAddress();
        return "" + (ip & 0xff) + "." + (ip >> 8 & 0xff) + "." + (ip >> 16 & 0xff) + "." + (ip >> 24 & 0xff);
    }
}
