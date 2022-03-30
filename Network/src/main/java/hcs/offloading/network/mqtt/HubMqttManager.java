package hcs.offloading.network.mqtt;

import android.content.Context;
import android.util.Log;

import org.eclipse.paho.client.mqttv3.MqttException;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import hcs.offloading.network.mqtt.datatypes.Device;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.mqtt.datatypes.RegisterHeader;
import hcs.offloading.network.mqtt.datatypes.Topic;

public class HubMqttManager extends MqttManager implements Runnable {
    private static final String TAG = HubMqttManager.class.getName();

    private static final int RESET_INTERVAL_MS = 6000;

    private final Thread mRegisterThread;

    private final Map<String, Device> mCurrIpDevices = new ConcurrentHashMap<>();
    private final Map<String, Device> mIpDevices = new ConcurrentHashMap<>();

    public HubMqttManager(Context context, String uri) {
        super(context, uri);
        mPacketHandlers.put(Topic.REGISTER.name(), registerTopicHandler);
        mRegisterThread = new Thread(this);
        mRegisterThread.start();
    }

    @Override
    public void close() {
        try {
            mRegisterThread.interrupt();
            mRegisterThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage());
        }
        super.close();
    }

    @Override
    public void run() {
        try {
            while (true) {
                mIpDevices.clear();
                mIpDevices.putAll(mCurrIpDevices);
                mCurrIpDevices.clear();
                Thread.sleep(RESET_INTERVAL_MS);
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    private final PacketHandler registerTopicHandler = packet -> {
        String device = packet.message;
        String deviceIp = packet.srcIp;
        if (packet.header.equals(RegisterHeader.REGISTER.name())) {
            if (device.equals(Device.SENSOR.name())) {
                mCurrIpDevices.put(deviceIp, Device.SENSOR);
            } else if (device.equals(Device.EDGE.name())) {
                mCurrIpDevices.put(deviceIp, Device.EDGE);
            }
        } else if (packet.header.equals(RegisterHeader.UNREGISTER.name())) {
            if (device.equals(Device.SENSOR.name())) {
                mCurrIpDevices.remove(deviceIp);
            } else if (device.equals(Device.EDGE.name())) {
                mCurrIpDevices.remove(deviceIp);
            }
        }
    };

    public void sendScheduleMessage(String targetEdgeIP) throws MqttException {
        publish(Topic.SCHEDULE, null, null, targetEdgeIP);
    }

    public Map<String, Device> getIpDevices() {
        return new HashMap<>(mIpDevices);
    }
}
