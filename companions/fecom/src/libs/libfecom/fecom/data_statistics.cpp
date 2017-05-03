// fecom/data_statistics.cpp

// Standard library:
#include <limits>

// Ourselves:
#include <fecom/data_statistics.hpp>

namespace fecom {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(data_statistics,
						    "fecom::data_statistics")

  data_statistics::data_statistics()
  {
    reset();
  }

  data_statistics::~data_statistics()
  {
  }

  bool data_statistics::is_valid() const
  {
    return initialized;
  }

  void data_statistics::reset()
  {
    _reset_();
    return;
  }

  void data_statistics::_reset_()
  {
    // General :
    number_of_events = std::numeric_limits<std::size_t>::max();
    number_of_calo_only_events = std::numeric_limits<std::size_t>::max();
    number_of_tracker_only_events = std::numeric_limits<std::size_t>::max();

    // Calorimeter :
    number_of_events_with_calo = std::numeric_limits<std::size_t>::max();
    mean_number_of_calo = std::numeric_limits<std::size_t>::max();
    mean_number_of_calo_ht = std::numeric_limits<std::size_t>::max();

    // Tracker :
    number_of_events_with_tracker = std::numeric_limits<std::size_t>::max();
    mean_number_of_tracker_cells = std::numeric_limits<std::size_t>::max();

    // Calo + tracker :
    number_of_calo_tracker_events = std::numeric_limits<std::size_t>::max();
    mean_number_of_tracker_cells_if_0_calo_ht = std::numeric_limits<std::size_t>::max();
    mean_number_of_tracker_cells_if_1_calo_ht = std::numeric_limits<std::size_t>::max();
    mean_number_of_tracker_cells_if_2_calo_ht = std::numeric_limits<std::size_t>::max();
    mean_number_of_tracker_cells_if_3p_calo_ht = std::numeric_limits<std::size_t>::max();

    // Histogram ptr :
    for (unsigned int icalo = 0; icalo < fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN; icalo++) {
     raw_charge_calo_TH1F[icalo] = nullptr;
     raw_charge_ht_calo_TH1F[icalo] = nullptr;
    }

    initialized = false;

    return;
  }

  void data_statistics::initialize()
  {
    // Initialize all histograms :
    std::string string_buffer = "";
    for (unsigned int icalo = 0; icalo < fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN; icalo++) {
      string_buffer = "raw_charge_calo_" + std::to_string(icalo);
      raw_charge_calo_TH1F[icalo] = new TH1F(string_buffer.c_str(),
					     Form("Calorimeter raw charge, row %i", icalo),
					     1000, 0, 40000);

      string_buffer = "raw_charge_ht_calo_" + std::to_string(icalo);
      raw_charge_ht_calo_TH1F[icalo] = new TH1F(string_buffer.c_str(),
						Form("Calorimeter raw charge if HT, row %i", icalo),
						1000, 0, 40000);
    } // end of for icalo

    string_buffer = "calo_distrib_TH2F";
    calo_distrib_TH2F =  new TH2F(string_buffer.c_str(),
				  Form("All calo distribution"),
				  20, 0, 20,
				  14, 0, 14);

    string_buffer = "calo_distrib_ht_TH2F";
    calo_distrib_ht_TH2F =  new TH2F(string_buffer.c_str(),
				  Form("Calo HT distribution"),
				  20, 0, 20,
				  14, 0, 14);

    string_buffer = "calo_distrib_no_ht_TH2F";
    calo_distrib_no_ht_TH2F =  new TH2F(string_buffer.c_str(),
				  Form("Calo no HT distribution"),
				  20, 0, 20,
				  14, 0, 14);


    string_buffer = "tracker_distrib_TH2F";
    tracker_distrib_TH2F = new TH2F(string_buffer.c_str(),
				    Form("Tracker cell distribution"),
				    6, 0, 6,
				    10, 0, 10);

    string_buffer = "calo_tracker_calo_distrib_TH2F";
    calo_tracker_calo_distrib_TH2F  = new TH2F(string_buffer.c_str(),
					       Form("Calo distribution if calo + tracker events"),
					       20, 0, 20,
					       14, 0, 14);

    string_buffer = "calo_tracker_tracker_distrib_TH2F";
    calo_tracker_tracker_distrib_TH2F  = new TH2F(string_buffer.c_str(),
						  Form("Tracker cell distribution if calo + tracker events"),
						  6, 0, 6,
						  10, 0, 10);

    string_buffer = "calo_tracker_delta_t_calo_tref_TH1F";
    calo_tracker_delta_t_calo_tref_TH1F = new TH1F(string_buffer.c_str(),
						   Form("Calo + tracker events, DT(calo_X - calo_tref)"),
						   100, 0, 150000);

    string_buffer = "calo_tracker_delta_t_anode_tref_TH1F";
    calo_tracker_delta_t_anode_tref_TH1F = new TH1F(string_buffer.c_str(),
						    Form("Calo + tracker events, DT(anode_X - calo_tref)"),
						    100, 0, 150000);

    string_buffer = "calo_tracker_delta_t_cathode_tref_TH1F";
    calo_tracker_delta_t_cathode_tref_TH1F = new TH1F(string_buffer.c_str(),
						      Form("Calo + tracker events, DT(cathode_X - calo_tref)"),
						      100, 0, 150000);

    string_buffer = "calo_tracker_delta_t_anode_cathode_same_hit_TH1F";
    calo_tracker_delta_t_anode_cathode_same_hit_TH1F = new TH1F(string_buffer.c_str(),
								Form("Calo + tracker events, DT(anode_X - cathode_X)"),
								100, 0, 150000);

    string_buffer = "calo_tracker_delta_t_anode_anode_TH1F";
    calo_tracker_delta_t_anode_anode_TH1F = new TH1F(string_buffer.c_str(),
						     Form("Calo + tracker events, DT(anode_X - anode_Y)"),
						     100, 0, 150000);

    initialized = true;

    return;
  }

