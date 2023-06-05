/**
 * @file log.cpp
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 日志器的实现
 * @version 1.0
 * @date 2023-05-25
 */
#include "log.h"
#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include "exception.h"

#define DEFAULT_LOGGER_NAME           "root"
#define DEFAULT_FORMATTER_PATTERN     "%d [%P](%r){%F:%L(%N)} %m"
#define DEFAULT_DATETIME_PATTERN      "%Y-%m-%d %H:%M"
#define EMPTY_PARAM                   ""

namespace seeker {
namespace log {

std::string Level::ToString(level l) {
    switch (l) {
#define TRANS(name) \
  case name:\
      return #name;

  TRANS(DEBUG)
  TRANS(INFO)
  TRANS(WARN)
  TRANS(ERROR)
  TRANS(FATAL)
#undef TRANS
    default:
      return "UNKNOWN";
  }
}

Level::level Level::FromString(std::string l_str) {
#define TRANS(level) \
  if (l_str == #level) \
    return level;

  TRANS(DEBUG)
  TRANS(INFO)
  TRANS(WARN)
  TRANS(ERROR)
  TRANS(FATAL)
#undef TRANS
  return UNKNOWN;
}

//// Formatter Item Begin

class LoggerNameItem : public FormattingMgr::IItem {
 public:
  LoggerNameItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << logger_name;
  }
};

/**
 * @brief 等级项
 */
class LevelItem : public FormattingMgr::IItem {
 public:
  LevelItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << Level::ToString(e->level_);
  }
};

/**
 * @brief 文件名项
 */
class FileNameItem : public FormattingMgr::IItem {
 public:
  FileNameItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->file_name_;
  }
};

/**
 * @brief 函数名项
 */
class FunctionItem : public FormattingMgr::IItem {
 public:
  FunctionItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->function_name_;
  }
};

/**
 * @brief 行号项
 */
class LineItem : public FormattingMgr::IItem {
 public:
  LineItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->line_num_;
  }
};

/**
 * @brief 时间戳项
 */
class TimeItem : public FormattingMgr::IItem {
 public:
  TimeItem(std::string buf)
    : format_(buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
  // 当没有格式时默认输出时间戳
  if (format_.length() == 0)
    ss << e->timestamp_;
  else
    TimeStampToString(format_, e->timestamp_);
  }
 private:
  std::string format_;
};

/**
 * @brief 线程ID项
 */
class ThreadIdItem : public FormattingMgr::IItem {
 public:
  ThreadIdItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->thread_id_;
  }
};

/**
 * @brief 线程名项
 */
class ThreadNameItem : public FormattingMgr::IItem {
 public:
  ThreadNameItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->thread_name_;
  }
};

/**
 * @brief 内容项
 */
class ContentItem : public FormattingMgr::IItem {
 public:
  ContentItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->content_.str();
  }
};

/**
 * @brief 其他字符项
 */
class StringItem : public FormattingMgr::IItem {
 public:
  StringItem(std::string buf) 
    : buff_(std::move(buf)) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << buff_;
  }
 private:
  std::string buff_;
};
//// Formatter Item End

