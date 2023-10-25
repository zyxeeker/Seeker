#include "net.h"

#include "net/mongoose_service.h"

namespace seeker {

NetServiceMgr::Ptr NetServiceMgr::inst_ = nullptr;

bool NetServiceMgr::RegisterService(const std::string& name, INetService::Ptr ptr) {
  std::lock_guard<std::mutex> l(mutex_);
  return service_.insert({ name, ptr }).second;
}

void NetServiceMgr::UnregisterService(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  service_.erase(name);
}

INetService::WPtr NetServiceMgr::GetService(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  return service_[name];
}

INetService::WPtr INetService::Create(INetService::TYPE type, const std::string& name, uint16_t port) {
  INetService::Ptr ptr = nullptr;
  if (type == INetService::SERVICE_HTTP) {
    ptr = INetService::Ptr(new MongooseService(port));
    NetServiceMgr::GetInstance().lock()->RegisterService(name, ptr);
  }
  return ptr;
}

void INetService::Destory(const std::string& name) {
  NetServiceMgr::GetInstance().lock()->UnregisterService(name);
}

INetService::WPtr INetService::Get(const std::string& name) {
  return NetServiceMgr::GetInstance().lock()->GetService(name);
}

} // namespace seeker