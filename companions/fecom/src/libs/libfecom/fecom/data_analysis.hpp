//! \file fecom/data_analysis.hpp
//
// Copyright (c) 2017 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_DATA_ANALYSIS_HPP
#define FECOM_DATA_ANALYSIS_HPP

// Standard library:
#include <string>
#include <iostream>
#include <array>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <fecom/calo_hit.hpp>
#include <fecom/tracker_hit.hpp>
#include <fecom/tracker_channel_hit.hpp>
#include <fecom/channel_mapping.hpp>

// Root :
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"

namespace fecom {

  //! \brief Data statistics struct
  struct data_analysis
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
  {

    /// Default constructor
    data_analysis();

    /// Destructor
    virtual ~data_analysis();

    /// Check if the commissioning event is valid
    bool is_valid() const;

    /// Reset
    virtual void reset();

    // Initialize
    void initialize();

    // Check if the object is initialized
    bool is_initialized() const;

    // Save histograms in root file
    void save_in_root_file(TFile * root_file_);

    /// Print in a text file data statistics
    virtual void print(std::ostream & out_);

    /// Smart print
    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;
  private :

    /// Effective reset method
    void _reset_();

  public :

    // Management :
    bool initialized;


    // Data :

    // Calorimeter :





    DATATOOLS_SERIALIZATION_DECLARATION()
  };

} // namespace fecom


#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(fecom::data_analysis,
			"fecom::data_analysis")

#endif // FECOM_DATA_ANALYSIS_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
