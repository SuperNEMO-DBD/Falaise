// Ourselves:
#include <fecom/tracker_hit.hpp>

// Third party:
// - Boost:
// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>
// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
// Datatools support for serializable objects :
#include <datatools/i_serializable.ipp>

/** The main Boost/Serialization template method for class calo_hit
 *  @arg ar an archive type (ASCII, XML or binary)
 *  @arg version the archive version number (not used)
 */

namespace fecom {

  template<class Archive>
  void tracker_hit::serialize(Archive & ar, const unsigned int version)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    // example :
    // ar & boost::serialization::make_nvp("hit_id",  hit_id);

    // TO COMPLETE
    return;
  }

} // namespace fecom
