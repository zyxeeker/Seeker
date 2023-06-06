#include "log.h"

int main() {
  seeker::log::Debug() << "TETEE";
  seeker::log::Info() << "TETEE";
  seeker::log::Warn() << "TETEE";
  seeker::log::Error() << "TETEE";
  seeker::log::Fatal() << "TETEE";
}