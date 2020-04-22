#include <iostream>

// Third Party
// - Bayeux
#include "bayeux/brio/reader.h"
#include "bayeux/datatools/multi_properties.h"
#include "bayeux/datatools/properties.h"

// - Boost
#include "boost/program_options.hpp"

// To be refactored
// This is essentially the "schema" for Falaise BRIO files, so should
// be an interface/defined in the core library
namespace {
std::string const GI_STORE{"GI"};
std::string const ER_STORE{"ER"};
}  // namespace

void do_dump_metadata(const std::string& path, std::ostream& os) {
  brio::reader brioFile;
  brioFile.open(path);
  datatools::properties p;
  datatools::multi_properties inputMP;
  std::size_t entry{0};

  // The basic loop to extract the full metadata record into a multi_propertie object
  while (brioFile.has_next(GI_STORE)) {
    brioFile.load(p, GI_STORE, entry);
    std::string key{p.fetch_string("__dpp.io.metadata.key")};
    std::string meta{p.fetch_string("__dpp.io.metadata.meta")};
    p.clean("__dpp.io.metadata.key");
    p.clean("__dpp.io.metadata.meta");
    p.clean("__dpp.io.metadata.rank");
    inputMP.add(key, meta, p);
    ++entry;
  }

  datatools::multi_properties::config w;
  w.write(os, inputMP);
}

int main(int argc, char* argv[]) {
  std::string inputFile{};

  namespace bpo = boost::program_options;
  bpo::options_description optDesc("Options");
  // clang-format off
  optDesc.add_options()
    ("help,h", "print this help message")
    ("input-file,f", bpo::value<std::string>(&inputFile)->value_name("file"),
      "BRIO file from which to read information");
  // clang-format on

  // - Parse...
  bpo::variables_map vMap;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, optDesc), vMap);
    bpo::notify(vMap);
  } catch (const std::exception& e) {
    std::cerr << "[OptionsException] " << e.what() << std::endl;
    return 1;
  }

  // Handle any non-bound options
  if (vMap.count("help") != 0u) {
    std::cout << "Usage:\n"
              << "  brio_file_dumper [options]\n\n"
              << optDesc << "\n";
  }

  // Actions (only one for now)
  if (!inputFile.empty()) {
    try {
      do_dump_metadata(inputFile, std::cout);
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
      return 1;
    }
  }

  return 0;
}