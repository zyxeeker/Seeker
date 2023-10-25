/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 12:59:51
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-25 18:30:48
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
class MongooseService : public base::HttpService {
 public:
  MongooseService(uint16_t port);
  ~MongooseService();

  bool Start() override;
  void Stop() override;

 private:
  void MsgLoop();
  static void OnMsgCallBack(struct mg_connection* conn, int ev, 
                            void *ev_data, void *fn_data);

 private:
  bool start_ = false;

  std::uint16_t port_;
  struct mg_mgr mgr_;

  std::thread msg_loop_;
};
#endif

} // namespace seeker


#endif // __SEEKER_SRC_NET_MONGOOSE_SERVICE_H__