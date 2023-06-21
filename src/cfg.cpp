/**
 * @file cfg.cpp
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 配置管理器具体实现
 * @version 1.0
 * @date 2023-06-06
 */

#include "cfg.h"
#include <fstream>

#if 0
#define FILE_PATH "cfg.json"
#else
#define FILE_PATH "test.json"
#endif

namespace seeker {
namespace cfg {

struct ManagerCfg {
  std::ifstream file_stream_;
  nlohmann::json data_;
};

static nlohmann::json& CfgJsonData() {
  static ManagerCfg kManagerCfg;
  if (!kManagerCfg.file_stream_.is_open()) {
    kManagerCfg.file_stream_.open(FILE_PATH);
    if (kManagerCfg.file_stream_.fail()) {
      std::cout << "failed to open file!" << std::endl;
      goto END;
    }

    try {
      kManagerCfg.data_ = nlohmann::json::parse(kManagerCfg.file_stream_);
    } catch (nlohmann::json::exception ex) {
      std::cout << ex.what() << std::endl;
      goto END;
    }
  }
END:
  return kManagerCfg.data_;
}

nlohmann::json& Manager::Impl::GetJsonData() {
  th::MutexGuard sg(GetMutex());
  return CfgJsonData();
}

nlohmann::json& Manager::GetCfgJsonData() {
  return impl_->GetJsonData();
}

Manager::Manager()
    : impl_(std::make_unique<Impl>()) {}

Manager::~Manager() = default;

bool Manager::Remove(const std::string& key) {
  auto &data = GetCfgJsonData();
  auto res = data.find(key);
  if (res == data.end()) {
    return false;
  }
  data.erase(key);
  return true;
}

void Manager::Reload() {
  //TODO
}

void Manager::List() {
  log::Info("cfg") << nlohmann::to_string(impl_->GetJsonData());
}

} // cfg
} // seeker