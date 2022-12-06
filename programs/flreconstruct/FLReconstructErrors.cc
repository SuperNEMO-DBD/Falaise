// Ourselves
#include "FLReconstructErrors.h"

namespace FLReconstruct {

  void do_error(std::ostream  & os_, const char * err_)
  {
    os_ << "flreconstruct : " << err_ << "\n";
    os_ << "Try `flreconstruct --help` for more information\n";
    return;
  }

  void do_error(std::ostream & os_, const std::string & serr_)
  {
    os_ << "flreconstruct : " << serr_ << "\n";
    os_ << "Try `flreconstruct --help` for more information\n";
    return;
  }

} // namespace FLReconstruct
