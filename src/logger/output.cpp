#include "output.h"

#include <fstream>

#include "core.h"
#include "../io.h"
#include "../io/logger_io.hpp"

namespace seeker {
namespace log {

/**
 * @brief 文件输出类
 */
class FileOutput : public Outputer::IItem, protected FileService {
 public:
  FileOutput(const std::string& path) 
      : FileService(path) {}
  void Output(const std::ostringstream& oss) {
    Write(oss);
  }
};

/**
 * @brief 控制台输出类
 */
class StdOutput : public Outputer::IItem {
 public:
  void Output(const std::ostringstream& oss) {
    std::cout << oss.str();
  }
};

Outputer::Outputer() {
  auto ptr = std::make_shared<StdOutput>();
  AddItem(ptr);
}

Outputer::Outputer(std::vector<LoggerOutputDefineMeta> meta) {
  for (auto& i : meta) {
    IItem::Ptr ptr = nullptr;
    if (i.Type == FILE_OUT) {
      ptr = std::make_shared<FileOutput>(i.Path);
    } else if (i.Type == STD_OUT) {
      ptr = std::make_shared<StdOutput>();
    }
    if (ptr) {
      AddItem(ptr);
    }
  }
}

} // namespace log
} // namespace seeker
