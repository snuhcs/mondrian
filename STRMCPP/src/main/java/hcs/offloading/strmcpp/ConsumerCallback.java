package hcs.offloading.strmcpp;

public interface ConsumerCallback<T> {
    void onProcessEnd(T item);
}
