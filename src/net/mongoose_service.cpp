#include "mongoose_service.h"

#include <cstring>

namespace seeker {

#ifdef MONGOOSE

IHttpService::METHOD GetMethod(struct mg_str* src) {
#define XX(METHOD_STR, METHOD)              \
  if (mg_vcasecmp(src, METHOD_STR) == 0) {  \
    return METHOD;                          \
  }                                         \

  XX("GET", IHttpService::GET)
  XX("POST", IHttpService::POST)
#undef XX

  return IHttpService::UNKNOWN;
}

MongooseService::MongooseService() = default;

MongooseService::~MongooseService() = default;

bool MongooseService::Start(uint16_t port) {
  if (start_) {
    return true;
  }
  // init
  mg_mgr_init(&mgr_);
  // address
  std::string address = "0.0.0.0:";
  address.append(std::to_string(port));
  // listen
  auto conn = mg_http_listen(&mgr_, address.c_str(), OnMsgCallBack, this);
  if (conn == nullptr) {
    return false;
  }

  start_ = true;

  msg_loop_ = std::thread(&MongooseService::MsgLoop, this);

  return true;
}

void MongooseService::Stop() {
  start_ = false;
  msg_loop_.join();
}

void MongooseService::SetWebSite(const std::string& path)  {
  web_dir_ = path;
}

void MongooseService::MsgLoop() {
  while (start_) {
    mg_mgr_poll(&mgr_, 10);
  }
  mg_mgr_free(&mgr_);
}

void MongooseService::OnMsgCallBack(struct mg_connection* conn, int ev, 
                                    void *ev_data, void *fn_data) {
  auto th = (MongooseService*)fn_data;

  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *msg = (struct mg_http_message *) ev_data;
    
    auto url = std::string(msg->uri.ptr + 1, msg->uri.len - 1);
  
    ReqMeta req_meta {};
    RespMeta resp_meta {};
    req_meta.Method = GetMethod(&(msg->method));
    req_meta.Url = url;
    req_meta.Query = std::string(msg->query.ptr, msg->query.len);
    for (int i = 0; i < MG_MAX_HTTP_HEADERS; i++) {
      const auto header_ptr = msg->headers + i;
      if (header_ptr->name.len == 0) {
        break;
      }
      req_meta.Complex.Headers.push_back({ 
        std::string(header_ptr->name.ptr, header_ptr->name.len), 
        std::string(header_ptr->value.ptr, header_ptr->value.len) 
      });
    }
    
    req_meta.Complex.Content = std::string(msg->body.ptr, msg->body.len);
    if (th->CallRouter(req_meta, resp_meta)) {
      std::ostringstream oss;
      for (auto &i : resp_meta.Complex.Headers) {
        oss << i.Key << ": " << i.Value << "\r\n";
      }
      mg_http_reply(conn, resp_meta.Code, oss.str().c_str(), resp_meta.Complex.Content.c_str());
    } else {
      if (th->web_dir_.empty()) {
        mg_http_reply(conn, 500, "Content-Type: text/plain\r\n", "Server Internal Error");
      } else {
        // web
        struct mg_http_serve_opts opts;
        opts = mg_http_serve_opts { .root_dir = th->web_dir_.c_str()};
        mg_http_serve_dir(conn, msg, &opts);
      }
    }
  }
}

#endif

} // namespace seeker
