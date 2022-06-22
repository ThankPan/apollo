// Implementation of UM_Timing
// Author: Yuting Xie
// 2022.2.22

#include "um_dev/profiling/timing/timing.h"

#include "um_dev/profiling/result_writer.h"

#include "cyber/time/duration.h"

namespace um_dev {
namespace profiling {

UM_Timing::UM_Timing(std::string taskname) : taskname_(taskname) {
  ts_start_ = apollo::cyber::Time::Now();
}

UM_Timing::~UM_Timing() {
  auto ts_end =  apollo::cyber::Time::Now();
  auto duration = ts_end - ts_start_;
  long long ns = duration.ToNanosecond();
  double micro_s = ns / MICRO_TO_NANO;
  ProfilingResultWriter::Instance().write_to_file(PROFILING_METRICS::TIMING,
                                                  ts_start_,
                                                  ts_end,
                                                  taskname_,
                                                  std::to_string(micro_s),
                                                  is_finish_);
}

void UM_Timing::set_finish() {
  is_finish_ = true;
}

}  // namespace profiling
}  // namespace um_dev
