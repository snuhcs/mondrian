package hcs.offloading.strm;

public interface ConsumerCallback<T> {
    void onProcessEnd(T item);
}
