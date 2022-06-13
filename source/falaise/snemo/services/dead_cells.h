//! \file falaise/snemo/services/dead_cells.h
// Falaise service for Dead Cells in SuperNEMO Tracker

#ifndef SNEMO_DEAD_CELLS_SVC_HH
#define SNEMO_DEAD_CELLS_SVC_HH

#include <iostream>
#include <fstream>

#include "bayeux/datatools/base_service.h"
#include "falaise/snemo/services/service_traits.h"
#include "falaise/bounded_int.h"

namespace snemo {

  //! Data member for the possible statuses of a cell
  enum class cell_status
    {
     good = 0,                     /**< good working cell */
     dead = 1,                   /**< dead cell with dead anode */
     cathode_ground_top = 2,     /**< top cathode-to-ground short */
     cathode_ground_bottom = 3,  /**< bottom cathode-to-ground short */
     cathode_ground_both = 4,    /**< top and bottom cathodes-to-ground short */
     cathode_cathode = 5,        /**< cathode-to-cathode short */
     other = 6                   /**< other problem */
    };

  //! Data member defining a cell structure and its accessors
  class cell_id
  {
  public:

    //! Side of the Tracker [0,1]
    using side_id = falaise::bounded_int<0,1>;
    //! Index of the layer of cells in the Tracker [0..8]
    using layer_id = falaise::bounded_int<0,8>;  
    //! Index of the column of cells in the Tracker [0..112]
    using column_id = falaise::bounded_int<0,112>;
    
    //! Constructor for a cell_id
    //! used to search for the status of a cell
    cell_id(side_id s, layer_id l, column_id c){  
      SetCell(s,l,c);
    }

    //! Constructor for a cell_id
    //! used to create a cell knowing coordinates and status
    cell_id(side_id s, layer_id l, column_id c, cell_status st){
      SetCell(s,l,c,st);
    }
    
    //! Function to set the cell_id content
    void SetCell(side_id s, layer_id l, column_id c){  // use this to search for the status of a cell
      side_ = s;
      layer_ = l;
      column_ = c;
    }

    //! Function to set the cell_id content knowing coordinates and status
    void SetCell(side_id s, layer_id l, column_id c, cell_status st){  // use this to create a cell knowing everything
      side_ = s;
      layer_ = l;
      column_ = c;
      status_ = st;
    }

    //! Accessor to get the side_id from a cell_id
    int GetSide(){
      return side_;
    }

    //! Accessor to get the layer_id from a cell_id
    int GetLayer(){
      return layer_;
    }

    //! Accessor to get the column_id from a cell_id
    int GetColumn(){
      return column_;
    }

    //! Accessor to get the cell_status from a cell_id
    cell_status GetStatus(){
      return status_;
    }

  private:
    side_id side_;
    layer_id layer_;
    column_id column_;
    cell_status status_;
  };
  
  /*!\brief Class containing the functions to interact with the bad cells
   *
   * Class used to provide a Falaise service for the problematic (bad) cells in the SuperNEMO Tracker.
   * The status of Trackers Geiger cells with problems will be stored
   * in the CDB. Reconstruction modules can use this information.
   * Each cell is defined by its coordinates (side,layer,column) and
   * has a fixed status assigned depending on the encountered problem.
   * The IOV of the status will be defined knowing the unique run number.
   * The class defines the data structure used to describe a cell and
   * includes functions to get informations about the status of a specific cell.
   * A vector of cells is created which can be used in Falaise for other purposes.
   *
   * At the moment the bad cells are read from a column based text file
   * given as input which should be in the format (side, layer, column, status);
   * this will be replaced by a DB interface.
   *
   * Example of usage:
   * ```cpp
   * datatools::service_manager dummyServices{};
   * datatools::multi_properties config;
   * config.add_section("dead_cells_svc", "snemo::dead_cells_svc");
   * dummyServices.load(config);
   * dummyServices.initialize();
   *
   * snemo::service_handle<snemo::dead_cells_svc> DC_SVC{dummyServices};
   *
   * DC_SVC->LoadCells("cells_test.txt");
   * snemo::cell_id cid(0,2,4);
   * std::cout << "Side: " << cid.GetSide() << std::endl;
   * std::cout << "Cell Status of 0,2,4: " << static_cast<int>(DC_SVC->CellStatus(cid,100)) << std::endl;
   * std::cout << "Is 0,2,4 a dead cell?: " << DC_SVC->isADeadCell(cid,100) << std::endl;
   * ```
   */
  class dead_cells_svc
    : public datatools::base_service
  {
  public:

    // Inherited pure virtual member functions that must be overridden
    int initialize(const datatools::properties&, datatools::service_dict_type&) override;
    int reset() override;
    bool is_initialized() const override;
    
    // Actual implementation of the Dead Cells service interfaces
    // have left the (side,layer,column) parameters option just for convenience

    //! Function to load a vector of cell_id from a text file
    int LoadCells(std::string deadcells_filename);

    //! Function to return the cell_status of a cell_id
    cell_status CellStatus(cell_id cell, int run_number);
    //! Function to return the cell_status of a cell, using directly the coordinates of the cell
    cell_status CellStatus(int side, int layer, int column, int run_number);

    //! Function to check if the cell has any status different from cell_status::good
    //! \returns true if the status of the cell is different from cell_status::good 
    bool isABadCell(cell_id cell, int run_number);
    //! Function to check if the cell has any status different from cell_status::good, using directly the coordinates of the cell
    //! \returns true if the status of the cell is different from cell_status::good 
    bool isABadCell(int side, int layer, int column, int run_number);

    //! Function to check if the cell has cell_status::good
    //! \returns true if the cell has cell_status::good
    bool isAGoodCell(cell_id cell, int run_number);
    //! Function to check if the cell has cell_status::good, using directly the coordinates of the cell
    //! \returns true if the cell has cell_status::good
    bool isAGoodCell(int side, int layer, int column, int run_number);

    //! Function to check if the cell has cell_status::dead
    //! \returns true if the cell has cell_status::dead
    bool isADeadCell(cell_id cell, int run_number);
    //! Function to check if the cell has cell_status::dead, using directly the coordinates of the cell
    //! \returns true if the cell has cell_status::dead
    bool isADeadCell(int side, int layer, int column, int run_number);

    //! Size of the private vector cells containing all the bad cells
    size_t size() const { return cells.size();}
    //! Function begin() for the iterator of the private vector cells
    std::vector<cell_id>::const_iterator begin() const { return cells.begin();}
    //! Function end() for the iterator of the private vector cells
    std::vector<cell_id>::const_iterator end() const { return cells.end();}
    
  private:
    
    //! Vector with list of bad cells to be filled by the function LoadCells()
    std::vector<cell_id> cells;    

    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(dead_cells_svc)
  };

  //! Falaise service template  
  template <>
  struct service_traits<dead_cells_svc>
  {
    using label_type = BOOST_METAPARSE_STRING("dead_cells_svc");
    using service_type = dead_cells_svc;
    using instance_type = service_type;
    static instance_type* get(service_type& sm) { return &sm; }
  };

} // namespace snemo

#endif  // SNEMO_DEAD_CELLS_SVC_HH
