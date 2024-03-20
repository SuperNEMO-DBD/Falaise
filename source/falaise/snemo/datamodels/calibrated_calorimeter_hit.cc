// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodel/calibrated_calorimeter_hit.cc
 */

// Ourselves
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>

// Third party
// - Bayeux/datatools
#include <datatools/clhep_units.h>
#include <datatools/utils.h>

namespace snemo {

  namespace datamodel {

    // serial tag for datatools::serialization::i_serializable interface :
    // DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_calorimeter_hit,
    //                                                        "snemo::core::model::calibrated_calorimeter_hit")
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_calorimeter_hit,
                                                      "snemo::datamodel::calibrated_calorimeter_hit")

    double calibrated_calorimeter_hit::get_time() const { return time_; }

    void calibrated_calorimeter_hit::set_time(double time) { time_ = time; }

    double calibrated_calorimeter_hit::get_sigma_time() const { return sigma_time_; }

    void calibrated_calorimeter_hit::set_sigma_time(double sigma_time) { sigma_time_ = sigma_time; }

    double calibrated_calorimeter_hit::get_energy() const { return energy_; }

    void calibrated_calorimeter_hit::set_energy(double energy) { energy_ = energy; }

    double calibrated_calorimeter_hit::get_sigma_energy() const { return sigma_energy_; }

    void calibrated_calorimeter_hit::set_sigma_energy(double sigma_energy) {
      sigma_energy_ = sigma_energy;
    }

    bool calibrated_calorimeter_hit::is_valid() const
    {
      return this->base_hit::is_valid() && std::isnormal(energy_);
    }

    void calibrated_calorimeter_hit::invalidate()
    {
      this->base_hit::invalidate();
      datatools::invalidate(energy_);
      datatools::invalidate(sigma_energy_);
      datatools::invalidate(time_);
      datatools::invalidate(sigma_time_);
    }

    void calibrated_calorimeter_hit::print_tree(std::ostream & out_,
                                                const boost::property_tree::ptree & options_) const
    {
      base_hit::print_tree(out_, base_print_options::force_inheritance(options_));
      base_print_options popts;
      popts.configure_from(options_);
      const std::string & indent = popts.indent;
      
      if (popts.title.length()) {
        out_ << indent << popts.title << std::endl;
      }

      out_ << indent << tag << "Energy  : ("
           << energy_ / CLHEP::keV 
           << " +/- " << sigma_energy_ / CLHEP::keV << ") keV"
           << std::endl;
      
      out_ << indent << inherit_tag(popts.inherit) << "Time : ("
           << time_ / CLHEP::ns
           << " +/- " << sigma_time_ / CLHEP::ns << ") ns"
           << std::endl;
     
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
