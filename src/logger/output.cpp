#include "output.h"

#include <fstream>

#include "core.h"

namespace seeker {
namespace log {

/**
 * @brief 文件输出类
 */
class FileOutput : public Outputer::IItem {
 public:
  FileOutput(std::string file_name) 
      : file_name_(std::move(file_name)) {}
  /**
   * @brief 打开目标文件
   */
  bool Open() {
    stream_.open(file_name_, std::ios::out | std::ios::app);
    if (!stream_.good()) {
      std::cout << "Fail to open file to write, file name: "
                << file_name_ 
                << std::endl;
      return false;
    }
    return true;
  }
  /**
   * @brief 重新打开文件
   */
  bool Reopen() {
    if (stream_.is_open())
      stream_.close();
    return Open();
  }
  void Output(const std::shared_ptr<Logger> logger,
              const std::vector<Formatter::IItem::Ptr>& items,
              const Event::Ptr event_ptr) override {
    for (auto &i : items) {
      i->ToStream(stream_, logger, event_ptr);
    }
    stream_ << std::endl;
  }
  /**
   * @brief 返回文件名
   */
  std::string file_name() const {
    return file_name_;
  }
 private:
  std::string file_name_;
  std::ofstream stream_;
};

/**
 * @brief 控制台输出类
 */
class StdOutput : public Outputer::IItem {
 public:
  void Output(const Logger::Ptr logger,
              const std::vector<Formatter::IItem::Ptr>& items,
              const Event::Ptr event_ptr) override {
    std::lock_guard<std::mutex> l(mutex_);
    for (auto &i : items) {
      i->ToStream(std::cout, logger, event_ptr);
    }
    std::cout << std::endl;
  }
 private:
  static std::mutex mutex_;
};

std::mutex StdOutput::mutex_ = std::mutex{};

Outputer::Outputer() {
  auto ptr = std::make_shared<StdOutput>();
  AddItem(ptr);
}

Outputer::Outputer(std::vector<Meta> meta) {
  for (auto& i : meta) {
    IItem::Ptr ptr = nullptr;
    if (i.Type == "file") {
      auto p = std::make_shared<FileOutput>(i.Path);
      if (!p->Open()) {
        continue;
      }
    } else if (i.Type == "std") {
      ptr = std::make_shared<StdOutput>();
    }
    if (ptr) {
      AddItem(ptr);
    }
  }
}

} // namespace log
} // namespace seeker
