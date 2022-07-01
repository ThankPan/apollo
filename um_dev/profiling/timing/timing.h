// A RAII Timing class
// Author: Yuting Xie
// 2022.2.22

#ifndef TIMING_H
#define TIMING_H

#include <string>

#include "cyber/time/time.h"

namespace um_dev {
namespace profiling {

class UM_Timing {
 public:
  UM_Timing() = delete;
  UM_Timing(UM_Timing&) = delete;
  UM_Timing(std::string taskname);
  ~UM_Timing();
  void set_finish(const long long ts_cam, 
                          const long long ts_lidar, 
                          const long long ts_radar, 
                          const long long ts_TL,
                          const long long ts_lane);
  void add_checkpoint(std::string name, const long long ts_cam, const long long ts_lidar, const long long ts_radar);
  void set_info(const int info);

 private:
  std::string taskname_;
  apollo::cyber::Time ts_start_;
  bool is_finish_;
  int info = 0; // Yuting@2022.6.29: Component-specified information
};

}  // namespace profiling
}  // namespace um_dev

#endif