void FormattingMgr::Init() {
  static std::map<std::string, 
                  std::function<FormattingMgr::IItem::Ptr(std::string)> > k_formatter_cb = {
#define ITEM(key, item) \
    {#key, [](std::string buf){ return FormattingMgr::IItem::Ptr(new item(std::move(buf))); }},

      ITEM(r, LoggerNameItem)       // r 日志名称
      ITEM(P, LevelItem)            // P 日志等级
      ITEM(F, FileNameItem)         // F 文件名
      ITEM(C, FunctionItem)         // C 函数名
      ITEM(L, LineItem)             // L 行号
      ITEM(d, TimeItem)             // d 时间
      ITEM(T, ThreadIdItem)         // T Thread ID
      ITEM(N, ThreadNameItem)       // N Thread Name
      ITEM(m, ContentItem)          // m 消息
      ITEM(S, StringItem)           // S 原始字符串
#undef ITEM
  };
  std::string sub_str;
  for (int i = 0, j; i < raw_.size(); i++) {
    // 不为%
    if (raw_[i] != '%') {
      sub_str.append(1, raw_[i]);
      continue;
    }
    // 为%
    item_arr_.push_back(k_formatter_cb["S"](sub_str));
    sub_str.clear();

    j = ++i;
    // 判断key值
    if (j < raw_.size()) {
      if (raw_[j] == 'd') {
        //// Parse Time Begin
        ++j;
        // 判断时间key后是否带有自定义时间格式 {YY-MM-dd}
        if (std::isspace(raw_[j]) || raw_[j] != '{') {
          item_arr_.push_back(k_formatter_cb["d"](DEFAULT_DATETIME_PATTERN));
          continue;
        }
        // { }对应判断
        int front_bracket_index = j, front_bracket_count = 0, last_bracket_index = 0;
        for (++j; j < raw_.size(); j++) {
          if (raw_[j] == '{')
            ++front_bracket_count;
          if (raw_[j] == '}') {
            last_bracket_index = j;
            if (front_bracket_count > 0)
              --front_bracket_count;
            else
              break;
          }
        }

        if (last_bracket_index <= front_bracket_index) {
          throw exception::LoggerParseInvalidKey("Fail to parse: Illegal datetime formatting string, maybe miss a bracket?");
        }
        std::string date_sub_string = raw_.substr(++front_bracket_index,
                                                     last_bracket_index - front_bracket_index - 1);
        // std::cout << date_sub_string << std::endl;

        item_arr_.push_back(k_formatter_cb["d"](date_sub_string));
        i = last_bracket_index;
        //// Parse Time End
      } else {
        auto res = k_formatter_cb.find(raw_.substr(j, 1));
        if (res == k_formatter_cb.end()) {
          std::stringstream ss;
          ss << "failed to parse: unknown param \"" << raw_[j]
             << "\" at " << j;
          throw exception::LoggerParseInvalidKey(ss.str());
        }
        item_arr_.push_back(res->second(EMPTY_PARAM));
      }
    }
  }
}

//// Output Begin
/**
 * @brief 文件输出类
 */
class FileOutput : public OutputMgr::IOutput {
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
  void Output(const std::string &buf) override {
    stream_ << buf << std::endl;
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
class StdOutput : public OutputMgr::IOutput {
 public:
  void Output(const std::string &buf) override {
    std::cout << buf << std::endl;
  }
};
//// Output End

Logger::Logger(std::string name, std::string format_str) 
    : name_(std::move(name)),
      formatting_mgr_(new FormattingMgr(std::move(format_str))),
      output_mgr_(new OutputMgr()) {
  // DEBUG ONLY
#if 1
  AddOutput(OutputMgr::IOutput::Ptr(new StdOutput()));
  auto a = new FileOutput("test.log");
  a->Open();
  AddOutput(OutputMgr::IOutput::Ptr(a));
#endif
  formatting_mgr_->Init();
}

void Logger::Output(Event::Ptr e) {
  std::stringstream ss;
  for (auto &i : formatting_mgr_->item_arr()) {
    i->ToStream(ss, name_, e);
  }
  for (auto &i : output_mgr_->output_arr()) {
    i->Output(ss.str());
  }
}

Manager::Manager() {
  // 默认日志器构建失败抛出异常
  try {
    default_logger_ = Logger::Ptr(
        new Logger(DEFAULT_LOGGER_NAME, 
                DEFAULT_FORMATTER_PATTERN));
  } catch (...) {
    throw exception::LoggerInitError(default_logger_->name());
  }
}

Logger::Ptr Manager::GetLogger(std::string key) {
  auto res = loggers_.find(key);
  // 若不存在返回管理器中默认日志器
  return res == loggers_.end() ? default_logger_ : res->second;
}

void Manager::AddLogger(Logger::Ptr l) {
  auto res = loggers_.find(l->name());
  // 存储的对象中未有重名的日志器时进行添加, 反之则替换格式和输出管理器
  if (res == loggers_.end())
    loggers_[l->name()] = l;
  else {
    res->second->set_formatting_mgr(l->formatting_mgr());
    res->second->set_output_mgr(l->output_mgr());
  }
}

void Manager::DeleteLogger(std::string logger_name) {
  loggers_.erase(logger_name);
}

} // namespace log
} // namespace seeker