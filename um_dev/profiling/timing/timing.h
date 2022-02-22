// A RAII Timing class
// Author: Yuting Xie
// 2022.2.22

#ifndef TIMING_H
#define TIMING_H

#include <fstream>
#include <string>

#include "cyber/time/time.h"

#define NS_TO_MS(ns) ((ns) / 1000)
#define MS_TO_NS(ms) ((ms)*1000)

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
  apollo::cyber::Time inner_time_;
  std::ofstream fout;
};

}  // namespace profiling
}  // namespace um_dev

#endif