// wctools/utils.cpp

// Ourselves:
#include <wctools/utils.hpp>

// Standard library:
#include <wordexp.h>
#include <sstream>
#include <stdexcept>

namespace wctools {

void expand_environ(std::string& text_) {
  std::string text = text_;
  wordexp_t p;
  // bool freed = false;
  std::string error_message;
  const char* word = text.c_str();
  int we_error = wordexp(word, &p, WRDE_NOCMD | WRDE_SHOWERR | WRDE_UNDEF);
  switch (we_error) {
    case WRDE_BADCHAR: {
      std::ostringstream message;
      message << "wordexp error, code = " << we_error << " : Unquoted characters for input = '"
              << text_ << "'";
      throw std::logic_error(message.str());
    }
    case WRDE_BADVAL: {
      std::ostringstream message;
      message << "wordexp error, code = " << we_error << " : Undefined shell variable for input = '"
              << text_ << "'";
      throw std::logic_error(message.str());
    }
    case WRDE_CMDSUB: {
      std::ostringstream message;
      message << "wordexp error, code = " << we_error
              << " : Unauthorized command substitution for input = '" << text_ << "'";
      throw std::logic_error(message.str());
    }
    case WRDE_NOSPACE: {
      wordfree(&p);
      // freed = true;
      std::ostringstream message;
      message << "wordexp error, code = " << we_error
              << " : Attempt to allocate memory failed for input = '" << text_ << "'";
      throw std::logic_error(message.str());
    }
    case WRDE_SYNTAX: {
      std::ostringstream message;
      message << "wordexp error, code = " << we_error << " : Shell syntax error for input = '"
              << text_ << "'";
      throw std::logic_error(message.str());
    }
    default: {
      if (p.we_wordc == 0) {
        throw std::logic_error("Nothing to expand !");
      } else {
        char** w = p.we_wordv;
        if (p.we_wordc > 1) {
          std::ostringstream message;
          message << "wordexp expands to many tokens : ";
          for (size_t i = 0; i < p.we_wordc; i++) {
            message << " '" << w[i] << "'";
          }
          throw std::logic_error(message.str());
        } else {
          text = w[0];
        }
      }
    }  // switch (we_error)
  }
  text_ = text;
  return;
}

}  // namespace wctools
