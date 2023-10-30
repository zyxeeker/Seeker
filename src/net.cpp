#include "net.h"

#include <iostream>

#include "net/mongoose_service.h"

namespace seeker {

bool NetServiceMgr::RegisterService(const std::string& name, 
                                    TYPE type, INetService::Ptr ptr) {
  std::lock_guard<std::mutex> l(mutex_);
  return service_.insert({ name, { ptr, type } }).second;
}

void NetServiceMgr::UnregisterService(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  service_.erase(name);
}

void INetService::ListAllService() {
  for (auto& i : NetServiceMgr::GetInstance().service()) {

    std::cout << "Service Name: " << i.first 
              << ", Type: " << i.second.Type << std::endl;
  }
}

IHttpService::WPtr IHttpService::Create(const std::string& name, uint16_t port) {
  auto ptr = std::make_shared<MongooseService>(port);
  auto res = NetServiceMgr::GetInstance().RegisterService(
                name, NetServiceMgr::SERVICE_HTTP, 
                std::dynamic_pointer_cast<INetService>(ptr));
  if (!res) {
    ptr.reset();
    return ptr;
  }
  return std::dynamic_pointer_cast<IHttpService>(ptr);
}

void IHttpService::Destory(const std::string& name) {
  NetServiceMgr::GetInstance().UnregisterService(name);
}

} // namespace seeker