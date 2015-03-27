// ID_convertor.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
//

#include "ID_convertor.h"
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>

#include <iostream>
#include <stdexcept>
#include <cmath>

namespace snemo {
  
  namespace digitization {

    ID_convertor::ID_convertor(){
      set_debug( false );
      //const::geomtools::manager &  	mgr_,  	uint32_t  	module_number_
      _geo_manager_ = 0;
    }
    
    ID_convertor::ID_convertor(const geomtools::manager & mgr_){
      set_debug( false );
      initialize(mgr_);
     }
    
    ID_convertor::~ID_convertor(){
      // insert your code here
    }

    bool ID_convertor::is_debug() const{
      return _debug_;
    }

    void ID_convertor::set_debug( bool debug_ ){
      if(debug_) std::clog<<"INFO : ID_convertor::set_debug()"<<std::endl;
      _debug_ = debug_;
    }

    void ID_convertor::initialize(const geomtools::manager & mgr_ ){
      if(_debug_) std::clog<<"INFO : ID_convertor::initialize()"<<std::endl;
      _geo_manager_ = & mgr_;
    }


    geomtools::geom_id ID_convertor::convert_GID_to_EID(const geomtools::geom_id & geom_id_){
      if(_debug_) std::clog<<"INFO : enter in ID_convertor::convert_GID_to_EID() "<<std::endl;

      DT_THROW_IF(!geom_id_.is_valid (), std::logic_error,
                  "Geom ID to convert is not valid !");
   
      //DT_THROW_IF(!geom_id_.is_complete(), std::logic_error,
      //   "Geom ID to convert is not complete !");

      geomtools::geom_id electronic_id;
      electronic_id.set_type(666);
      // uint32_t calo_block_type
      //   = a_mgr.get_id_mgr ().categories_by_name ().find ("xcalo_block")->second.get_type ();
      // geomtools::smart_id_locator xcalo_locator (a_mgr.get_mapping (), calo_block_type);
      unsigned int rack_id  = 666;
      unsigned int crate_id = 666;
      unsigned int board_id = 666;
   
      if( geom_id_.get_type() == 1203 ){ // Drift cell --> Side [0;1] Layer [0;9] Row [0;112]
	rack_id = 5;
	geometry::gg_locator GGL;
	
	GGL.set_geo_manager(*_geo_manager_);
	GGL.set_module_number (0);
	GGL.initialize ();

	unsigned int row_index = GGL.extract_row(geom_id_);
	unsigned int shift = 0;	
	
	if(row_index < 38){crate_id = 0;}
	if(row_index > 37 && row_index  < 75 ){
	  crate_id = 1; shift = 38;
	  if(row_index > 56) shift -= 1; // in order to take into account the lonely row at the middle
	}
	if(row_index > 74) {crate_id = 2;shift = 75;}

	board_id = (row_index-shift) / 2;
      }


      if( geom_id_.get_type() == 1302 ){ // MCALO -- Side [0;1] Column [0;19] (Row[0;12] )type --> 1301
	rack_id = 3;
     
	geometry::calo_locator CL;

	CL.set_geo_manager (*_geo_manager_);
	CL.set_module_number (0);
	CL.initialize ();

	unsigned int module_index_ = CL.extract_module(geom_id_);
	unsigned int column_index_ = CL.extract_column(geom_id_);
	unsigned int side_index_ = CL.extract_side(geom_id_);
     
	if(side_index_ == 0)crate_id = 0;
	else if(side_index_ == 1)crate_id = 1;
     
	if (column_index_ < 10)board_id = column_index_;
	else if (column_index_ >= 10)board_id = column_index_+1;
      }
   
      if( geom_id_.get_type() == 1232 ){//XCALO  -- Side [0;1 ] Wall [0;1] Column [0;1] (Row[0;15])  type --> 1231
	rack_id = 3;
	crate_id = 2;
	geometry::xcalo_locator CL;
	CL.set_geo_manager (*_geo_manager_);
	CL.set_module_number (0);
	CL.initialize ();

	unsigned int column_index_ = CL.extract_column(geom_id_);
	unsigned int wall_index_ = CL.extract_wall(geom_id_);
	unsigned int side_index_ = CL.extract_side(geom_id_);
      
	if ( side_index_ == 0){
	  if ( wall_index_ == 0){
	    if ( column_index_ == 0)board_id = 6;
	    if ( column_index_ == 1)board_id = 7;
	  }
	  if ( wall_index_ == 1){
	    if ( column_index_ == 0)board_id = 8;
	    if ( column_index_ == 1)board_id = 9;
	  }
	}//end of side == 0 
	if ( side_index_ == 1){
	  if ( wall_index_ == 0){
	    if ( column_index_ == 0)board_id = 14;
	    if ( column_index_ == 1)board_id = 13;
	  }
	  if ( wall_index_ == 1){
	    if ( column_index_ == 0)board_id = 12;
	    if ( column_index_ == 1)board_id = 11;
	  }
	}//end of side == 1     
      }


      if( geom_id_.get_type() == 1251 ){//GVETO -- Side [0;1 ] Wall [0;1] (Column [0;5])type --> 1251
	geometry::gveto_locator CL;
	CL.set_geo_manager (*_geo_manager_);
	CL.set_module_number (0);
	CL.initialize ();
	unsigned int wall_index_ = CL.extract_wall(geom_id_);
	unsigned int side_index_ = CL.extract_side(geom_id_);
	if ( side_index_ == 0){
	  if ( wall_index_ == 0)board_id = 4;
	  if ( wall_index_ == 1)board_id = 5;
	}
	if ( side_index_ == 1){
	  if ( wall_index_ == 0)board_id = 16;
	  if ( wall_index_ == 1)board_id = 15;
	}

      }
   
      electronic_id.set_address(rack_id,crate_id,board_id);   
   
      // geom_id_.get_depth();
      // geom_id_.get_type();


   
      return  electronic_id;

    }


    geomtools::geom_id ID_convertor::convert_EID_to_GID(const geomtools::geom_id & geom_id_){


    }

    void ID_convertor::tree_dump(std::ostream & out_,
				 const std::string & title_ ,
				 const std::string & indent_,
				 bool inherit_){
      if(_debug_) std::clog<<"INFO : enter in ID_convertor::tree_dump() "<<std::endl;

      //   out_<<"["<<_status_a<<":"<< _status_b<<"]";//<<std::endl
      out_<<"test : "<<std::endl;
    }

  }// end of namespace digitization
} // end of namespace snemo
