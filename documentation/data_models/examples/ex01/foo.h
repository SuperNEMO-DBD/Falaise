// -*- mode: c++; -*-

#ifndef FALAISE_FOO_H
#define FALAISE_FOO_H

// Third party
// - Boost
#include <boost/cstdint.hpp> // Portable integral types
// - Bayeux/datatools
#include <datatools/i_serializable.h> // Serializable interface
#include <datatools/properties.h> // Multipurpose property container

namespace falaise {
namespace example {

  class foo : DATATOOLS_SERIALIZABLE_CLASS
  {
  public:

    /// Default constructor
    foo();

    /// Destructor
    virtual ~foo();

    /// Getter
    uint32_t get_id() const;

    /// Setter
    void set_id(uint32_t);

    /// Return a reference to the mutable container of auxiliary properties
    datatools::properties & grab_auxiliaries();

    /// Return a reference to the non-mutable container of auxiliary properties
    const datatools::properties & get_auxiliaries() const;

  private:

    uint32_t _id_; //!< ID of the object
    datatools::properties _auxiliaries_; //!< Auxiliary properties stored in the object

    DATATOOLS_SERIALIZATION_DECLARATION();
  };

} // end of namespace example
} // end of namespace falaise

#include <boost/serialization/export.hpp>
/* Register the class in the Boost/Serialization system.
 * The "falaise::foo" key must be unique.
 */
BOOST_CLASS_EXPORT_KEY2(falaise::example::foo, "falaise::example::foo");

#endif // FALAISE_FOO_H
