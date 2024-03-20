/// \file falaise/snemo/datamodels/calibrated_cluster.cc

// Ourselves:
#include <falaise/snemo/datamodels/calibrated_cluster.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_cluster,
                                                      "snemo::datamodel::calibrated_cluster")

    int calibrated_cluster::get_cluster_id() const
    {
      return get_hit_id();
    }

    void calibrated_cluster::set_cluster_id(int32_t id)
    {
      set_hit_id(id);
    }

    CalorimeterHitHdlCollection& calibrated_cluster::calorimeter_hits()
    {
      return _calorimeter_hits_;
    }

    const CalorimeterHitHdlCollection& calibrated_cluster::calorimeter_hits() const
    {
      return _calorimeter_hits_;
    }

    size_t calibrated_cluster::calorimeter_size() const
    {
      return _calorimeter_hits_.size();
    }

    TrackerHitHdlCollection& calibrated_cluster::tracker_hits()
    {
      return _tracker_hits_;
    }

    const TrackerHitHdlCollection& calibrated_cluster::tracker_hits() const
    {
      return _tracker_hits_;
    }

    size_t calibrated_cluster::tracker_size() const
    {
      return _tracker_hits_.size();
    }

    void calibrated_cluster::clear()
    {
      _calorimeter_hits_.clear();
      _tracker_hits_.clear();
      base_hit::clear();
      return;
    }


    void calibrated_cluster::print_tree(std::ostream & out_,
                                        const boost::property_tree::ptree & options_) const
    {
      base_hit::print_tree(out_, base_print_options::force_inheritance(options_));
      base_print_options popts;
      popts.configure_from(options_);
      const std::string & indent = popts.indent;

      out_ << indent << tag << "CalorimeterHits[" << _calorimeter_hits_.size() << "]:" << std::endl;
      for (size_t i = 0; i < _calorimeter_hits_.size(); ++i) {
        out_ << indent << skip_tag;
        if (i + 1 == _calorimeter_hits_.size()) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "Hit[" << i << "] : (Id : " << _calorimeter_hits_[i]->get_hit_id()
             << ", GID : " << _calorimeter_hits_[i]->get_geom_id() << ")" << std::endl;
      }

      out_ << indent << tag << "TrackerHits[" << _tracker_hits_.size() << "]:" << std::endl;
      for (size_t i = 0; i < _tracker_hits_.size(); ++i) {
        out_ << indent << skip_tag;
        if (i + 1 == _tracker_hits_.size()) {
          out_ << last_tag;
        } else {
          out_ << tag;
        }
        out_ << "Hit[" << i << "] : (Id : " << _tracker_hits_[i]->get_hit_id()
             << ", GID : " << _tracker_hits_[i]->get_geom_id() << ")" << std::endl;
      }

      out_ << indent << inherit_tag(popts.inherit)
           << "Cluster ID  : " << get_cluster_id() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
