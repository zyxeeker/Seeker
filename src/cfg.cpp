#include "cfg.h"

#include <unistd.h>

#include <fstream>

namespace seeker {
Cfg::Impl::Impl(size_t th_nums)
    : start_(false),
      th_(std::make_unique<seeker::ThreadPool>(th_nums)) {}

Cfg::Impl::~Impl() {
  Deinit();
}

bool Cfg::Impl::Init(std::initializer_list<Meta>&& list) {
  if (start_) {
    return true;
  }
  std::lock_guard<std::mutex> l(mutex_);
  th_->Start();
  
  std::vector<Meta> meta = list;
  ReadFile(meta);

  start_ = true;
  writer_ = std::thread(&Cfg::Impl::WriterThread, this);

  return true;
}

void Cfg::Impl::Deinit() {
  if (!start_) {
    return;
  }
  th_->Stop();
  start_ = false;
  writer_.join();
}

bool Cfg::Impl::Query(const std::string& cfg_name, const std::string& key, nlohmann::json& value) {
  std::unordered_map<std::string, JsonMeta>::iterator cfg;
  nlohmann::json::iterator json;
  if (!CheckExist(cfg_name, key, cfg, json)) {
    return false;
  }
  value = json.value();
  return true;
}

bool Cfg::Impl::Append(const std::string& cfg_name, const std::string& key, nlohmann::json& value) {
  std::lock_guard<std::mutex> l(mutex_);
  auto cfg = jsons_.find(cfg_name);
  if (cfg == jsons_.end()) {
    return false;
  }
  cfg->second.Data[key] = value;
  return true;
}

bool Cfg::Impl::Update(const std::string& cfg_name, const std::string& key, const nlohmann::json value) {
  auto func = std::bind(&Cfg::Impl::UpdateTask, this, 
                        std::placeholders::_1, std::placeholders::_2, 
                        std::placeholders::_3);
  th_->CreateTask("notifyCfg", func, cfg_name, key, value);
  return true;
}

bool Cfg::Impl::RegisterListener(const std::string& key, const std::string& name,
                                   std::function<bool(const nlohmann::json&, const nlohmann::json&)> cb) {
  std::lock_guard<std::mutex> l(mutex_);
  for (auto& i : listeners_[key]) {
    if (i.Name == name) {
      return false;
    }
  }
  Listener Listener;
  Listener.Name = std::move(name);
  Listener.CallBack = std::move(cb);
  listeners_[key].emplace_back(Listener);
  return true;
}

bool Cfg::Impl::UnregisterListener(const std::string& key, const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  auto& arr = listeners_[key];
  for (auto i = arr.begin(); i != arr.end(); i++) {
    if (i->Name == name) {
      listeners_[key].erase(i);
      return true;
    }
  }
  return false;
}
bool Cfg::Impl::CheckExist(const std::string& cfg_name, const std::string& key, 
                             std::unordered_map<std::string, JsonMeta>::iterator& cfg,
                             nlohmann::json::iterator& json) {
  cfg = jsons_.find(cfg_name);
  if (cfg == jsons_.end()) {
    return false;
  }
  
  json = cfg->second.Data.find(key);
  if (json == cfg->second.Data.end()) {
    return false;
  }
  return true;
}

bool Cfg::Impl::ReadFile(std::vector<Meta>& list) {
  for (auto& i : list) {
    JsonMeta json;
    std::ifstream ifs(i.Path);
    if (!ifs.good()) {
      continue;
    }
    try {
      ifs >> json.Data;
    } catch (...) {}
    
    json.Info = std::move(i);
    auto res = jsons_.insert({ json.Info.Name, json });
    if (!res.second) {
      std::cout << "conflict name: " << json.Info.Name << std::endl;
    }
    ifs.close();
  }
  return true;
}

bool Cfg::Impl::WriteFile() {
  for (auto& i : jsons_) {
    if (!i.second.Changed) {
      continue;
    }
    std::ofstream ofs(i.second.Info.Path);
    if (!ofs.good()) {
      continue;
    }
    ofs << i.second.Data;
    ofs.close();
    i.second.Changed = false;
  }
#if __linux__
  system("sync");
#endif
  return true;
}

// TODO: Modify In Future
void Cfg::Impl::WriterThread() {
  while (start_) {
    {
      std::lock_guard<std::mutex> l(mutex_);
      WriteFile();
    }
    usleep(500);
  }
}

void Cfg::Impl::UpdateTask(const std::string cfg_name, const std::string key, const nlohmann::json value) {
  std::lock_guard<std::mutex> l(mutex_);
  std::unordered_map<std::string, JsonMeta>::iterator cfg;
  nlohmann::json::iterator json;
  if (!CheckExist(cfg_name, key, cfg, json)) {
    return;
  }
  bool need_revert = false;
  for (auto& i : listeners_[key]) {
    if (!i.CallBack(json.value(), value)) {
      need_revert = true;
      break;
    }
  }
  // revert cfg
  if (need_revert) {
    for (auto& i : listeners_[key]) {
      i.CallBack(value, json.value());
    }
  } else {
    cfg->second.Data[key] = value;
    cfg->second.Changed = true;
  }
}

Cfg::Cfg(size_t th_size)
    : impl_(new Impl(th_size)) {}

Cfg::~Cfg() = default;

bool Cfg::Init(std::initializer_list<Meta>&& list) {
  return impl_->Init(std::forward<decltype(list)>(list));
}

void Cfg::Deinit() {
  impl_->Deinit();
}

bool Cfg::QueryImpl(const std::string& cfg_name, const std::string& key, nlohmann::json& value) {
  return impl_->Query(cfg_name, key, value);
}

bool Cfg::AppendImpl(const std::string& cfg_name, const std::string& key, nlohmann::json& value) {
  return impl_->Append(cfg_name, key, value);
}

bool Cfg::UpdateImpl(const std::string& cfg_name, const std::string& key, const nlohmann::json value) {
  return impl_->Update(cfg_name, key, value);
}

bool Cfg::RegisterListenerImpl(const std::string& key, const std::string& name,
                                 std::function<bool(const nlohmann::json&, const nlohmann::json&)> cb) {
  return impl_->RegisterListener(key ,name, cb);
}

bool Cfg::UnregisterListenerImpl(const std::string& key, const std::string& name) {
  return impl_->UnregisterListener(key ,name);
}

} // namespace seeker