// Ourselves
#include "FLSimulateErrors.h"

namespace FLSimulate {

  FLConfigUserError::FLConfigUserError()
    : std::invalid_argument("")
  {
  }

  FLConfigUserError::FLConfigUserError(const std::string & what_arg)
    : std::invalid_argument(what_arg)
  {
  }

  FLConfigUserError::FLConfigUserError(const char * what_arg)
    : std::invalid_argument(what_arg)
  {
  }

  FLDialogOptionsError::FLDialogOptionsError()
    : std::invalid_argument("")
  {
  }

  FLDialogOptionsError::FLDialogOptionsError(const std::string & what_arg)
    : std::invalid_argument(what_arg)
  {
  }

  FLDialogOptionsError::FLDialogOptionsError(const char * what_arg)
    : std::invalid_argument(what_arg)
  {
  }

} // namespace FLSimulate
