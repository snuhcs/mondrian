package hcs.offloading.network.webrtc;

import org.webrtc.MediaStream;

public interface WebRTCCallback {
    void onDisconnect(String ip);

    void onAddStream(String ip, MediaStream mediaStream);
}
