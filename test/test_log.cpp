#include "log.h"

int main() {
  seeker::log::Obj obj("TE", "%d [%P](%r){%F:%L(%N)} %m");
  obj.Init();
}