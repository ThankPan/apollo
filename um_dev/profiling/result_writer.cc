// Implementation of ProfilingResultWriter
// Author: Yuting Xie
// 2022.2.22

#include "./result_writer.h"

#include <iostream>

namespace um_dev {
namespace profiling {

ProfilingResultWriter::ProfilingResultWriter() {
  // TODO: (yuting) figure out the run-time working directory to write to
  // fout_timing_.open("");
  std::cout << "[Profiling] Simulate open files\n";
}

ProfilingResultWriter::~ProfilingResultWriter() {
  // TODO: (yuting) close files here
  // fout_timing_.close();
  std::cout << "[Profiling] Simulate close files\n";
}

ProfilingResultWriter& ProfilingResultWriter::Instance() { return instance_; }

bool ProfilingResultWriter::write_to_file(PROFILING_METRICS profiling_type,
                                          const std::string& content) {
  // TODO: (yuting) write to files here
  switch (profiling_type) {
    case TIMING: {
      std::lock_guard<std::mutex> lock(mutex_timing_);
      std::cout << "[Profiling] type = Timing" << std::endl;
      std::cout << content << std::endl;
      break;
    }
    default:
      break;
  }

  return true;
}

}  // namespace profiling
}  // namespace um_dev