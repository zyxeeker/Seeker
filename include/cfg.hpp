/**
 * @file cfg.hpp
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 配置值转换
 * @version 1.0
 * @date 2023-10-17
 */

#ifndef __SEEKER_CFG_HPP__
#define __SEEKER_CFG_HPP__

#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

#include <nlohmann/json.hpp>

namespace seeker {

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
