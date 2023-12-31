#include "formatter.h"

#include <map>
#include <functional>

#include "core.h"
#include "../include/exception.h"

#define DEFAULT_DATETIME_PATTERN      "%Y-%m-%d %H:%M"
#define EMPTY_PARAM                   ""

#define CONSOLE_RED                   "\e[1;31m"
#define CONSOLE_GREEN                 "\e[1;32m"
#define CONSOLE_YELLOW                "\e[1;33m"
#define CONSOLE_BLUE                  "\e[1;34m"
#define CONSOLE_PINK                  "\e[1;35m"
#define CONSOLE_END                   "\e[0m"

namespace seeker {
namespace log {

class LoggerNameItem : public Formatter::IItem {
 public:
  LoggerNameItem(std::string buf) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
    os << logger->name();
  }
};

/**
 * @brief 等级项
 */
class LevelItem : public Formatter::IItem {
 public:
  LevelItem(std::string buf) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
    auto level_str = level::ToString(event_ptr->Level);
    switch (event_ptr->Level) {
#define TRANS(LEVEL_NAME, COLOR) \
      case log::LEVEL::LEVEL_NAME: \
        os << COLOR << level_str; \
        break;

      TRANS(DEBUG, CONSOLE_PINK)
      TRANS(INFO, CONSOLE_BLUE)
      TRANS(WARN, CONSOLE_YELLOW)
      TRANS(ERROR, CONSOLE_RED)
      TRANS(FATAL, CONSOLE_RED)
#undef TRANS
      default:
        os << level_str; \
        break;
    }
    os << CONSOLE_END;
  }
};

/**
 * @brief 文件名项
 */
class FileNameItem : public Formatter::IItem {
 public:
  FileNameItem(std::string buf) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
    os << event_ptr->FileName;
  }
};

/**
 * @brief 函数名项
 */
class FunctionItem : public Formatter::IItem {
 public:
  FunctionItem(std::string buf) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
    os << event_ptr->FunctionName;
  }
};

/**
 * @brief 行号项
 */
class LineItem : public Formatter::IItem {
 public:
  LineItem(std::string buf) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
    // 行号为0时输出"(null)"
    if (event_ptr->Line)
      os << event_ptr->Line;
    else
      os << "(null)";
  }
};

/**
 * @brief 时间戳项
 */
class TimeItem : public Formatter::IItem {
 public:
  TimeItem(std::string buf)
    : format_(buf) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
  // 当没有格式时默认输出时间戳
  if (format_.length() == 0)
    os << event_ptr->Timestamp;
  else
    util::TimeStampToString(format_, event_ptr->Timestamp);
  }
 private:
  std::string format_;
};

/**
 * @brief 线程ID项
 */
class ThreadIdItem : public Formatter::IItem {
 public:
  ThreadIdItem(std::string buf) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
    os << event_ptr->ThreadId;
  }
};

/**
 * @brief 线程名项
 */
class ThreadNameItem : public Formatter::IItem {
 public:
  ThreadNameItem(std::string buf) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
    os << event_ptr->ThreadName;
  }
};

/**
 * @brief 内容项
 */
class ContentItem : public Formatter::IItem {
 public:
  ContentItem(std::string buf) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
    os << event_ptr->Content.str();
  }
};

/**
 * @brief 其他字符项
 */
class StringItem : public Formatter::IItem {
 public:
  StringItem(std::string buf) 
    : buff_(std::move(buf)) {}
  void ToStream(std::ostream &os, 
                const Logger::Ptr logger, 
                const Event::Ptr event_ptr) override {
    os << buff_;
  }
 private:
  std::string buff_;
};

void Formatter::Init() {
  static std::map<std::string, 
                  std::function<Formatter::IItem::Ptr(std::string)> > k_formatter_cb = {
#define ITEM(key, item) \
    {#key, [](std::string buf){ return Formatter::IItem::Ptr(new item(std::move(buf))); }},

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
    items_.push_back(k_formatter_cb["S"](sub_str));
    sub_str.clear();

    j = ++i;
    // 判断key值
    if (j < raw_.size()) {
      if (raw_[j] == 'd') {
        //// Parse Time Begin
        ++j;
        // 判断时间key后是否带有自定义时间格式 {YY-MM-dd}
        if (std::isspace(raw_[j]) || raw_[j] != '{') {
          items_.push_back(k_formatter_cb["d"](DEFAULT_DATETIME_PATTERN));
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
          throw LogicError::Create(MODULE_NAME, 
                                   "<bad parse> illegal datetime formatting string, maybe miss a bracket?");
        }
        std::string date_sub_string = raw_.substr(++front_bracket_index,
                                                     last_bracket_index - front_bracket_index - 1);
        // std::cout << date_sub_string << std::endl;

        items_.push_back(k_formatter_cb["d"](date_sub_string));
        i = last_bracket_index;
        //// Parse Time End
      } else {
        auto res = k_formatter_cb.find(raw_.substr(j, 1));
        if (res == k_formatter_cb.end()) {
          std::ostringstream oss;
          oss << "<bad parse> unknown param \"" << raw_[j]
              << "\" at " << j;
          throw LogicError::Create(MODULE_NAME, oss.str());
        }
        items_.push_back(res->second(EMPTY_PARAM));
      }
    }
  }
}

} // namespace log
} // namespace seeker
