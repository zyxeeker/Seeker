/**
 * @file cfg.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 配置管理相关类与接口
 * @version 1.0
 * @date 2023-06-06
 */

#ifndef __SEEKER_CFG_H__
#define __SEEKER_CFG_H__

#include <set>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include <type_traits>
#include <nlohmann/json.hpp>
#include "util.h"

namespace seeker {
namespace cfg {

/**
 * @brief 类型转换模板
 * @tparam SrcValueType 原元素类型
 * @tparam DstValueType 目标元素类型
 */
template<typename SrcValueType, 
         typename DstValueType,
         typename = void>
struct VarCast {
  DstValueType operator()(const SrcValueType& value) {
    std::cout << "unknown cast behavior!" << std::endl;
    return DstValueType{};
  }
};

/**
 * @brief nlohmann::json类型转bool
 */
template<>
struct VarCast<nlohmann::json, bool> {
  bool operator()(const nlohmann::json& value) {
    if (!value.is_boolean()) {
      std::cout << "bad type: the value is not BOOL type!" << std::endl;
      return bool{};
    }
    return value.get<bool>();
  }
};

/**
 * @brief nlohmann::json类型转整数类型
 * @tparam DstValueType 整数类型
 */
template<typename DstValueType>
struct VarCast<nlohmann::json, 
               DstValueType, 
               std::enable_if_t<std::is_integral<DstValueType>::value> > {
  DstValueType operator()(const nlohmann::json& value) {
    if (!value.is_number_integer()) {
      std::cout << "bad type: the value is not INT type!" << std::endl;
      return DstValueType{};
    }
    return value.get<DstValueType>();
  }
};

/**
 * @brief nlohmann::json类型转浮点类型
 * @tparam DstValueType 浮点类型
 */
template<typename DstValueType>
struct VarCast<nlohmann::json, 
               DstValueType, 
               std::enable_if_t<std::is_floating_point<DstValueType>::value> > {
  DstValueType operator()(const nlohmann::json& value) {
    if (!value.is_number_float()) {
      std::cout << "bad type: the value is not FLOAT type!" << std::endl;
      return DstValueType{};
    }
    return value.get<DstValueType>();
  }
};

/**
 * @brief nlohmann::json类型转无符号类型
 * @tparam DstValueType 无符号类型
 */
template<typename DstValueType>
struct VarCast<nlohmann::json, 
               DstValueType, 
               std::enable_if_t<std::is_unsigned<DstValueType>::value> > {
  DstValueType operator()(const nlohmann::json& value) {
    if (!value.is_number_unsigned()) {
      std::cout << "bad type: the value is not UNSIGNED type!" << std::endl;
      return DstValueType{};
    }
    return value.get<DstValueType>();
  }
};

/**
 * @brief nlohmann::json类型转顺序/关联容器
 * @tparam DstValueType 顺序/关联容器类型
 */
template<typename DstValueType>
struct VarCast<nlohmann::json, 
               DstValueType, 
               std::enable_if_t<util::is_std_vector<DstValueType>::value ||
                                util::is_std_list<DstValueType>::value ||
                                util::is_std_set<DstValueType>::value> > {
  DstValueType operator()(const nlohmann::json& value) {
    if (!value.is_array()) {
      std::cout << "bad type: the value is not Sequential type!" << std::endl;
      return DstValueType{};
    }
    auto arr = value.get<std::vector<nlohmann::json> >();
    DstValueType res;
    for (auto &i : arr)
      res.emplace_back(std::move(VarCast<nlohmann::json, 
                                         typename DstValueType::value_type>()(i)));
    return res;
  }
};

/**
 * @brief 整型/浮点型/无符号型转nlohmann::json
 * @tparam SrcValueType 整型/浮点型/无符号型
 */
template<typename SrcValueType>
struct VarCast<SrcValueType, 
               nlohmann::json, 
               std::enable_if_t<std::is_integral<SrcValueType>::value ||
                                std::is_floating_point<SrcValueType>::value ||
                                std::is_unsigned<SrcValueType>::value> > {
  nlohmann::json operator()(const SrcValueType& value) {
    return nlohmann::json(value);
  }
};

/**
 * @brief 布尔型转nlohmann::json
 */
template<>
struct VarCast<bool, nlohmann::json> {
  nlohmann::json operator()(const bool& value) {
    return nlohmann::json(value);
  }
};

/**
 * @brief 顺序/关联容器类型转nlohmann::json
 * @tparam SrcValueType 顺序/关联容器
 */
template<typename SrcValueType>
struct VarCast<SrcValueType, 
               nlohmann::json, 
               std::enable_if_t<util::is_std_vector<SrcValueType>::value ||
                                util::is_std_list<SrcValueType>::value ||
                                util::is_std_set<SrcValueType>::value> > {
  nlohmann::json operator()(const SrcValueType& value) {
    auto arr = nlohmann::json();
    for (auto &i : value) {
      arr.emplace_back(std::move(
              VarCast<typename SrcValueType::value_type,
                      nlohmann::json>()(i)));
    }
    return arr;
  }
};

/**
 * @brief 从配置管理器返回的基本数据包
 * @tparam ValueType 目标数据类型
 */
template <typename ValueType>
class Var {
 public:
  using ValuePtr = std::shared_ptr<ValueType>;
  using WatcherFunc = std::function<void(ValuePtr, ValuePtr)>;
  Var() : value_ptr_(new ValueType()) {}
  Var(nlohmann::json json_value)
    : value_ptr_(new ValueType(
          VarCast<nlohmann::json, ValueType>()(std::move(json_value)))) {}
  Var(ValueType value) 
    : value_ptr_(new ValueType(std::move(value))) {}
  /**
   * @brief 获取值
   */
  ValueType GetValue() const {
    return *value_ptr_;
  }
  /**
   * @brief 序列化 
   */
  std::string ToString() {
    return nlohmann::to_string(
              VarCast<ValueType, nlohmann::json>()(*value_ptr_));
  }
  /**
   * @brief 添加观察者回调函数
   * @param key 回调函数key
   * @param watcher 回调函数
   * @return true 添加成功时返回
   * @return false 添加失败时返回
   */
  bool AddWatcher(int key, WatcherFunc watcher) {
    auto res = watcher_list_.insert({key, watcher});
    if (!res.second)
      std::cout << "the key" << key << "is obtained by other watcher,"
                << "please remove first or use [] to cover it!" 
                << std::endl;
    return res.second;
  }
  /**
   * @brief 删除指定key的观察者回调函数
   * @param key 
   */
  void DeleteWatcher(int key) {
    watcher_list_.erase(key);
  }
  /**
   * @brief 同类型Var进行交换
   * @param var 同类型Var
   */
  void Swap(Var<ValueType> var) {
    auto this_value_ptr = value_ptr_;
    auto this_watcher_list = watcher_list_;

    set_value_ptr(var.value_ptr());
    set_watcher_list(var.watcher_list());

    var.set_value_ptr(std::move(this_value_ptr));
    var.set_watcher_list(std::move(this_watcher_list));
  }
  /**
   * @brief 添加/覆盖指定key值的观察者回调函数
   * @param key 
   * @return WatcherFunc& 观察者回调函数
   */
  WatcherFunc& operator [](int key) {
    return watcher_list_[key];
  }
  Var<ValueType>& operator =(const Var<ValueType>& var) {
    Swap(var);
    return this;
  }
  Var<ValueType>& operator =(const ValueType& value) {
    value_ptr_ = new ValueType(value);
    return this;
  }
  /**
   * @brief 获取观察者回调函数列表
   */
  const std::unordered_map<int, WatcherFunc>& watcher_list() const {
    return watcher_list_;
  }
  /**
   * @brief 设置观察者回调函数列表
   * @param watcher_list 观察者回调函数列表
   */
  void set_watcher_list(std::unordered_map<int, WatcherFunc> watcher_list) {
    watcher_list_ = std::move(watcher_list);
  }
  /**
   * @brief 获取数据指针
   */
  ValuePtr value_ptr() const {
    return value_ptr_;
  }
  /**
   * @brief 设置数据指针
   */
  void set_value_ptr(ValuePtr value_ptr) {
    // 当进行设置时调用所有的观察者函数
    for (auto &i : watcher_list_) {
      i.second(value_ptr_, value_ptr);
    }
    value_ptr_.swap(value_ptr);
  }
 private:
  /**
   * @brief 数据智能指针
   */
  std::shared_ptr<ValueType> value_ptr_;
  /**
   * @brief 观察者回调函数列表
   */
  std::unordered_map<int, WatcherFunc> watcher_list_;
};

/**
 * @brief 管理器
 */
class Manager {
 public:
  /**
   * @brief 初始化 
   */
  bool Init();
  /**
   * @brief 查询并返回需要的元素
   * @tparam ValueType 元素数据类型
   * @param key 配置文件中的key值
   * @return 未查询到则返回ValueType默认值
   */
  template<typename ValueType>
  Var<ValueType> Query(std::string key);
 private:
  std::ifstream file_stream_;
  nlohmann::json data_;
};

using Mgr = util::Single<Manager>;

template<typename ValueType>
Var<ValueType> Manager::Query(std::string key) {
  if (data_.is_null())
    return Var<ValueType>(ValueType{});
  auto res = data_.find(key);
  if (res == data_.end())
    return Var<ValueType>(ValueType{});
  return Var<ValueType>(res.value());
}

} // cfg
} // seeker

#endif // __SEEKER_CFG_H__