//! \file falaise/snemo/services/dead_cells.h
//! \brief Tracker Dead Cells service in SuperNEMO

#ifndef SNEMO_DEAD_CELLS_SVC_HH
#define SNEMO_DEAD_CELLS_SVC_HH

#include <iostream>
#include <fstream>

#include "bayeux/datatools/base_service.h"
#include "falaise/snemo/services/service_traits.h"
#include "falaise/bounded_int.h"

namespace snemo {

  // data member for the possible statuses of a cell  
  enum class cell_status {
    good = 0,                   // good working
    dead = 1,                   // dead cells with dead anode |
    cathode_ground_top = 2,     // cathode-to-ground short    |
    cathode_ground_bottom = 3,  // cathode-to-ground short    | bad
    cathode_ground_both = 4,    // cathode-to-ground short    |
    cathode_cathode = 5,        // cathode-to-cathode short   |
    other = 6                   // other                      |
  };

  // data member defining a cell structure
  class cell_id {
  public:

    using side_id = falaise::bounded_int<0,1>;                                                                                                                      
    using layer_id = falaise::bounded_int<0,8>;  
    using column_id = falaise::bounded_int<0,112>;
    
    // constructors
    cell_id(side_id s, layer_id l, column_id c){  // use this to search for the status of a cell
      SetCell(s,l,c);
    }

    cell_id(side_id s, layer_id l, column_id c, cell_status st){ // use this to create a cell knowing everything
      SetCell(s,l,c,st);
    }
    
    // functions
    void SetCell(side_id s, layer_id l, column_id c){  // use this to search for the status of a cell
      side_ = s;
      layer_ = l;
      column_ = c;
    }

    void SetCell(side_id s, layer_id l, column_id c, cell_status st){  // use this to create a cell knowing everything
      side_ = s;
      layer_ = l;
      column_ = c;
      status_ = st;
    }

    // accessors
    int GetSide(){
      return side_;
    }

    int GetLayer(){
      return layer_;
    }

    int GetColumn(){
      return column_;
    }

    cell_status GetStatus(){
      return status_;
    }

  private:
    side_id side_;
    layer_id layer_;
    column_id column_;
    cell_status status_;
  };
  
  class dead_cells_svc : public datatools::base_service {
  public:

    // Inherited pure virtual member functions that must be overridden
    int initialize(const datatools::properties&, datatools::service_dict_type&) override;
    int reset() override;
    bool is_initialized() const override;
    
    // Actual implementation of the Dead Cells service interfaces
    // have left the (side,layer,column) parameters option just for convenience
    int LoadCells(std::string deadcells_filename);

    cell_status CellStatus(cell_id cell, int run_number);
    cell_status CellStatus(int side, int layer, int column, int run_number);
    bool isABadCell(cell_id cell, int run_number);
    bool isABadCell(int side, int layer, int column, int run_number);
    bool isAGoodCell(cell_id cell, int run_number);
    bool isAGoodCell(int side, int layer, int column, int run_number);
    bool isADeadCell(cell_id cell, int run_number);
    bool isADeadCell(int side, int layer, int column, int run_number);

    size_t size() const { return cells.size();}
    std::vector<cell_id>::const_iterator begin() const { return cells.begin();}
    std::vector<cell_id>::const_iterator end() const { return cells.end();}
    
  private:

    // Vector with list of Bad cells to be filled by the function LoadCells()
    std::vector<cell_id> cells;    

    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(dead_cells_svc)
      };
  
  template <>
    struct service_traits<dead_cells_svc> {
    using label_type = BOOST_METAPARSE_STRING("dead_cells_svc");
    using service_type = dead_cells_svc;
    using instance_type = service_type;
    static instance_type* get(service_type& sm) { return &sm; }
  };

}  // namespace snemo

#endif  // SNEMO_DEAD_CELLS_SVC_HH
