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

typedef struct CfgMgr_ {
  std::fstream file_stream_;
  JsonDataPtr data_;
} CfgDataMgr;

static CfgDataMgr& CfgData() {
  static CfgDataMgr kCfgDataMgr;
  if (!kCfgDataMgr.data_) {
    // 关闭已打开的文件流
    if (kCfgDataMgr.file_stream_.is_open()) {
      kCfgDataMgr.file_stream_.close();
    }
    // 以只读模式打开
    kCfgDataMgr.file_stream_.open(FILE_PATH, std::ios::in);
    if (kCfgDataMgr.file_stream_.fail()) {
      throw RunTimeError::Create(MODULE_NAME, 
                                "failed to open cfg flie to read!");
    }
    // 解析配置
    try {
      kCfgDataMgr.data_ = std::make_shared<nlohmann::json>
                              (nlohmann::json::parse(kCfgDataMgr.file_stream_));
    } catch (nlohmann::json::exception ex) {
      throw RunTimeError::Create(MODULE_NAME, ex.what());
    }
  }
  return kCfgDataMgr;
}

//// Impl Begin

void Manager::Impl::Init() {
  th::MutexGuard sg(GetMutex());
  auto& cfg_data = CfgData();
  // 关闭已打开的文件流
  if (cfg_data.file_stream_.is_open()) {
    cfg_data.file_stream_.close();
  }
  // 以只读模式打开
  cfg_data.file_stream_.open(FILE_PATH, std::ios::in);
  if (cfg_data.file_stream_.fail()) {
    throw RunTimeError::Create(MODULE_NAME, 
                               "failed to open cfg flie to read!");
  }
  // 解析配置
  try {
    cfg_data.data_ = std::make_shared<nlohmann::json>
                        (nlohmann::json::parse(cfg_data.file_stream_));
  } catch (nlohmann::json::exception ex) {
    throw RunTimeError::Create(MODULE_NAME, ex.what());
  }
}

void Manager::Impl::Save() {
  th::MutexGuard sg(GetMutex());
  auto &data = CfgData();
  if (data.file_stream_.is_open()) {
    data.file_stream_.close();
  }
  // 以只写模式打开
  data.file_stream_.open(FILE_PATH, std::ios::out | std::ios::trunc);
  if (data.file_stream_.fail()) {
    throw RunTimeError::Create(MODULE_NAME, 
                               "failed to open cfg flie to save!");
  }
  data.file_stream_ << *(data.data_);
}

JsonDataPtr Manager::Impl::GetJsonDataPtr() {
  th::MutexGuard sg(GetMutex());
  return CfgData().data_;
}
//// Impl End

Manager::Manager()
    : impl_(std::make_unique<Impl>()) {}

Manager::~Manager() = default;

bool Manager::Remove(const std::string& key) {
  auto data = GetCfgDataPtr();
  if (!data) {
    return false;
  }
  auto res = data->find(key);
  if (res == data->end()) {
    return false;
  }
  data->erase(key);
  return true;
}

void Manager::Save() {
  impl_->Save();
}

void Manager::List() {
  log::Info("cfg") << nlohmann::to_string(*(impl_->GetJsonDataPtr()));
}

JsonDataPtr Manager::GetCfgDataPtr() {
  try {
    auto ptr = impl_->GetJsonDataPtr();
    return ptr;
  } catch (RunTimeError ex) {
    log::Fatal("cfg") << ex.what();
  }
  return nullptr;
}

} // cfg
} // seeker