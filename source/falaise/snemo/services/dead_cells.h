//! \file falaise/snemo/services/dead_cells.h
//! \brief Dead Cells service in SuperNEMO
//
// more doxygen to come...
//
// status  description
// ---------------------------------------------------------------------------
// 0       good working cell            isAGoodCell==true
// 1       dead cells with dead anode   isADeadCell==true and isABadCell==true
// 2       cathode-to-ground short      isABadCell==true
// 3       cathode-to-cathode short     isABadCell==true
// 4       other                        isABadCell==true


#ifndef SNEMO_DEAD_CELLS_SVC_HH
#define SNEMO_DEAD_CELLS_SVC_HH

#include <iostream>
#include <fstream>

#include "bayeux/datatools/base_service.h"
#include "falaise/snemo/services/service_traits.h"
#include "falaise/bounded_int.h"

namespace snemo {
  
  enum status {
    good = 0,            // good working cell
    dead = 1,            // dead cells with dead anode
    cathode_ground = 2,  // cathode-to-ground short
    cathode_cathode = 3, // cathode-to-cathode short
    other = 4            // other
  };

  class cell_id {
  public:
    using side_id = falaise::bounded_int<0,1>;
    using layer_id = falaise::bounded_int<0,8>;
    using column_id = falaise::bounded_int<0,112>;
    using status_id = falaise::bounded_int<0,4>;

    cell_id(side_id s, layer_id l, column_id c){  // use this to search for the status of a cell
      SetCell(s,l,c);
    }

    cell_id(side_id s, layer_id l, column_id c, status_id st){ // use this to create a cell knowing everything
      SetCell(s,l,c,st);
    }
    
    void SetCell(side_id s, layer_id l, column_id c){  // use this to search for the status of a cell
      side_ = s;
      layer_ = l;
      column_ = c;
    }

    void SetCell(side_id s, layer_id l, column_id c, status_id st){  // use this to create a cell knowing everything
      side_ = s;
      layer_ = l;
      column_ = c;
      status_ = st;
    }

    // accessors:
    int GetSide(){
      return side_;
    }

    int GetLayer(){
      return layer_;
    }

    int GetColumn(){
      return column_;
    }

    int GetStatus(){
      return status_;
    }


  private:
    side_id side_;
    layer_id layer_;
    column_id column_;
    status_id status_;
  };
  
  class dead_cells_svc : public datatools::base_service {
  public:
    
    // Inherited pure virtual member functions that must be overridden
    int initialize(const datatools::properties&, datatools::service_dict_type&) override;
    int reset() override;
    bool is_initialized() const override;
    
    // Actual implementation of the Dead Cells service interface
    // have left (side,layer,column) parameters option just for convenience)
    int LoadCells(std::string deadcells_filename);
    int CellStatus(cell_id cell, int run_number);
    int CellStatus(int side, int layer, int column, int run_number);
    bool isABadCell(cell_id cell, int run_number);
    bool isABadCell(int side, int layer, int column, int run_number);
    bool isAGoodCell(cell_id cell, int run_number);
    bool isAGoodCell(int side, int layer, int column, int run_number);
    bool isADeadCell(cell_id cell, int run_number);
    bool isADeadCell(int side, int layer, int column, int run_number);
    
  private:
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
