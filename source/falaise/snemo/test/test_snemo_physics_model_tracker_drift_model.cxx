// test_snemo_physics_model_tracker_drift_model.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/physics_model/tracker_drift_model.hpp>

void test1();

int main(int /*argc_*/, char** /*argv_*/)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::physics_model::tracker_drift_model'!" << std::endl;
    test1();

    std::clog << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}

void test1()
{
  namespace snt = snemo::time;
  namespace snpm = snemo::physics_model;

  // Service manager:
  datatools::properties serviceManagerConfig;
  serviceManagerConfig.store("logging", "debug");
  serviceManagerConfig.store_flag("force_initialization_at_load");
  std::vector<std::string> servicesConfigFiles = {
    "@falaise:snemo/demonstrator/geometry/5.0/GeometryServices.conf",   
    "@falaise:snemo/demonstrator/reconstruction/3.0/services/TrackerCellStatusService.conf"};
  serviceManagerConfig.store("services.configuration_files", servicesConfigFiles);
  datatools::service_manager serviceManager;
  serviceManager.initialize(serviceManagerConfig);

  {
    datatools::properties tdmConfig;
    tdmConfig.store("model", "betsy");
    tdmConfig.store("version", 0);
    tdmConfig.store_with_explicit_unit("default_gas_pressure", 880.e-3 * CLHEP::bar);
    tdmConfig.set_unit_symbol("default_gas_pressure", "millibar");
    // tdmConfig.store("default_neighbour_cells_off_pattern", 0);
    {
      boost::property_tree::ptree popts;
      popts.put("title", "Tracker drift model configuration:");
      popts.put("indent", "[info] ");
      tdmConfig.print_tree(std::cerr, popts);
    }
    snpm::tracker_drift_model tdm(datatools::logger::PRIO_DEBUG);
    tdm.initialize(tdmConfig, serviceManager.get_local_services() );
    DT_THROW_IF(not tdm.has_default_gas_pressure(), std::logic_error,
		"'default_gas_pressure' check failed");
    DT_THROW_IF(tdm.has_default_neighbour_cells_off_pattern(), std::logic_error,
		"'default_cell_infos' check failed");

    {
      // Define a cell GID:
      geomtools::geom_id cellGid(1204, 0, 0, 0, 0);
      snt::time_point evenTimestamp = snt::time_point_from_string("2024-04-11 09:45:00");
      auto cellInfo = tdm.fetch_cell_info(cellGid, evenTimestamp);
      {
	boost::property_tree::ptree popts;
	popts.put("indent", "[info] ");
	std::clog << "Cell info for " << cellGid << " : \n";
	cellInfo.print_tree(std::clog, popts);
	auto cellPos = tdm.gg_locator().getCellPosition(cellGid);
	cellInfo.export_viewer(std::clog, cellPos.x(), cellPos.y(), tdm.gg_locator().cellRadius());
      }

      double gas_pressure = 880e-3 * CLHEP::bar;
      std::ofstream fdt("test_tdm_drift_time.data");
      for (int quarterIndex = 0; quarterIndex < 4; quarterIndex++) {
	const snpm::cell_quarter_info & cqi = cellInfo.efield_info->qinfos[quarterIndex];
	std::clog << "cqi[" << quarterIndex << "] = " << cqi << '\n';
	for (double radial_dist = 0.0 * CLHEP::mm;
	     radial_dist < 32. * CLHEP::mm;
	     radial_dist += 0.5 * CLHEP::mm) {
	  auto result = tdm.compute_drift_time(cqi.category,
					       gas_pressure,
					       radial_dist);
	  double tdown, tmed, tup;
	  std::tie(tdown, tmed, tup) = result;
	  fdt << radial_dist << ' '
	      << tdown  << ' '
	      << tmed << ' '
	      << tup
	      << '\n';
	}
	fdt << '\n' << '\n';
      }
      fdt.close();
    }

    {
      std::ofstream fviewer("test_tdm.data");
      geomtools::geom_id allCellsGidPattern(1204,
					    0,
					    geomtools::geom_id::ANY_ADDRESS,
					    geomtools::geom_id::ANY_ADDRESS,
					    geomtools::geom_id::ANY_ADDRESS);
      std::set<geomtools::geom_id> allCellsGids;
      tdm.gg_locator().buildGeigerCells(allCellsGidPattern, allCellsGids);
      snt::time_point evenTimestamp = snt::time_point_from_string("2024-04-11 09:45:00");
      int cellCounter = 0;
      for (const auto & cellGid : allCellsGids) {
	auto cellInfo = tdm.fetch_cell_info(cellGid, evenTimestamp);
	auto cellPos = tdm.gg_locator().getCellPosition(cellGid);
	if (cellCounter > 0) fviewer << '\n';
	fviewer << "# Cell " << cellGid << '\n';
	cellInfo.export_viewer(fviewer, cellPos.x(), cellPos.y(), tdm.gg_locator().cellRadius());
	cellCounter++;
      }  
      fviewer.close();
    }

    
    tdm.reset();
  }
  
  serviceManager.reset();
  return;
}
