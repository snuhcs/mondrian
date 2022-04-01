#ifndef CONSUMER_HPP_
#define CONSUMER_HPP_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>

namespace rm {

template<typename T>
class ConsumerCallback {
 public:
  virtual void onProcessEnd(T& item) = 0;
};

template<typename T>
class Consumer {
 public:
  Consumer(int maxNumItems, ConsumerCallback<T>* consumerCallback)
          : mMaxNumItems(maxNumItems), mConsumerCallback(consumerCallback),
            isClosed(false), mThread([this]() {
            while (!isClosed.load()) {
              T item = takeItem();
              process(item);
              if (mConsumerCallback != nullptr) {
                mConsumerCallback->onProcessEnd(item);
              }
            }
          }) {
  };

  virtual ~Consumer() {
    if (!isClosed.load()) {
      isClosed.store(true);
      mThread.join();
    }
  }

  virtual void process(T& item) = 0;

  void enqueue(T item);

 private:
  T takeItem();

  std::atomic_bool isClosed;
  std::thread mThread;
  ConsumerCallback<T>* mConsumerCallback;

  int mMaxNumItems;
  std::queue<T> mItems;
  std::condition_variable mItemsCV;
  std::mutex mItemsMtx;
};

}

#endif // CONSUMER_HPP_
