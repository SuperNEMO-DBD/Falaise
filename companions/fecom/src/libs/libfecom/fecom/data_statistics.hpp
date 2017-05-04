//! \file fecom/data_statistics.hpp
//
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_DATA_STATISTICS_HPP
#define FECOM_DATA_STATISTICS_HPP

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

  //! \brief Commissioning event, contain a collection of calo_hits and tracker_channel_hits and can build associated tracker hits from channels
  struct data_statistics
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
  {

    struct register_for_cell {

      /// Default constructor
      register_for_cell() {};

      /// Default destructor
      virtual ~register_for_cell(){};

			std::size_t hit_counter = 0;
      std::size_t R0_count = 0;
      std::size_t R1_count = 0;
      std::size_t R2_count = 0;
      std::size_t R3_count = 0;
      std::size_t R4_count = 0;
			std::size_t Rbot_cath_count = 0;
      std::size_t Rtop_cath_count = 0;
			std::size_t bot_cathode_efficiency = 0;
    };

    /// Default constructor
    data_statistics();

    /// Destructor
    virtual ~data_statistics();

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

    // Datas :
    // General :
    std::size_t number_of_events;

    // Calorimeter :
    std::size_t number_of_events_with_calo;
    std::size_t number_of_calo_only_events;
		std::pair<double, std::size_t> mean_number_of_calo; // pair <mean_number_of_calo, counter_calo>
		std::pair<double, std::size_t> mean_number_of_calo_ht; // pair <mean_number_of_calo_ht, counter_calo_HT>

    // Tracker :
    std::size_t number_of_events_with_tracker;
    std::size_t number_of_tracker_only_events;
    std::pair<double, std::size_t> mean_number_of_tracker_cells;
		std::pair<double, std::size_t> mean_number_of_tracker_cells_if_0_calo_ht;
    std::array<register_for_cell, fecom::tracker_constants::NUMBER_OF_LAYERS> cell_registers{};

    // Calo + tracker :
    std::size_t number_of_calo_tracker_events;
		std::pair<double, std::size_t> mean_number_of_calo_ht_if_calo_tracker;
    std::pair<double, std::size_t> mean_number_of_tracker_cells_if_calo_tracker;
		std::pair<double, std::size_t> mean_number_of_tracker_cells_if_1_calo_ht;
		std::pair<double, std::size_t> mean_number_of_tracker_cells_if_2_calo_ht;
		std::pair<double, std::size_t> mean_number_of_tracker_cells_if_3p_calo_ht;
    std::array<register_for_cell, fecom::tracker_constants::NUMBER_OF_LAYERS> cell_registers_if_calo_tracker{};


		std::vector<std::string> unmapped_channels_in_raw_data;

    // Histograms :
    // General :

    // Calorimeter :
    std::array<TH1F *, fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN> charge_picocoulomb_calo_TH1F{};
    std::array<TH1F *, fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN> charge_picocoulomb_ht_calo_TH1F{};
    std::array<TH1F *, fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN> charge_picocoulomb_no_ht_calo_TH1F{};

    TH2F * calo_distrib_TH2F;
    TH2F * calo_distrib_ht_TH2F;
    TH2F * calo_distrib_no_ht_TH2F;

    TH1F * calo_delta_t_calo_tref_TH1F;

    // Tracker :
    TH2F * tracker_distrib_TH2F;
    TH2F * tracker_only_distrib_TH2F;

    // Calo + tracker :
		TH2F * calo_ht_number_tracker_number_distrib_TH2F;

    TH2F * calo_tracker_calo_distrib_TH2F;
    TH2F * calo_tracker_calo_ht_distrib_TH2F;
    TH2F * calo_tracker_tracker_distrib_TH2F;

    TH1F * calo_tracker_delta_t_calo_tref_TH1F;
    TH1F * calo_tracker_delta_t_anode_tref_TH1F;
    TH1F * calo_tracker_delta_t_anode_anode_TH1F;
    TH1F * calo_tracker_delta_t_cathode_tref_TH1F;
    TH1F * calo_tracker_delta_t_anode_cathode_same_hit_TH1F;


    DATATOOLS_SERIALIZATION_DECLARATION()
  };

} // namespace fecom


#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(fecom::data_statistics,
												"fecom::data_statistics")

#endif // FECOM_DATA_STATISTICS_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
