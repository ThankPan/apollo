// Implementation of ProfilingResultWriter
// Author: Yuting Xie
// 2022.2.22

#include "um_dev/profiling/result_writer.h"

#include <assert.h>
#include <unistd.h>

#include <iostream>

#include "um_dev/utils/files.h"

namespace um_dev {
namespace profiling {

ProfilingResultWriter* ProfilingResultWriter::instance_ =
    new ProfilingResultWriter();

ProfilingResultWriter::ProfilingResultWriter()
    : throttle_threshold_(1.f), profiling_suffix_("default") {

  if (Init()) {
    if (confs_.count("profiling_result_suffix") > 0) {
      profiling_suffix_ = confs_["profiling_result_suffix"];
    }
  } else {
    std::cout << "[um_dev] unable to open conf file for result writer.\n";
  }
  // Make profiling output directory for this time
  apollo::cyber::Time now = apollo::cyber::Time::Now();
  const std::string result_dir =
      "/apollo/um_dev/profiling/results/" + profiling_suffix_;
  um_dev::utils::um_mkdir(result_dir);
  std::cout << "[um_dev] Profiling result writes to: " + result_dir << std::endl;

  auto pid_str = std::to_string(getpid());

  // Open result files here
  auto time_str = now.ToString();
  fout_.open(result_dir + "/profiling_" + time_str + '_' + pid_str + ".log");
  assert(fout_.is_open());
}

ProfilingResultWriter::~ProfilingResultWriter() {
  if (fout_.is_open()) {
    fout_.close();
  }
}

bool ProfilingResultWriter::Init() {
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

ProfilingResultWriter& ProfilingResultWriter::Instance() { return *instance_; }

bool ProfilingResultWriter::write_to_file(PROFILING_METRICS profiling_type,
                                          const std::string& task_name,
                                          const std::string& content) {
  {
    std::lock_guard<std::mutex> lock(mutex_map_);
    apollo::cyber::Time now = apollo::cyber::Time::Now();
    auto it = task_to_timestamp_.find(task_name);
    if (it == task_to_timestamp_.end()) {
      task_to_timestamp_[task_name] = now;
    } else if (now - it->second <
               apollo::cyber::Duration(throttle_threshold_)) {
      return true;
    } else {
      it->second = now;
    }
  }

  switch (profiling_type) {
    case TIMING: {
      std::lock_guard<std::mutex> lock(mutex_result_file_);
      fout_ << '[' << apollo::cyber::Time::Now().ToMicrosecond() / 1000 << ']' << " [Timing]: " << content << std::endl;
      break;
    }
    default:
      break;
  }

  return true;
}

}  // namespace profiling
}  // namespace um_dev