package hcs.offloading.strm;

public interface ConsumerCallback<V> {
    void onProcessEnd(V result);
}
