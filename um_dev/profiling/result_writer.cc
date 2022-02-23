// Implementation of ProfilingResultWriter
// Author: Yuting Xie
// 2022.2.22

#include "./result_writer.h"

#include <assert.h>

#include <iostream>

namespace um_dev {
namespace profiling {

ProfilingResultWriter* ProfilingResultWriter::instance_ =
    new ProfilingResultWriter();
apollo::cyber::Time ProfilingResultWriter::last_write_time_ =
    apollo::cyber::Time::Now();

ProfilingResultWriter::ProfilingResultWriter()
    : timing_output_file_("/apollo/um_dev/profiling/results/timing_result_"),
      memory_output_file_("/apollo/um_dev/profiling/results/memory_result_"),
      gpu_output_file_("/apollo/um_dev/profiling/results/gpu_result_") {
  apollo::cyber::Time now = apollo::cyber::Time::Now();
  fout_timing_.open(timing_output_file_ + std::to_string(now.ToSecond()) +
                    ".log");
  assert(fout_timing_.is_open());
  std::cout << "[Profiling] Simulate open files\n";
}

ProfilingResultWriter::~ProfilingResultWriter() {
  if (fout_timing_.is_open()) {
    fout_timing_.close();
  }
  if (fout_memory_.is_open()) {
    fout_memory_.close();
  }
  if (fout_gpu_.is_open()) {
    fout_gpu_.close();
  }
}

ProfilingResultWriter& ProfilingResultWriter::Instance() { return *instance_; }

bool ProfilingResultWriter::write_to_file(PROFILING_METRICS profiling_type,
                                          const std::string& content) {
  // TODO: more reasonable throttle by timer rather than thread-unsafe static
  // member
  apollo::cyber::Time now = apollo::cyber::Time::Now();
  switch (profiling_type) {
    case TIMING: {
      if (now - last_write_time_ < apollo::cyber::Duration(1.0)) {
        return true;
      }
      std::lock_guard<std::mutex> lock(mutex_timing_);
      fout_timing_ << "[Profiling] type = Timing, " << content << std::endl;
      break;
    }
    default:
      break;
  }
  last_write_time_ = now;

  return true;
}

}  // namespace profiling
}  // namespace um_dev