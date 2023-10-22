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
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

#include <nlohmann/json.hpp>

#define DEFINE_PROPERTIES(STRUCT, ...)                              \
  using ValueType = STRUCT;                                         \
  static constexpr auto Properties = std::make_tuple(__VA_ARGS__);  \

#define DEFINE_PROPERTY_SCHME(NAME, DST_NAME)         \
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
                   std::make_index_sequence<std::tuple_size_v<Tuple>>());
}

template <typename T>
nlohmann::json ToJson(T& value) {
  nlohmann::json json;
    TupleForEach(value.Properties, [&](const auto e) {
    using ValueType = typename decltype(e)::Type;
    if constexpr (std::is_class<ValueType>::value) {
      json[e.Name] = ToJson<ValueType>(value.*(e.Member));
    } else {
      json[e.Name] = value.*(e.Member);
    }
  });
  return json;
}

template<typename T>
T FromJson(nlohmann::json &json) {
  T value;
  TupleForEach(value.Properties, [&](const auto e) {
    using ValueType = typename decltype(e)::Type;
    if constexpr (std::is_class<ValueType>::value) {
      value.*(e.Member) = FromJson<ValueType>(json[e.Name]);
    } else {
      value.*(e.Member) = ((nlohmann::json&)json[e.Name]).get<ValueType>();
    }
  });
  return value;
}

template <typename T>
class Transfer {
 public:
  static T Convert(const nlohmann::json& value) {
    T element;
    try {
      element = value.get<T>();
    } catch (nlohmann::json::exception ex) {
      std::cout << "bad convert: " << ex.what() << std::endl;
    }
    return std::move(element);
  }
  static nlohmann::json Serialize(const T& element) {
    try {
      nlohmann::json value(element);
      return std::move(value);
    } catch (nlohmann::json::exception ex) {
      std::cout << "bad serialize: " << ex.what() << std::endl;
    }
    return {};
  }
};

template <typename U>
class Transfer<std::vector<U> > {
 public:
  static std::vector<U> Convert(const nlohmann::json& value) {
    std::vector<U> element;
    try {
      for (auto &i : value) {
        element.push_back(Transfer<U>::Convert(i));
      }
    } catch (nlohmann::json::exception ex) {
      std::cout << "bad convert: " << ex.what() << std::endl;
    }
    return std::move(element);
  }
  static nlohmann::json Serialize(const std::vector<U>& element) {
    nlohmann::json value;
    for (auto &i : element) {
      try {
        value.push_back(Transfer<U>::Serialize(i));
      } catch (nlohmann::json::exception ex) {
        std::cout << "bad serialize: " << ex.what() << std::endl;
      }
    }
    return std::move(value);
  }
};

template <typename U>
class Transfer<std::unordered_map<std::string, U> > {
 public:
  static std::unordered_map<std::string, U> Convert(const nlohmann::json& value) {
    std::unordered_map<std::string, U> element;
    try {
      for (auto &i : value.items()) {
        element.insert({ i.key(), Transfer<U>::Convert(i.value()) });
      }
    } catch (nlohmann::json::exception ex) {
      std::cout << "bad convert: " << ex.what() << std::endl;
    }
    return std::move(element);
  }
  static nlohmann::json Serialize(const std::unordered_map<std::string, U>& element) {
    nlohmann::json value;
    for (auto &i : element) {
      try {
        value[i.first] = Transfer<U>::Serialize(i.second);
      } catch (nlohmann::json::exception ex) {
        std::cout << "bad serialize: " << ex.what() << std::endl;
      }
    }
    return std::move(value);
  }
};

template <typename U>
class Transfer<std::unordered_set<U> > {
 public:
  static std::unordered_set<U> Convert(const nlohmann::json& value) {
    std::unordered_set<U> element;
    try {
      for (auto &i : value) {
        element.insert(Transfer<U>::Convert(i));
      }
    } catch (nlohmann::json::exception ex) {
      std::cout << "bad convert: " << ex.what() << std::endl;
    }
    return std::move(element);
  }
  static nlohmann::json Serialize(const std::unordered_set<U>& element) {
    nlohmann::json value;
    for (auto &i : element) {
      try {
        value.push_back(Transfer<U>::Serialize(i));
      } catch (nlohmann::json::exception ex) {
        std::cout << "bad serialize: " << ex.what() << std::endl;
      }
    }
    return std::move(value);
  }
};

} // namespace seeker

#endif // __SEEKER_CFG_HPP__
