#include <iostream>
#include <vector>
// #include "../src/thread.h"
#include "thread.h"

int gCount = 0;
seeker::th::Mutex gMutex;

void func() {
  std::cout <<"111" << std::endl;
  std::cout << "func: " 
            << seeker::th::GetThreadName() 
            << std::endl;
}

void CountFunc() {
  for (int i = 0; i < 100000; i++) {
    // gMutex.Lock();
    seeker::th::MutexGuard sg(gMutex);
    ++gCount;
    // gMutex.Unlock();
  }
}

void test() {
  //seeker::thread::Thread t(&func, "FUNC1");
}

int main() {
  std::vector<seeker::th::Thread::Ptr> thread_vec;
  for (int i = 0; i < 5; i++) {
    thread_vec.emplace_back(new seeker::th::Thread(&CountFunc, "COUNT_FUNC"));
  }
  for (auto &i : thread_vec) {
    i->Join();
  }
  std::cout << "COUNT: " << gCount << std::endl;
}