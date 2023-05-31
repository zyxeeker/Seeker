#include "log.h"

int main() {
  auto event = seeker::log::Event::Ptr(new seeker::log::Event{
    .file_name = "tst"
  });
  seeker::log::Obj obj("TE", "%d [%P](%r){%F:%L(%N)} %m");
  obj.Output(event);
  auto p = seeker::log::Mgr::GetInstance();
}