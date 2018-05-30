// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>

// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
#include <datatools/things.h>
// - Bayeux/dpp:
#include <dpp/output_module.h>

// This project:
#include <fecom/hit_reader.hpp>
#include <fecom/commissioning_event.hpp>
#include <fecom/channel_mapping.hpp>
#include <fecom/calo_calibration.hpp>

/// \brief Event builder
class event_builder
{

public :

  static constexpr double EVENT_BUILDING_CALO_L1_GATE_IN_NS = 100.0; // 100ns to stack calorimeters hits in a commissioning event
  static constexpr double EVENT_BUILDING_CALO_TRACKER_L2_GATE_IN_NS = 150000.0; // 150µs to build a commissioning event

  event_builder(double l1_time_gate_ns_ = -1.0,
		double l2_time_gate_ns_ = -1.0)
  {
    l1_time_gate = l1_time_gate_ns_;
    if (l1_time_gate < 0.0) {
      l1_time_gate = EVENT_BUILDING_CALO_L1_GATE_IN_NS;
    }
    l2_time_gate = l2_time_gate_ns_;
    if (l2_time_gate < 0.0) {
      l2_time_gate = EVENT_BUILDING_CALO_TRACKER_L2_GATE_IN_NS;
    }

    datatools::invalidate(time_start);
    datatools::invalidate(calo_time_stop);
    datatools::invalidate(l2_time_stop);
    return;
  }

  virtual ~event_builder()
  {
  }

  void set_times(const double time_start_)
  {
    time_start = time_start_;
    calo_time_stop = time_start + l1_time_gate;
    l2_time_stop = time_start + l2_time_gate;
    //std::clog << "Tstart = " << time_start << " Tstop = " << l2_time_stop << std::endl;
    working_commissioning_event.set_time_start_ns(time_start);
    return;
  }

  void build_tracker_hits_from_channels()
  {
    if (commissioning_event_for_serialization.get_tracker_channel_hit_collection().size() != 0)
      {
        commissioning_event_for_serialization.set_channel_mapping(*channel_mapping);
        commissioning_event_for_serialization.build_tracker_hit_from_channels();
      }
    return;
  }

  void reset_serialization()
  {
    commissioning_event_for_serialization.reset();
    ready_for_serialization = false;
    return;
  }

  // Configuration:
  double l1_time_gate = 0.0; //!< Time is in implicit ns
  double l2_time_gate = 0.0; //!< Time is in implicit ns

