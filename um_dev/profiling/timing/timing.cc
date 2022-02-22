// Implementation of UM_Timing
// Author: Yuting Xie
// 2022.2.22

#include "um_dev/profiling/timing/timing.h"

#include "um_dev/profiling/result_writer.h"

#include "cyber/time/duration.h"

namespace um_dev {
namespace profiling {

UM_Timing::UM_Timing(std::string taskname) : taskname_(taskname) {
  inner_time_ = apollo::cyber::Time::Now();
}

UM_Timing::~UM_Timing() {
  auto duration = apollo::cyber::Time::Now() - inner_time_;
  long long ns = duration.ToNanosecond();
  std::string timing_result =
      "task: [" + taskname_ + "], time elapsed: " + std::to_string(ns) + "ns.";

  ProfilingResultWriter::Instance().write_to_file(PROFILING_METRICS::TIMING,
                                                  timing_result);
}

}  // namespace profiling
}  // namespace um_dev
