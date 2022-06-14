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

 private:
  std::string taskname_;
  apollo::cyber::Time ts_start_;
};

}  // namespace profiling
}  // namespace um_dev

#endif