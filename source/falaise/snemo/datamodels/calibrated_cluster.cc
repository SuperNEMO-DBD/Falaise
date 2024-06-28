/// \file falaise/snemo/datamodels/calibrated_cluster.cc

// Ourselves:
#include <falaise/snemo/datamodels/calibrated_cluster.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_cluster,
                                                      "snemo::datamodel::calibrated_cluster")

    int32_t calibrated_cluster::get_cluster_id() const
    {
      return _cluster_id_;
    }

    void calibrated_cluster::set_cluster_id(int32_t id)
    {
      _cluster_id_ = id;
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
      _properties_.clear();
      return;
    }


    void calibrated_cluster::print_tree(std::ostream & out_,
                                        const boost::property_tree::ptree & options_) const
    {
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

      {
	boost::property_tree::ptree auxOpts;
	auxOpts.put("indent", popts.indent);
        _properties_.print_tree(out_, auxOpts);
      }

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
