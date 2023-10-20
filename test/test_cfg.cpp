#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>

#include "cfg.hpp"

int main() {
  std::fstream fs;
  fs.open("test.json", std::ios::in);
  nlohmann::json data;
  try {
    data = nlohmann::json::parse(fs);
  } catch(nlohmann::json::exception ex) {
    std::cout << ex.what() << std::endl;
  }

  auto i = seeker::Transfer<int>::Convert(data["int"]);
  std::cout << "res: " << i << std::endl;
  auto json_i = seeker::Transfer<int>::Serialize(i);
  std::cout << "json_res: " << nlohmann::to_string(json_i) << std::endl;
  
  auto obj = seeker::Transfer<std::unordered_map<std::string, std::string> >::Convert(data["object"]);
  for (auto &e : obj) {
    std::cout << "obj(" << e.first << ", " << e.second << ") " << std::endl;
  }
  auto json_obj = seeker::Transfer<std::unordered_map<std::string, std::string> >::Serialize(obj);
  std::cout << "json_obj: " << nlohmann::to_string(json_obj) << std::endl;

  auto set = seeker::Transfer<std::unordered_set<int> >::Convert(data["set"]);
  for (auto &e : set) {
    std::cout << "set(" << e << ") " << std::endl;
  }
  auto json_set = seeker::Transfer<std::unordered_set<int> >::Serialize(set);
  std::cout << "json_set: " << nlohmann::to_string(json_set) << std::endl;

#if 0
  // Array
  auto res = seeker::cfg::Mgr::GetInstance().Query<std::vector<int> >("arr");
  std::cout << res.GetValue().size() << std::endl;
  for (auto i : res.GetValue()) {
    std::cout << i << std::endl;
  }
  // INT
  seeker::cfg::Var<int> int_num;
  int_num.AddChangedEventCb([=](seeker::cfg::Var<int>::ValuePtr old_value,
                                seeker::cfg::Var<int>::ValuePtr new_value) {
    std::cout << "OLD:" << *(old_value) << std::endl;
    std::cout << "NEW:" << *(new_value) << std::endl;
  });

  auto cb = [=](seeker::cfg::Var<int>::ValuePtr old_value,
                            seeker::cfg::Var<int>::ValuePtr new_value) {
    std::cout << "OLD#1:" << *(old_value) << std::endl;
    std::cout << "NEW#1:" << *(new_value) << std::endl;
  };
  int_num.AddChangedEventCb(cb);
  auto int_num_res = seeker::cfg::Mgr::GetInstance().Query<int>("int");
  int_num.set_value_ptr(int_num_res.value_ptr());
  std::cout << "INT VALUE: " << *(int_num.value_ptr()) << std::endl;
  std::cout << "INT STR: " << int_num.ToString() << std::endl;
  // FLOAT
  auto float_num = seeker::cfg::Mgr::GetInstance().Query<double>("float");
  std::cout << "FLOAT VALUE: " << *(float_num.value_ptr()) << std::endl;
  std::cout << "FLOAT STR: " << float_num.ToString() << std::endl;
  // BOOL
  auto bool_num = seeker::cfg::Mgr::GetInstance().Query<bool>("bool");
  std::cout << std::boolalpha << "bool: " << bool_num.GetValue() << std::endl;
  // ADD
  seeker::cfg::Mgr::GetInstance().Add("add_bool",bool_num);
  seeker::cfg::Mgr::GetInstance().Add("mod_bool",bool_num);
  seeker::cfg::Mgr::GetInstance().List();
  seeker::cfg::Mgr::GetInstance().Save();
  // REMOVE
  // seeker::cfg::Mgr::GetInstance().Remove("add_bool");
  // seeker::cfg::Mgr::GetInstance().List();
  // // MODIFY
  // seeker::cfg::Mgr::GetInstance().Modify("mod_bool", float_num);
  // seeker::cfg::Mgr::GetInstance().List();
#endif
}