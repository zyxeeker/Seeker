#include "cfg.h"

#include <fstream>

namespace seeker {
namespace cfg {

Manager::Manager() = default;

Manager::~Manager() {
  file_->Write(data_, true);
}

bool Manager::Start(const std::string& path) {
  file_ = std::make_shared<FileService>(path, 3);
  try {
    if (!file_->Read(data_)) {
      return false;
    }
    // std::cout << "DATA: " << nlohmann::to_string(data_) << std::endl;
  } catch(nlohmann::json::exception ex) {
    std::cout << ex.what() << std::endl;
    return false;
  }
  return true;
}

void Manager::Register(const std::string& key, 
                 std::function<void(nlohmann::json)> cb) {
  std::lock_guard<std::mutex> l(callback_ops_mutex_);
  callbacks_[key] = cb;
}

void Manager::Unregister(const std::string& key) {
  std::lock_guard<std::mutex> l(callback_ops_mutex_);
  callbacks_.erase(key);
}

nlohmann::json Manager::Query(const std::string& key) {
  std::lock_guard<std::mutex> l(data_ops_mutex_);
  // std::cout << "DATA: " << nlohmann::to_string(data_) << std::endl;
  auto res = data_.find(key);
  if (res == data_.end()) {
    return {};
  }
  return res.value();
}

void Manager::Update(const std::string& key, const nlohmann::json& json) {
  std::lock_guard<std::mutex> l(data_ops_mutex_);
  data_[key] = json;

  file_->Write(data_);
}

void Manager::Notify(const std::string& key, const nlohmann::json& json) {
  std::lock_guard<std::mutex> l(callback_ops_mutex_);
  auto e = callbacks_[key];
  if (e) {
    (e)(json);
  }
}

} // namespace cfg

bool seeker::Cfg::Init(const std::string& cfg_path) {
  return cfg::Mgr::GetInstance().Start(cfg_path);
}

nlohmann::json Cfg::Query(const std::string& key) {
  return std::move(cfg::Mgr::GetInstance().Query(key));
}

void Cfg::Update(const std::string& key, const nlohmann::json& json) {
  cfg::Mgr::GetInstance().Update(key, json);
}

void Cfg::RegisterChangedEvent(const std::string& key, JsonChangedEventCallBack cb) {
  cfg::Mgr::GetInstance().Register(key, cb);
}

} // namespace seeker