/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-17 17:08:11
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-08 16:09:21
 * @Description: 配置模块
 */

#ifndef __SEEKER_CFG_HPP__
#define __SEEKER_CFG_HPP__

#include <mutex>
#include <tuple>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <exception>

#include <nlohmann/json.hpp>

#define DEFINE_PROPERTIES(STRUCT, ...)                              \
  using ValueType = STRUCT;                                         \
  static constexpr auto Properties = std::make_tuple(__VA_ARGS__);  \

#define PROPERTY(NAME)                                \
  seeker::CfgVarProperty(&ValueType::NAME, #NAME)     \

#define PROPERTY_SCHME(NAME, DST_NAME)                \
  seeker::CfgVarProperty(&ValueType::NAME, DST_NAME)  \

namespace seeker {

template <typename Class, typename T>
struct CfgVarPropertyImpl {
  constexpr CfgVarPropertyImpl(T Class::*member, const char* name)
      : Name(name),
        Member(member) {}
  using Type = T;
  const char* Name;
  T Class::*Member;
};

template <typename Class, typename T>
constexpr auto CfgVarProperty(T Class::*member, const char* name) {
  return CfgVarPropertyImpl<Class, T>(member, name);
}

template <typename Tuple, typename Handler, std::size_t... Indexs>
constexpr void TupleForEachImpl(const Tuple& tuple, Handler&& handler, 
                                std::index_sequence<Indexs...>) {
  (handler(std::get<Indexs>(tuple)), ...);
}
 
template <typename Tuple, typename Handler>
constexpr void TupleForEach(const Tuple& tuple, Handler&& handler) {
  TupleForEachImpl(tuple, std::forward<Handler>(handler), 
                   std::make_index_sequence<std::tuple_size_v<Tuple> >());
}

template <typename T>
struct ToJsonImpl {
  nlohmann::json operator()(const T& value) {
    nlohmann::json json;
    if constexpr (std::is_class<T>::value) {
      TupleForEach(value.Properties, [&](const auto e) {
        using ValueType = typename decltype(e)::Type;
        json[e.Name] = ToJsonImpl<ValueType>()(value.*(e.Member));
      });
    } else {
      json = nlohmann::json(value);
    }
    return json;
  }
};

template <>
struct ToJsonImpl<std::string> {
  nlohmann::json operator()(const std::string& value) {
    return nlohmann::json(value);
  }
};

template <typename T>
struct ToJsonImpl<std::vector<T> > {
  nlohmann::json operator()(const std::vector<T>& value) {
    nlohmann::json json;
    for (auto& i : value) {
      json.emplace_back(ToJsonImpl<T>()(i));
    }
    return json;
  }
};

template <typename T>
static bool ToString(const T& src, std::string& dst) {
  try {
    auto res = (nlohmann::json)ToJsonImpl<T>()(src);
    dst = std::move(nlohmann::to_string(res));
  } catch (...) {
    return false;
  }
  return true;
}

template <typename T>
static bool ToJson(const T& src, nlohmann::json& dst) {
  try {
    dst = ToJsonImpl<T>()(src);
  } catch (...) {
    return false;
  }
  return true;
}

template <typename T>
struct FromJsonImpl {
  T operator()(const nlohmann::json& json) {
    T value;
    if constexpr (std::is_class<T>::value) {
      TupleForEach(value.Properties, [&](const auto e) {
        using ValueType = typename decltype(e)::Type;
        auto res = json.find(e.Name);
        if (res == json.end()) {
          throw std::runtime_error("prase error: not find key!");
          value.*(e.Member) = ValueType {};
        } else {
          value.*(e.Member) = FromJsonImpl<ValueType>()(json[e.Name]);
        }
      });
    } else {
      value = json.get<T>();
    }
    return value;
  }
};

template <>
struct FromJsonImpl<std::string> {
  std::string operator()(const nlohmann::json& json) {
    return json.get<std::string>();
  }
};

template <typename T>
struct FromJsonImpl<std::vector<T> > {
  std::vector<T> operator()(const nlohmann::json& json) {
    std::vector<T> value;
    for (auto& i : json) {
      value.emplace_back(FromJsonImpl<T>()(i));
    }
    return value;
  }
};


template <typename T>
static bool FromString(const std::string& src, T& dst) {
  nlohmann::json json;
  try {
    json = nlohmann::json::parse(src);
    dst = std::move(FromJsonImpl<T>()(json));
  } catch(...) {
    return false;
  }
  return true;
}

template <typename T>
static bool FromJson(const nlohmann::json& src, T& dst) {
  try {
    dst = std::move(FromJsonImpl<T>()(src));
  } catch(...) {
    return false;
  }
  return true;
}

class Cfg {
 public:
  // enum UPDATE_METHOD {
  //   UPDATE_BY_CHANGED_NUMS,
  //   UPDATE_BY_TIME_INTERVAL
  // };
  struct Meta {
    std::string Name;
    std::string Path;
    // UPDATE_METHOD UpdateMethod;
    uint32_t Nums;
  };
 public:
  Cfg(size_t th_size);
  ~Cfg();

  bool Init(std::initializer_list<Meta>&& list);
  void Deinit();
  
  template <typename T>
  bool Append(const std::string& cfg_name, const std::string& key, const T& value) {
    nlohmann::json json;
    if (!seeker::ToJson(value, json)) {
      return false;
    }
    if (!AppendImpl(cfg_name, key, json)) {
      return false;
    }
    return true;
  }
  template <typename T>
  bool Query(const std::string& cfg_name, const std::string& key, T& value) {
    nlohmann::json json;
    if (!QueryImpl(cfg_name, key, json)) {
      return false;
    }
    if (!seeker::FromJson(json, value)) {
      std::cout << "convert failed" << std::endl;
      return false;
    }
    return true;
  }
  template <class C, typename Old, typename New>
  bool RegisterListener(const std::string& key, 
                        const std::string& name, C* c, bool (C::*M)(const Old&, New&)) {
    auto func = [=](const Old& old_value, New& new_value) {
      return (c->*M)(std::forward<const Old&>(old_value), std::forward<New&>(new_value));
    };
    auto cb = [func](const nlohmann::json& old_value, const nlohmann::json& new_value) {
      Old old_conv_value;
      New new_conv_value;
      if (!seeker::FromJson(old_value, old_conv_value) || 
          !seeker::FromJson(new_value, new_conv_value)) {
        return false;
      }
      return func(old_conv_value, new_conv_value);
    };
    return RegisterListenerImpl(key, name, cb);
  }
  bool UnregisterListener(const std::string& key, const std::string& name) {
    return UnregisterListenerImpl(key, name);
  }
  template <typename T>
  bool Update(const std::string& cfg_name, const std::string& key, const T& value) {
    nlohmann::json json;
    if (!seeker::ToJson(value, json)) {
      return false;
    }
    return UpdateImpl(cfg_name, key, json);
  }
 private:
  bool QueryImpl(const std::string& cfg_name, const std::string& key, nlohmann::json& value);
  bool AppendImpl(const std::string& cfg_name, const std::string& key, nlohmann::json& value);
  bool UpdateImpl(const std::string& cfg_name, const std::string& key, const nlohmann::json value);
  bool RegisterListenerImpl(const std::string& key, const std::string& name,
                            std::function<bool(const nlohmann::json&, const nlohmann::json&)> cb);
  bool UnregisterListenerImpl(const std::string& key, const std::string& name);
 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace seeker

#endif // __SEEKER_CFG_HPP__
