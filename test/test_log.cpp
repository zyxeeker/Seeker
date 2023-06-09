#include "log.h"

int main() {
  seeker::log::SetMinLogLevel(seeker::log::Level::DEBUG);

  seeker::log::Debug() << "TETEE";
  seeker::log::Info() << "TETEE";
  seeker::log::Warn() << "TETEE";
  seeker::log::Error() << "TETEE";
  seeker::log::Fatal() << "TETEE";

  seeker::log::Debug("system") << "system";
  seeker::log::Debug("cfg") << "cfg";
}