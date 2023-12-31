#include <unistd.h>

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <future>
#include <condition_variable>
#include <memory>
#include <functional>
// #include "../src/thread.h"
#include "thread.h"
#include "thread.hpp"

void Test1(int a) {
  std::this_thread::sleep_for(std::chrono::seconds(a));
  // std::cout << "SLEEP FOR : " << a << " DONE!" << std::endl;
  // return a+b;
}

int main() {
  seeker::ThreadPool tp(10);
  tp.Start();
  auto task = tp.CreateTask("TASK1", Test1, 3);
  task->result().get();
  std::cout << " NAME: " << task->name()
            << " START: " << task->start_time()
            << " DONE: " << task->done_time() << std::endl;

  task = tp.CreateTask("TASK2", Test1, 4);
  task->result().get();
  std::cout << " NAME: " << task->name()
            << " START: " << task->start_time()
            << " DONE: " << task->done_time() << std::endl;

  task = tp.CreateTask("TASK3", Test1, 5);
  task->result().get();
  std::cout << " NAME: " << task->name()
            << " START: " << task->start_time()
            << " DONE: " << task->done_time() << std::endl;

  task = tp.CreateTask("TASK4", Test1, 1);
  task->result().get();
  std::cout << " NAME: " << task->name()
            << " START: " << task->start_time()
            << " DONE: " << task->done_time() << std::endl;

  task = tp.CreateTask("TASK5", Test1, 2);
  task->result().get();
  std::cout << " NAME: " << task->name()
            << " START: " << task->start_time()
            << " DONE: " << task->done_time() << std::endl;

  task = tp.CreateTask("TASK6", Test1, 3);
  task->result().get();
  std::cout << " NAME: " << task->name()
            << " START: " << task->start_time()
            << " DONE: " << task->done_time() << std::endl;

  task = tp.CreateTask("TASK7", Test1, 4);
  task->result().get();
  std::cout << " NAME: " << task->name()
            << " START: " << task->start_time()
            << " DONE: " << task->done_time() << std::endl;
            
  tp.Stop();
  std::cout << "STOP" << std::endl;
  return 0;
}
