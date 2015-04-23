// snemo/digitization/ID_convertor.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library :
#include <map>

// This project :
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>

// Ourselves
#include <snemo/digitization/ID_convertor.h>

namespace snemo {
  
  namespace digitization {

    ID_convertor::ID_convertor()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      _initialized_ = false;
      _module_number_ = mapping::INVALID_MODULE_NUMBER;
      _geo_manager_ = 0;
    }
    
    ID_convertor::ID_convertor(const geomtools::manager & mgr_, int module_number_)
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      _initialized_ = false;
      _module_number_ = mapping::INVALID_MODULE_NUMBER;
      _geo_manager_ = 0;
      set_module_number(module_number_);
      set_geo_manager(mgr_);
    }
    
    ID_convertor::~ID_convertor()
    {
      if(is_initialized())
	{
	  reset();
	}
    }

    bool ID_convertor::is_initialized() const
    {
      return _initialized_;
    }

    void ID_convertor::initialize()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Convertor is already initialized ! ");
      DT_THROW_IF(_module_number_ < mapping::DEMONSTRATOR_MODULE_NUMBER, std::logic_error, "Missing module number ! ");
      DT_THROW_IF(_geo_manager_ == 0, std::logic_error, "Missing geometry manager ! ");
  
      _gg_locator_.reset(new geometry::gg_locator);
      _gg_locator_->set_geo_manager(*_geo_manager_);
      _gg_locator_->set_module_number(mapping::DEMONSTRATOR_MODULE_NUMBER);
      _gg_locator_->initialize();

      _calo_locator_.reset(new geometry::calo_locator);
      _calo_locator_->set_geo_manager(*_geo_manager_);
      _calo_locator_->set_module_number(mapping::DEMONSTRATOR_MODULE_NUMBER);
      _calo_locator_->initialize();

      _xcalo_locator_.reset(new geometry::xcalo_locator);
      _xcalo_locator_->set_geo_manager(*_geo_manager_);
      _xcalo_locator_->set_module_number(mapping::DEMONSTRATOR_MODULE_NUMBER);
      _xcalo_locator_->initialize();

      _gveto_locator_.reset(new geometry::gveto_locator);
      _gveto_locator_->set_geo_manager(*_geo_manager_);
      _gveto_locator_->set_module_number(mapping::DEMONSTRATOR_MODULE_NUMBER);
      _gveto_locator_->initialize();

      _initialized_ = true;
      return;
    }

    void ID_convertor::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Convertor is not initialized ! ");
      _initialized_ = false;
      _gg_locator_.reset();
      _calo_locator_.reset();
      _xcalo_locator_.reset();
      _gveto_locator_.reset();
      _logging_ = datatools::logger::PRIO_FATAL;
      _initialized_ = false;
      _module_number_ = mapping::INVALID_MODULE_NUMBER;
      _geo_manager_ = 0;
      return;
    }

    void ID_convertor::set_logging(datatools::logger::priority prio_)
    {
      _logging_ = prio_;
      return;
    }

    datatools::logger::priority ID_convertor::get_logging() const
    {
      return _logging_;
    }
    
    void ID_convertor::set_geo_manager(const geomtools::manager & mgr_ )
    {
      _geo_manager_ = & mgr_;
    }

    void ID_convertor::set_module_number(int module_number_)
    {
      _module_number_ = module_number_;
      return;
    }

    geomtools::geom_id ID_convertor::convert_GID_to_EID(const geomtools::geom_id & geom_id_) const
    {
      DT_THROW_IF(!geom_id_.is_valid (), std::logic_error,
                  "Geom ID to convert is not valid !");
   
      //DT_THROW_IF(!geom_id_.is_complete(), std::logic_error,
      //   "Geom ID to convert is not complete !");

      geomtools::geom_id electronic_id;
      electronic_id.set_type(mapping::FEB_CATEGORY_TYPE);
      unsigned int rack_id  = 666;
      unsigned int crate_id = 666;
      unsigned int board_id = 666;
      unsigned int channel_id = 666;
      if( geom_id_.get_type() == mapping::GEIGER_CATEGORY_TYPE){ // Drift cell --> Side [0;1] Layer [0;8] Row [0;112]
	rack_id = mapping::GEIGER_RACK_ID;
	unsigned int side_index = _gg_locator_->extract_side(geom_id_);
	unsigned int layer_index = _gg_locator_->extract_layer(geom_id_);
	unsigned int row_index = _gg_locator_->extract_row(geom_id_);
	unsigned int shift = 0;
	unsigned int row_shift = 0;
	
	if (row_index <= mapping::BOARD_ID_SHIFT_CRATE_0_LIMIT)
	  {
	    crate_id = 0;
	  }
	
	if (row_index > mapping::BOARD_ID_SHIFT_CRATE_0_LIMIT && row_index <= mapping::THREE_WIRES_CRATE_0_LIMIT)
	  {
	    crate_id = 0;
	    row_shift = mapping::NO_FEB_NUMBER_10_SHIFT;
	  }

	if (row_index > mapping::THREE_WIRES_CRATE_0_LIMIT && row_index  <= mapping::THREE_WIRES_LONELY_ROW )
	  {
	    crate_id = 1; 
	    shift = mapping::THREE_WIRES_CRATE_1_BEGINNING;
	  }

	if (row_index > mapping::THREE_WIRES_LONELY_ROW && row_index  <= mapping::THREE_WIRES_CRATE_1_LIMIT )
	  {
	    crate_id = 1; 
	    shift = mapping::THREE_WIRES_CRATE_1_BEGINNING;
	    shift -= 1; // in order to take into account the lonely row at the middle
	    row_shift = mapping::NO_FEB_NUMBER_10_SHIFT;
	  }

	if (row_index > mapping::THREE_WIRES_CRATE_1_LIMIT && row_index <= mapping::BOARD_ID_SHIFT_CRATE_2_LIMIT)
	  {
	  crate_id = 2;
	  shift = mapping::THREE_WIRES_CRATE_2_BEGINNING;	  
	  }

	if (row_index > mapping::BOARD_ID_SHIFT_CRATE_2_LIMIT)
	  {
	  crate_id = 2;
	  shift = mapping::THREE_WIRES_CRATE_2_BEGINNING;
	  row_shift = mapping::NO_FEB_NUMBER_10_SHIFT;
	  }

	board_id = (row_index + row_shift -shift) / 2;
	std::clog << "DEBUG : geom id in ID CONVERTOR class " << geom_id_ << std::endl;
	std::clog << "DEBUG : board id in ID CONVERTOR class " << board_id << std::endl;

	if (row_index < mapping::THREE_WIRES_LONELY_ROW)
	  {
	    shift = 2 * side_index + (row_index % 2);
	    channel_id = mapping::GEIGER_LAYER_SIZE * shift + layer_index;
	  }

	else if (row_index == mapping::THREE_WIRES_LONELY_ROW)
	  {
	    shift = 2 * side_index;
	    channel_id = mapping::GEIGER_LAYER_SIZE * shift + layer_index;
	  }

	else if (row_index > mapping::THREE_WIRES_LONELY_ROW)
	  {
	    shift = 2 * side_index + (1 - (row_index % 2));
	    channel_id = mapping::GEIGER_LAYER_SIZE * shift + layer_index;
	  }
      }

      if( geom_id_.get_type() == mapping::CALO_MAIN_WALL_CATEGORY_TYPE ){ // MCALO -- Side [0;1] Column [0;19] (Row[0;12] )type --> 1301
	rack_id = mapping::CALO_RACK_ID;
     
	unsigned int module_index_ = _calo_locator_-> extract_module(geom_id_);
	unsigned int column_index_ = _calo_locator_-> extract_column(geom_id_);
	unsigned int row_index_ = _calo_locator_-> extract_row(geom_id_);
	unsigned int side_index_ = _calo_locator_-> extract_side(geom_id_);
     
	if(side_index_ == 0) crate_id = 0;
	else if(side_index_ == 1) crate_id = 1;
     
	if (column_index_ < mapping::CONTROL_BOARD_ID) board_id = column_index_;
	else if (column_index_ >= mapping::CONTROL_BOARD_ID) board_id = column_index_+1;
	
	channel_id = row_index_;
      }
      
      if( geom_id_.get_type() == mapping::CALORIMETER_X_WALL_CATEGORY_TYPE ){//XCALO  -- Side [0;1 ] Wall [0;1] Column [0;1] (Row[0;15])  type --> 1231
	rack_id  = mapping::CALO_RACK_ID;
	crate_id = mapping::XWALL_GVETO_CRATE_ID;

	unsigned int column_index_ = _xcalo_locator_->extract_column(geom_id_);
	unsigned int wall_index_   = _xcalo_locator_->extract_wall(geom_id_);
	unsigned int side_index_   = _xcalo_locator_->extract_side(geom_id_);
      
	if ( side_index_ == 0){
	  if ( wall_index_ == 0){
	    if ( column_index_ == 0) board_id = 6;
	    if ( column_index_ == 1) board_id = 7;
	  }
	  if ( wall_index_ == 1){
	    if ( column_index_ == 0) board_id = 8;
	    if ( column_index_ == 1) board_id = 9;
	  }
	} //end of side == 0 
	if ( side_index_ == 1){
	  if ( wall_index_ == 0){
	    if ( column_index_ == 0) board_id = 14;
	    if ( column_index_ == 1) board_id = 13;
	  }
	  if ( wall_index_ == 1){
	    if ( column_index_ == 0) board_id = 12;
	    if ( column_index_ == 1) board_id = 11;
	  }
	} //end of side == 1     
      }


      if( geom_id_.get_type() == mapping::CALORIMETER_GVETO_CATEGORY_TYPE){//GVETO -- Side [0;1 ] Wall [0;1] (Column [0;5])type --> 1251
	rack_id = mapping::CALO_RACK_ID;
	crate_id = mapping::XWALL_GVETO_CRATE_ID;

	unsigned int wall_index_ = _gveto_locator_->extract_wall(geom_id_);
	unsigned int side_index_ = _gveto_locator_->extract_side(geom_id_);
	if ( side_index_ == 0){
	  if ( wall_index_ == 0)board_id = 4;
	  if ( wall_index_ == 1)board_id = 5;
	}
	if ( side_index_ == 1){
	  if ( wall_index_ == 0)board_id = 16;
	  if ( wall_index_ == 1)board_id = 15;
	}
      }
   
      electronic_id.set_address(rack_id,crate_id,board_id,channel_id);   
  
      return  electronic_id;
    }


    geomtools::geom_id ID_convertor::convert_EID_to_GID(const geomtools::geom_id & geom_id_) const{

    }

    void ID_convertor::tree_dump(std::ostream & out_,
				 const std::string & title_ ,
				 const std::string & indent_  ,
				 bool inherit_){
    }

  }// end of namespace digitization

} // end of namespace snemo
