// Profiling result writer
// Author: Yuting Xie
// 2022.2.22

#ifndef RESULT_WRITER_H
#define RESULT_WRITER_H

#include <fstream>
#include <mutex>
#include <string>
#include <map>

#include "cyber/time/time.h"

namespace um_dev {
namespace profiling {

enum PROFILING_METRICS {
  TIMING = 0,
  MEMORY,
  GPU,
};

class ProfilingResultWriter {
 public:
  ProfilingResultWriter(ProfilingResultWriter&) = delete;
  ~ProfilingResultWriter();
  static ProfilingResultWriter& Instance();
  bool write_to_file(PROFILING_METRICS profiling_type, const std::string& task_name,
                     const std::string& content);

 private:
  ProfilingResultWriter();

 private:
  static ProfilingResultWriter* instance_;
  static std::mutex mutex_instance_;

  const float throttle_threshold_; // throttle threshold in second
  std::string profiling_scenario_;

  std::map<std::string, apollo::cyber::Time> task_to_timestamp_;

  std::mutex mutex_result_file_;
  std::mutex mutex_map_;

  // Result files
  std::string result_file_;
  std::ofstream fout_;
};

}  // namespace profiling
}  // namespace um_dev
#endif