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

    // Tracker :
    number_of_events_with_tracker = std::numeric_limits<std::size_t>::max();

    // Calo + tracker :
    number_of_calo_tracker_events = std::numeric_limits<std::size_t>::max();

    unmapped_channels_in_raw_data.clear();

    // Histogram ptr :
    for (unsigned int icalo = 0; icalo < fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN; icalo++) {
      charge_picocoulomb_calo_TH1F[icalo] = nullptr;
      charge_picocoulomb_ht_calo_TH1F[icalo] = nullptr;
      charge_picocoulomb_no_ht_calo_TH1F[icalo] = nullptr;
    }

    calo_distrib_TH2F = nullptr;
    calo_distrib_ht_TH2F = nullptr;
    calo_distrib_no_ht_TH2F = nullptr;
    calo_delta_t_calo_tref_TH1F = nullptr;
    tracker_distrib_TH2F = nullptr;
    tracker_only_distrib_TH2F = nullptr;
    calo_ht_number_tracker_number_distrib_TH2F = nullptr;
    calo_tracker_calo_distrib_TH2F = nullptr;
    calo_tracker_calo_ht_distrib_TH2F = nullptr;
    calo_tracker_tracker_distrib_TH2F = nullptr;
    calo_tracker_delta_t_calo_tref_TH1F = nullptr;
    calo_tracker_delta_t_anode_tref_TH1F = nullptr;
    calo_tracker_delta_t_cathode_tref_TH1F = nullptr;
    calo_tracker_delta_t_anode_cathode_same_hit_TH1F = nullptr;
    calo_tracker_delta_t_anode_anode_TH1F = nullptr;

    initialized = false;

    return;
  }

  void data_statistics::initialize()
  {
    // Init variables at 0
    // General :
    number_of_events = 0;

    // Calorimeter :
    number_of_events_with_calo = 0;
    number_of_calo_only_events = 0;
    mean_number_of_calo = std::make_pair(0,0);
    mean_number_of_calo_ht = std::make_pair(0,0);

    // Tracker :
    number_of_events_with_tracker = 0;
    number_of_tracker_only_events = 0;
    mean_number_of_tracker_cells = std::make_pair(0,0);

    // Calo + tracker :
    number_of_calo_tracker_events = 0;
    mean_number_of_calo_ht_if_calo_tracker       = std::make_pair(0,0);
    mean_number_of_tracker_cells_if_calo_tracker = std::make_pair(0,0);
    mean_number_of_tracker_cells_if_0_calo_ht    = std::make_pair(0,0);
    mean_number_of_tracker_cells_if_1_calo_ht    = std::make_pair(0,0);
    mean_number_of_tracker_cells_if_2_calo_ht    = std::make_pair(0,0);
    mean_number_of_tracker_cells_if_3p_calo_ht   = std::make_pair(0,0);

    // Initialize all histograms :
    std::string string_buffer = "";
    for (unsigned int icalo = 0; icalo < fecom::calo_constants::NUMBERS_OF_CALO_PER_COLUMN; icalo++) {
      string_buffer = "charge_picocoulomb_calo_" + std::to_string(icalo);
      charge_picocoulomb_calo_TH1F[icalo] = new TH1F(string_buffer.c_str(),
						     Form("Calorimeter charge picocoulomb, row %i", icalo),
						     1000, 0, 0.5);

      string_buffer = "charge_picocoulomb_ht_calo_" + std::to_string(icalo);
      charge_picocoulomb_ht_calo_TH1F[icalo] = new TH1F(string_buffer.c_str(),
							Form("Calorimeter charge picocoulomb if HT, row %i", icalo),
							1000, 0, 0.5);

      string_buffer = "charge_picocoulomb_no_ht_calo_" + std::to_string(icalo);
      charge_picocoulomb_no_ht_calo_TH1F[icalo] = new TH1F(string_buffer.c_str(),
							   Form("Calorimeter charge picocoulomb no HT, row %i", icalo),
							   1000, 0, 0.5);

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

    string_buffer = "calo_delta_t_calo_tref_TH1F";
    calo_delta_t_calo_tref_TH1F = new TH1F(string_buffer.c_str(),
					   Form("Calo events 2+ calo HT, DT(calo_X - calo_tref)"),
					   1000, 0, 300);

    string_buffer = "tracker_distrib_TH2F";
    tracker_distrib_TH2F = new TH2F(string_buffer.c_str(),
				    Form("Tracker cell distribution"),
				    6, 0, 6,
				    10, 0, 10);

    string_buffer = "tracker_only_distrib_TH2F";
    tracker_only_distrib_TH2F = new TH2F(string_buffer.c_str(),
					 Form("Tracker only cell distribution"),
					 6, 0, 6,
					 10, 0, 10);

    string_buffer = "calo_ht_number_tracker_number_distrib_TH2F";
    calo_ht_number_tracker_number_distrib_TH2F  = new TH2F(string_buffer.c_str(),
							   Form("Number of calo HT vs number of tracker distribution"),
							   8, 0, 8,
							   13, 0, 13);

    string_buffer = "calo_tracker_calo_distrib_TH2F";
    calo_tracker_calo_distrib_TH2F  = new TH2F(string_buffer.c_str(),
					       Form("Calo distribution if calo + tracker events"),
					       20, 0, 20,
					       14, 0, 14);

    string_buffer = "calo_tracker_calo_ht_distrib_TH2F";
    calo_tracker_calo_ht_distrib_TH2F  = new TH2F(string_buffer.c_str(),
						  Form("Calo distribution if calo HT + tracker events"),
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
						   1000, 0, 1000);

    string_buffer = "calo_tracker_delta_t_anode_tref_TH1F";
    calo_tracker_delta_t_anode_tref_TH1F = new TH1F(string_buffer.c_str(),
						    Form("Calo + tracker events, DT(anode_X - calo_tref)"),
						    1000, 0, 200000);

    string_buffer = "calo_tracker_delta_t_anode_anode_TH1F";
    calo_tracker_delta_t_anode_anode_TH1F = new TH1F(string_buffer.c_str(),
						     Form("Calo + tracker events, DT(anode_X - anode_Y)"),
						     1000, 0, 200000);

    string_buffer = "calo_tracker_delta_t_cathode_tref_TH1F";
    calo_tracker_delta_t_cathode_tref_TH1F = new TH1F(string_buffer.c_str(),
						      Form("Calo + tracker events, DT(cathode_X - calo_tref)"),
						      1000, 0, 200000);

    string_buffer = "calo_tracker_delta_t_anode_cathode_same_hit_TH1F";
    calo_tracker_delta_t_anode_cathode_same_hit_TH1F = new TH1F(string_buffer.c_str(),
								Form("Calo + tracker events, DT(anode_X - cathode_X)"),
								1000, 0, 10000);


    initialized = true;

    return;
  }

  bool data_statistics::is_initialized() const
  {
    return initialized;
  }

  void data_statistics::save_in_root_file(TFile * root_file_)
  {
    root_file_->cd();
    root_file_->Write("",  TObject::kOverwrite);

    return;
  }

  void data_statistics::print(std::ostream & out_)
  {
    double eff_only_calo    = (static_cast<double>(number_of_calo_only_events) / static_cast<double>(number_of_events)) * 100.;
    double eff_only_tracker = (static_cast<double>(number_of_tracker_only_events) /  static_cast<double>(number_of_events)) * 100.;
    double eff_calo_tracker = (static_cast<double>(number_of_calo_tracker_events) /  static_cast<double>(number_of_events)) * 100.;
    double eff_with_calo    = (static_cast<double>(number_of_events_with_calo) /  static_cast<double>(number_of_events)) * 100.;
    double eff_with_tracker = (static_cast<double>(number_of_events_with_tracker) /  static_cast<double>(number_of_events)) * 100.;

    out_ << "***GENERAL STATISTICS***" << std::endl;
    out_ << "Total number of commissioning events : " << number_of_events <<  std::endl;
    out_ << "Total number of calo only events     : " << number_of_calo_only_events    << " Eff : " << eff_only_calo    << "%" << std::endl;
    out_ << "Total number of tracker only events  : " << number_of_tracker_only_events << " Eff : " << eff_only_tracker << "%" << std::endl;
    out_ << "Total number of calo tracker events  : " << number_of_calo_tracker_events << " Eff : " << eff_calo_tracker << "%" << std::endl;
    out_ << "Total number of events with calo     : " << number_of_events_with_calo    << " Eff : " << eff_with_calo    << "%" << std::endl;
    out_ << "Total number of events with tracker  : " << number_of_events_with_tracker << " Eff : " << eff_with_tracker << "%" << std::endl << std::endl;

    out_ << "***CALO STATISTICS***" << std::endl;
    out_ << "Mean number of calo (all events)     : " << mean_number_of_calo.first << " Calo hit counter : " << mean_number_of_calo.second << std::endl;
    out_ << "Mean number of calo HT (all events)  : " << mean_number_of_calo_ht.first << " Calo HT hit counter : " << mean_number_of_calo_ht.second << std::endl << std::endl;


    out_ << "***TRACKER STATISTICS***" << std::endl;
    out_ << "Mean number of tracker cells (all events) : " << mean_number_of_tracker_cells.first << " Tracker hit counter : " << mean_number_of_tracker_cells.second << std::endl;
    out_ << "Mean number of tracker cells (no ht calo events) : " << mean_number_of_tracker_cells_if_0_calo_ht.first << " Tracker hit counter : " << mean_number_of_tracker_cells_if_0_calo_ht.second << std::endl;

    for (std::size_t i = 0;
	 i < cell_registers.size();
	 i++)
      {
	double bot_cathode_efficiency = 0;
	if (cell_registers[i].bot_cathode_efficiency != 0) bot_cathode_efficiency = (static_cast<double>(cell_registers[i].bot_cathode_efficiency) / static_cast<double>(cell_registers[i].R0_count)) * 100.;
	out_ << "Layer=" << i
	     << " Counter=" << cell_registers[i].hit_counter
	     << " R0=" << cell_registers[i].R0_count
	     << " R1=" << cell_registers[i].R1_count
	     << " R2=" << cell_registers[i].R2_count
	     << " R3=" << cell_registers[i].R3_count
	     << " R4=" << cell_registers[i].R4_count
	     << " RBOT=" << cell_registers[i].Rbot_cath_count
	     << " RTOP=" << cell_registers[i].Rtop_cath_count
	     << " BOT_EFF=" << bot_cathode_efficiency << "%" << std::endl;
      }
    out_ << std::endl;

    out_ << "***CALO+TRACKER STATISTICS***" << std::endl;
    out_ << "Mean number of calo HT (calo+tracker events)       : " << mean_number_of_calo_ht_if_calo_tracker.first << " Calo HT hit counter : " << mean_number_of_calo_ht_if_calo_tracker.second << std::endl;
    out_ << "Mean number of tracker cells (calo+tracker events) : " << mean_number_of_tracker_cells_if_calo_tracker.first << " Tracker hit counter : " << mean_number_of_tracker_cells_if_calo_tracker.second << std::endl;
    out_ << "Mean number of tracker cells (1 calo events)       : " << mean_number_of_tracker_cells_if_1_calo_ht.first << " Tracker hit counter : " << mean_number_of_tracker_cells_if_1_calo_ht.second << std::endl;
    out_ << "Mean number of tracker cells (2 calo events)       : " << mean_number_of_tracker_cells_if_2_calo_ht.first << " Tracker hit counter : " << mean_number_of_tracker_cells_if_2_calo_ht.second << std::endl;
    out_ << "Mean number of tracker cells (3+ calo events)      : " << mean_number_of_tracker_cells_if_3p_calo_ht.first << " Tracker hit counter : " << mean_number_of_tracker_cells_if_3p_calo_ht.second << std::endl;

    for (std::size_t i = 0;
	 i < cell_registers_if_calo_tracker.size();
	 i++)
      {
	double bot_cathode_efficiency = 0;
	if (cell_registers_if_calo_tracker[i].bot_cathode_efficiency != 0) bot_cathode_efficiency = (static_cast<double>(cell_registers_if_calo_tracker[i].bot_cathode_efficiency) / static_cast<double>(cell_registers_if_calo_tracker[i].R0_count)) * 100.;
	out_ << "Layer=" << i
	     << " Counter=" << cell_registers_if_calo_tracker[i].hit_counter
	     << " R0=" << cell_registers_if_calo_tracker[i].R0_count
	     << " R1=" << cell_registers_if_calo_tracker[i].R1_count
	     << " R2=" << cell_registers_if_calo_tracker[i].R2_count
	     << " R3=" << cell_registers_if_calo_tracker[i].R3_count
	     << " R4=" << cell_registers_if_calo_tracker[i].R4_count
	     << " RBOT=" << cell_registers_if_calo_tracker[i].Rbot_cath_count
	     << " RTOP=" << cell_registers_if_calo_tracker[i].Rtop_cath_count
	     << " BOT_EFF=" << bot_cathode_efficiency << "%" << std::endl;
      }
    out_ << std::endl;

    out_ << "***OTHERS***" << std::endl;
    out_ << "Number of unmapped channels in raw data : " << unmapped_channels_in_raw_data.size() << std::endl;
    // for (std::size_t i = 0;
    // 	 i < unmapped_channels_in_raw_data.size();
    // 	 i++)
    //   {
    // 	out_ << unmapped_channels_in_raw_data[i] << std::endl;
    //   }

    // Footer :
    out_  << std::endl;

    return;
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