  // Working data:
  fecom::channel_mapping * channel_mapping = nullptr;
  fecom::commissioning_event commissioning_event_for_serialization;
  fecom::commissioning_event working_commissioning_event;
  double time_start;     //!< Time is in implicit ns
  double calo_time_stop; //!< Time is in implicit ns
  double l2_time_stop;   //!< Time is in implicit ns
  bool   com_event_is_active     = false;
  bool   ready_for_serialization = false;
  bool   begin_by_calorimeter    = false;

};

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_INFORMATION;

  try {

    bool        is_debug       = false;
    std::string input_filename = "";
    std::string output_path = "";
    std::size_t max_events     = 0;
    std::string input_tracker_mapping_file = "";
    std::string input_calo_mapping_file = "";
    int32_t     run_number = 0;
    std::size_t first_event_number = 0;
    std::size_t min_nb_calo    = 0;
    std::size_t min_nb_tracker = 0;
    double      l1_build_gate  = event_builder::EVENT_BUILDING_CALO_L1_GATE_IN_NS;
    double      l2_build_gate  = event_builder::EVENT_BUILDING_CALO_TRACKER_L2_GATE_IN_NS;

    // Parse options:
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")
      ("debug,d", "debug mode")
      ("input,i",
       po::value<std::string>(& input_filename),
       "set an input file")
      ("output,o",
       po::value<std::string>(& output_path),
       "set the output path")
      ("max-events,M",
       po::value<std::size_t>(& max_events)->default_value(10),
       "set the maximum number of events")
      ("calo-map,c",
       po::value<std::string>(& input_calo_mapping_file),
       "set the calorimeter map")
      ("tracker-map,t",
       po::value<std::string>(& input_tracker_mapping_file),
       "set the tracker map")
      ("run-number,r",
       po::value<int32_t>(& run_number)->default_value(-1),
       "set the run number")
      ("first-event-number,e",
       po::value<std::size_t>(& first_event_number)->default_value(0),
       "set the first event number")
      ("min-calo-hits,C",
       po::value<std::size_t>(& min_nb_calo)->default_value(0),
       "set the minimum number of calo hits in an event to record in a brio file")
      ("min-tracker-hits,T",
       po::value<std::size_t>(& min_nb_tracker)->default_value(0),
       "set the minimum number of tracker hits in an event to record in a brio file")
      ("l1-gate-value",
       po::value<double>(& l1_build_gate),
       "set the size (double) of the L1 gate (calo) for the event building")
      ("l2-gate-value",
       po::value<double>(& l2_build_gate),
       "set the size (double) of the L2 gate (tracker) for the event building")
      ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
              .options(opts)
              .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "Usage : " << std::endl;
      std::cout << opts << std::endl;
      return(1);
    }

    // Use command line arguments :
    if (vm.count("debug")) {
      is_debug = true;
    }

    if (is_debug) logging = datatools::logger::PRIO_DEBUG;
    DT_LOG_INFORMATION(logging, "Entering HC Event Builder...");

    // Set the input file :
    DT_THROW_IF(input_filename.empty(), std::logic_error, "Missing input file!");
    datatools::fetch_path_with_env(input_filename);

    DT_LOG_INFORMATION(logging, "Input filename      : " + input_filename);

    // Default output path in input path :
    if (output_path.empty()) {
      output_path = "/tmp/";
      DT_LOG_WARNING(logging, "The output path is empty, did you forget it in the option ? Default directory for output :" + output_path);
    }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);

    DT_LOG_INFORMATION(logging, "Serialization input file :" + input_filename);
    datatools::data_reader deserializer(input_filename, datatools::using_multiple_archives);

    std::string output_filename = output_path + '/' + "hc_event_builder_serialized.brio";
    DT_LOG_INFORMATION(logging, "Serialization output file :" + output_filename);

    // Event serializer module :
    dpp::output_module serializer;
    datatools::properties writer_config;
    writer_config.store ("logging.priority", "debug");
    writer_config.store ("files.mode", "single");
    writer_config.store ("files.single.filename", output_filename);
    serializer.initialize_standalone(writer_config);
    serializer.tree_dump(std::clog, "HC Event builder writer module");

    DT_THROW_IF(input_calo_mapping_file.empty() || input_tracker_mapping_file.empty(), std::logic_error, "Missing calo or/and tracker mapping config file ! ");

    DT_LOG_INFORMATION(logging, "Mapping calo file    :" + input_calo_mapping_file);
    DT_LOG_INFORMATION(logging, "Mapping tracker file :" + input_tracker_mapping_file);

    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();

    fecom::calo_hit chit;
    fecom::tracker_channel_hit tchit;

    DT_LOG_INFORMATION(logging, "Building commisioning events...");
    DT_LOG_INFORMATION(logging, "...");

    event_builder eb(l1_build_gate,
		     l2_build_gate);
    eb.channel_mapping = & my_channel_mapping;

    std::size_t hit_counter = 0;
    std::size_t event_serialized = 0;
    uint64_t event_number = first_event_number;
    int modulo = 10000;

    // Deserialize the calorimeter calibration object if the file countain one :
    fecom::calo_calibration ccalib;
    bool has_calo_pedestal = false;
    if (deserializer.record_tag_is(fecom::calo_calibration::SERIAL_TAG)) {
      deserializer.load(ccalib);
      has_calo_pedestal = true;
    }
    ccalib.tree_dump(std::clog, "Calorimeter calibration:");
    if (has_calo_pedestal) DT_LOG_INFORMATION(logging, "File has calo pedestals, calo calibration can happen !");

    while (deserializer.has_record_tag()) {
      DT_LOG_DEBUG(logging, "Entering has record tag...");
      chit.reset();
      tchit.reset();
      if ((hit_counter % modulo) == 0){
        std::clog << "Hit #" << hit_counter << std::endl;
      }
      if ((event_number % modulo) == 0){
        std::clog << "Event #" << event_number << std::endl;
      }
      std::string deserialized_hit = "none";
      if (deserializer.record_tag_is(fecom::calo_hit::SERIAL_TAG)) {
        DT_LOG_DEBUG(logging, "Entering record tag is calo_hit...");
        deserializer.load(chit);

	// Add in auxiliaries the fecom 'GID' associated to the 'EID'
	const geomtools::geom_id calo_EID = chit.electronic_id;
	geomtools::geom_id calo_GID;
	my_channel_mapping.get_geometric_id_from_electronic_id(calo_EID, calo_GID);
	chit.fecom_geom_id = calo_GID;

        DT_THROW_IF(!chit.is_valid(), std::logic_error,
                    "The deserialized calo_hit is not valid ! ");
        deserialized_hit = "calo";
      } else if (deserializer.record_tag_is(fecom::tracker_channel_hit::SERIAL_TAG)) {
        DT_LOG_DEBUG(logging, "Entering record tag is tracker_channel_hit...");
        deserializer.load(tchit);
        // tchit.tree_dump(std::clog, "A Tracker Channel Hit");
        DT_THROW_IF(!tchit.is_valid(), std::logic_error,
                    "The deserialized tracker_channel_hit is not valid ! ");
        deserialized_hit = "tracker";
      } else {
        DT_THROW(std::logic_error,
                 "Unknown object type '" << deserializer.get_record_tag() << "'!");
      }
      hit_counter++;

      // Check if the working commissioning event is ready or not for serialization
      if (eb.ready_for_serialization) {
        // Build tracker hits from tracker channels
        // DT_LOG_INFORMATION(logging, "Build tracker hits from channels");
        eb.build_tracker_hits_from_channels();

        // Store it in a datatools::things and reset it :
        datatools::things commissioning_event_record;
        commissioning_event_record.set_name("CER");
        commissioning_event_record.set_description("A single data record with banks in it");

        fecom::commissioning_event & CE
          = commissioning_event_record.add<fecom::commissioning_event>("HCRD", "The Half Commissioning Raw Data bank");

        CE = eb.commissioning_event_for_serialization;
        if (datatools::logger::is_debug(logging)) {
	  std::cerr.precision(15);
          CE.tree_dump(std::cerr, "Commissioning event before serialization", "[debug] ");
          // commissioning_event_record.tree_dump(std::cerr, "Dump CER things", "[debug] ");
        }
        bool save_it = true;
        if (CE.get_calo_hit_collection().size() < min_nb_calo) {
          save_it = false;
        }
        if (CE.get_tracker_hit_collection().size() < min_nb_tracker) {
          save_it = false;
        }
        if (save_it) {
	  datatools::event_id event_id;
	  event_id.set_run_number(run_number);
	  event_id.set_event_number(event_number++);
	  CE.set_event_id(event_id);
	  // CE.print(std::cout);
	  // CE.tree_dump(std::clog, "A com event to serialize");
          serializer.process(commissioning_event_record);
          event_serialized++;
        }
        eb.reset_serialization();
        if (max_events > 0 && (event_serialized >= max_events)) {
          DT_LOG_INFORMATION(logging, "Maximum number of serialized events is reached.");
          break;
        }
      } // end of ready for serialization

      if (deserialized_hit == "calo") {
        // First calo hit of the event
        if (!eb.com_event_is_active) {
	  eb.working_commissioning_event.add_calo_hit(chit);
	  // Fill time reference only if the hit is LTO or HT
	  if (!eb.working_commissioning_event.is_valid() && (chit.low_threshold || chit.high_threshold)) {
	    double time_start = chit.tdc_ns + chit.falling_time_ns;
	    eb.set_times(time_start);
	  }
	  eb.com_event_is_active = true;
	  eb.begin_by_calorimeter = true;
	}
        else {
	  // Fill time reference only if the hit is LTO or HT
	  if (!eb.working_commissioning_event.has_time_start_ns() && (chit.low_threshold || chit.high_threshold)) {
	    double time_start = chit.tdc_ns + chit.falling_time_ns;
	    eb.set_times(time_start);
	  }

          if (eb.begin_by_calorimeter) {
            // Check if the calo hit is in the L1 gate
            if (chit.tdc_ns < eb.calo_time_stop) {
              // Add the calo hit in the working commissioning event :
              eb.working_commissioning_event.add_calo_hit(chit);
            }
            // Outside gate, close actual commissioning event, copy for serialization, reset and fill it with the new calo hit
            else {
              eb.ready_for_serialization = true;
              eb.commissioning_event_for_serialization = eb.working_commissioning_event;
              eb.working_commissioning_event.reset();
              eb.working_commissioning_event.add_calo_hit(chit);
	      // Fill time reference only if the hit is LTO or HT
	      if (!eb.working_commissioning_event.has_time_start_ns() && (chit.low_threshold || chit.high_threshold)) {
		double time_start = chit.tdc_ns + chit.falling_time_ns;
		eb.set_times(time_start);
	      }
              eb.begin_by_calorimeter = true;
            }
          } // end of begin by calo
          // if not begin by calo, close actual commissioning event, copy for serialization, reset and fill it with the new calo hit
          else {
            eb.ready_for_serialization = true;
            eb.commissioning_event_for_serialization = eb.working_commissioning_event;
            eb.working_commissioning_event.reset();
            eb.working_commissioning_event.add_calo_hit(chit);
	    // Fill time reference only if the hit is LTO or HT
	    if (!eb.working_commissioning_event.has_time_start_ns() && (chit.low_threshold || chit.high_threshold)) {
	      double time_start = chit.tdc_ns + chit.falling_time_ns;
	      eb.set_times(time_start);
	    }
            eb.begin_by_calorimeter = true;
          } // end of else not begin by calo
        }
      } // end of if "calo"

      else if (deserialized_hit == "tracker") {
        // If the commissioning event begin by a tracker hit, create a new event and add tracker hits until there is a new calorimeter hit
        if (!eb.com_event_is_active)
	  {
	    eb.working_commissioning_event.add_tracker_channel_hit(tchit);
	    double time_start = tchit.timestamp_time_ns;
	    eb.set_times(time_start);
	    eb.begin_by_calorimeter = false;
	    eb.com_event_is_active = true;
	    // Event tracker only, add it to the first bit of the traits :
	    eb.working_commissioning_event.grab_traits().set(0, true);
	  }
        else {
          if (eb.begin_by_calorimeter) {
            // Check if the tracker channel hit is in the L2 gate
            if (tchit.timestamp_time_ns < eb.l2_time_stop) {
              // Add the tracker channel hit in the working commissioning event :
              eb.working_commissioning_event.add_tracker_channel_hit(tchit);
            }
            // Outside gate, close actual commissioning event, copy for serialization, reset and fill it with the new tracker channel hit
            else {
              eb.ready_for_serialization = true;
              eb.commissioning_event_for_serialization = eb.working_commissioning_event;
              eb.working_commissioning_event.reset();
              eb.working_commissioning_event.add_tracker_channel_hit(tchit);
              double time_start = tchit.timestamp_time_ns;
              eb.set_times(time_start);
              eb.begin_by_calorimeter = false;
              // Event tracker only, add it to the first bit of the traits :
              eb.working_commissioning_event.grab_traits().set(0, true);
            }
          } // end of if begin by calo
          else {
            // If com event not begin by a calo add the tracker channel hit until we found a new calo hit :
            eb.working_commissioning_event.add_tracker_channel_hit(tchit);
          }

        }
      } // end of if tracker

    } // end of while deserializer

    // Serialize the last commissioning event :
    eb.commissioning_event_for_serialization = eb.working_commissioning_event;
    eb.build_tracker_hits_from_channels();
    datatools::things commissioning_event_record;
    commissioning_event_record.set_name("CER");
    commissioning_event_record.set_description("Half commissioning event record");
    fecom::commissioning_event & CE = commissioning_event_record.add<fecom::commissioning_event>("HCRD", "The Half Commissioning Raw Data bank");
    CE = eb.commissioning_event_for_serialization;
    bool save_it = true;
    if (CE.get_calo_hit_collection().size() < min_nb_calo) {
      save_it = false;
    }
    if (CE.get_tracker_hit_collection().size() < min_nb_tracker) {
      save_it = false;
    }
    if (save_it) {
      datatools::event_id event_id;
      event_id.set_run_number(run_number);
      event_id.set_event_number(event_number++);
      CE.set_event_id(event_id);

      serializer.process(commissioning_event_record);
      event_serialized++;
    }
    eb.reset_serialization();

    DT_LOG_INFORMATION(logging, "Job done !");

    DT_LOG_INFORMATION(logging, "Run number : " + std::to_string(run_number));
    DT_LOG_INFORMATION(logging, "Number of commissioning event serialized : " + std::to_string(event_serialized));

    DT_LOG_INFORMATION(logging, "Output file : " << output_filename);
    DT_LOG_INFORMATION(logging, "End of reader file...");
    deserializer.reset();

    std::cout << "run_number="     << run_number << std::endl;
    std::cout << "event_number="   << event_number << std::endl;
    std::cout << "min_nb_calo="    << min_nb_calo << std::endl;
    std::cout << "min_nb_tracker=" << min_nb_tracker << std::endl;

    std::string sname = output_path + "/last_event_number.txt";
    std::ofstream offile(sname.c_str());
    offile << event_number << std::endl;
    offile.close();
    offile.clear();

    DT_LOG_INFORMATION(logging, "The end.");
  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
