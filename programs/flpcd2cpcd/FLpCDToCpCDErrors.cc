// Ourselves
#include "FLpCDToCpCDErrors.h"

namespace FLpCDToCpCD {

  void do_error(std::ostream  & os_, const char * err_)
  {
    os_ << "flpcd2cpcd : " << err_ << "\n";
    os_ << "Try `flpcd2cpcd --help` for more information\n";
    return;
  }

  void do_error(std::ostream & os_, const std::string & serr_)
  {
    os_ << "flpcd2cpcd : " << serr_ << "\n";
    os_ << "Try `flpcd2cpcd --help` for more information\n";
    return;
  }

} // namespace FLpCDToCpCD
