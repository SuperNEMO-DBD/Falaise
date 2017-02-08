// Ourselves:
#include <fecom/commissioning_event.hpp>

// Third party:
// - Boost:
// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
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
  void commissioning_event::serialize(Archive & ar, const unsigned int /*version*/)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("trigger_id",  _trigger_id_);
    ar & boost::serialization::make_nvp("calo_hit_collection", _calo_hit_collection_);
    ar & boost::serialization::make_nvp("tracker_channel_hit_collection", _tracker_channel_hit_collection_);
    return;
  }

} // namespace fecom
