package hcs.offloading.network.mqtt.datatypes;

import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

public class Packet {
    private static final String TAG = Packet.class.getName();

    private static final Charset encodingFormat = StandardCharsets.UTF_8;
    private static final String KEY_SRC_IP = "SRC_IP";
    private static final String KEY_DST_IP = "DST_IP";
    private static final String KEY_HEADER = "HEADER";
    private static final String KEY_MESSAGE = "MESSAGE";

    public final String srcIp;
    public final String dstIp;
    public final String header;
    public final String message;

    private Packet(String srcIp, String dstIp, String header, String message) {
        this.srcIp = srcIp;
        this.dstIp = dstIp;
        this.header = header;
        this.message = message;
    }

    public static Packet deserialize(byte[] payload) {
        String jsonStr = new String(payload, encodingFormat);
        JSONObject json;
        Packet packet = null;
        try {
            json = new JSONObject(jsonStr);
            packet = new Packet(
                    get(json, KEY_SRC_IP),
                    get(json, KEY_DST_IP),
                    get(json, KEY_HEADER),
                    get(json, KEY_MESSAGE)
            );
        } catch (JSONException e) {
            Log.e(TAG, e.getMessage());
        }
        return packet;
    }

    public static byte[] serialize(String srcIp, String dstIp, String header, String message) {
        JSONObject json = new JSONObject();
        put(json, KEY_SRC_IP, srcIp);
        put(json, KEY_DST_IP, dstIp);
        put(json, KEY_HEADER, header);
        put(json, KEY_MESSAGE, message);
        return json.toString().getBytes(encodingFormat);
    }

    private static String get(JSONObject jsonObject, String key) {
        String value = null;
        try {
            value = (String) jsonObject.get(key);
        } catch (JSONException ignored) {
        }
        return value;
    }

    private static void put(JSONObject jsonObject, String key, String value) {
        try {
            if (value != null) {
                jsonObject.put(key, value);
            }
        } catch (JSONException ignored) {
        }
    }
}
