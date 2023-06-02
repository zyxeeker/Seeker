#include "log.h"

int main() {
  auto event = seeker::log::Event::Ptr(new seeker::log::Event{
    .file_name = "tst",
    .time = GetTimeStamp(),
    .thread_id = GetThreadId()
  });
  seeker::log::Obj obj("TE", "%d{} [%P](%r){%F:%L(%N)} %T %m");
  obj.Output(event);
  auto p = seeker::log::Mgr::GetInstance();
}