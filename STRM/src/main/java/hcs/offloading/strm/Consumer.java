package hcs.offloading.strm;

import android.util.Log;

import java.util.concurrent.LinkedBlockingQueue;

abstract class Consumer<T, V> implements Runnable {
    protected final String TAG;

    protected Thread mThread;
    protected ConsumerCallback<V> mConsumerCallback;
    protected LinkedBlockingQueue<T> mItems;

    protected Consumer(String tag, int maxNumItems, ConsumerCallback<V> consumerCallback) {
        TAG = tag;
        mItems = new LinkedBlockingQueue<>(maxNumItems);
        mConsumerCallback = consumerCallback;
        mThread = new Thread(this);
        mThread.start();
    }

    public abstract V process(T item);

    @Override
    public void run() {
        try {
            while (true) {
                T item = mItems.take();
                V result = process(item);
                if (mConsumerCallback != null) {
                    mConsumerCallback.onProcessEnd(result);
                }
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    public void enqueue(T item) {
        try {
            mItems.put(item);
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    public void close() {
        try {
            mThread.interrupt();
            mThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }
}
