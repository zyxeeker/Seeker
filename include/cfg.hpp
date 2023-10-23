/**
 * @file cfg.hpp
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 配置值转换
 * @version 1.0
 * @date 2023-10-17
 */

#ifndef __SEEKER_CFG_HPP__
#define __SEEKER_CFG_HPP__

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

} // namespace seeker

#endif // __SEEKER_CFG_HPP__