  void data_statistics::print(std::ostream & out_)
  {
    double eff_only_calo    = (static_cast<double>(number_of_calo_only_events) / static_cast<double>(number_of_events)) * 100.;
    double eff_only_tracker = (static_cast<double>(number_of_tracker_only_events) /  static_cast<double>(number_of_events)) * 100.;
    double eff_calo_tracker = (static_cast<double>(number_of_calo_tracker_events) /  static_cast<double>(number_of_events)) * 100.;

    out_ << "***GENERAL STATISTICS***" << std::endl;
    out_ << "Total number of commissioning events : " << number_of_events <<  std::endl;
    out_ << "Total number of calo only events     : " << number_of_calo_only_events    << " Eff : " << eff_only_calo    << "%" << std::endl;
    out_ << "Total number of tracker only events  : " << number_of_tracker_only_events << " Eff : " << eff_only_tracker << "%" <<std::endl;
    out_ << "Total number of calo tracker events  : " << number_of_calo_tracker_events << " Eff : " << eff_calo_tracker << "%" <<std::endl << std::endl;

    // out_ << "***CALO STATISTICS***" << std::endl;
    // out_ << "Total Calo hits (if 2+ calos HT)         : " << calo_hit_number << std::endl << std::endl;

    // out_ << "***TRACKER STATISTICS***" << std::endl;
    // out_ << "Total Tracker hits if calo + tracker     : " << tracker_hit_number << std::endl;
    // out_ << "Total anode t0 hits if calo + tracker    : " << anode_t0_counter << std::endl;
    // out_ << "Total anode t0 only if calo + tracker    : " << anode_t0_only_counter << std::endl;
    // out_ << "Total bot cathode hits if calo + tracker : " << bot_cathode_counter << std::endl;
    // out_ << "Total bot cathode only if calo + tracker : " << bot_cathode_only_counter << std::endl << std::endl;

    // Footer :
    out_  << std::endl;

    return;
  }

  bool data_statistics::is_initialized() const
  {
    return initialized;
  }

  void data_statistics::tree_dump(std::ostream & out_,
				  const std::string & title_,
				  const std::string & indent_,
				  bool /* inherit_*/) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl; }

    out_ << indent_ <<  datatools::i_tree_dumpable::tag
	 << "Number of events : " << number_of_events << std::endl;

    return;
  }

} // namespace fecom
