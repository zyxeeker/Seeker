#include "core.h"

#include <iostream>
#include <fstream>
#include <map>
#include <functional>

#include "../cfg.h"
#include "exception.h"

namespace seeker {
namespace log {

Logger::Logger(std::string name, LEVEL level)
    : name_(name),
      level_(level),
      formatter_(new Formatter(DEFAULT_FORMATTER_PATTERN)),
      outputer_(new Outputer()) {}


Logger::Logger(LoggerDefineMeta meta)
    : name_(meta.Name),
      level_(meta.Level),
      formatter_(new Formatter(meta.FormattingStr)),
      outputer_(new Outputer(meta.Output)) {}

void Logger::Init() {
  formatter_->Init();
}

void Logger::Output(Event::Ptr event_ptr) {
  if (formatter_->items().empty()) {
    return;
  }
  std::ostringstream oss;
  for (auto& i : formatter_->items()) {
    i->ToStream(oss, shared_from_this(), event_ptr);
  }
  oss << std::endl;
  for (auto& i : outputer_->items()) {
    i->Output(oss);
  }
}

//// Manager Begin
Manager::Manager()
    : min_level_(LEVEL::UNKNOWN) {
  // 默认日志器构建失败抛出异常
  try {
    default_logger_ = Logger::Ptr(new Logger(DEFAULT_LOGGER_NAME, min_level_));
    default_logger_->Init();
  } catch (...) {
    throw LogicError::Create(MODULE_NAME, 
                             "Logger(" + default_logger_->name() + ") failed to initialize");
  }
}

void Manager::Output(const std::string& key, const Event::Ptr& event) {
  std::lock_guard<std::mutex> l(mutex_);
  auto res = loggers_.find(key);
  auto logger = res == loggers_.end() ? default_logger_ : res->second;

  // 如果小于设置的最小等级则不进行输出
  if (min_level_ > logger->level()) {
    return;
  }
  logger->Output(event);
}

void Manager::AddLogger(LoggerDefineMeta&& logger) {
  std::lock_guard<std::mutex> l(mutex_);
  auto logger_ptr = std::make_shared<Logger>(logger);
  try {
    logger_ptr->Init();
  } catch (...) {
    logger_ptr->set_formatter(default_logger_->formatter());
  }
  loggers_[logger_ptr->name()] = logger_ptr;
}

void Manager::AddLogger(std::vector<LoggerDefineMeta>&& meta) {
  for (auto& i : meta) {
  // 如果日志名为空则跳过
  if (i.Name.length() == 0)
    continue;

#if 0
    std::cout << "LName: " << i.Name << std::endl
              << "LLevel: " << i.Level << std::endl
              << "LFStr: " << i.FormattingStr << std::endl
              << "---- OUTPUTER ----" << std::endl;
    for (auto& o : i.Output) {
      std::cout << "OType: " << o.Type << std::endl
                << "OPath: " << o.Path << std::endl;
    }
    std::cout << "---- -------- ----" << std::endl;
#endif
    AddLogger(std::forward<LoggerDefineMeta>(i));
  }
}

void Manager::DeleteLogger(const std::string& logger_name) {
  std::lock_guard<std::mutex> l(mutex_);
  loggers_.erase(logger_name);
}

//// Manager End

} // namespace log
} // namespace seeker