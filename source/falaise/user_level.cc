// Ourselves:
#include "falaise/user_level.h"

namespace falaise {

const std::set<std::string>& validUserLevels() {
  static std::set<std::string> levels = {"expert", "normal", "production"};
  return levels;
}

}  // namespace falaise
