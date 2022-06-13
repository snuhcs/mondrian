#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <queue>
#include <thread>

#include "strm/DataType.hpp"

namespace rm {

std::vector<BoundingBox> nms(const std::vector<BoundingBox>& boxes,
                             const int numLabels, const float iouThreshold);

template<typename T>
class ConcurrentQueue : public std::queue<T> {
 public:
  ConcurrentQueue(int numMaxItems = INT_MAX);

  void enqueue(T item);

  T take();

 private:
  int mNumMaxItems;
  std::mutex mtx;
  std::condition_variable cv;
};

class ThreadPool {
 public:
  ThreadPool(int numThreads);

  ~ThreadPool();

  void enqueue(std::function<void()> job);

  void pause();

  void resume();

 private:
  void work();

  std::vector<std::thread> threads;
  std::queue<std::function<void()>> jobs;
  std::condition_variable cv;
  std::mutex mtx;
  bool isStopped;
  bool isPaused;
};

} // namespace rm

#endif // UTILS_HPP_
