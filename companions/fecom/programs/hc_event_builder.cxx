// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>

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
  datatools::logger::priority logging = datatools::logger::PRIO_INFORMATION;;
  std::string input_filename = "";
  std::string output_filename = "";
  std::string input_path = "";
  std::string output_path = "";
  std::string input_tracker_mapping_file = "";
  std::string input_calo_mapping_file = "";
  int32_t     run_number = 0;
  int32_t     first_event_number = 0;
  bool        is_debug       = false;
  bool        is_help        = false;
  double      l1_build_gate  = event_builder::EVENT_BUILDING_CALO_L1_GATE_IN_NS;
  double      l2_build_gate  = event_builder::EVENT_BUILDING_CALO_TRACKER_L2_GATE_IN_NS;
  std::size_t min_nb_calo    = 0;
  std::size_t min_nb_tracker = 0;
  std::size_t max_events     = 0;

  try {

    // Parsing arguments:
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-i" || arg == "--input") {
        input_filename = argv_[++iarg];
      }

      else if (arg == "-o" || arg == "--output") {
        output_filename = argv_[++iarg];
      }

      else if (arg == "-op" || arg == "--output-path") {
        output_path = argv_[++iarg];
      }

      else if (arg == "-cm" || arg == "--calo-map") {
	input_calo_mapping_file = argv_[++iarg];
      }

      else if (arg == "-tm" || arg == "--tracker-map") {
	input_tracker_mapping_file = argv_[++iarg];
      }

      else if (arg == "-g1" || arg == "--build-gate-l1") {
        l1_build_gate = std::stod(argv_[++iarg]);
      }

      else if (arg == "-g2" || arg == "--build-gate-l2") {
        l2_build_gate = std::stod(argv_[++iarg]);
      }

      else if (arg == "-r" || arg == "--run-number") {
        run_number = boost::lexical_cast<std::size_t>(argv_[++iarg]);
      }

      else if (arg == "-e" || arg == "--event-number") {
        first_event_number = boost::lexical_cast<std::size_t>(argv_[++iarg]);
      }

      else if (arg == "-C" || arg == "--min-calo-hits") {
        min_nb_calo = boost::lexical_cast<std::size_t>(argv_[++iarg]);
      }

      else if (arg == "-T" || arg == "--min-tracker-hits") {
        min_nb_tracker = boost::lexical_cast<std::size_t>(argv_[++iarg]);
      }

      else if (arg == "-M" || arg == "--max-events") {
        max_events = boost::lexical_cast<std::size_t>(argv_[++iarg]);
      }

      else if (arg == "-d" || arg == "--debug") {
        is_debug = true;
      }

      else if (arg == "-h" || arg == "--help") {
        is_help = true;
      }

      else {
        DT_THROW(std::logic_error, "Unrecognised argument '" << arg << "'!");
      }

      iarg++;
    }

    if (is_help) {
      std::cerr << std::endl << "Usage :" << std::endl << std::endl
                << "fecom-hc_event_builder [OPTIONS]" << std::endl << std::endl
                << "Allowed options: " << std::endl
                << "-h    [ --help ]        produce help message" << std::endl
                << "-i    [ --input ] file  set an input file" << std::endl
                << "-o    [ --output ] file set an output file" << std::endl
                << "-op   [ --output ] path set a path where all files are store" << std::endl
                << "-d    [ --debug ]       debug mode" << std::endl << std::endl;
      return 0;
    }

    if (is_debug) logging = datatools::logger::PRIO_DEBUG;
    DT_LOG_INFORMATION(logging, "Entering HC Event Builder...");

    // Set the input file :
    DT_THROW_IF(input_filename.empty(), std::logic_error, "Missing input file!");
    // if (input_filename.empty()) input_filename = "${FECOM_RESOURCES_DIR}/data/samples/fake_run/toto.xml";
    datatools::fetch_path_with_env(input_filename);

    if (input_path.empty()) {
      boost::filesystem::path p(input_filename);
      input_path = p.parent_path().string();
      if (input_path.empty()) {
        input_path = ".";
      }
    }
    DT_LOG_INFORMATION(logging, "Input filename      : " + input_filename);
    DT_LOG_INFORMATION(logging, "Input path          : " + input_path);

    // Default output path in input path :
    if (output_path.empty()) {
      output_path = input_path;
      DT_LOG_WARNING(logging, "The output path is empty, did you forget it in the option ? Default directory for output :" + output_path);
    }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);

    DT_LOG_INFORMATION(logging, "Serialization input file :" + input_filename);
    datatools::data_reader deserializer(input_filename, datatools::using_multiple_archives);

    if (output_filename.empty()) {
      output_filename = output_path + '/' + "hc_event_builder_serialized.data.bz2";
    }
    DT_LOG_INFORMATION(logging, "Serialization output file :" + output_filename);


    // Event serializer module :
    dpp::output_module serializer;
    datatools::properties writer_config;
    writer_config.store ("logging.priority", "debug");
    writer_config.store ("files.mode", "single");
    writer_config.store ("files.single.filename", output_filename);
    serializer.initialize_standalone(writer_config);
    serializer.tree_dump(std::clog, "HC Event builder writer module");

    // Event writer :
    std::string  output_filename2 = output_path + '/' + "__test.xml";
    //    datatools::data_writer serializer2(output_filename2, datatools::using_multiple_archives);


    if (input_tracker_mapping_file.empty()) {
      input_tracker_mapping_file = input_path + "/" + "mapping_tracker.csv";
    }
    datatools::fetch_path_with_env(input_tracker_mapping_file);
    if (input_calo_mapping_file.empty()) {
      input_calo_mapping_file = input_path + "/" + "mapping_calo.csv";
    }
    datatools::fetch_path_with_env(input_calo_mapping_file);

    DT_LOG_INFORMATION(logging, "Mapping tracker file :" + input_tracker_mapping_file);
    DT_LOG_INFORMATION(logging, "Mapping calo file :" + input_calo_mapping_file);

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
    int modulo = 1000;

    while (deserializer.has_record_tag()) {
      //DT_LOG_DEBUG(logging, "Entering has record tag...");
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
        // chit.tree_dump(std::clog, "A Calo Hit");
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

        datatools::event_id event_id;
        event_id.set_run_number(run_number);
        event_id.set_event_number(event_number++);
        eb.commissioning_event_for_serialization.set_event_id(event_id);
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
	  //serializer2.store(CE);
	  std::cout.precision(15);
	  CE.print(std::cout);
          serializer.process(commissioning_event_record);
          event_serialized++;
        }
        eb.reset_serialization();
        if (max_events > 0 && (event_number >= max_events)) {
          DT_LOG_INFORMATION(logging, "Maximum number of events is reached.");
          break;
        }
      } // end of ready for serialization

      if (deserialized_hit == "calo" &&
	  !chit.low_threshold &&
	  !chit.high_threshold) {
	continue;
      }

      if (deserialized_hit == "calo") {
        // First calo hit of the event
        if (!eb.com_event_is_active) {
	  eb.working_commissioning_event.add_calo_hit(chit);
	  double time_start = chit.tdc_ns + chit.falling_time_ns;
	  eb.set_times(time_start);
	  eb.com_event_is_active = true;
	  eb.begin_by_calorimeter = true;
	}
        else {
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
              double time_start = chit.tdc_ns + chit.falling_time_ns;
              eb.set_times(time_start);
              eb.begin_by_calorimeter = true;
            }
          } // end of begin by calo
          // if not begin by calo, close actual commissioning event, copy for serialization, reset and fill it with the new calo hit
          else {
            eb.ready_for_serialization = true;
            eb.commissioning_event_for_serialization = eb.working_commissioning_event;
            eb.working_commissioning_event.reset();
            eb.working_commissioning_event.add_calo_hit(chit);
            double time_start = chit.tdc_ns + chit.falling_time_ns;
            eb.set_times(time_start);
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
      //serializer2.store(CE);
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

    std::cout << "run_number="   << run_number << std::endl;
    std::cout << "event_number=" << event_number << std::endl;
    std::cout << "min_nb_calo=" << min_nb_calo << std::endl;
    std::cout << "min_nb_tracker=" << min_nb_tracker << std::endl;

    DT_LOG_INFORMATION(logging, "The end.");
  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
