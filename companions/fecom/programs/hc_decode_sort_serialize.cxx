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
#include <boost/program_options.hpp>

// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>

// This project:
#include <fecom/hit_reader.hpp>
#include <fecom/calo_calibration.hpp>

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

  try {

    bool        is_debug = false;
    std::string input_filename = "";
    std::string input_calo_pedestal_filename = "";
    std::string output_path = "";
    std::size_t max_hits = 0;
    int         modulo   = 10000;

    // Parse options:
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")
      ("debug,d", "debug mode")
      ("input,i",
       po::value<std::string>(& input_filename),
       "set an input file")
      ("calo-pedestal,p",
       po::value<std::string>(& input_calo_pedestal_filename),
       "set a calo pedestal input file")
      ("output,o",
       po::value<std::string>(& output_path),
       "set the output path")
      ("max-events,M",
       po::value<std::size_t>(& max_hits)->default_value(10),
       "set the maximum number of hits")
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

    DT_LOG_INFORMATION(logging, "Starting...");

    // Set the input file from Jihane's DAQ :
    DT_THROW_IF(input_filename.empty(), std::logic_error, "Missing input file ! ");
    datatools::fetch_path_with_env(input_filename);

    DT_LOG_INFORMATION(logging, "Input filename      : " + input_filename);

    // Default output path in input path :
    if (output_path.empty()){
      output_path = "/tmp/";
      DT_LOG_WARNING(logging, "The output path is empty, did you forget it in the option ? Default directory for output :" + output_path);
    }
    datatools::fetch_path_with_env(output_path);
    DT_LOG_INFORMATION(logging, "Output path : " + output_path);

    std::string output_filename = output_path + '/' + "decode_sort-output.data.bz2";

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

    bool has_calo_pedestal = false;
    if (!input_calo_pedestal_filename.empty()) {
      has_calo_pedestal = true;
      DT_LOG_INFORMATION(logging, "Process do not have calo pedestal filename and no calibration...");
    }

    // Read, decode and serialize calo_pedestals from input file into calo_calibration object
    fecom::calo_calibration ccalib;
    if (has_calo_pedestal) {
      ccalib.load_pedestals(input_calo_pedestal_filename);
      ccalib.tree_dump(std::clog, "Calorimeter calibration:");
      DT_LOG_INFORMATION(logging, "Calo pedestal filename :" << input_calo_pedestal_filename);
      serializer.store(ccalib);
    }

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
        DT_LOG_DEBUG(logging, "Invalid entry!");
      }

      if (max_hits > 0 && (hit_counter >= max_hits)) {
        DT_LOG_INFORMATION(logging, "Maximum number of hits is reached.");
        break;
      }
    } // end of while reader
    // List -> [C][T][T][T][C][C][T][T][!C][T][!T]

    // Remove unvalid calo / tracker hits
    std::size_t nb_removed_hits = hl.remove_bad_hits();
    DT_LOG_DEBUG(logging, "Number of removed hits = " << nb_removed_hits);

    DT_LOG_DEBUG(logging, "Sorting...");
    hl.sort();
    DT_LOG_DEBUG(logging, "Sorting success...");

    DT_LOG_DEBUG(logging, "Begin hits serialization...");
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

    DT_LOG_INFORMATION(logging, "End of reader file...");
    reader.reset();
    DT_LOG_INFORMATION(logging, "The end.");
  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
