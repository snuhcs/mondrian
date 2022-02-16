package hcs.offloading.network.mqtt;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.util.Log;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import hcs.offloading.network.mqtt.datatypes.Packet;
import hcs.offloading.network.mqtt.datatypes.PacketHandler;
import hcs.offloading.network.mqtt.datatypes.Topic;

public abstract class MqttManager implements MqttCallback {
    private static final String TAG = MqttManager.class.getName();

    private static final int QOS = 0;

    private final String mLocalIP;
    private final MqttAndroidClient mMqttAndroidClient;
    protected final Context mContext;
    protected final Map<String, PacketHandler> mPacketHandlers = new ConcurrentHashMap<>();

    protected MqttManager(Context context, String uri) {
        mContext = context;
        mLocalIP = getLocalIPAddress(context);

        mMqttAndroidClient = new MqttAndroidClient(mContext, uri, MqttClient.generateClientId());
        mMqttAndroidClient.setCallback(this);
        try {
            Log.d(TAG, uri + " MqttAndroidClient created");
            mMqttAndroidClient.connect(null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.d(TAG, "MqttAndroidClient connect success");
                    try {
                        for (Topic topic : Topic.values()) {
                            mMqttAndroidClient.subscribe(topic.name(), QOS, null, new IMqttActionListener() {
                                @Override
                                public void onSuccess(IMqttToken asyncActionToken) {
                                    Log.d(TAG, topic.name() + " subscribe success");
                                }

                                @Override
                                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                                    Log.d(TAG, topic.name() + " subscribe failure");
                                }
                            });
                        }
                    } catch (MqttException e) {
                        Log.e(TAG, e.getMessage());
                    }
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.d(TAG, "MqttAndroidClient connect failure");
                }
            });
        } catch (MqttException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    public void close() {
        try {
            for (Topic topic : Topic.values()) {
                mMqttAndroidClient.unsubscribe(topic.name());
            }
            mMqttAndroidClient.disconnect();
        } catch (MqttException e) {
            Log.e(TAG, e.getMessage());
        }
        Log.d(TAG, "closed");
    }

    public boolean isLocalIP(String ip) {
        return mLocalIP.equals(ip);
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) {
        Log.d(TAG, String.format("Topic.%s : %s", topic, message.toString()));
        PacketHandler packetHandler = mPacketHandlers.get(topic);
        if (packetHandler != null) {
            packetHandler.handlePacket(getPayload(message));
        }
    }

    @Override
    public void connectionLost(Throwable cause) {
        Log.d(TAG, "connectionLost");
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        Log.d(TAG, "deliveryComplete");
    }

    protected void publish(Topic topic, String dstIp, String header, String message) {
        try {
            if (mMqttAndroidClient == null || !mMqttAndroidClient.isConnected()) {
                return;
            }
            MqttMessage msg = new MqttMessage();
            msg.setPayload(Packet.serialize(mLocalIP, dstIp, header, message));
            msg.setQos(QOS);
            mMqttAndroidClient.publish(topic.name(), msg);
        } catch (MqttException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    private Packet getPayload(MqttMessage message) {
        return Packet.deserialize(message.getPayload());
    }

    private static String getLocalIPAddress(Context context) {
        WifiManager wifiManager = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        int ip = wifiManager.getConnectionInfo().getIpAddress();
        return "" + (ip & 0xff) + "." + (ip >> 8 & 0xff) + "." + (ip >> 16 & 0xff) + "." + (ip >> 24 & 0xff);
    }
}
