/**
 * @file cfg.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 配置管理具体实现
 * @version 1.0
 * @date 2023-06-06
 */

#ifndef __SEEKER_SRC_CFG__
#define __SEEKER_SRC_CFG__

#include "../include/cfg.h"
#include "log.h"

namespace seeker {
namespace cfg {

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

  nlohmann::json& GetJsonData();
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
} // seeker

#endif // __SEEKER_SRC_CFG__