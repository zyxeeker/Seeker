#include <nlohmann/json.hpp>

#include "net.hpp"

seeker::HttpRespMsgMeta TestResp(const seeker::HttpReqMsgMeta& req) {
  seeker::HttpRespMsgMeta resp;
  resp.Content = "HELLO!";
  return std::move(resp);
}

int main() {
  auto p = seeker::INetService::Create(seeker::INetService::SERVICE_HTTP, "HTTP_TEST", 9666);
  if (p.expired()) {
    return 1;
  }
  p.lock()->Start();
  auto inet = seeker::INetService::Get("HTTP_TEST");
  auto http = std::dynamic_pointer_cast<seeker::IHttpService>(inet.lock());
  http->RegisterRouter("test", &TestResp);
  while(true) {}
}