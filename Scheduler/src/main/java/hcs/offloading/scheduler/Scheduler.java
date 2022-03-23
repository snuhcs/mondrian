package hcs.offloading.scheduler;

import android.content.Context;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.view.View;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import org.eclipse.paho.client.mqttv3.MqttException;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Random;

import hcs.offloading.network.mqtt.HubMqttManager;
import hcs.offloading.network.mqtt.datatypes.Device;

@RequiresApi(api = Build.VERSION_CODES.N)
public class Scheduler implements Runnable {
    private static final String TAG = Scheduler.class.getName();

    private static final int SCHEDULE_INTERVAL_MS = 5000;

    private final RadioGroup mSensorIPList;
    private final RadioGroup mEdgeIPList;

    private final Context mContext;

    private final HubMqttManager mHubMqttManager;

    private final Thread mScheduleThread;

    Scheduler(Context context, String uri, RadioGroup sensorIPList, RadioGroup edgeIPList) {
        mContext = context;
        mSensorIPList = sensorIPList;
        mEdgeIPList = edgeIPList;
        mHubMqttManager = new HubMqttManager(mContext, uri);

        mScheduleThread = new Thread(this);
        mScheduleThread.start();
    }

    private static String schedule(Map<String, Device> ipDevices) {
        // TODO: Schedule with edge information
        List<String> edgeIPs = new ArrayList<>();
        for (Map.Entry<String, Device> ipDevice : ipDevices.entrySet()) {
            if (ipDevice.getValue().equals(Device.EDGE)) {
                edgeIPs.add(ipDevice.getKey());
            }
        }
        if (edgeIPs.size() <= 0) {
            return null;
        }
        Random rand = new Random();
        return edgeIPs.get(rand.nextInt(edgeIPs.size()));
    }

    void close() {
        try {
            mScheduleThread.interrupt();
            mScheduleThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage());
        }
        mHubMqttManager.close();
        mEdgeIPList.post(mEdgeIPList::removeAllViews);
        mSensorIPList.post(mSensorIPList::removeAllViews);
    }

    @Override
    public void run() {
        try {
            while (true) {
                Map<String, Device> ipDevices = mHubMqttManager.getIpDevices();

                try {
                    String targetEdgeIP = Scheduler.schedule(ipDevices);
                    mHubMqttManager.sendScheduleMessage(targetEdgeIP);
                } catch (MqttException e) {
                    Log.e(TAG, e.getMessage());
                }

                mEdgeIPList.post(mEdgeIPList::removeAllViews);
                mSensorIPList.post(mSensorIPList::removeAllViews);
                for (Map.Entry<String, Device> ipDevice : ipDevices.entrySet()) {
                    RadioButton button = new RadioButton(mContext);
                    button.setId(View.generateViewId());
                    button.setText(ipDevice.getKey());
                    if (ipDevice.getValue().equals(Device.SENSOR)) {
                        mSensorIPList.post(() -> mSensorIPList.addView(button));
                    } else if (ipDevice.getValue().equals(Device.EDGE)) {
                        mEdgeIPList.post(() -> mEdgeIPList.addView(button));
                    }
                }
                Thread.sleep(SCHEDULE_INTERVAL_MS);
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }
}
