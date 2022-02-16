package hcs.offloading.network.mqtt.datatypes;

public interface PacketHandler {
    void handlePacket(Packet packet);
}
