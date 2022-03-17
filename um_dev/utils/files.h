#ifndef FILES_H
#define FILES_H

#include <sys/stat.h>
#include <unistd.h>

#include <string>

namespace um_dev {
namespace utils {

bool um_mkdir(const std::string &path);
bool um_rmdir(const std::string &path);

}  // namespace utils
}  // namespace um_dev

#endif