// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>

// Third party:
// - Boost:
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
// Code dedicated to the serialization of the ``std::set`` template class :
// #include <boost/serialization/serialization.hpp>
// #include <boost/serialization/set.hpp>
#include <boost/program_options.hpp>

// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>

// This project:
#include <fecom/hit_reader.hpp>
#include <fecom/channel_mapping.hpp>

/// \brief Working list of raw hits
struct hit_list
{
  typedef std::shared_ptr<fecom::base_hit> hit_shptr;
  typedef std::list<hit_shptr> hit_coll_type;
  hit_coll_type hits;
  fecom::calo_hit * chit_ptr = nullptr;
  fecom::tracker_channel_hit * tchit_ptr = nullptr;

  void make_new_calo_hit()
  {
    hit_shptr sh(new fecom::calo_hit);
    hits.push_back(sh);
    chit_ptr = dynamic_cast<fecom::calo_hit *>(hits.back().get());
    chit_ptr->tdc_cell_step_ns = 0.390625;
    return;
  }

  void make_new_tracker_channel_hit()
  {
    hit_shptr sh(new fecom::tracker_channel_hit);
    hits.push_back(sh);
    tchit_ptr = dynamic_cast<fecom::tracker_channel_hit *>(hits.back().get());
    return;
  }

  fecom::calo_hit & chit()
  {
    return *chit_ptr;
  }

  fecom::tracker_channel_hit & tchit()
  {
    return *tchit_ptr;
  }

  static bool comparetimestamp(const std::shared_ptr<fecom::base_hit> & a,
                               const std::shared_ptr<fecom::base_hit> & b)
  {
    const fecom::base_hit * hit_1 = a.get();
    const fecom::base_hit * hit_2 = b.get();
    return hit_1->get_timestamp() < hit_2->get_timestamp();
  }

  std::size_t remove_bad_hits()
  {
    std::size_t sz1 = hits.size();
    for (hit_coll_type::iterator it_list = hits.begin(); it_list != hits.end();) {
      bool do_erase = false;
      if (!it_list->get()->is_valid() || it_list->get()->get_timestamp() == 0) {
        do_erase = true;
      }
      if (do_erase) {
        it_list = hits.erase(it_list);
      } else {
        ++it_list;
      }
    }
    std::size_t sz2 = hits.size();
    return sz1 - sz2;
  }

