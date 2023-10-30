/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-06-06 11:36:44
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-26 09:27:18
 * @Description: 配置管理具体实现
 */

#ifndef __SEEKER_SRC_CFG__
#define __SEEKER_SRC_CFG__

#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>

#include <nlohmann/json.hpp>

#include "../include/cfg.h"
#include "log.h"

namespace seeker {
namespace cfg {

static const char* MODULE_NAME = "seeker::cfg";

/**
 * @brief nlohmann::json类型转换为log::LoggerOutputerJsonObj
 * @tparam  
 */
template<>
struct VarCast<nlohmann::json, log::LoggerOutputerJsonObj> {
  log::LoggerOutputerJsonObj operator ()(const nlohmann::json &value) {
    log::LoggerOutputerJsonObj outputer_;
    if (!value.is_object()) {
      std::cout << "bad type: the value is not LoggerOutputerJsonObj!" << std::endl;
      return outputer_;
    }
    if (!value.contains("type") && !value["type"].is_string())
      return outputer_;
    
    auto type = value["type"].get<std::string>();
    if (type == "file") {
      outputer_.type_ = log::OUTPUT_FILE;
      if (value.contains("path") && value["path"].is_string()) {
        outputer_.path_ = value["path"].get<std::string>();
      }
    } else if (type == "std") {
      outputer_.type_ = log::OUTPUT_STD;
    }
    return outputer_;
  }
};

/**
 * @brief nlohmann::json类型转换为log::LoggerJsonObj
 */
template<>
struct VarCast<nlohmann::json, log::LoggerJsonObj> {
  log::LoggerJsonObj operator ()(const nlohmann::json &value) {
    log::LoggerJsonObj obj_;
    if (!value.is_object()) {
      std::cout << "bad type: the value is not LoggerJsonObj!" << std::endl;
      return obj_;
    }

    if (!value.contains("name") && !value["name"].is_string())
      return obj_;
    // name
    obj_.name_ = value["name"].get<std::string>();
    // level
    if (value.contains("level")) 
      obj_.level_ = value["level"].get<std::string>();
    // format
    if (value.contains("format")) 
      obj_.formatting_str_ = value["format"].get<std::string>();
    // output
    if (value.contains("output") && value["output"].is_array()) {
      for (auto &i : value["output"]) {
        obj_.output_arr_.emplace_back(
            std::move(VarCast<nlohmann::json, log::LoggerOutputerJsonObj>()(i)));
      }
    }
    return obj_;
  }
};

/**
 * @brief 管理器中间实现
 */
class Manager::Impl {
 public:
  Impl() = default;
  ~Impl() = default;
  /**
   * @brief 初始化并解析
   */
  void Init();
  /**
   * @brief 保存至文件
   */
  void Save();
  /**
   * @brief 获取Json数据指针
   */
  JsonDataPtr GetJsonDataPtr();
 private:
  /**
   * @brief 数据互斥锁, 防止使用Query时还未初始化
   */
  static th::Mutex& GetMutex() {
    static th::Mutex mutex_;
    return mutex_;
  }
};

} // cfg

class CfgMgr {
 public:
  static CfgMgr& GetInstance() {
    static CfgMgr inst;
    return inst;
  }

  bool Start(const std::string path);
  void Register(const std::string& key, 
                std::function<void(nlohmann::json)> cb);
  void Unregister(const std::string& key);
  nlohmann::json Query(const std::string& key);
  void Update(const std::string& key, const nlohmann::json& json);
  void Notify(const std::string& key, const nlohmann::json& json);

 private:
  CfgMgr();
  ~CfgMgr();

  bool ReadFile();
  void WriteFile();

 private:
  std::mutex callback_ops_mutex_;
  std::mutex data_ops_mutex_;

  std::string path_;
  nlohmann::json data_;
  std::unordered_map<std::string, 
                     std::function<void(nlohmann::json)> > callbacks_;

  DO_NOT_ASSIGN_AND_COPY(CfgMgr)
};

} // seeker

#endif // __SEEKER_SRC_CFG__