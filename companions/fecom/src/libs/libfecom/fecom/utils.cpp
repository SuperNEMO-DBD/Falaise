// fecom/utils.cpp

// Ourselves:
#include <fecom/utils.hpp>

namespace fecom {

  namespace io {

    std::string tag()
    {
      return "|-- ";
    }

    std::string last_tag()
    {
      return "`-- ";
    }

    std::string inherit_last_tag(bool inherit_)
    {
      if (inherit_) {
        return tag();
      } else {
        return last_tag();
      }
    }

    std::string skip_tag()
    {
      return "|   ";
    }

    std::string last_skip_tag()
    {
      return "    ";
    }

  } // namespace io

} // namespace fecom
