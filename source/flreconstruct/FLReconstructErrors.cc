// Ourselves
#include "FLReconstructErrors.h"

namespace FLReconstruct {

void do_error(std::ostream& os, const char* err) {
  os << "flreconstruct : " << err << "\n";
  os << "Try `flreconstruct --help` for more information\n";
}

void do_error(std::ostream& os, const std::string& serr) {
  os << "flreconstruct : " << serr << "\n";
  os << "Try `flreconstruct --help` for more information\n";
}

}  // namespace FLReconstruct
