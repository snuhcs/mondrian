#ifndef CHROME_TRACER_TRACER_H_
#define CHROME_TRACER_TRACER_H_

#include <atomic>
#include <chrono>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "event.h"

namespace chrome_tracer {

class ChromeTracer {
 public:
  ChromeTracer(const std::string& name = "");

  int BeginEvent(const std::string& stream,
                 const std::string& event);

  void EndEvent(const std::string& stream,
                const int& handle,
                const std::string& args = "");

  std::pair<bool, std::string> Dump(const bool validate = false) const;
  bool DumpToFile(const std::string& logPath, const bool validate = false) const;

  void Clear();

 private:
  std::pair<bool, std::string> Validate() const;

  std::map<std::string, std::map<int, Event>> event_table_;
  std::chrono::system_clock::time_point anchor_;

  int count_;
  const std::string name_;
  const int pid_;
  inline static std::atomic_int pid_counter_ = 0;

  mutable std::mutex mtx_;
};

}  // namespace chrome_tracer

#endif  // CHROME_TRACER_TRACER_H_