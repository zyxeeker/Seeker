/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-06-06 11:36:44
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-08 16:09:45
 * @Description: 配置管理具体实现
 */

#ifndef __SEEKER_SRC_CFG__
#define __SEEKER_SRC_CFG__

#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include <condition_variable>

#include <nlohmann/json.hpp>

#include "../include/cfg.hpp"
#include "../include/thread.hpp"

namespace seeker {

class Cfg::Impl {
  struct Listener {
    std::string Name;
    std::function<bool(const nlohmann::json&, const nlohmann::json&)> CallBack;
  };

  struct JsonMeta {
    struct Cfg::Meta Info;
    bool Changed;
    nlohmann::json Data;
  };
 public:
  Impl(size_t th_nums);
  ~Impl();

  bool Init(std::initializer_list<Meta>&& list);
  void Deinit();
  bool Query(const std::string& cfg_name, const std::string& key, nlohmann::json& value);
  bool Append(const std::string& cfg_name, const std::string& key, nlohmann::json& value);
  bool Update(const std::string& cfg_name, const std::string& key, const nlohmann::json value);
  bool RegisterListener(const std::string& key, const std::string& name,
                        std::function<bool(const nlohmann::json&, const nlohmann::json&)> cb);
  bool UnregisterListener(const std::string& key, const std::string& name);
 private:
  bool CheckExist(const std::string& cfg_name, const std::string& key, 
                  std::unordered_map<std::string, JsonMeta>::iterator& cfg,
                  nlohmann::json::iterator& json);
  bool ReadFile(std::vector<Meta>& list);
  bool WriteFile();
  // TODO: Modify In Future
  void WriterThread();

  void UpdateTask(const std::string cfg_name, const std::string key, const nlohmann::json value);
 private:
  bool start_;
  std::mutex mutex_;
  std::unordered_map<std::string, std::vector<Listener> > listeners_;
  std::unordered_map<std::string, JsonMeta> jsons_;
  std::unique_ptr<seeker::ThreadPool> th_;
  // TODO: Modify In Future
  std::thread writer_;
};

} // namespace seeker

#endif // __SEEKER_SRC_CFG__