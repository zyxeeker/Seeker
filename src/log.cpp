#include "log.h"

#include "logger/core.h"

namespace seeker {
namespace log {

Log::Impl::Impl(Event::Ptr event,
                std::string logger_name)
    : event_(std::move(event)),
      name_(std::move(logger_name)) {}

Log::Impl::~Impl() {
  Mgr::GetInstance().Output(name_, event_);
}

Log Log::Impl::Create(Event::Ptr event,
                      std::string logger_name) {
  return Log(std::make_unique<Impl>(event, std::move(logger_name)));
}

Log::Log() = default;

Log::~Log() {
  oss.swap(impl_->event()->Content);
}

Log::Log(std::unique_ptr<Impl> impl) {
  impl_ = std::move(impl);
}

void SetMinLogLevel(LEVEL level) {
  Mgr::GetInstance().set_min_level(level);
}

#define LOG_API_IMPLEMENT(LOG_NAME, LEVEL)                \
  Log LOG_NAME(std::string logger_name,                   \
               const char* file_name,                     \
               const char* function_name,                 \
               int line_num,                              \
               uint64_t timestamp) {                      \
    return Log::Impl::Create(                             \
        Event::Ptr(new Event {                            \
            .Level          = LEVEL,                      \
            .FileName       = std::move(file_name),       \
            .FunctionName   = std::move(function_name),   \
            .Line           = std::move(line_num),        \
            .Timestamp      = std::move(timestamp),       \
            .ThreadId       = 0,                          \
            .ThreadName     = "",                         \
        }),                                               \
        std::move(logger_name));                          \
  }                                                       \

  LOG_API_IMPLEMENT(Debug,  LEVEL::DEBUG)
  LOG_API_IMPLEMENT(Info,   LEVEL::INFO)
  LOG_API_IMPLEMENT(Warn,   LEVEL::WARN)
  LOG_API_IMPLEMENT(Error,  LEVEL::ERROR)
  LOG_API_IMPLEMENT(Fatal,  LEVEL::FATAL)
#undef LOG_API_IMPLEMENT

void RegisterLogger(std::vector<LoggerDefineMeta> loggers) {
  log::Mgr::GetInstance().AddLogger(std::forward<std::vector<LoggerDefineMeta> >(loggers));
}

void RegisterLogger(LoggerDefineMeta logger) {
  log::Mgr::GetInstance().AddLogger(std::forward<LoggerDefineMeta>(logger));
}

void UnregisterLogger(const std::string& name) {
  log::Mgr::GetInstance().DeleteLogger(name);
}

} // namespace log
} // namespace seeker