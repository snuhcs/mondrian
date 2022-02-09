package hcs.offloading.network;

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

    public static final String ALL = "ALL";

    public String srcIp;
    public String dstIp;
    public String header;
    public String message;

    Packet(String srcIp, String dstIp, String header, String message) {
        this.srcIp = srcIp;
        this.dstIp = dstIp;
        this.header = header;
        this.message = message;
    }

    public byte[] serialize() throws JSONException {
        JSONObject json = new JSONObject();
        json.put(KEY_SRC_IP, srcIp);
        json.put(KEY_DST_IP, dstIp);
        json.put(KEY_HEADER, header);
        json.put(KEY_MESSAGE, message);
        return json.toString().getBytes(encodingFormat);
    }

    private static String get(JSONObject jsonObject, String key) {
        String value = null;
        try {
            value = (String) jsonObject.get(key);
        } catch (JSONException ignored) {}
        return value;
    }

    public static Packet deserialize(byte[] payload) {
        String jsonStr = new String(payload, encodingFormat);
        JSONObject json;
        Packet packet = null;
        try {
            json = new JSONObject(jsonStr);
            packet = new Packet(
                    (String) get(json, KEY_SRC_IP),
                    (String) get(json, KEY_DST_IP),
                    (String) get(json, KEY_HEADER),
                    (String) get(json, KEY_MESSAGE)
            );
        } catch (JSONException e) {
            Log.e(TAG, e.getMessage());
        }
        return packet;
    }
}