  void sort()
  {
    hits.sort(comparetimestamp);
  }

};

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_INFORMATION;
  std::string input_filename = "";
  std::string output_filename = "";
  std::string input_tracker_mapping_file = "";
  std::string input_calo_mapping_file = "";
  std::string input_path  = "";
  std::string output_path = "";
  std::size_t max_hits = 0;
  bool        is_debug = false;
  bool        is_help  = false;
  int         modulo   = 1000;

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

    else if (arg == "-cm" || arg == "--calo-map") {
      input_calo_mapping_file = argv_[++iarg];
    }

    else if (arg == "-tm" || arg == "--tracker-map") {
      input_tracker_mapping_file = argv_[++iarg];
    }

    else if (arg == "-op" || arg == "--output-path") {
      output_path = argv_[++iarg];
    }

    else if (arg == "-M" || arg == "--max-hits") {
      max_hits = boost::lexical_cast<std::size_t>(argv_[++iarg]);
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
              << "fecom-decode_sort_serialize [OPTIONS] " << std::endl << std::endl
              << "Allowed options: " << std::endl
              << "-h         [ --help ]        produce help message" << std::endl
              << "-i         [ --input ]       set an input file" << std::endl
              << "-o         [ --output ]      set a path where all files are store" << std::endl
              << "-d         [ --display ]     display things for debug" << std::endl << std::endl;
    return 0;
  }

  if (is_debug) logging = datatools::logger::PRIO_DEBUG;

  try {
    DT_LOG_INFORMATION(logging, "Starting...");

    // Set the input file from Jihane's DAQ :
    DT_THROW_IF(input_filename.empty(), std::logic_error, "Missing input file!");
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
    if (output_path.empty()){
      output_path = input_path;
      DT_LOG_WARNING(logging, "The output path is empty, did you forget it in the option ? Default directory for output :" + output_path);
    }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);

    if (output_filename.empty()) {
      output_filename = output_path + '/' + "decode_sort-output.data.bz2";
    }
    DT_LOG_INFORMATION(logging, "Serialization output file :" + output_filename);
    datatools::data_writer serializer(output_filename,
                                      datatools::using_multiple_archives);

    // Input file (from Jihanne) hits reader :
    fecom::hit_reader reader;
    reader.set_logging(datatools::logger::PRIO_FATAL);
    reader.set_input_filename(input_filename);
    reader.initialize();
    fecom::run_header header;
    reader.load_run_header(header);
    DT_LOG_INFORMATION(logging, "File [" << input_filename << "]   header : ");
    header.tree_dump(std::clog, "Run header:");

    // Build all tracker hit after the build of all commissioning event :
    if (input_tracker_mapping_file.empty()) {
      input_tracker_mapping_file = input_path + '/' + "mapping_tracker.csv";
    }
    datatools::fetch_path_with_env(input_tracker_mapping_file);

    if (input_calo_mapping_file.empty()) {
      input_calo_mapping_file = input_path + '/' + "mapping_calo.csv";
    }
    datatools::fetch_path_with_env(input_calo_mapping_file);

    DT_LOG_INFORMATION(logging, "Mapping tracker file :" + input_tracker_mapping_file);
    DT_LOG_INFORMATION(logging, "Mapping calo file :" + input_calo_mapping_file);

    fecom::channel_mapping my_channel_mapping;
    my_channel_mapping.build_tracker_mapping_from_file(input_tracker_mapping_file);
    my_channel_mapping.build_calo_mapping_from_file(input_calo_mapping_file);
    my_channel_mapping.initialize();

    //    std::size_t event_serialized = 0;
    hit_list hl;
    hl.make_new_calo_hit();
    hl.make_new_tracker_channel_hit();
    // List -> [!C][!T]
    //           ^   ^

    DT_LOG_INFORMATION(logging, "Read and store raw hits in object...");
    DT_LOG_INFORMATION(logging, "...");

    std::size_t entry_counter = 0;
    std::size_t hit_counter = 0;
    std::size_t bad_hit_counter = 0;
    while(reader.has_next_hit()) {
      DT_LOG_DEBUG(logging, "Entry counter = " << entry_counter);
      if ((entry_counter % modulo) == 0) {
        DT_LOG_INFORMATION(logging, "Entry #" << entry_counter);
      }
      reader.load_next_hit(hl.chit(), hl.tchit());
      entry_counter++;
      if (hl.chit().is_valid()) {
        // hl.chit().tree_dump(std::clog, "Calo hit is valid :");
        hl.make_new_calo_hit();
        hit_counter++;
        // List -> [C][!T][!C]
        //              ^   ^
      } else if (hl.tchit().is_valid()) {
        // Do not add tracker channel hit with a null value in the register :
        if (hl.tchit().timestamp_time_ns > 0.0) {
          // tchit.tree_dump(std::clog, "Tracker channel hit is valid :");
          hl.make_new_tracker_channel_hit();
          hit_counter++;
          // List -> [!C][T][!T]
          //           ^      ^
        } else {
          bad_hit_counter++;
          hl.tchit().reset();
        }
      } else {
        DT_LOG_INFORMATION(logging, "Invalid entry!");
      }

      if (max_hits > 0 && (hit_counter >= max_hits)) {
        DT_LOG_INFORMATION(logging, "Maximum number of hits is reached.");
        break;
      }
    } // end of while reader
    // List -> [C][T][T][T][C][C][T][T][!C][T][!T]
    std::clog << "Number of entries      = " << entry_counter << std::endl;
    std::clog << "Size of the list       = " << hl.hits.size() << std::endl;
    std::clog << "Number of bad hits     = " << bad_hit_counter << std::endl;

    // Remove unvalid calo / tracker hits
    std::size_t nb_removed_hits = hl.remove_bad_hits();
    std::clog << "Number of removed hits = " << nb_removed_hits << std::endl;
    std::clog << "Final number of hits   = " << hl.hits.size() << std::endl;

    DT_LOG_INFORMATION(logging, "Sorting...");
    hl.sort();
    DT_LOG_INFORMATION(logging, "Sorting success...");

    DT_LOG_INFORMATION(logging, "Begin hits serialization...");
    std::size_t serial_counter = 0;
    for (auto it_list = hl.hits.begin();
         it_list != hl.hits.end();
         it_list++) {
      // it_list->get()->tree_dump(std::clog, "Hit number #" + std::to_string(serial_counter));

      if ((serial_counter % modulo) == 0) {
        DT_LOG_INFORMATION(logging, "Serialized hit #" << serial_counter);
      }

      fecom::calo_hit * chit_to_serialize = nullptr;
      fecom::tracker_channel_hit * tchit_to_serialize = nullptr;

      if (it_list->get()->hitmode == fecom::base_hit::SIG_CALORIMETER) {
        chit_to_serialize = dynamic_cast<fecom::calo_hit *>(it_list->get());
        serializer.store(*chit_to_serialize);
      } else if (it_list->get()->hitmode == fecom::base_hit::SIG_TRACKER) {
        tchit_to_serialize = dynamic_cast<fecom::tracker_channel_hit *>(it_list->get());
        serializer.store(*tchit_to_serialize);
      }

      serial_counter++;
    }

    DT_LOG_INFORMATION(logging, "Number of commissioning hits serialized : " + std::to_string(serial_counter));

    DT_LOG_INFORMATION(logging, "Output file : " << output_filename);
    DT_LOG_INFORMATION(logging, "End of reader file...");
    reader.reset();
    DT_LOG_INFORMATION(logging, "The end.");
  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
