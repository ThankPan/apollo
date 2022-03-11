// Implementation of ProfilingResultWriter
// Author: Yuting Xie
// 2022.2.22

#include "um_dev/profiling/result_writer.h"

#include "um_dev/utils/files.h"

#include <iostream>
#include <unistd.h>
#include <assert.h>

namespace um_dev {
namespace profiling {

ProfilingResultWriter* ProfilingResultWriter::instance_ =
    new ProfilingResultWriter();
apollo::cyber::Time ProfilingResultWriter::last_write_time_ =
    apollo::cyber::Time::Now();

ProfilingResultWriter::ProfilingResultWriter() : profiling_scenario_("default"){
  // Make profiling output directory for this time
  apollo::cyber::Time now = apollo::cyber::Time::Now();
  const std::string result_dir = 
    "/apollo/um_dev/profiling/results/" + profiling_scenario_ + "_" + std::to_string(now.ToNanosecond());
  assert(um_dev::utils::um_mkdir(result_dir));
  std::cout << "Profiling result path created: " + result_dir << std::endl;

  auto pid_str = std::to_string(getpid());

  // Open result files here
  fout_timing_.open(result_dir + "/timing_" + pid_str + ".log");
  fout_memory_.open(result_dir + "/memory_" + pid_str + ".log");
  fout_gpu_.open(result_dir + "/gpu_" + pid_str + ".log");
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
  // member
  apollo::cyber::Time now = apollo::cyber::Time::Now();
  switch (profiling_type) {
    case TIMING: {
      if (now - last_write_time_ < apollo::cyber::Duration(0.1)) {
        return true;
      }
      std::lock_guard<std::mutex> lock(mutex_timing_);
      fout_timing_ << "[Timing]: " << content << std::endl;
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