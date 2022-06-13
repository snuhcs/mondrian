#include "strm/Utils.hpp"

#include <set>

namespace rm {

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                                         const int numLabels, const float iouThreshold) {
  std::vector<BoundingBox> nmsList;

  auto comp = [](const BoundingBox& l, const BoundingBox& r) -> bool {
    return l.confidence > r.confidence;
  };
  for (int k = 0; k < numLabels; k++) {
    if (k != 0) {
      continue;
    }
    std::set<BoundingBox, decltype(comp)> sortedBoxes(comp);

    for (const BoundingBox& box : boxes) {
//      if (box.labelName == <labelName for k>) {
      if (box.labelName == "person") {
        sortedBoxes.insert(box);
      }
    }

    while (!sortedBoxes.empty()) {
      auto startIt = sortedBoxes.begin();
      const BoundingBox& max = *startIt;
      nmsList.push_back(max);
      sortedBoxes.erase(startIt);

      for (auto it = sortedBoxes.begin(); it != sortedBoxes.end();) {
        if (max.location.iou(it->location) >= iouThreshold) {
          it = sortedBoxes.erase(it);
        } else {
          it++;
        }
      }
    }
  }
  return nmsList;
}

template<typename T>
ConcurrentQueue<T>::ConcurrentQueue(int numMaxItems) : mNumMaxItems(numMaxItems) {}

template<typename T>
void ConcurrentQueue<T>::enqueue(T item) {
  std::lock_guard<std::mutex> lock(mtx);
  cv.wait(lock, [this](){ return std::queue<T>::size() < mNumMaxItems; });
  std::queue<T>::push(item);
}

template<typename T>
T ConcurrentQueue<T>::take() {
  std::lock_guard<std::mutex> lock(mtx);
  cv.wait(lock, [this](){ return !std::queue<T>::empty(); });
  T item = std::queue<T>::front();
  std::queue<T>::pop();
  return item;
}

ThreadPool::ThreadPool(int numThreads) : isStopped(false), isPaused(false) {
  threads.reserve(numThreads);
  for (int i = 0; i < numThreads; i++) {
    threads.emplace_back([this](){ work(); });
  }
}

ThreadPool::~ThreadPool() {
  isStopped = true;
  cv.notify_all();
  for (std::thread& thread : threads) {
    thread.join();
  }
}

void ThreadPool::enqueue(std::function<void()> job) {
  if (isStopped) {
    return;
  }
  std::lock_guard<std::mutex> lock(mtx);
  jobs.push(std::move(job));
  cv.notify_one();
}

void ThreadPool::work() {
  while (true) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return !isPaused && (isStopped || !jobs.empty()); });
    if (isStopped) {
      return;
    }
    std::function<void()> job = std::move(jobs.front());
    jobs.pop();
    lock.unlock();

    job();
  }
}

void ThreadPool::pause() {
  isPaused = true;
}

void ThreadPool::resume() {
  isPaused = false;
}

} // namespace rm
