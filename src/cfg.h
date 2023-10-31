/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-06-06 11:36:44
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-31 12:03:11
 * @Description: 配置管理具体实现
 */

#ifndef __SEEKER_SRC_CFG__
#define __SEEKER_SRC_CFG__

#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>

#include <nlohmann/json.hpp>

#include "../include/cfg.hpp"

#include "util.hpp"

namespace seeker {
namespace cfg {

class Manager {
 public:
  Manager();
  ~Manager();

  bool Start(const std::string path);
  void Register(const std::string& key, 
                std::function<void(nlohmann::json)> cb);
  void Unregister(const std::string& key);
  nlohmann::json Query(const std::string& key);
  void Update(const std::string& key, const nlohmann::json& json);
  void Notify(const std::string& key, const nlohmann::json& json);

 private:
  bool ReadFile();
  void WriteFile();

 private:
  std::mutex callback_ops_mutex_;
  std::mutex data_ops_mutex_;

  std::string path_;
  nlohmann::json data_;
  std::unordered_map<std::string, 
                     std::function<void(nlohmann::json)> > callbacks_;
};

using Mgr = util::Single<Manager>;

} // namespace cfg
} // namespace seeker

#endif // __SEEKER_SRC_CFG__