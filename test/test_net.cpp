#include <nlohmann/json.hpp>

#include "net.hpp"

seeker::IHttpService::RespMsgMeta TestResp(const seeker::IHttpService::ReqMsgMeta& req) {
  seeker::IHttpService::RespMsgMeta resp;
  resp.Complex.Content = "HELLO!";
  return std::move(resp);
}

int main() {
  auto p = seeker::IHttpService::Create("HTTP_TEST", 9666);
  // INetService::Create(seeker::INetService::SERVICE_HTTP, "HTTP_TEST", 9666);
  if (p.expired()) {
    return 1;
  }
  if (!p.lock()->Start()) {
    return 1;
  }
  p.lock()->RegisterRouter("test", &TestResp);
  // auto inet = seeker::INetService::Get("HTTP_TEST");
  // auto http = std::dynamic_pointer_cast<seeker::IHttpService>(inet.lock());
  // http->RegisterRouter("test", &TestResp);
  while(true) {}
}