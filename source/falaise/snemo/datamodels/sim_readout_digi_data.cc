/// \file falaise/snemo/datamodels/sim_readout_digi_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/sim_readout_digi_data.h>

namespace snemo {

  namespace datamodel {

    // Serial tag :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(sim_readout_digi_data,
                                                      "snemo::datamodel::sim_readout_digi_data")

    sim_readout_digi_data::sim_readout_digi_data()
    {
      return;
    }

    sim_readout_digi_data::~sim_readout_digi_data()
    {
      return;
    }

    bool sim_readout_digi_data::is_valid() const
    {
      return true;
    }

    void sim_readout_digi_data::reset()
    {
      return;
    }

    void sim_readout_digi_data::tree_dump(std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit_) const
    {
      if (! title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      // Validity:
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
      out_ << "Valid: " << is_valid() << std::endl;

      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
