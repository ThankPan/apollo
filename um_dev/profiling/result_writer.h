// Profiling result writer
// Author: Yuting Xie
// 2022.2.22

#ifndef RESULT_WRITER_H
#define RESULT_WRITER_H

#include <fstream>
#include <mutex>
#include <string>

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
  static ProfilingResultWriter& Instance();
  bool write_to_file(PROFILING_METRICS profiling_type,
                     const std::string& content);

 private:
  ProfilingResultWriter();

 private:
  static ProfilingResultWriter* instance_;
  static std::mutex mutex_instance_;

  // Mutexs to write to different profiling result files, according to
  // supporting types
  std::mutex mutex_timing_;
  std::mutex mutex_memory_;
  std::mutex mutex_gpu_;

  // Result files
  std::ofstream fout_timing_;
  std::ofstream fout_memory_;
  std::ofstream fout_gpu_;
};

}  // namespace profiling
}  // namespace um_dev
#endif