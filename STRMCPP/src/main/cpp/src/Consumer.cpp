#include "strm/Consumer.hpp"

namespace rm {

template<typename T>
void Consumer<T>::enqueue(T item) {
  std::lock_guard<std::mutex> lock(mItemsMtx);
  mItemsCV.wait(lock, [this] {
    return mItems.size() < mMaxNumItems;
  });
  mItems.push(std::move(item));
}

template<typename T>
T Consumer<T>::takeItem() {
  std::lock_guard<std::mutex> lock(mItemsMtx);
  mItemsCV.wait(lock, [this] {
    return !mItems.empty();
  });
  T item = mItems.front();
  mItems.pop();
  return item;
}

}
