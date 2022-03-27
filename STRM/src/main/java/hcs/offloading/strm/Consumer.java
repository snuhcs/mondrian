package hcs.offloading.strm;

import android.util.Log;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicBoolean;

abstract class Consumer<T> implements Runnable {
    private final String TAG;

    private final AtomicBoolean isClosed = new AtomicBoolean(false);
    private final Thread mThread;
    private final ConsumerCallback<T> mConsumerCallback;
    private final LinkedBlockingQueue<T> mItems;

    protected Consumer(String tag, int maxNumItems, ConsumerCallback<T> consumerCallback) {
        TAG = tag;
        mItems = new LinkedBlockingQueue<>(maxNumItems);
        mConsumerCallback = consumerCallback;
        mThread = new Thread(this);
        mThread.start();
    }

    public abstract void process(T item) throws InterruptedException;

    @Override
    public void run() {
        try {
            while (true) {
                T item = mItems.take();
                Log.v(TAG, "Start process(item)");
                process(item);
                Log.v(TAG, "End process(item)");
                if (mConsumerCallback != null) {
                    mConsumerCallback.onProcessEnd(item);
                }
            }
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    public void enqueue(T item) throws InterruptedException {
        if (isClosed.get()) {
            return;
        }
        mItems.put(item);
    }

    public void close() {
        try {
            isClosed.set(true);
            mThread.interrupt();
            mThread.join();
        } catch (InterruptedException e) {
            Log.e(TAG, e.getMessage() != null ? e.getMessage() : "e.getMessage() == null");
        }
    }

    protected boolean isClosed() {
        return isClosed.get();
    }
}
