/// \file falaise/snemo/datamodels/precalibrated_cluster.cc

// Ourselves:
#include <falaise/snemo/datamodels/precalibrated_cluster.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(precalibrated_cluster,
                                                      "snemo::datamodel::precalibrated_cluster")

    int precalibrated_cluster::get_cluster_id() const {
      return get_hit_id();
    }

    void precalibrated_cluster::set_cluster_id(int32_t id) {
      set_hit_id(id);
    }

    PreCalibratedCalorimeterHitHdlCollection& precalibrated_cluster::calorimeter_hits() {
      return _calorimeter_hits_;
    }

    const PreCalibratedCalorimeterHitHdlCollection& precalibrated_cluster::calorimeter_hits() const {
      return _calorimeter_hits_;
    }

    size_t precalibrated_cluster::calorimeter_size() const {
      return _calorimeter_hits_.size();
    }

    PreCalibratedTrackerHitHdlCollection& precalibrated_cluster::tracker_hits() {
      return _tracker_hits_;
    }

    const PreCalibratedTrackerHitHdlCollection& precalibrated_cluster::tracker_hits() const {
      return _tracker_hits_;
    }

    size_t precalibrated_cluster::tracker_size() const {
      return _tracker_hits_.size();
    }

    void precalibrated_cluster::clear()
    {
      _calorimeter_hits_.clear();
      _tracker_hits_.clear();
      base_hit::clear();
      return;
    }


    void precalibrated_cluster::print_tree(std::ostream & out,
                                     const boost::property_tree::ptree & options) const
    {
      base_hit::print_tree(out, base_print_options::force_inheritance(options));
      base_print_options popts;
      popts.configure_from(options);
      const std::string & indent = popts.indent;

      out << indent << tag << "CalorimeterHits[" << _calorimeter_hits_.size() << "]:" << std::endl;
      for (size_t i = 0; i < _calorimeter_hits_.size(); ++i) {
        out << indent << skip_tag;
        if (i + 1 == _calorimeter_hits_.size()) {
          out << last_tag;
        } else {
          out << tag;
        }
        out << "Hit[" << i << "] : (Id : " << _calorimeter_hits_[i]->get_hit_id()
            << ", GID : " << _calorimeter_hits_[i]->get_geom_id() << ")" << std::endl;
      }

      out << indent << tag << "TrackerHits[" << _tracker_hits_.size() << "]:" << std::endl;
      for (size_t i = 0; i < _tracker_hits_.size(); ++i) {
        out << indent << skip_tag;
        if (i + 1 == _tracker_hits_.size()) {
          out << last_tag;
        } else {
          out << tag;
        }
        out << "Hit[" << i << "] : (Id : " << _tracker_hits_[i]->get_hit_id()
            << ", GID : " << _tracker_hits_[i]->get_geom_id() << ")" << std::endl;
      }

      out << indent << inherit_tag(popts.inherit)
          << "Cluster ID  : " << get_cluster_id() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
