package hcs.offloading.network;

import android.content.Context;
import android.util.Log;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.json.JSONException;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class NetworkManager implements MqttCallback {
    private static final String TAG = NetworkManager.class.getName();

    private MqttAndroidClient mqttAndroidClient;
    private final int qos = 0;

    private String localIP;
    private final List<String> subscribedTopics = new ArrayList<>();
    private final Map<String, PacketHandler> packetHandlers;

    public NetworkManager(Map<String, PacketHandler> packetHandlers) {
        this.packetHandlers = packetHandlers;
    }

    public void start(Context context, String localIP, String uri) {
        this.localIP = localIP;
        if (mqttAndroidClient == null) {
            mqttAndroidClient = new MqttAndroidClient(context, uri, MqttClient.generateClientId());
            mqttAndroidClient.setCallback(this);
            Log.d(TAG, uri + ":" + localIP + " mqttAndroidClient created");
        }
    }

    public void stop() {
        Log.d(TAG, "stop()");
        if (mqttAndroidClient != null) {
            for (String topic : subscribedTopics) {
                try {
                    mqttAndroidClient.unsubscribe(topic);
                    Log.d(TAG, "Unsubscribed " + topic);
                } catch (MqttException e) {
                    Log.e(TAG, "Unsubscribe failed " + topic);
                }
            }
            Log.d(TAG, "mqttAndroidClient closed");
            mqttAndroidClient.close();
            mqttAndroidClient = null;
        }
        subscribedTopics.clear();
    }

    public IMqttToken connect(Callback onSuccess) {
        IMqttToken token = null;
        try {
            if(mqttAndroidClient != null) {
                token = mqttAndroidClient.connect(null, new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        Log.d(TAG, "connect success");
                        if (onSuccess != null) {
                            onSuccess.run();
                        }
                    }

                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        Log.e(TAG, "connect fail");
                    }
                });
            }
        } catch (MqttException e) {
            Log.e(TAG, "Error occurred in connect: " + e.getMessage());
        }
        return token;
    }

    public IMqttToken subscribe(String topic, Callback onSuccess) {
        IMqttToken token = null;
        try {
            checkMqttConnection();
            if(mqttAndroidClient != null) {
                token = mqttAndroidClient.subscribe(topic, qos, null, new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        Log.d(TAG, String.format("\"%s\" topic subscribe success", topic));
                        subscribedTopics.add(topic);
                        if (onSuccess != null) {
                            onSuccess.run();
                        }
                    }

                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        Log.e(TAG, String.format("\"%s\" topic subscribe failed", topic));
                    }
                });
            }
        } catch (MqttException e) {
            Log.e(TAG, "Error occurred in subscribe: " + e.getMessage());
        }
        return token;
    }

    public IMqttDeliveryToken publish(String topic, String dstIp, String header, String message, Callback onSuccess) {
        IMqttDeliveryToken token = null;
        try {
            checkMqttConnection();
            if(mqttAndroidClient != null) {
                MqttMessage msg = new MqttMessage();
                Packet packet = new Packet(localIP, dstIp, header, message);
                msg.setPayload(packet.serialize());
                msg.setQos(qos);
                token = mqttAndroidClient.publish(topic, msg, null, new IMqttActionListener() {
                    @Override
                    public void onSuccess(IMqttToken asyncActionToken) {
                        Log.d(TAG, String.format("\"%s\" %s %s publish success", topic, header, message));
                        if (onSuccess != null) {
                            onSuccess.run();
                        }
                    }

                    @Override
                    public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                        Log.e(TAG, String.format("\"%s\" %s %s publish failed", topic, header, message));
                    }
                });
            }
        } catch (JSONException | MqttException e) {
            Log.e(TAG, "Error occurred in publish: " + e.getMessage());
        }
        return token;
    }

    public boolean isLocalIP(String ip) {
        if (ip == null) {
            return false;
        }
        return ip.equals(localIP);
    }

    private Packet getPayload(MqttMessage message) {
        return Packet.deserialize(message.getPayload());
    }

    private void checkMqttConnection() throws MqttException {
        if ((mqttAndroidClient != null) && (!mqttAndroidClient.isConnected())) {
            Log.e(TAG, "mqttAndroidClient is not connected");
            throw new MqttException(MqttException.REASON_CODE_SERVER_CONNECT_ERROR);
        }
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) {
        Log.d(TAG,  "messageArrived " + topic + " : " + message.toString());
        PacketHandler packetHandler = packetHandlers.get(topic);
        if (packetHandler != null) {
            packetHandler.handlePacket(getPayload(message));
        }
    }

    @Override
    public void connectionLost(Throwable cause) {
        Log.d(TAG, "connectionLost: " + cause.toString());
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        Log.d(TAG, "deliveryComplete: " + token.toString());
    }
}
