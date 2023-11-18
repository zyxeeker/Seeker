#include "net.h"

#include <iostream>

#include <log.h>

#include "net/mongoose_service.h"

namespace seeker {
namespace net {

bool Manager::RegisterService(const std::string& name, 
                              TYPE type, INetService::WPtr ptr) {
  std::lock_guard<std::mutex> l(mutex_);
  return service_.insert({ name, { ptr, type } }).second;
}

void Manager::UnregisterService(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  service_.erase(name);
}

void Manager::ListService() {
  for (auto& i : service_) {
    if (i.second.WPtr.expired()) {
      service_.erase(i.first);
    } else {
      log::Info() << "Service Name: " << i.first 
                  << ", Type: " << i.second.Type << std::endl;
    }
  }
}

} // namespace net

void INetService::ListAllService() {
  net::Mgr::GetInstance().ListService();
}

IHttpService::Ptr IHttpService::Create(const std::string& name) {
  auto ptr = std::make_shared<MongooseService>();
  auto res = net::Mgr::GetInstance().RegisterService(
                name, net::Manager::SERVICE_HTTP, 
                std::dynamic_pointer_cast<INetService>(ptr));
  if (!res) {
    ptr.reset();
    return ptr;
  }
  return std::dynamic_pointer_cast<IHttpService>(ptr);
}

} // namespace seeker