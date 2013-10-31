// Ourselves
#include "foo.h"

namespace falaise {
namespace example {

  /* Register the serialization tag for the foo class.
   * The "falaise::example::foo" tag must be unique.
   */
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(foo,"falaise::example::foo")

  foo::foo()
  {
    _id_ = 0;
    return;
  }

  foo::~foo()
  {
    return;
  }

  uint32_t foo::get_id() const
  {
    return _id_;
  }

  void foo::set_id(uint32_t id_)
  {
    _id_ = id_;
    return;
  }

  datatools::properties & foo::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  const datatools::properties & foo::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

} // end of namespace example
} // end of namespace falaise
