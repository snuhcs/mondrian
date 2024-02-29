#include <chrono>
#include <fstream>
#include <iostream>
#include <cassert>

#include "chrome_tracer/tracer.h"
#include "mondrian/Log.hpp"

namespace chrome_tracer {

namespace {

std::string GenerateInstantEvent(const std::string& name, int pid, int tid,
                                 std::chrono::system_clock::time_point timestamp,
                                 std::chrono::system_clock::time_point anchor) {
  int ts = std::chrono::duration_cast<std::chrono::microseconds>(timestamp - anchor).count();
  std::string result = "{";
  result += "\"name\": \"" + name + "\", ";
  result += "\"ph\": \"i\", ";
  result += "\"ts\": " + std::to_string(ts) + ", ";
  result += "\"tid\": " + std::to_string(tid) + ", ";
  result += "\"pid\": " + std::to_string(pid);
  result += "}";
  return result;
}

std::string GenerateProcessMetaEvent(const std::string& name,
                                     const std::string& meta_name,
                                     int pid,
                                     int tid) {
  std::string result = "{";
  result += "\"name\": \"" + meta_name + "\", ";
  result += "\"ph\": \"M\", ";
  result += "\"pid\": " + std::to_string(pid) + ", ";
  result += "\"tid\": " + std::to_string(tid) + ", ";
  result += "\"args\": {";
  result += "\"name\": \"" + name + "\"";
  result += "}";
  result += "}";
  return result;
}

std::string GenerateBeginEvent(const std::string& name, int pid, int tid,
                               std::chrono::system_clock::time_point timestamp,
                               std::chrono::system_clock::time_point anchor) {
  int ts = std::chrono::duration_cast<std::chrono::microseconds>(timestamp - anchor).count();
  std::string result = "{";
  result += "\"name\": \"" + name + "\", ";
  result += "\"ph\": \"B\", ";
  result += "\"ts\": " + std::to_string(ts) + ", ";
  result += "\"tid\": " + std::to_string(tid) + ", ";
  result += "\"pid\": " + std::to_string(pid);
  result += "}";
  return result;
}

std::string GenerateEndEvent(const std::string& name, int pid, int tid,
                             std::chrono::system_clock::time_point timestamp,
                             std::chrono::system_clock::time_point anchor,
                             const std::string& args) {
  int ts = std::chrono::duration_cast<std::chrono::microseconds>(timestamp - anchor).count();
  std::string result = "{";
  result += "\"name\": \"" + name + "\", ";
  result += "\"ph\": \"E\", ";
  result += "\"ts\": " + std::to_string(ts) + ", ";
  result += "\"tid\": " + std::to_string(tid) + ", ";
  result += "\"pid\": " + std::to_string(pid);
  if (!args.empty()) {
    result += ", \"args\": " + args;
  }
  result += "}";
  return result;
}

std::pair<std::string, std::string> GenerateDurationEvent(
    const std::string& name, int pid, int tid,
    std::pair<std::chrono::system_clock::time_point,
              std::chrono::system_clock::time_point>
    duration,
    std::chrono::system_clock::time_point anchor, std::string args) {
  return {GenerateBeginEvent(name, pid, tid, duration.first, anchor),
          GenerateEndEvent(name, pid, tid, duration.second, anchor, args)};
}

}  // anonymous namespace

ChromeTracer::ChromeTracer(const std::string& name)
    : anchor_(std::chrono::system_clock::now()),
      count_(0),
      name_(name),
      pid_(pid_counter_++) {}

int ChromeTracer::BeginEvent(const std::string& stream,
                             const std::string& name) {
  std::lock_guard<std::mutex> lock(mtx_);
  event_table_[stream].insert({count_, Event(name)});
  return count_++;
}

void ChromeTracer::EndEvent(const std::string& stream,
                            const int& handle,
                            const std::string& args) {
  std::lock_guard<std::mutex> lock(mtx_);
  auto& event = event_table_.at(stream).at(handle);
  event.args = args;
  event.Finish();
}

std::pair<bool, std::string> ChromeTracer::Validate() const {
  for (const auto& [stream, events] : event_table_) {
    for (const auto& [_, event] : events) {
      if (event.GetStatus() == Event::EventStatus::Running) {
        std::cerr << stream << " " << event.name;
        return {false, event.name};
      }
    }
  }
  return {true, ""};
}

// Returns the json string.
std::pair<bool, std::string> ChromeTracer::Dump(const bool do_validate) const {
  std::lock_guard<std::mutex> lock(mtx_);
  if (do_validate) {
    const auto& [valid, event_name] = Validate();
    if (!valid) {
      std::cerr << "There is unfinished event." << std::endl;
      return {false, event_name};
    }
  }

  std::map<std::string, int> stream_tid_map;
  int i = 1;
  for (const auto& [stream, events] : event_table_) {
    stream_tid_map[stream] = i;
    i++;
  }

  std::string result = "{";
  std::string trace_events = "[";
  // 1. Start event per stream
  for (const auto& [stream, events] : event_table_) {
    trace_events +=
        GenerateInstantEvent("Start", pid_, stream_tid_map[stream],
                             anchor_, anchor_) +
            ",";
  }

  // 2. Metadata event per process and thread (stream)
  trace_events +=
      GenerateProcessMetaEvent(name_, "process_name", pid_, 0) + ",";
  for (auto const& stream : event_table_) {
    std::string stream_name = stream.first;
    trace_events += GenerateProcessMetaEvent(stream_name, "thread_name", pid_,
                                             stream_tid_map[stream_name]) +
        ",";
  }

  // 3. Duration event per events
  for (const auto& [stream, events] : event_table_) {
    for (const auto& [_, event] : events) {
      if (event.GetStatus() == Event::EventStatus::Finished) {
        auto dur_events = GenerateDurationEvent(
            event.name, pid_, stream_tid_map[stream],
            std::make_pair(event.start, event.end), anchor_,
            event.args);
        trace_events += dur_events.first + ",";
        trace_events += dur_events.second + ",";
      }
    }
  }
  trace_events = trace_events.substr(0, trace_events.size() - 1) + "]";

  result += "\"traceEvents\": " + trace_events;

  result += "}";

  return {true, result};
}

// Dump the json string to the file path.
bool ChromeTracer::DumpToFile(const std::string& logPath,
                              const bool validate) const {
  auto [valid, log] = Dump(validate);
  if (!valid) return false;

  std::ofstream logFile;
  std::remove(logPath.c_str());
  logFile = std::ofstream(logPath, std::ofstream::app);
  if (!logFile.is_open()) {
    return false;
  }
  logFile << log;
  logFile.flush();
  logFile.close();
  return true;
}

void ChromeTracer::Clear() {
  event_table_.clear();
  anchor_ = std::chrono::system_clock::now();
}

}  // namespace chrome_tracer