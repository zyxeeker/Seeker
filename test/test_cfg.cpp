#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include "cfg.h"

int main() {
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
  // REMOVE
  seeker::cfg::Mgr::GetInstance().Remove("add_bool");
  seeker::cfg::Mgr::GetInstance().List();
  // MODIFY
  seeker::cfg::Mgr::GetInstance().Modify("mod_bool", float_num);
  seeker::cfg::Mgr::GetInstance().List();
}