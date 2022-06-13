// Standard library
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <memory>

// Third Party:
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
// Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/utils.h>
#include <bayeux/geomtools/geom_id.h>

// This project
#include "falaise/exitcodes.h"
#include "falaise/version.h"
#include "falaise/snemo/time/time_utils.h"
#include "falaise/snemo/rc/tracker_cell_status.h"

struct app_params
{
  datatools::logger::priority log_level = datatools::logger::PRIO_FATAL;
  int module_id = 0;
  std::vector<std::string> list_of_dead_cells;
  std::vector<std::string> list_of_off_cells;
  std::string output_file;
  std::string default_period_repr = "[2022-01-01 00:00:00/2035-01-01 00:00:00)";
  bool all_cells = false;
};

void app_help(std::ostream & out_, const boost::program_options::options_description & od_);

void app_run(const app_params & params_);

int main(int argc_, char * argv_[])
{
  app_params appParams;
  {
    std::string verbosityLabel;
    namespace bpo = boost::program_options;
    bpo::options_description optDesc("Options");
    optDesc.add_options()
      ("help,h", "print this help message")
      ("verbosity,V", bpo::value<std::string>(&verbosityLabel)->value_name("level"),
       "set the verbosity level")

      ("dead-cell,d",
       bpo::value<std::vector<std::string>>(&appParams.list_of_dead_cells)->value_name("cellID"),
       "dead cell ID\n" 
       "Format: \"S.L.R\"\n"
       "  S : side ID (0..1)\n"
       "  L : layer ID (0..8)\n"
       "  E : row ID (0..112)\n"
       "Examples: \n"
       "  \"1.0.0\"\n"
       "  \"0.8.112\"\n"
       "  \"0.*.53\" (for a full row)\n"
       "  \"0.*.54\" (for a full row)\n"
       "  \"0.4.*\" (for a full layer)\n"
      )

      ("off-cell,o",
       bpo::value<std::vector<std::string>>(&appParams.list_of_off_cells)->value_name("cellID"),
       "off cell ID (see --dead-cell switch for the syntax)")

      ("file,l",
       bpo::value<std::string>(&appParams.output_file)->value_name("filename"),
       "file in which to store the final list of cells' status")

      ("module,M",
       bpo::value<int>(&appParams.module_id)->value_name("moduleID")->default_value(0),
       "module ID")

      ("period,P",
       bpo::value<std::string>(&appParams.default_period_repr)
       ->value_name("period")
       ->default_value("[2022-01-01 00:00:00/2035-01-01 00:00:00)"),
       "default validity period for tracker cell status")

      ("all,a",
       bpo::value<bool>(&appParams.all_cells)->zero_tokens()->default_value(false),
       "generate status for all cells, even the `good' ones")
      
    ;
    // clang-format on
    
    // - Store first, handling parse errors
    bpo::variables_map vMap;
    try {
      bpo::store(bpo::parse_command_line(argc_, argv_, optDesc), vMap);
      bpo::notify(vMap);
    } catch (const bpo::required_option & e) {
      // We need to handle help/version even if required_option thrown
      if ((vMap.count("help") == 0u)) {
        std::cerr << "flsimrc-mkcellstatuslist : " << e.what() << "\n";
        return falaise::EXIT_ERROR;
      }
    } catch (const std::exception & e) {
      std::cerr << "flsimrc-mkcellstatuslist : " << e.what() << "\n";
      return falaise::EXIT_ERROR;
    }

    // Handle any non-bound options
    if (vMap.count("help") != 0u) {
      app_help(std::cout, optDesc);
      return falaise::EXIT_OK;
    }

    if (vMap.count("verbosity") != 0u) {
      appParams.log_level = datatools::logger::get_priority(verbosityLabel);
      if (appParams.log_level == datatools::logger::PRIO_UNDEFINED) {
        std::cerr << "flsimrc-mkcellstatuslist : " << "Invalid verbosity level '" + verbosityLabel + "'!\n";
        return falaise::EXIT_ERROR;
      }
    }
        
  }

  try {
    app_run(appParams);
  } catch (const std::exception & e) {
    std::cerr << "flsimrc-mkcellstatuslist : " << e.what() << "\n";
    return falaise::EXIT_ERROR;
  }
  
  return falaise::EXIT_OK;
}

