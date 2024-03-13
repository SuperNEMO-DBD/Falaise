// falaise/snemo/datamodels/precalibrated_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/precalibrated_data.h>

// Third party:
// - Bayeux/datatools
#include <datatools/clhep_units.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(precalibrated_data,
                                                      "snemo::datamodel::precalibrated_data")

    const PreCalibCalorimeterHitHdlCollection& precalibrated_data::calorimeter_hits() const {
      return calorimeter_hits_;
    }

    PreCalibCalorimeterHitHdlCollection& precalibrated_data::calorimeter_hits() { return calorimeter_hits_; }

    const PreCalibTrackerHitHdlCollection& precalibrated_data::tracker_hits() const { return tracker_hits_; }

    PreCalibTrackerHitHdlCollection& precalibrated_data::tracker_hits() { return tracker_hits_; }

    void precalibrated_data::clear() {
      calorimeter_hits_.clear();
      tracker_hits_.clear();
      _properties_.clear();
    }
     
    const datatools::properties & precalibrated_data::get_properties() const
    {
      return _properties_;
    }
     
    datatools::properties & precalibrated_data::grab_properties()
    {
      return _properties_;
    }
 
    // virtual
    void precalibrated_data::print_tree(std::ostream & out_,
                                        const boost::property_tree::ptree & options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);
      bool list_hits_opt = options_.get("list_hits", false);
      bool list_properties_opt = options_.get("list_properties", false);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }
      auto printPreCalibCaloHit = [&](const PreCalibCalorimeterHitHdl& x) {
        out_ << "Id: " << x->get_hit_id() << ", GID: " << x->get_geom_id()
            << ", Baseline: " << x->get_baseline() / (1E-3 * CLHEP::volt) << " mV"
             << ", Amplitude: " << x->get_amplitude() / (1E-3 * CLHEP::volt) << " mV"
             << ", Charge: " << x->get_charge() / (CLHEP::volt * CLHEP::second * 1E-9) << " nV.s"
             << ", Time: " << x->get_time() / CLHEP::second << " s"
             << " (+" << x->get_auxiliaries().size() << " prop.)"
             << std::endl;
      };

      auto printPreCalibTrackerHit = [&](const PreCalibTrackerHitHdl& x) {
        out_ << "Id: " << x->get_hit_id() << ", GID: " << x->get_geom_id()
             << ", AnodicTime = " << x->get_anodic_time() / CLHEP::microsecond << " us";
        if (x->has_bottom_cathode_drift_time()) {
          out_ << ", BottomCathodeDriftTime = " << x->get_bottom_cathode_drift_time() / CLHEP::microsecond << " us";
        }
        if (x->has_top_cathode_drift_time()) {
          out_ << ", TopCathodeDriftTime = " << x->get_top_cathode_drift_time() / CLHEP::microsecond << " us";
        }
        out_ << " (+" << x->get_auxiliaries().size() << " prop.)"
             << std::endl;
      };

      // Precalibrated calorimeter hits:
      out_ << popts.indent << tag
           << "PreCalibCalorimeterHits : " << calorimeter_hits_.size() << std::endl;
      if (list_hits_opt and not calorimeter_hits_.empty()) {
        std::for_each(calorimeter_hits_.begin(),
                      --calorimeter_hits_.end(),
                      [&](const PreCalibCalorimeterHitHdl& x) {
                        out_ << popts.indent << skip_tag << tag;
                        printPreCalibCaloHit(x);
                      });
        out_ << popts.indent << skip_tag << last_tag;
        printPreCalibCaloHit(calorimeter_hits_.back());
      }

      // Precalibrated tracker hits:
      out_ << popts.indent << tag
           << "PreCalibTrackerHits : " << tracker_hits_.size() << std::endl;
      if (list_hits_opt and not tracker_hits_.empty()) {
        std::for_each(tracker_hits_.begin(),
                      --tracker_hits_.end(),
                      [&](const PreCalibTrackerHitHdl& x) {
                        out_ << popts.indent << skip_tag << tag;
                        printPreCalibTrackerHit(x);
                      });
        out_ << popts.indent << skip_tag << last_tag;
        printPreCalibTrackerHit(tracker_hits_.back());
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

  } // end of namespace datamodel

} // end of namespace snemo
