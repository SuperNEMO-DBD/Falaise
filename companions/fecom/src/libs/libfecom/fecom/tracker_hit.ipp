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
  void tracker_hit::serialize(Archive & ar,
			      const unsigned int /* version */)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("cell_geometric_id", cell_geometric_id);
    ar & boost::serialization::make_nvp("anodic_t0", anodic_t0);
    ar & boost::serialization::make_nvp("anodic_t1", anodic_t1);
    ar & boost::serialization::make_nvp("anodic_t2", anodic_t2);
    ar & boost::serialization::make_nvp("anodic_t3", anodic_t3);
    ar & boost::serialization::make_nvp("anodic_t4", anodic_t4);
    ar & boost::serialization::make_nvp("bot_cathodic_time", bot_cathodic_time);
    ar & boost::serialization::make_nvp("top_cathodic_time", top_cathodic_time);

    return;
  }

} // namespace fecom
