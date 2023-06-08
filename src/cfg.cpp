/**
 * @file cfg.cpp
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 配置管理器具体实现
 * @version 1.0
 * @date 2023-06-06
 */

#include "cfg.h"
#include <fstream>

#if 0
#define FILE_PATH "cfg.json"
#else
#define FILE_PATH "test.json"
#endif

namespace seeker {
namespace cfg {

bool Manager::Init() {
  file_stream_ = std::ifstream("test.json");
  if (!file_stream_.good()) {
    std::cout << "failed to open file!" << std::endl;
    return false;
  }
  try {
    data_ = nlohmann::json::parse(file_stream_);
  } catch (nlohmann::json::exception ex) {
    std::cout << ex.what() << std::endl;
    return false;
  }
  return true;
}

} // cfg
} // seeker