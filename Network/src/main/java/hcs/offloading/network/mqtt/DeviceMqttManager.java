package hcs.offloading.network.mqtt;

import android.content.Context;
import android.util.Log;

import hcs.offloading.network.mqtt.datatypes.Device;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.mqtt.datatypes.RegisterHeader;
import hcs.offloading.network.mqtt.datatypes.Topic;
import hcs.offloading.network.mqtt.datatypes.WebRTCHeader;

public class DeviceMqttManager extends MqttManager implements Runnable {
    private static final String TAG = DeviceMqttManager.class.getName();

    private static final int REGISTER_INTERVAL_MS = 3000;

    private final Device mDevice;
    private final Thread mRegisterThread;

    public DeviceMqttManager(Context context, String uri, Device device, PacketHandler scheduleTopicHandler, PacketHandler webrtcTopicHandler) {
        super(context, uri);
        mDevice = device;

        mPacketHandlers.put(Topic.SCHEDULE.name(), scheduleTopicHandler);
        mPacketHandlers.put(Topic.WEBRTC.name(), webrtcTopicHandler);

        mRegisterThread = new Thread(this);
        mRegisterThread.start();
    }

    @Override
    public void close() {
        sendUnregister();
        try {
            mRegisterThread.interrupt();
            mRegisterThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage());
        }
        super.close();
        Log.d(TAG, "closed");
    }

    @Override
    public void run() {
        try {
            while (true) {
                publish(Topic.REGISTER, null, RegisterHeader.REGISTER.name(), mDevice.name());
                Thread.sleep(REGISTER_INTERVAL_MS);
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    void sendUnregister() {
        publish(Topic.REGISTER, null, RegisterHeader.UNREGISTER.name(), mDevice.name());
    }

    public void sendIceMessage(String dstIp, String message) {
        publish(Topic.WEBRTC, dstIp, WebRTCHeader.ICE.name(), message);
    }

    public void sendSdpMessage(String dstIp, String message) {
        publish(Topic.WEBRTC, dstIp, WebRTCHeader.SDP.name(), message);
    }
}
