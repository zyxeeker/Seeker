/**
 * @file cfg.hpp
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 配置值转换
 * @version 1.0
 * @date 2023-10-17
 */

#ifndef __SEEKER_CFG_HPP__
#define __SEEKER_CFG_HPP__

#include <mutex>
#include <tuple>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

#include <nlohmann/json.hpp>

#define DEFINE_PROPERTIES(STRUCT, ...)                              \
  using ValueType = STRUCT;                                         \
  static constexpr auto Properties = std::make_tuple(__VA_ARGS__);  \

#define PROPERTY_SCHME(NAME, DST_NAME)         \
  seeker::CfgVarProperty(&ValueType::NAME, DST_NAME)  \

namespace seeker {

template <typename Class, typename T>
struct CfgVarPropertyImpl {
  constexpr CfgVarPropertyImpl(T Class::*member, const char* name)
      : Member(member), 
        Name(name) {}
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
    TupleForEach(value.Properties, [&](const auto e) {
      using ValueType = typename decltype(e)::Type;
      if constexpr (std::is_class<ValueType>::value) {
        json[e.Name] = ToJsonImpl<ValueType>()(value.*(e.Member));
      } else {
        json[e.Name] = value.*(e.Member);
      }
    });
    return std::move(json);
  }
};

template <typename T>
struct ToJsonImpl<std::vector<T> > {
  nlohmann::json operator()(const std::vector<T>& value) {
    nlohmann::json json;
    for (auto& i : value) {
      json.push_back(std::move(ToJsonImpl<T>()(i)));
    }
    return std::move(json);
  }
};

template <typename T>
struct ToJsonImpl<std::unordered_set<T> > {
  nlohmann::json operator()(const std::unordered_set<T>& value) {
    nlohmann::json json;
    for (auto& i : value) {
      json.push_back(std::move(ToJsonImpl<T>()(i)));
    }
    return std::move(json);
  }
};

template <typename T>
struct ToJsonImpl<std::unordered_map<std::string, T> > {
  nlohmann::json operator()(const std::unordered_map<std::string, T>& value) {
    nlohmann::json json;
    for (auto& i : value) {
      json[i.first] = std::move(ToJsonImpl<T>()(i.second));
    }
    return std::move(json);
  }
};

template <typename T>
nlohmann::json ToJson(const T& value) {
  return ToJsonImpl<T>()(value);
}

template <typename T>
struct FromJsonImpl {
  T operator()(const nlohmann::json& json) {
    T value;
    TupleForEach(value.Properties, [&](const auto e) {
      using ValueType = typename decltype(e)::Type;
      if constexpr (std::is_class<ValueType>::value) {
        value.*(e.Member) = FromJsonImpl<ValueType>()(json[e.Name]);
      } else {
        value.*(e.Member) = ((nlohmann::json&)json[e.Name]).get<ValueType>();
      }
    });
    return std::move(value);
  }
};

template <typename T>
struct FromJsonImpl<std::vector<T> > {
  std::vector<T> operator()(const nlohmann::json& json) {
    std::vector<T> value;
    for (auto& i : json) {
      value.push_back(std::move(FromJsonImpl<T>()(i)));
    }
    return std::move(value);
  }
};

template <typename T>
struct FromJsonImpl<std::unordered_set<T> > {
  std::unordered_set<T> operator()(const nlohmann::json& json) {
    std::unordered_set<T> value;
    for (auto& i : json) {
      value.insert(std::move(FromJsonImpl<T>()(i)));
    }
    return std::move(value);
  }
};

template <typename T>
struct FromJsonImpl<std::unordered_map<std::string, T> > {
  std::unordered_map<std::string, T> operator()(const nlohmann::json& json) {
    std::unordered_map<std::string, T> value;
    for (auto& i : json.items()) {
      value.insert({ i.key(), FromJsonImpl<T>()(i.value()) });
    }
    return std::move(value);
  }
};

template <typename T>
T FromJson(const nlohmann::json& json) {
  return FromJsonImpl<T>()(json);
}

template <typename T>
class CfgVar {
 public:
  static CfgVar<T>& Get() {
    static CfgVar<T> _inst;
    return _inst;
  }
  void AddCallBack(std::function<void(T)> cb) {
    std::lock_guard<std::mutex> l(mutex_);
    callbacks_.push_back(cb);
  }
  void RemoveCallBack(std::function<void(T)> cb) {
    std::lock_guard<std::mutex> l(mutex_);
    for (auto i : callbacks_) {
      if ((*i) == cb) {
        callbacks_.erase(i);
        break;
      }
    }
  }
  void Update(const T& t) {
    std::lock_guard<std::mutex> l(mutex_);
    for (auto &i : callbacks_) {
      (i)(t);
    }
  }
  std::function<void(nlohmann::json)> GetNotifyCallBack() {
    std::function<void(nlohmann::json)> func = 
        std::bind(&seeker::CfgVar<T>::Notify, this, std::placeholders::_1);
    return func;  
  }
 protected:
  void Notify(nlohmann::json json) {
    T value = FromJson<T>(json);
    {
      std::lock_guard<std::mutex> l(mutex_); 
      for (auto &i : callbacks_) {
        (i)(value);
      }
    }
  }
 private:
  std::mutex mutex_;
  std::vector<std::function<void(T)> > callbacks_;
};

bool InitializeCfg(const std::string& cfg_path);

void RegisterCfgChangedEvent(const std::string& key, 
                             std::function<void(nlohmann::json)> cb);

template <typename T>
void RegisterCfgChangedEvent(const std::string& key, 
                             std::function<void(T)> cb) {
  seeker::CfgVar<T>::Get().AddCallBack(cb);
  RegisterCfgChangedEvent(key, seeker::CfgVar<T>::Get().GetNotifyCallBack());
}

template <typename T>
void UnRegisterCfgChangedEvent(std::function<void(T)> cb) {
  seeker::CfgVar<T>::Get().RemoveCallBack(cb);
}

nlohmann::json QueryCfg(const std::string& key);

template <typename T>
T QueryCfg(const std::string& key) {
  nlohmann::json json = QueryCfg(key);
  return FromJson<T>(json);
}

void UpdateCfg(const std::string& key, const nlohmann::json& json);

template <typename T>
void UpdateCfg(const std::string& key, const T& value) {
  UpdateCfg(key, ToJson(value));
  seeker::CfgVar<T>::Get().Update(value);
}

} // namespace seeker

#endif // __SEEKER_CFG_HPP__
