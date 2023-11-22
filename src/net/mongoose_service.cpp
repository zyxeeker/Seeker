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
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    // MG_INFO(("New request to: [%.*s], body size: %lu", (int) hm->uri.len,
    //          hm->uri.ptr, (unsigned long) hm->body.len));

    auto url = std::string(hm->uri.ptr + 1, hm->uri.len - 1);
    
    ReqMeta req_meta {};
    RespMeta resp_meta {};
    req_meta.Method = GetMethod(&(hm->method));
    req_meta.Url = url;
    req_meta.Query = std::string(hm->query.ptr, hm->query.len);
    for (int i = 0; i < MG_MAX_HTTP_HEADERS; i++) {
      const auto header_ptr = hm->headers + i;
      if (header_ptr->name.len == 0) {
        break;
      }
      req_meta.Complex.Headers.push_back({ 
        std::string(header_ptr->name.ptr, header_ptr->name.len), 
        std::string(header_ptr->value.ptr, header_ptr->value.len) 
      });
    }

    req_meta.Complex.Body.Buff = (char*)(hm->body.ptr);
    req_meta.Complex.Body.Len = hm->body.len;

    RouterBase::Meta::Ptr ptr;
    if (!th->QueryRouter(url, ptr)) {
      if (th->web_dir_.empty()) {
        mg_http_reply(conn, 404, "Content-Type: text/plain\r\n", "Not Found");
      } else {
        // web
        struct mg_http_serve_opts opts;
        opts = mg_http_serve_opts { .root_dir = th->web_dir_.c_str()};
        mg_http_serve_dir(conn, hm, &opts);
      }
    } else if (ptr->ReqHandler) {
      th->CallRouter(ptr, req_meta, resp_meta);
    } else if (ptr->FileReqHandler) {
      struct mg_http_part part;
      size_t ofs = 0;
      while ((ofs = mg_http_next_multipart(hm->body, ofs, &part)) > 0) {
        // MG_INFO(("Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
                // (int) part.name.len, part.name.ptr, (int) part.filename.len,
                // part.filename.ptr, (unsigned long) part.body.len));
        if (!th->CallFileRouter(ptr, req_meta, resp_meta, 
                                std::string(part.filename.ptr, part.filename.len), 
                                part.body.ptr, part.body.len)) {
          break;
        }
      }
    }
    std::ostringstream oss;
    for (auto &i : resp_meta.Complex.Headers) {
      oss << i.Key << ": " << i.Value << "\r\n";
    }
    mg_http_reply(conn, resp_meta.Code, oss.str().c_str(), resp_meta.Complex.Body.c_str());
  }
}

#endif

} // namespace seeker
