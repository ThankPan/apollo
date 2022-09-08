// Singleton trace timing tool
// Author: Yuting Xie
// 2022.8.31

#ifndef TRACE_TIMER_H
#define TRACE_TIMER_H

#include <fstream>
#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>

#include "cyber/time/time.h"

namespace um_dev {
namespace profiling {

typedef enum CheckpointEvent {
  UNKNOWN = 0, 
  START, 
  END, 
  GPU_START, 
  GPU_END
} Event;

namespace {
  typedef struct CheckpointInfo {
    std::string event_name = "unkown";
    uint64_t timestamp = 0;
    uint64_t timestamp_start = 0;
    uint64_t since_last = 0;
  } Checkpoint;
}

class TraceTimer {
 public:
  TraceTimer(TraceTimer&) = delete;
  ~TraceTimer();
  static TraceTimer& Instance();
  bool set(const std::string taskname, const Event event);

 private:
  TraceTimer();
  bool InitConfig();
  bool InitFile();
  bool InitEvents();
  bool write_record(const std::string taskname, const Checkpoint &checkpoint);

 private:
  static TraceTimer* instance_;

  // The write lock to the unordered maps
  static std::mutex mutex_checkpoint_map_;
  static std::mutex mutex_file_;

  // Taskname to recorded checkpoints
  std::unordered_map<std::string, Checkpoint> task_to_checkpoints_;

  // Output file
  std::ofstream fout;

  // Configs
  std::unordered_map<std::string, std::string> confs_;

  // Event strings
  std::unordered_map<Event, std::string> event_to_name_;

  // Default checkpoint max size
  const int max_checkpoints_;
};

}  // namespace profiling
}  // namespace um_dev
#endif