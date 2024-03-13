// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/data_model.cc

// Ourselves:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <bayeux/mctools/simulated_data.h>
#include <falaise/snemo/datamodels/unified_digitized_data.h>
#include <falaise/snemo/datamodels/precalibrated_data.h>
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/datamodels/particle_track_data.h>

namespace snedm {

  const std::string & labels::event_record() {
    static std::string lbl("ER");
    return lbl;
  }

  const std::string & labels::event_header() {
    static std::string lbl("EH");
    return lbl;
  }

  const std::string & labels::event_builder_key() {
    static std::string lbl("event_builder");
    return lbl;
  }

  const std::string & labels::event_builder_model_key() {
    static std::string lbl("event_builder.model");
    return lbl;
  }

  const std::string & labels::simulated_data() {
    static std::string lbl("SD");
    return lbl;
  }

  const std::string & labels::unified_digitized_data() {
    static std::string lbl("UDD");
    return lbl;
  }

  const std::string & labels::precalibrated_data() {
    static std::string lbl("pCD");
    return lbl;
  }

  const std::string & labels::calibrated_data() {
    static std::string lbl("CD");
    return lbl;
  }

  const std::string & labels::tracker_clustering_data() {
    static std::string lbl("TCD");
    return lbl;
  }

  const std::string & labels::tracker_trajectory_data() {
    static std::string lbl("TTD");
    return lbl;
  }

  const std::string & labels::particle_track_data() {
    static std::string lbl("PTD");
    return lbl;
  }

  data_record_printer::data_record_printer(const datatools::things & data_record_)
    : _data_record_(data_record_)
  {
  }
  
