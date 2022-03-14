#ifndef FILES_H
#define FILES_H

#include <unistd.h>
#include <sys/stat.h>
#include <string>

namespace um_dev {
namespace utils {

bool um_mkdir(const std::string &path);
bool um_rmdir(const std::string &path);

}  // utils
}  // um_dev





#endif