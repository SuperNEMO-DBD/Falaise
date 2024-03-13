// falaise/snemo/datamodels/calibrated_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/calibrated_data.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_data,
                                                      "snemo::datamodel::calibrated_data")

    const CalorimeterHitHdlCollection& calibrated_data::calorimeter_hits() const {
      return calorimeter_hits_;
    }

    CalorimeterHitHdlCollection& calibrated_data::calorimeter_hits() { return calorimeter_hits_; }

    const TrackerHitHdlCollection& calibrated_data::tracker_hits() const { return tracker_hits_; }

    TrackerHitHdlCollection& calibrated_data::tracker_hits() { return tracker_hits_; }

    void calibrated_data::clear() {
      calorimeter_hits_.clear();
      tracker_hits_.clear();
      _properties_.clear();
    }
    
    void calibrated_data::print_tree(std::ostream & out_,
                                     const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_hits_opt = options_.get("list_hits", false);
      bool list_properties_opt = options_.get("list_properties", false);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      auto printCaloHit = [&](const CalorimeterHitHdl& x) {
        out_ << "(Id : " << x->get_hit_id() << ", GID : " << x->get_geom_id()
             << ", Energy : " << x->get_energy() / CLHEP::keV << " keV"
             << ", Time : " << x->get_time() / CLHEP::ns << " ns)" << std::endl;
      };

      auto printTrackerHit = [&](const TrackerHitHdl& x) {
        out_ << "(Id : " << x->get_hit_id() << ", GID : " << x->get_geom_id()
             << ", Type : " << (x->is_prompt() ? "prompt" : "delayed [time=" + std::to_string(x->get_delayed_time() / CLHEP::microsecond) + " us]" ) << ")" << std::endl;
      };
      

      // Calibrated calorimeter hits:
      out_ << popts.indent << tag << "CalorimeterHits : " << calorimeter_hits_.size() << std::endl;
      if (list_hits_opt and not calorimeter_hits_.empty()) {
        std::for_each(calorimeter_hits_.begin(), --calorimeter_hits_.end(), [&](const CalorimeterHitHdl& x) {
          out_ << popts.indent << skip_tag << tag;
          printCaloHit(x);
        });
        out_ << popts.indent << skip_tag << last_tag;
        printCaloHit(calorimeter_hits_.back());
      }

      // Calibrated tracker hits:
      out_ << popts.indent << tag << "TrackerHits : " << tracker_hits_.size() << std::endl;
      if (list_hits_opt and not tracker_hits_.empty()) {
        std::for_each(tracker_hits_.begin(), --tracker_hits_.end(), [&](const TrackerHitHdl& x) {
          out_ << popts.indent << skip_tag << tag;
          printTrackerHit(x);
        });
        out_ << popts.indent << skip_tag << last_tag;
        printTrackerHit(tracker_hits_.back());
      }
      
      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Auxiliary properties : " << _properties_.size() << std::endl;
      if (list_properties_opt) {
        boost::property_tree::ptree auxOpts;
        auxOpts.put("indent", popts.indent + tags::item(not popts.inherit, true)); 
        _properties_.print_tree(out_, auxOpts);
      }
  
      return;
    }

  }  // end of namespace datamodel

}  // end of namespace snemo
