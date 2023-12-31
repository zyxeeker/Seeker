/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-31 15:18:47
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-01 17:02:37
 * @Description: 
 */

#ifndef __SEEKER_SRC_LOGGER_IO_H__
#define __SEEKER_SRC_LOGGER_IO_H__

#include "../io.h"
#include "base/tiny_file_service.h"

namespace seeker {
namespace log {

class FileService : protected base::TinyFileService {
 public:
  FileService(const std::string& path)
      : base::TinyFileService(path) {}
  ~FileService() = default;
  
  void Write(const std::ostringstream& oss) {
    io::Manager::Service::WPtr service;
    io::Mgr::GetInstance().GetService(io::Manager::TINY_FILE_SERVICE, service);
    if (!service.expired()) {
      auto str = oss.str();
      service.lock()->CreateTask("UpdateLog", [=](){ WriteImpl(str, true); });
    }
  }
};

} // namespace cfg
} // namespace seeker

#endif // __SEEKER_SRC_LOGGER_IO_H__
