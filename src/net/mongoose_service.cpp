#include "mongoose_service.h"

#include <cstring>

namespace seeker {

#ifdef MONGOOSE

HTTP_METHOD GetMethod(struct mg_str* src) {
#define XX(METHOD_STR, METHOD)              \
  if (mg_vcasecmp(src, METHOD_STR) == 0) {  \
    return METHOD;                          \
  }                                         \

  XX("GET", GET)
  XX("POST", POST)
#undef XX

  return UNKNOWN;
}

MongooseService::MongooseService(uint16_t port = 8080)
    : port_(port) {}

MongooseService::~MongooseService() = default;

bool MongooseService::Start() {
  if (start_) {
    return true;
  }
  // init
  mg_mgr_init(&mgr_);
  // address
  std::string address = "0.0.0.0:";
  address.append(std::to_string(port_));
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
    if (msg->uri.len == 1) {
      mg_http_reply(conn, 502, "Content-Type: text/plain\r\n", "Bad Request");
      return;
    }
    auto url = std::string(msg->uri.ptr + 1, msg->uri.len - 1);
    // judge router exist, reduce usage
    if (th->QueryRouter(url)) {
      HttpReqMsgMeta msg_meta {};
      msg_meta.Method = GetMethod(&(msg->method));
      msg_meta.Url = url;
      for (int i = 0; i < MG_MAX_HTTP_HEADERS; i++) {
        const auto header_ptr = msg->headers + i;
        if (header_ptr->name.len == 0) {
          break;
        }
        msg_meta.Headers.push_back({ 
          std::string(header_ptr->name.ptr, header_ptr->name.len), 
          std::string(header_ptr->value.ptr, header_ptr->value.len) 
        });
      }
      msg_meta.Content = std::string(msg->body.ptr, msg->body.len);
      auto res = th->UpdateHttpMsg(msg_meta);
      mg_http_reply(conn, 200, "Content-Type: text/plain\r\n", res.Content.c_str());
    } else {
      mg_http_reply(conn, 404, "Content-Type: text/plain\r\n", "Not Found");
    }
  }
}

#endif

} // namespace seeker
