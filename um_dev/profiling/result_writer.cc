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
  // Make profiling output directories for this time
  apollo::cyber::Time now = apollo::cyber::Time::Now();
  const std::string result_root_dir =
      "/apollo/um_dev/profiling/results/" + profiling_suffix_ + "/";
  um_dev::utils::um_mkdir(result_root_dir);
  for (auto it : metrics_to_names_) {
    um_dev::utils::um_mkdir(result_root_dir + it.second);
  }
  

  auto pid_str = std::to_string(getpid());

  // Open result files here
  auto time_str = now.ToString();
  std::vector<std::string> fields = {"timestamp", "ts_start", "ts_end", "type", "component", "result", "is_finish"};
  for (auto it : metrics_to_names_) {
    metrics_to_files_[it.first].open(result_root_dir + it.second + "/" + time_str + '_' + pid_str + ".csv");
    assert(metrics_to_files_[it.first].is_open());
    metrics_to_files_[it.first] << utils::make_csv_header(fields) << std::endl;
  }
}

ProfilingResultWriter::~ProfilingResultWriter() {
  // Can only be referenced
  for (auto& it : metrics_to_files_) {
    if (it.second.is_open()) {
      it.second.close();
    }
  }
}

bool ProfilingResultWriter::Init() {
  metrics_to_names_[PROFILING_METRICS::TIMING] = "Timing";
  metrics_to_names_[PROFILING_METRICS::LATENCY_CAMERA] = "Lat_Cam";
  metrics_to_names_[PROFILING_METRICS::LATENCY_RADAR] = "Lat_Rad";
  metrics_to_names_[PROFILING_METRICS::LATENCY_LIDAR] = "Lat_Lid";

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
                                          apollo::cyber::Time ts_start,
                                          apollo::cyber::Time ts_end,
                                          const std::string& component,
                                          const std::string& result,
                                          const bool is_finish,
                                          bool is_throttled) {
  if (is_throttled) {
    std::lock_guard<std::mutex> lock(mutex_map_);
    apollo::cyber::Time now = apollo::cyber::Time::Now();
    auto throttle_timestamp = component + std::to_string(profiling_type);
    auto it = task_to_timestamp_.find(throttle_timestamp);
    if (it == task_to_timestamp_.end()) {
      task_to_timestamp_[throttle_timestamp] = now;
    } else if (now - it->second <
               apollo::cyber::Duration(throttle_threshold_)) {
      return true;
    } else {
      it->second = now;
    }
  }

  // Yuting@2022.6.15 Now reference metrics by their name strings
  {
    std::lock_guard<std::mutex> locker(metrics_to_mutex_[profiling_type]);
    auto& fout_ = metrics_to_files_[profiling_type];
    // The CSV header: "timestamp", "ts_start", "ts_end", "type", "component", "result"
    fout_ << apollo::cyber::Time::Now().ToMicrosecond() << ',' // "timestamp"
          << ts_start.ToMicrosecond() << ',' // "ts_start"
          << ts_end.ToMicrosecond() << ',' // "ts_end"
          << metrics_to_names_[profiling_type] << ',' // "type"
          << component << ',' // "component"
          << result << ',' // "result"
          << int(is_finish) << std::endl; // "is_finish"
  }

  return true;
}

}  // namespace profiling
}  // namespace um_dev