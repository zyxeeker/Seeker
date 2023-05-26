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

class LoggerNameItem : public Formatter::IItem {
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
class LevelItem : public Formatter::IItem {
 public:
  LevelItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << Level::ToString(e->level);
  }
};

/**
 * @brief 文件名项
 */
class FileNameItem : public Formatter::IItem {
 public:
  FileNameItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->file_name;
  }
};

/**
 * @brief 函数名项
 */
class FunctionItem : public Formatter::IItem {
 public:
  FunctionItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->function;
  }
};

/**
 * @brief 行号项
 */
class LineItem : public Formatter::IItem {
 public:
  LineItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->line;
  }
};

/**
 * @brief 时间戳项
 */
class TimeItem : public Formatter::IItem {
 public:
  TimeItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    // TODO
    ss << e->time;
  }
};

/**
 * @brief 线程ID项
 */
class ThreadIdItem : public Formatter::IItem {
 public:
  ThreadIdItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    // TODO
  }
};

/**
 * @brief 线程名项
 */
class ThreadNameItem : public Formatter::IItem {
 public:
  ThreadNameItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    // TODO
  }
};

/**
 * @brief 内容项
 */
class ContentItem : public Formatter::IItem {
 public:
  ContentItem(std::string buf) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << e->content;
  }
};

/**
 * @brief 其他字符项
 */
class StringItem : public Formatter::IItem {
 public:
  StringItem(std::string buf) 
    : m_buff(std::move(buf)) {}
  void ToStream(std::stringstream &ss, 
                std::string& logger_name, 
                Event::Ptr e) override {
    ss << m_buff;
  }
 private:
  std::string m_buff;
};
//// Formatter Item End

bool Formatter::Init() {
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
  for (uint32_t i = 0, j; i < m_raw.size(); i++) {
    // 不为%
    if (m_raw[i] != '%') {
      sub_str.append(1, m_raw[i]);
      continue;
    }
    // 为%
    m_items.push_back(k_formatter_cb["S"](sub_str));
    sub_str.clear();

    j = ++i;
    // 判断key值
    if (j < m_raw.size()) {
      if (m_raw[j] == 'd') {
        //// Parse Time Begin
        ++j;
        // 判断时间key后是否带有自定义时间格式 {YY-MM-dd}
        if (std::isspace(m_raw[j]) || m_raw[j] != '{') {
          m_items.push_back(k_formatter_cb["d"](DEFAULT_DATETIME_PATTERN));
          continue;
        }
        // { }对应判断
        uint32_t front_bracket_index = j, front_bracket_count = 0, last_bracket_index = 0;
        for (++j; j < m_raw.size(); j++) {
          if (m_raw[j] == '{')
            ++front_bracket_count;
          if (m_raw[j] == '}') {
            last_bracket_index = j;
            if (front_bracket_count > 0)
              --front_bracket_count;
            else
              break;
          }
        }

        if (last_bracket_index <= front_bracket_index) {
          printf("Fail to parse: Illegal datetime formatting string, maybe miss a bracket?\n");
          return false;
        }
        std::string date_sub_string = m_raw.substr(++front_bracket_index,
                                                     last_bracket_index - front_bracket_index - 1);
        // std::cout << date_sub_string << std::endl;

        m_items.push_back(k_formatter_cb["d"](date_sub_string));
        i = last_bracket_index;
        //// Parse Time End
      } else {
        auto res = k_formatter_cb.find(m_raw.substr(j, 1));
        if (res == k_formatter_cb.end()) {
          printf("Fail to parse: Unknown param: \"%c\" at %d\n", m_raw[j], j);
          return false;
        }
        m_items.push_back(res->second(EMPTY_PARAM));
      }
    }
  }
  return true;
}

//// Outputer Begin
/**
 * @brief 文件输出类
 */
class FileOutputer : public IOutput {
 public:
  FileOutputer(std::string file_name) 
    : m_file_name(std::move(file_name)) {}
  /**
   * @brief 打开目标文件
   */
  bool Open() {
    m_stream.open(m_file_name, std::ios::out | std::ios::app);
    if (!m_stream.good()) {
      std::cout << "Fail to open file to write, file name: "
                << m_file_name 
                << std::endl;
      return false;
    }
    return true;
  }
  /**
   * @brief 重新打开文件
   */
  bool Reopen() {
    if (m_stream.is_open())
      m_stream.close();
    return Open();
  }
  void Output(std::string &buf) override {
    m_stream << buf << std::endl;
  }
  /**
   * @brief 返回文件名
   */
  std::string GetFileName() const {
    return m_file_name;
  }
 private:
  std::string m_file_name;
  std::ofstream m_stream;
};

/**
 * @brief 控制台输出类
 */
class StdOutputer : public IOutput {
 public:
  void Output(std::string &buf) override {
    std::cout << buf << std::endl;
  }
};
//// Outputer End

bool Obj::Init() {
  return m_formatter->Init();
}

void Obj::Output(Event::Ptr e) {
  std::stringstream ss;
  // TODO: 多方输出
  for (auto &i : m_formatter->GetItems()) {
    i->ToStream(ss, m_name, e);
  }
  // TODO
  std::cout << ss.str() << std::endl;
}

} // namespace log
} // namespace seeker