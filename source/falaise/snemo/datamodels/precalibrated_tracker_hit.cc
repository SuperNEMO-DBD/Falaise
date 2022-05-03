// falaise/snemo/datamodels/precalibrated_tracker_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/precalibrated_tracker_hit.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/utils.h>


namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(precalibrated_tracker_hit,
                                                      "snemo::datamodel::precalibrated_tracker_hit")


    bool precalibrated_tracker_hit::is_valid() const {
      return this->base_hit::is_valid();
    }

    void precalibrated_tracker_hit::invalidate() {
      this->base_hit::invalidate();
    }

    void precalibrated_tracker_hit::clear() { precalibrated_tracker_hit::invalidate(); }

    void precalibrated_tracker_hit::tree_dump(std::ostream& out, const std::string& title,
                                              const std::string& indent, bool /* is_last */) const {
      base_hit::tree_dump(out, title, indent, true);

      std::ostringstream prefix_os;
      prefix_os << indent << datatools::i_tree_dumpable::tag;
      std::string prefix = prefix_os.str();

    }

  }  // end of namespace datamodel

}  // end of namespace snemo
