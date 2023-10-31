#include <iostream>
#include <string>
#include <vector>

#include <log.h>
#include <thread.h>
#include <cfg.hpp>

void Test1() {
  while(1) {
    seeker::log::Debug("system") << "****************";
  }
}

void Test2() {
  int i = 0;
  while (1) {
    seeker::log::Debug("cfg") << "================" << i++;
  }
}

int main() {
  std::cout << std::boolalpha << seeker::Cfg::Init("test.json") << std::endl;
#if 1
  std::string s{"TEST"};
  seeker::log::Info() << "TEST" << 1111 << 2222 << s << std::boolalpha << true;
  
  std::vector<seeker::th::Thread::Ptr> ths;
  for (int i = 0; i < 5; i++) {
    ths.push_back(seeker::th::Thread::Ptr(new seeker::th::Thread(&Test1, "VNA#1" + std::to_string(i))));
    ths.push_back(seeker::th::Thread::Ptr(new seeker::th::Thread(&Test2, "VNA#2" + std::to_string(i))));
  }

  for (auto &i : ths) {
    i->Join();
  }
#endif
  seeker::log::Debug() << "TETEE";
  seeker::log::Info() << "TETEE";
  seeker::log::Warn() << "TETEE";
  seeker::log::Error() << "TETEE";
  seeker::log::Fatal() << "TETEE";

  seeker::log::Debug("system") << "system";
  seeker::log::Debug("cfg") << "cfg";
}