// test_io_data.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
#include <bayeux/datatools/ioutils.h>
#include <bayeux/datatools/multi_properties.h>
#include <bayeux/dpp/input_module.h>
#include <bayeux/dpp/i_data_source.h>
#include <bayeux/dpp/output_module.h>
#include <bayeux/dpp/dump_module.h>

// This project:
#include <falaise/falaise.h>

struct io_data_config {
  std::string in;
  std::string out;
};

void test_1(const io_data_config & cfg_);

int main(int argc_, char ** argv_)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test I/O data program for the 'Falaise' library."
              << std::endl;

    io_data_config conf;

    {
      int iarg = 1;
      while (iarg < argc_) {
        std::string token = argv_[iarg];
        if (token[0] == '-') {
          std::string option = token;
          if (option == "-i") {
            conf.in = argv_[++iarg];
          } else if (option == "-o") {
            conf.out = argv_[++iarg];
          } else {
            std::clog << datatools::io::warning << "ignoring option '"
                      << option << "'!" << std::endl;
          }
        } else {
          std::string argument = token;
          {
            std::clog << datatools::io::warning << "ignoring argument '"
                      << argument << "'!" << std::endl;
          }
        }
        iarg++;
      }
    }

    test_1(conf);

    std::clog << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}

void test_1(const io_data_config & config_)
{
  std::clog << "\ntest_1: Entering...\n";

  std::string indata = config_.in;
  if (indata.empty()) {
    indata = "${FALAISE_TESTING_DIR}/samples/flsim1.xml";
  }

  if (indata.empty()) {
    DT_THROW(std::logic_error, "Missing input file!");
  }

  std::string outdata = config_.out;
  if (outdata.empty()) {
    outdata = "out.xml";
  }

  if (outdata.empty()) {
    DT_THROW(std::logic_error, "Missing output file!");
  }

  dpp::input_module in_mod;
  in_mod.set_name("In");
  in_mod.set_logging_priority(datatools::logger::PRIO_TRACE);
  in_mod.set_single_input_file(indata);
  in_mod.set_clear_record(true);
  const datatools::multi_properties& in_metadata_store = in_mod.get_metadata_store();
  in_mod.initialize_simple();
  in_metadata_store.tree_dump(std::clog, "Metadata store:");
  std::clog << "test_1: Number of entries  = " << in_mod.get_source().get_number_of_entries() << "\n";
  std::clog << "test_1: Number of metadata = " << in_mod.get_source().get_number_of_metadata() << "\n";

  dpp::output_module out_mod;
  out_mod.set_name("Out");
  out_mod.set_single_output_file(outdata);
  datatools::multi_properties & out_metadata_store = out_mod.grab_metadata_store();

  dpp::dump_module dump_mod;
  dump_mod.set_name("Dump");
  dump_mod.initialize_simple();

  // Copy metadata from the input module:
  out_metadata_store = in_metadata_store;
  out_mod.initialize_simple();

  std::clog << "test_1: Event record loop...\n";
  datatools::things event_record;
  std::size_t counter = 0;
  while (true) {
    if (in_mod.is_terminated()) break;
    dpp::base_module::process_status pStatus = in_mod.process(event_record);
    if (pStatus != dpp::base_module::PROCESS_OK) {
      std::clog << "test_1: in_mod break\n";
      break;
    }
    pStatus = dump_mod.process(event_record);
    counter++;
    pStatus = out_mod.process(event_record);
    if (pStatus != dpp::base_module::PROCESS_OK) {
      std::clog << "test_1: out_mod break\n";
      break;
    }
    std::clog << "test_1: Next event...\n";
  }
  std::clog << "test_1: Event record counter = " << counter << "\n";
  std::clog << "test_1: Exiting.\n";
  return;
}
