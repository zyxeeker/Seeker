/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 12:59:51
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-21 12:45:04
 * @Description: 
 */

#ifndef __SEEKER_SRC_NET_MONGOOSE_SERVICE_H__
#define __SEEKER_SRC_NET_MONGOOSE_SERVICE_H__

#include <mutex>
#include <thread>
#include <memory>
#include <string>
#include <unordered_map>


#define MONGOOSE

#ifdef MONGOOSE
#include <mongoose/mongoose.h>
#endif

#include "base/http_service.h"

namespace seeker {

#ifdef MONGOOSE
class MongooseService : public base::HttpServiceBase {
 public:
  MongooseService();
  ~MongooseService();

  bool Start(uint16_t port) override;
  void Stop() override;

  void SetWebSite(const std::string& path) override;

 private:
  void MsgLoop();
  static void OnMsgCallBack(struct mg_connection* conn, int ev, 
                            void *ev_data, void *fn_data);

 private:
  bool start_ = false;

  struct mg_mgr mgr_;
  std::string web_dir_;
  std::map<void* , std::string> file_r_;

  std::thread msg_loop_;
};
#endif

} // namespace seeker


#endif // __SEEKER_SRC_NET_MONGOOSE_SERVICE_H__