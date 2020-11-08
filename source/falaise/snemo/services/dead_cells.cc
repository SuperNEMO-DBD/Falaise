// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/dead_cells.h */

// Ourselves:
#include <falaise/snemo/services/dead_cells.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::dead_cells_svc, "snemo::dead_cells_svc")

namespace snemo {

  int dead_cells_svc::initialize(const datatools::properties&, datatools::service_dict_type&) {
    // Where you configure the service if it needs/allows it
    // Establish the connection to the CDB?
    return 0;
  }
  
  bool dead_cells_svc::is_initialized() const {
    // Return true/false to indicate initialization state
    // Here we are always initialized, so return true
    return true;
  }
  
  int dead_cells_svc::reset() {
    // Where you free any resources the service holds (you would also need to implement a
    // destructor!!) We don't do anything special so just return "success"
    return 0;
  }

  int dead_cells_svc::LoadCells(std::string deadcells_filename) {
    // Creates a vector of _ONLY_ Bad cells (e.g reading from a DB or from a file like now);
    // cells not included in the vector are considered to be Good;
    // the vector here created can be also accessed (size, loops).

    try {
      std::ifstream infile(deadcells_filename);
      int side, layer, column, status;
      // I assume here that the `status` is saved as `int`, therefore the switch
      
      while (infile >> side >> layer >> column >> status){
	cell_status cs = cell_status::good;
	switch(status) {
	case 1: cs = cell_status::dead; break;
	case 2: cs = cell_status::cathode_ground_top; break;
	case 3: cs = cell_status::cathode_ground_bottom; break;
	case 4: cs = cell_status::cathode_ground_both; break;
	case 5: cs = cell_status::cathode_cathode; break;
	case 6: cs = cell_status::other; break;
	}  

	cell_id c(side, layer, column, cs);
	cells.push_back(c);

      }
      std::cout << "Read " << cells.size() << " bad cells" << std::endl;
    }
    catch (std::logic_error& e) {
      std::cerr << "Problem in the file with the list of bad cells " << e.what() << std::endl;
    };

    return 0;
  }

  cell_status dead_cells_svc::CellStatus(cell_id cell, int run_number) {
    
    if ( run_number != 0 ) // left like this to be used, since will be a DB IOV thing
      // search for the specific cell in the vector
      for(cell_id c : cells) {
	if ( (c.GetSide()==cell.GetSide()) && (c.GetLayer()==cell.GetLayer()) && (c.GetColumn()==cell.GetColumn()) )
	  return c.GetStatus(); // Bad cell, present in the vector
      }

    return cell_status::good; // Good cell, because not present in the vector

  } // int dead_cells_svc::CellStatus
  
  cell_status dead_cells_svc::CellStatus(int side, int layer, int column, int run_number) {
    cell_id cell(side,layer,column);
    return dead_cells_svc::CellStatus(cell, run_number);
  }
  
  bool dead_cells_svc::isABadCell(cell_id cell, int run_number) {
    
    if (CellStatus(cell, run_number) != cell_status::good)
      return 1;
    else
      return 0;
    
  }
  
  bool dead_cells_svc::isABadCell(int side, int layer, int column, int run_number) {
    cell_id cell(side,layer,column);
    return dead_cells_svc::isABadCell(cell, run_number);
  }

  bool dead_cells_svc::isAGoodCell(cell_id cell, int run_number) {
    return !isABadCell(cell, run_number);
  }
  
  bool dead_cells_svc::isAGoodCell(int side, int layer, int column, int run_number) {
    cell_id cell(side,layer,column);
    return dead_cells_svc::isAGoodCell(cell, run_number);
  }
  
  bool dead_cells_svc::isADeadCell(cell_id cell, int run_number) {

    if (CellStatus(cell, run_number)== cell_status::dead)
      return 1;
    else
      return 0;
    
  }
  
  bool dead_cells_svc::isADeadCell(int side, int layer, int column, int run_number) {
    cell_id cell(side,layer,column);
    return dead_cells_svc::isADeadCell(cell, run_number);
  }

}  // namespace snemo