void app_help(std::ostream & out_, const boost::program_options::options_description & od_)
{
  out_ << "flsimrc-mkcellstatuslist (" << falaise::version::get_version()
       << ") : Build a skeleton list of SuperNEMO tracker cell status (dead/off)\n";
  out_ << "Usage:\n"
       << "  flsimrc-mkcellstatuslist [options]\n"
       << od_ << "\n";
  out_ << "Example:\n\n";
  out_ << "  flsimrc-mkcellstatuslist -d \"0.1.1\" \\\n";
  out_ << "                           -d \"1.3.5\" \\\n";
  out_ << "                           -d \"0.8.89\" \\\n";
  out_ << "                           -d \"1.7.112\" \\\n";
  out_ << "                           -o \"0.*.34\" \\\n";
  out_ << "                           -o \"0.*.35\" \\\n";
  out_ << "                           -o \"*.*.9\" \\\n";
  out_ << "                           -o \"*.*.10\" \\\n";
  out_ << "                           -p \"[2022-01-01 00:00:00/2023-03-01 12:00:00)\" \\\n";
  out_ << "                           -l traker_cell_status.lis \n\n";
  return;
}
  
void app_run(const app_params & params_)
{

  DT_LOG_DEBUG(params_.log_level, "Module ID : " << params_.module_id);
  DT_LOG_DEBUG(params_.log_level, "Number of dead cells : " << params_.list_of_dead_cells.size());
  DT_LOG_DEBUG(params_.log_level, "Number of off cells : " << params_.list_of_off_cells.size());
  DT_LOG_DEBUG(params_.log_level, "Output file : '" << params_.output_file << "'");
  DT_LOG_DEBUG(params_.log_level, "Default period : '" << params_.default_period_repr << "'");
  DT_LOG_DEBUG(params_.log_level, "All cells : " << std::boolalpha << params_.all_cells);
  
  // Hard coded informations about the tracker:
  int driftCellCoreCat = 1204;
  std::uint32_t nSides = 2;
  std::uint32_t nLayers = 9;
  std::uint32_t nRows = 113;
  
  std::set<geomtools::geom_id> deadCells;
  std::set<geomtools::geom_id> offCells;

  // List dead cells patterns:
  for (auto deadCellRepr : params_.list_of_dead_cells) {
    std::vector<std::string> tokens;
    boost::split(tokens, deadCellRepr, boost::is_any_of("."));
    std::uint32_t sideId  = geomtools::geom_id::INVALID_ADDRESS;
    std::uint32_t layerId = geomtools::geom_id::INVALID_ADDRESS;
    std::uint32_t rowId   = geomtools::geom_id::INVALID_ADDRESS;
    DT_THROW_IF(tokens.size() != 3, std::logic_error, "Invalid cell ID token '" << deadCellRepr << "'!");
    if (tokens[0] == "*") {
      sideId = geomtools::geom_id::ANY_ADDRESS;
    } else {
      sideId = std::stoi(tokens[0]);
    }
    if (tokens[1] == "*") {
      layerId = geomtools::geom_id::ANY_ADDRESS;
    } else {
      layerId = std::stoi(tokens[1]);
    }
    if (tokens[2] == "*") {
      rowId = geomtools::geom_id::ANY_ADDRESS;
    } else {
      rowId = std::stoi(tokens[2]);
    }
    DT_THROW_IF(sideId  != geomtools::geom_id::ANY_ADDRESS and sideId  >= nSides,  std::domain_error, "Invalid side ID!");
    DT_THROW_IF(layerId != geomtools::geom_id::ANY_ADDRESS and layerId >= nLayers, std::domain_error, "Invalid layer ID!");
    DT_THROW_IF(rowId   != geomtools::geom_id::ANY_ADDRESS and rowId   >= nRows,   std::domain_error, "Invalid row ID!");
    geomtools::geom_id deadCellGid(driftCellCoreCat, params_.module_id, sideId, layerId, rowId);
    deadCells.insert(deadCellGid);
  }
  for (auto deadCellGid : deadCells) {
    DT_LOG_DEBUG(params_.log_level, "Dead cell : " << deadCellGid);
  }
  
  // List off cells patterns:
  for (auto offCellRepr : params_.list_of_off_cells) {
    std::vector<std::string> tokens;
    boost::split(tokens, offCellRepr, boost::is_any_of("."));
    std::uint32_t sideId  = geomtools::geom_id::INVALID_ADDRESS;
    std::uint32_t layerId = geomtools::geom_id::INVALID_ADDRESS;
    std::uint32_t rowId   = geomtools::geom_id::INVALID_ADDRESS;
    DT_THROW_IF(tokens.size() != 3, std::logic_error, "Invalid cell ID token '" << offCellRepr << "'!");
    if (tokens[0] == "*") {
      sideId = geomtools::geom_id::ANY_ADDRESS;
    } else {
      sideId = std::stoi(tokens[0]);
    }
    if (tokens[1] == "*") {
      layerId = geomtools::geom_id::ANY_ADDRESS;
    } else {
      layerId = std::stoi(tokens[1]);
    }
    if (tokens[2] == "*") {
      rowId = geomtools::geom_id::ANY_ADDRESS;
    } else {
      rowId = std::stoi(tokens[2]);
    }
    DT_THROW_IF(sideId  != geomtools::geom_id::ANY_ADDRESS and sideId  >= nSides,  std::domain_error, "Invalid side ID!");
    DT_THROW_IF(layerId != geomtools::geom_id::ANY_ADDRESS and layerId >= nLayers, std::domain_error, "Invalid layer ID!");
    DT_THROW_IF(rowId   != geomtools::geom_id::ANY_ADDRESS and rowId   >= nRows,   std::domain_error, "Invalid row ID!");
    geomtools::geom_id offCellGid(driftCellCoreCat, params_.module_id, sideId, layerId, rowId);
    offCells.insert(offCellGid);
  }
  for (auto offCellGid : offCells) {
    DT_LOG_DEBUG(params_.log_level, "Off cell : " << offCellGid);
  }

  std::ostream * outPtr = &std::cout;
  std::unique_ptr<std::ofstream> outFilePtr;
  if (not params_.output_file.empty()) { 
    std::string outFileName(params_.output_file);
    datatools::fetch_path_with_env(outFileName);
    outFilePtr = std::make_unique<std::ofstream>(outFileName);
    DT_THROW_IF(!*outFilePtr, std::runtime_error, "Cannot open output file!");
    outPtr = outFilePtr.get();
  }
  snemo::time::time_period defaultPeriod = snemo::time::time_period_from_string(params_.default_period_repr);
  DT_THROW_IF(not snemo::time::is_valid(defaultPeriod), std::logic_error,
              "Invalid time period '" << params_.default_period_repr << "'!");
  for (unsigned int iSide = 0; iSide < nSides; iSide++) {
    for (unsigned int iLayer = 0; iLayer < nLayers; iLayer++) {
      for (unsigned int iRow = 0; iRow < nRows; iRow++) {
        geomtools::geom_id cellId(driftCellCoreCat,
                                  params_.module_id,
                                  iSide,
                                  iLayer,
                                  iRow);
        std::uint32_t cellStatus = snemo::rc::tracker_cell_status::CELL_GOOD;
        // Search for dead cells:
        for (auto & deadCell : deadCells) {
          if (geomtools::geom_id::match(cellId, deadCell)) {
            cellStatus = snemo::rc::tracker_cell_status::CELL_DEAD;
            break;
          }
        }
        // Then search for off cells:
        if (cellStatus == snemo::rc::tracker_cell_status::CELL_GOOD) {
          for (auto & offCell : offCells) {
            if (geomtools::geom_id::match(cellId, offCell)) {
              cellStatus = snemo::rc::tracker_cell_status::CELL_OFF;
              break;
            }
          }
        }
        if (params_.all_cells or cellStatus != snemo::rc::tracker_cell_status::CELL_GOOD) {
          *outPtr << cellId
                  << " ; " << snemo::time::to_string(defaultPeriod, snemo::time::TIME_PERIOD_FORMAT_EXCLUDE_END)
                  << " ; " << snemo::rc::tracker_cell_status::status_to_string(cellStatus)
                  << '\n';
        }
      }
    }
  }
  outPtr = nullptr;
  if (outFilePtr) {
    outFilePtr->close();
    outFilePtr.reset();
  }
  return;
}
