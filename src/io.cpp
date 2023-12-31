#include "io.h"

namespace seeker {
namespace io {

Manager::Manager() {
  InitService();
}

bool Manager::GetService(TYPE type, Service::WPtr& wptr) {
  std::lock_guard<std::mutex> l(mutex_);
  Service::Ptr ptr = nullptr;
  auto res = service_.find(type);
  if (res == service_.end()) {
    return false;
  }
  wptr = res->second;
  return true;
}

void Manager::InitService() {
  std::lock_guard<std::mutex> l(mutex_);
  // TODO: Support More...
  auto ptr = std::make_shared<Service>(3);
  ptr->Start();
  service_.insert({TINY_FILE_SERVICE, ptr});
}


} // namespace io
} // namespace seeker
