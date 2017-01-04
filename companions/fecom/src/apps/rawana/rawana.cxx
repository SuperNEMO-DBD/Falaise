// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>

// Third Party
// - Boost
#include <boost/program_options.hpp>
// - Bayeux
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/exception.h>

// This project:
#include <fecom/calo_calibration.hpp>
#include <fecom/calo_hit_reader.hpp>

struct rawana_config
{
  std::string logging_level;
  uint16_t    board_id;
  std::string calib_file;
  std::string data_file;
};

void build_options(boost::program_options::options_description & opts_,
                   rawana_config & params_);

void print_usage(boost::program_options::options_description & opts_,
                 std::ostream & out_);

//! \brief Raw data analysis
struct rawana
{
  rawana();
  void initialize(const rawana_config &);
  void run();
  void reset();
  void process_hit(const fecom::calo_hit & chit);

public:

  datatools::logger::priority logging;
  fecom::calo_calibration calo_calib;
  fecom::calo_hit_reader  calo_reader;

};

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {

    namespace po = boost::program_options;

    rawana_config params;
    po::options_description opts;
    build_options(opts, params);

    po::options_description optPublic;
    optPublic.add(opts);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(optPublic)
      .run();
    po::store(parsed, vm);
    po::notify(vm);

    // Fetch the opts/args :
    if (vm.count("help")) {
      if (vm["help"].as<bool>()) {
        print_usage(optPublic, std::cout);
        return (1);
      }
    }

    if (vm.count("logging")) {
      const std::string logging_label = vm["logging"].as<std::string>();
      datatools::logger::priority logging = datatools::logger::get_priority(params.logging_level);
      DT_THROW_IF(logging == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority label '" << params.logging_level << "' !");
    }

    rawana ana;
    ana.initialize(params);
    ana.run();
    ana.reset();

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void build_options(boost::program_options::options_description & opts_,
                   rawana_config & params_)
{
  namespace po = boost::program_options;
  opts_.add_options()

    ("help,h",
     po::value<bool>()
     ->zero_tokens()
     ->default_value(false),
     "Produce help message")

    ("logging,P",
     po::value<std::string>(&params_.logging_level)->default_value("fatal")
     ->value_name("level"),
     "Set the logging priority threshold.\n"
     "Example : \n"
     " --logging \"debug\""
     )

    ("board-id,b",
     po::value<uint16_t>(&params_.board_id)
     ->value_name("file"),
     "set the board ID."
     )

    ("calib-data,c",
     po::value<std::string>(&params_.calib_file)
     ->value_name("file"),
     "set the calibration data file name."
     )

    ("input-data,i",
     po::value<std::string>(&params_.data_file)
     ->value_name("file"),
     "set the input data file name."
     )

    ;
  return;
}

void print_usage(boost::program_options::options_description & opts_,
                 std::ostream & out_)
{
  const std::string APP_NAME = "rawana";
  out_ << "\n" << APP_NAME << " -- Raw data analysis" << std::endl;
  out_ << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << std::endl;
  out_ << "  " << APP_NAME << " [OPTIONS] [ARGUMENTS] " << std::endl;
  out_ << opts_ << std::endl;
  out_ << std::endl;
  return;
}

rawana::rawana()
{
  return;
}

void rawana::initialize(const rawana_config & cfg_)
{
  calo_calib.load_pedestals(cfg_.board_id, cfg_.calib_file);
  calo_reader.set_input_filename(cfg_.data_file);
  calo_reader.initialize();
  return;
}

void rawana::process_hit(const fecom::calo_hit & chit_)
{
  chit_.tree_dump(std::clog, "Calo hit: ");

  return;
}


void rawana::run()
{
  fecom::calo_hit chit;
  std::size_t hit_counter = 0;
  while(calo_reader.has_next_hit()) {
    chit.reset();
    calo_reader.load_next_hit(chit);
    process_hit(chit);
    hit_counter++;
  }
  return;
}

void rawana::reset()
{
  calo_reader.reset();
  calo_calib.clear_pedestals();
  return;
}
