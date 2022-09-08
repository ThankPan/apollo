// Implementation of ProfilingResultWriter
// Author: Yuting Xie
// 2022.2.22

#include "um_dev/profiling/trace_timer/trace_timer.h"

#include <assert.h>
#include <unistd.h>

#include <iostream>

#include "um_dev/utils/files.h"

namespace um_dev {
namespace profiling {

TraceTimer* TraceTimer::instance_ = nullptr;

// The write lock to the unordered maps
std::mutex TraceTimer::mutex_checkpoint_map_;
std::mutex TraceTimer::mutex_file_;

TraceTimer::TraceTimer() : max_checkpoints_(100) {
  InitConfig();
  InitFile();
  InitEvents();
}

TraceTimer::~TraceTimer() {
  fout.close();
}

bool TraceTimer::InitConfig() {
std::ifstream conf_fin("/apollo/um_dev/profiling/conf/result_writer_conf.txt");
  if (!conf_fin.is_open()) {
    return false;
  }
  std::string conf_name, conf_value;
    while (!conf_fin.eof()) {
    conf_fin >> conf_name >> conf_value;
    confs_[conf_name] = conf_value;
  }
  return true;
}

bool TraceTimer::InitFile() {
  // Make directory
  const std::string result_root_dir =
    "/apollo/um_dev/profiling/results/" + confs_["profiling_result_suffix"] + "/Trace/";
  um_dev::utils::um_mkdir(result_root_dir);

  // Open file
  auto pid_str = std::to_string(getpid());
  auto time_str = apollo::cyber::Time::Now().ToString();
  time_str[4] = time_str[7] = time_str[10] = time_str[13] = time_str[16] = time_str[19] = '_';
  fout.open(result_root_dir + time_str + "_" + pid_str + ".csv");
  if (!fout.is_open()) {
    std::cout << "Yuting: unable to open file for trace timer\n";
    assert(false);
  }

  // CSV header
  std::vector<std::string> fields = {
    "timestamp", // Current time stamp
    "task", // The task name (e.g., planning, control.)
    "checkpoint", // Internal checkpoint (e.g., GPU start, GPU end)
    "from_start", // Time elasped since current timing cycle
    "from_last", // Time elapsed since last event
    };
  fout << utils::make_csv_header(fields) << std::endl;
  return true;
}

bool TraceTimer::InitEvents() {
  event_to_name_[UNKNOWN] = "unknown";
  event_to_name_[START] = "start";
  event_to_name_[END] = "end";
  event_to_name_[GPU_START] = "gpu_start";
  event_to_name_[GPU_END] = "gpu_end";
  return true;
}

TraceTimer& TraceTimer::Instance() {
  static std::once_flag s_flag;
  std::call_once(s_flag, [&](){
    instance_ = new TraceTimer();
  });
  return *instance_;
}

bool TraceTimer::set(const std::string taskname, const Event event) {
  std::lock_guard<std::mutex> lock(mutex_checkpoint_map_);
  auto now_ns = apollo::cyber::Time::Now().ToNanosecond();
  auto &checkpoint = task_to_checkpoints_[taskname];
  checkpoint.event_name = event_to_name_[event];
  if (Event::START == event) {
    checkpoint.timestamp_start = now_ns;
    checkpoint.since_last = 0;
  } else {
    checkpoint.since_last = now_ns - checkpoint.timestamp;
  }
  checkpoint.timestamp = now_ns;
  write_record(taskname, checkpoint);
  return true;
}

bool TraceTimer::write_record(const std::string taskname, const Checkpoint &checkpoint) {
  std::lock_guard<std::mutex> lock(mutex_file_);
  fout << checkpoint.timestamp << ','                               // Current timestamp
       << taskname << ','                                           // Task
       << checkpoint.event_name << ','                              // Internal checkpoint (e.g., GPU start, GPU end)
       << checkpoint.timestamp - checkpoint.timestamp_start << ','  // Time elasped since current timing cycle
       << checkpoint.since_last << std::endl;                  // Time elapsed since last event
  return true;
}


}  // namespace profiling
}  // namespace um_dev