#include "net.h"

#include <iostream>

#include <log.h>

#include "net/mongoose_service.h"

namespace seeker {
namespace net {

bool Manager::RegisterService(const std::string& name, 
                              TYPE type, INetService::Ptr ptr) {
  std::lock_guard<std::mutex> l(mutex_);
  return service_.insert({ name, { ptr, type } }).second;
}

void Manager::UnregisterService(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  service_.erase(name);
}

} // namespace net

void INetService::ListAllService() {
  for (auto& i : net::Mgr::GetInstance().service()) {
    log::Info() << "Service Name: " << i.first 
                << ", Type: " << i.second.Type << std::endl;
  }
}

IHttpService::WPtr IHttpService::Create(const std::string& name, uint16_t port) {
  auto ptr = std::make_shared<MongooseService>(port);
  auto res = net::Mgr::GetInstance().RegisterService(
                name, net::Manager::SERVICE_HTTP, 
                std::dynamic_pointer_cast<INetService>(ptr));
  if (!res) {
    ptr.reset();
    return ptr;
  }
  return std::dynamic_pointer_cast<IHttpService>(ptr);
}

void IHttpService::Destory(const std::string& name) {
  net::Mgr::GetInstance().UnregisterService(name);
}

} // namespace seeker