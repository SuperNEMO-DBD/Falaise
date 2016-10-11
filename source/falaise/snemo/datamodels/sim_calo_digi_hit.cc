/// \file falaise/snemo/datamodels/sim_calo_digi_hit.cc

// Ourselves:
#include <falaise/snemo/datamodels/sim_calo_digi_hit.h>

namespace snemo {

  namespace datamodel {

    // Serial tag :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(sim_calo_digi_hit,
                                                      "snemo::datamodel::sim_calo_digi_hit")

    sim_calo_digi_hit::sim_calo_digi_hit()
    {
      return;
    }

    sim_calo_digi_hit::~sim_calo_digi_hit()
    {
      return;
    }

    bool sim_calo_digi_hit::is_valid() const
    {
      if (!has_hit_id()) {
        return false;
      }
      if (!has_geom_id()) {
        return false;
      }
      if (!has_sampling_frequency()) {
        return false;
      }
      // if (!has_xxx()) {
      //   return false;
      // }
      return true;
    }

    void sim_calo_digi_hit::reset()
    {
      return;
    }

    void sim_calo_digi_hit::tree_dump(std::ostream & out_,
                                      const std::string & title_,
                                      const std::string & indent_,
                                      bool inherit_) const
    {
      this->mctools::digitization::sampled_signal::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Validity : " << (is_valid() ? "<yes>" : "<no>") << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
