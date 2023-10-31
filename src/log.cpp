#include "log.h"

#include "logger/core.h"

namespace seeker {
namespace log {

Log::Impl::Impl(Event::Ptr event,
                std::string logger_name)
    : event_(std::move(event)),
      logger_(Mgr::GetInstance().GetLogger(logger_name)) {}

Log::Impl::~Impl() {
  logger_->Output(event_);
}

Log Log::Impl::Create(Event::Ptr event,
                      std::string logger_name) {
  return Log(std::make_unique<Impl>(event, std::move(logger_name)));
}

Log::Log() = default;

Log::~Log() {
  oss.swap(impl_->event()->content_);
}

Log::Log(std::unique_ptr<Impl> impl) {
  impl_ = std::move(impl);
}

void SetMinLogLevel(Level::level level) {
  Mgr::GetInstance().set_min_level(level);
}

#define LOG_API_IMPLEMENT(LogName, Level)                 \
  Log LogName(std::string logger_name,                    \
              const char* file_name,                      \
              const char* function_name,                  \
              int line_num,                               \
              uint64_t timestamp) {                       \
    return Log::Impl::Create(                             \
        Event::Ptr(new Event {                            \
            .level_         = Level,                      \
            .file_name_     = std::move(file_name),       \
            .function_name_ = std::move(function_name),   \
            .line_num_      = std::move(line_num),        \
            .timestamp_     = std::move(timestamp),       \
            .thread_id_     = th::GetThreadId(),          \
            .thread_name_   = th::GetThreadName(),        \
        }),                                               \
        std::move(logger_name));                          \
  }                                                       \

  LOG_API_IMPLEMENT(Debug, Level::DEBUG)
  LOG_API_IMPLEMENT(Info, Level::INFO)
  LOG_API_IMPLEMENT(Warn, Level::WARN)
  LOG_API_IMPLEMENT(Error, Level::ERROR)
  LOG_API_IMPLEMENT(Fatal, Level::FATAL)
#undef LOG_API_IMPLEMENT

} // namespace log
} // namespace seeker