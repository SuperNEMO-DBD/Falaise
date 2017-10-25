// Ourselves:
#include "falaise/common/user_profile.h"

namespace falaise {

namespace common {

const std::set<std::string>& supported_user_profiles() {
  static std::set<std::string> _ups;
  if (_ups.size() == 0) {
    _ups.insert("expert");
    _ups.insert("normal");
    _ups.insert("production");
  }
  return _ups;
}

}  // namespace common

}  // namespace falaise
