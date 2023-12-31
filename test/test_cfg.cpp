#include <fstream>
#include <set>
#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

#include <nlohmann/json.hpp>

#include "cfg.hpp"

struct TestB {
  int a;

  DEFINE_PROPERTIES(
    TestB,
    PROPERTY_SCHME(a, "a")
  )
};

struct Test {
  int a;
  double A;
  struct TestB b;

  DEFINE_PROPERTIES(
    Test,
    PROPERTY_SCHME(A, "A"),
    PROPERTY_SCHME(a, "a"),
    PROPERTY_SCHME(b, "b")
  )
};

bool operator==(const Test& lhs, const Test& rhs) {
  return lhs.a == rhs.a && 
         lhs.A == rhs.A && 
         lhs.b.a == rhs.b.a;
}

template<>
struct std::hash<Test>
{
  std::size_t operator()(const Test& p) const noexcept
  {
    return std::hash<int>()(p.a) ^ 
           std::hash<double>()(p.A) ^ 
           std::hash<int>()(p.b.a);
  }
};

class TA {
 public:
  TA() {
    std::function<void(TestB)> func = std::bind(&TA::OnChanged, this, std::placeholders::_1);
    std::function<void(TestB)> func1 = std::bind(&TA::OnChanged1, this, std::placeholders::_1);
    seeker::Cfg::RegisterChangedEvent<TestB>("A1", func);
    seeker::Cfg::RegisterChangedEvent<TestB>("B1", func1);
  }
  void OnChanged(TestB t) {
    std::cout << "A1 CHANGED: " << t.a << std::endl;
  }
  void OnChanged1(TestB t) {
    std::cout << "B1 CHANGED1: " << t.a << std::endl;
  }
};

class TB {
 public:
  TB() {
    std::function<void(TestB)> func = std::bind(&TB::OnChanged, this, std::placeholders::_1);
    std::function<void(TestB)> func1 = std::bind(&TB::OnChanged1, this, std::placeholders::_1);
    seeker::Cfg::RegisterChangedEvent<TestB>("A", func);
    seeker::Cfg::RegisterChangedEvent<TestB>("B", func1);
  }
  void OnChanged(TestB t) {
    std::cout << "A CHANGED: " << t.a << std::endl;
  }
  void OnChanged1(TestB t) {
    std::cout << "B CHANGED1: " << t.a << std::endl;
  }
};

int main() {
  std::cout << std::boolalpha << seeker::Cfg::Init("test.json") << std::endl;
  
  auto int_json = seeker::Cfg::Query<int>("int");
  std::cout << "INT: " << int_json << std::endl;
  int_json = 200;
  seeker::Cfg::Update("int", int_json);

  TA ta;
  TB tb;
  auto test_json = seeker::Cfg::Query<TestB>("test");
  std::cout << "STRUCT TESTB a:" << test_json.a << std::endl;
  test_json.a = 2;
  seeker::Cfg::Update("test", test_json);
  test_json.a = 3;
  seeker::Cfg::Update("test", test_json);
  test_json.a = 4;
  seeker::Cfg::Update("test", test_json);
#if 0
  std::fstream fs;
  fs.open("test.json", std::ios::in);
  nlohmann::json data;
  try {
    data = nlohmann::json::parse(fs);
  } catch(nlohmann::json::exception ex) {
    std::cout << ex.what() << std::endl;
  }

  Test test;
  test.a = 1;
  test.A = 2.2f;
  test.b.a = 100;
  auto test_json = seeker::ToJson(test);
  auto test_json_str = nlohmann::to_string(test_json);
  std::cout << "TEST STR1: " << test_json_str << std::endl;

  auto test_struct = seeker::FromJson<Test>(test_json);
  auto test_struct_json = seeker::ToJson(test_struct);
  auto test_struct_json_str = nlohmann::to_string(test_struct_json);
  std::cout << "TEST STR2: " << test_struct_json_str << std::endl;

  std::vector<Test> vec{
    { 1, 2.3f , { 500 } },
    { 3, 2.3f , { 600 } },
  };

  auto vec_json = seeker::ToJson(vec);
  auto vec_json_str = nlohmann::to_string(vec_json);
  std::cout << "VEC STR1: " << vec_json_str << std::endl;
  auto json_vec = seeker::FromJson<std::vector<Test> >(vec_json);
  auto json_vec_json = seeker::ToJson(json_vec);
  auto json_vec_json_str = nlohmann::to_string(json_vec_json);
  std::cout << "VEC STR2: " << json_vec_json_str << std::endl;

  std::unordered_set<Test> set {
    { 11, 12.3f , { 5100 } },
    { 31, 13.3f , { 6100 } },
  };

  auto set_json = seeker::ToJson(set);
  auto set_json_str = nlohmann::to_string(set_json);
  std::cout << "SET STR1: " << set_json_str << std::endl;
  auto json_set = seeker::FromJson<std::unordered_set<Test>>(set_json);
  auto json_set_json = seeker::ToJson(json_set);
  auto json_set_json_str = nlohmann::to_string(json_set_json);
  std::cout << "SET STR2: " << json_set_json_str << std::endl;

  std::unordered_map<std::string, Test> map {
    {"A", { 11, 12.3f , { 5100 } }},
    {"B", { 31, 13.3f , { 6100 } }},
  };
  
  auto map_json = seeker::ToJson(map);
  auto map_json_str = nlohmann::to_string(map_json);
  std::cout << "MAP STR1: " << map_json_str << std::endl;
  auto json_map = seeker::FromJson<std::unordered_map<std::string, Test>>(map_json);
  auto json_map_json = seeker::ToJson(json_map);
  auto json_map_json_str = nlohmann::to_string(json_map_json);
  std::cout << "MAP STR2: " << json_map_json_str << std::endl;
  
  // TestB testb;
  // testb.a = 2;
  // seeker::TupleForEach(testb.Properties, [&](const auto& e){
  //   std::cout << "AA1: " << testb.*(e.Member) << std::endl;
  // });
#endif

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