/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-31 14:15:42
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-01 17:00:06
 * @Description: 
 */

#ifndef __SEEKER_SRC_BASE_TINY_FILE_SERVICE_H__
#define __SEEKER_SRC_BASE_TINY_FILE_SERVICE_H__

#include <string>
#include <fstream>
#include <sstream>

#include "../../io.h"

namespace seeker {
namespace base {

class TinyFileService {
 public:
  enum RESULT {
    OK        = 0,
    NO_READ   = 0x0001,
    NO_WRITE  = 0x0010,
    NO_EXIST  = 0x0100,
  };

 public:
  TinyFileService(std::string path);
  ~TinyFileService();

 protected:
  RESULT ReadImpl(std::stringstream& ss);
  RESULT WriteImpl(std::string str, bool append = false);
  
 private:
  RESULT FileCanBeRead();
  RESULT FileCanBeWrite();

 private:
  std::string path_;
};

} // namespace base
} // namespace seeker

#endif // __SEEKER_SRC_BASE_TINY_FILE_SERVICE_H__
