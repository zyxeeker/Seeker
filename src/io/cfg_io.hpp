/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-31 15:18:47
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-01 17:01:01
 * @Description: 
 */

#ifndef __SEEKER_SRC_CFG_IO_H__
#define __SEEKER_SRC_CFG_IO_H__

#include <nlohmann/json.hpp>

#include "../io.h"
#include "base/tiny_file_service.h"

namespace seeker {
namespace cfg {

class FileService : protected base::TinyFileService {
 public:
  FileService(const std::string& path, int32_t modify_num)
      : base::TinyFileService(path),
        dst_modify_num_(modify_num) {}
  ~FileService() = default;

  bool Read(nlohmann::json& json) {
    std::stringstream ss;
    if (ReadImpl(ss)) {
      return false;
    }
    ss >> json;
    return true;
  }

  void Write(const nlohmann::json& json, bool immediately = false) {
    ++modify_num_;
    if (immediately || modify_num_ == dst_modify_num_) {
      io::Manager::Service::WPtr service;
      io::Mgr::GetInstance().GetService(io::Manager::TINY_FILE_SERVICE, service);
      if (!service.expired()) {
        std::stringstream ss;
        ss << json;
        auto str = ss.str();
        service.lock()->CreateTask("UpdateCfg", [=]() { WriteImpl(str); });
      }
      modify_num_ = 0;
    }
  }

 private:
  int32_t dst_modify_num_;
  int32_t modify_num_;
};

} // namespace cfg
} // namespace seeker

#endif // __SEEKER_SRC_CFG_IO_H__