  void data_record_printer::print(std::ostream & out_,
				  const boost::property_tree::ptree & options_) const
  {
    std::string title  = options_.get<std::string>(datatools::i_tree_dumpable::base_print_options::title_key(), "");
    std::string indent = options_.get<std::string>(datatools::i_tree_dumpable::base_print_options::indent_key(), "");

    if (!title.empty()) {
      out_ << indent << title << '\n';
    }

    std::string bank_name;
    int nbPrintedBanks = 0;
    bool simulatedData = false;
    bool eventBuilderData = false;

    {
      bank_name = "Event Header";
      if (_data_record_.has(labels::event_header()) and _data_record_.is_a<snemo::datamodel::event_header>(labels::event_header())) {
	const auto & eh = _data_record_.get<snemo::datamodel::event_header>(labels::event_header());
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << '\n';
	{
	  boost::property_tree::ptree popts;
	  popts.put("indent", indent + datatools::i_tree_dumpable::tags::skip_item());
	  popts.put("list_properties", options_.get("eh.list_properties", true));
	  eh.print_tree(out_, popts);
	}
	nbPrintedBanks++;
	if (eh.is_simulated()) {
	  simulatedData = true;
	}
	eventBuilderData = eh.is_event_builder();
      } else {
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << "<none>" << '\n';
      }
    }

    // Possible expected SD bank for simulated data:
    if (simulatedData) {
      bank_name = "Simulated Data";
      if (_data_record_.has(labels::simulated_data()) and _data_record_.is_a<mctools::simulated_data>(labels::simulated_data())) {
	const auto & sd = _data_record_.get<mctools::simulated_data>(labels::simulated_data());
	out_ << indent << datatools::i_tree_dumpable::tags::item() << std::quoted(bank_name) << " : " << '\n';
	sd.tree_dump(out_, "", indent + datatools::i_tree_dumpable::tags::skip_item());
	nbPrintedBanks++;
      } else {
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << "<none>" << '\n';
      }
    }

    // Unified digitized data:
    {
      bank_name = "Unified Digitized Data";
      if (_data_record_.has(labels::unified_digitized_data()) and _data_record_.is_a<snemo::datamodel::unified_digitized_data>(labels::unified_digitized_data())) {
	const auto & udd = _data_record_.get<snemo::datamodel::unified_digitized_data>(labels::unified_digitized_data());
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << '\n';
	{
	  boost::property_tree::ptree popts;
	  popts.put("indent", indent + datatools::i_tree_dumpable::tags::skip_item());
	  popts.put("list_hits", options_.get("udd.list_hits", true));
	  popts.put("hit_details", options_.get("udd.hit_details", true));
	  popts.put("list_properties", options_.get("udd.list_properties", true));
	  udd.print_tree(out_, popts);
	}
	nbPrintedBanks++;
      } else {
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << "<none>" << '\n';
      }
    }

    // Precalibrated data:
    {
      bank_name = "Precalibrated Data";
      if (_data_record_.has(labels::precalibrated_data()) and _data_record_.is_a<snemo::datamodel::precalibrated_data>(labels::precalibrated_data())) {
	const auto & pcd = _data_record_.get<snemo::datamodel::precalibrated_data>(labels::precalibrated_data());
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << '\n';
	{
	  boost::property_tree::ptree popts;
	  popts.put("indent", indent + datatools::i_tree_dumpable::tags::skip_item());
	  popts.put("list_hits", options_.get("pcd.list_hits", true));
	  popts.put("list_properties", options_.get("pcd.list_properties", true));
	  pcd.print_tree(out_, popts);
	}
	nbPrintedBanks++;
      } else {
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << "<none>" << '\n';
      }
    }

    // Calibrated data:
    {
      bank_name = "Calibrated Data";
      if (_data_record_.has(labels::calibrated_data()) and _data_record_.is_a<snemo::datamodel::calibrated_data>(labels::calibrated_data())) {
	const auto & cd = _data_record_.get<snemo::datamodel::calibrated_data>(labels::calibrated_data());
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << '\n';
	{
	  boost::property_tree::ptree popts;
	  popts.put("indent", indent + datatools::i_tree_dumpable::tags::skip_item());
	  popts.put("list_hits", options_.get("cd.list_hits", true));
	  popts.put("list_properties", options_.get("cd.list_properties", true));
	  cd.print_tree(out_, popts);
	}
	nbPrintedBanks++;
      } else {
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << "<none>" << '\n';
      }
    }

    // Tracker clustering data:
    {
      bank_name = "Tracker Clustering Data";
      if (_data_record_.has(labels::tracker_clustering_data()) and _data_record_.is_a<snemo::datamodel::tracker_clustering_data>(labels::tracker_clustering_data())) {
	const auto & tcd = _data_record_.get<snemo::datamodel::tracker_clustering_data>(labels::tracker_clustering_data());
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << '\n';
	{
	  boost::property_tree::ptree popts;
	  popts.put("indent", indent + datatools::i_tree_dumpable::tags::skip_item());
	  popts.put("list_solutions", options_.get("tcd.list_solutions", true));
	  popts.put("list_properties", options_.get("tcd.list_properties", true));
	  popts.put("solutions.list_clusters", options_.get("tcd.solutions.list_clusters", true));
	  popts.put("solutions.list_unclustered_hits", options_.get("tcd.solutions.list_unclustered_hits", true));
	  popts.put("solutions.list_properties", options_.get("tcd.solutions.list_properties", true));
	  tcd.print_tree(out_, popts);
	}
	nbPrintedBanks++;
      } else {
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << "<none>" << '\n';
      }
    }

    // Tracker trajectory data:
    {
      bank_name = "Tracker Trajectory Data";
      if (_data_record_.has(labels::tracker_trajectory_data()) and _data_record_.is_a<snemo::datamodel::tracker_trajectory_data>(labels::tracker_trajectory_data())) {
	const auto & cd = _data_record_.get<snemo::datamodel::tracker_trajectory_data>(labels::tracker_trajectory_data());
	out_ << indent << datatools::i_tree_dumpable::tags::item() << std::quoted(bank_name) << " : " << '\n';
	cd.tree_dump(out_, "", indent + datatools::i_tree_dumpable::tags::skip_item());
	nbPrintedBanks++;
      } else {
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << "<none>" << '\n';
      }
    }

    // Particle track data:
    {
      bank_name = "Particle Track Data";
      if (_data_record_.has(labels::particle_track_data()) and _data_record_.is_a<snemo::datamodel::particle_track_data>(labels::particle_track_data())) {
	const auto & cd = _data_record_.get<snemo::datamodel::particle_track_data>(labels::particle_track_data());
	out_ << indent << datatools::i_tree_dumpable::tags::item() << std::quoted(bank_name) << " : " << '\n';
	cd.tree_dump(out_, "", indent + datatools::i_tree_dumpable::tags::skip_item());
	nbPrintedBanks++;
      } else {
	out_ << indent << datatools::i_tree_dumpable::tags::item() << "Bank " << std::quoted(bank_name) << " : " << "<none>" << '\n';
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tags::last_item() << "Printer summary:\n";
      out_ << indent << datatools::i_tree_dumpable::tags::last_skip_item()
	   << datatools::i_tree_dumpable::tags::item() << "Simulated data: " << std::boolalpha << simulatedData << '\n';
      out_ << indent << datatools::i_tree_dumpable::tags::last_skip_item()
	   << datatools::i_tree_dumpable::tags::item() << "Event builder data: " << std::boolalpha << eventBuilderData << '\n';
      out_ << indent << datatools::i_tree_dumpable::tags::last_skip_item()
	   << datatools::i_tree_dumpable::tags::last_item() << "Number of printed banks: " << nbPrintedBanks << '\n';
    }

    out_ << std::flush;
    
    return;
  }

} // namespace snedm
