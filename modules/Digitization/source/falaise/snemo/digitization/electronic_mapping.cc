// snemo/digitization/electronic_mapping.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library :
#include <string>

// - Boost:
#include <boost/scoped_ptr.hpp>

// - Bayeux/datatools:
#include <datatools/exception.h>

// Ourselves :
#include <snemo/digitization/electronic_mapping.h>

namespace snemo {
  
  namespace digitization {

    const std::set<int32_t> & electronic_mapping::supported_types()
    {
      static boost::scoped_ptr<std::set<int32_t> > _types;
      if (!_types) {
	_types.reset(new std::set<int32_t>);
	_types->insert(mapping::GEIGER_CATEGORY_TYPE);
	_types->insert(mapping::CALO_MAIN_WALL_CATEGORY_TYPE);
	_types->insert(mapping::CALORIMETER_X_WALL_CATEGORY_TYPE);
	_types->insert(mapping::CALORIMETER_GVETO_CATEGORY_TYPE);
      }
      return *_types;
    }

    electronic_mapping::electronic_mapping()
    {
      _initialized_ = false;
      _geo_manager_ = 0;
      _geo_manager_status_ = false;
      _module_number_ = mapping::INVALID_MODULE_NUMBER;
      _module_number_status_ = false;
    }

    electronic_mapping::~electronic_mapping()
    { 
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void electronic_mapping::add_preconstructed_type(int type_)
    {
      DT_THROW_IF(supported_types().count(type_) == 0, std::logic_error, "Type ["<< type_ << "] is not supported ! ");
      _pre_constructed_types_.insert(type_);
      return;
    }

    void electronic_mapping::set_geo_manager(const geomtools::manager & mgr_ )
    {
      _geo_manager_ = & mgr_;
      _geo_manager_status_ = true;
      _ID_convertor_.set_geo_manager(mgr_);
    }
    
    bool electronic_mapping::geo_manager_is_set() const
    {
      return _geo_manager_status_;
    }

    void electronic_mapping::set_module_number(int module_number_)
    {
      _module_number_ = module_number_;
      _module_number_status_ = true;
      _ID_convertor_.set_module_number(module_number_);
      return;
    }

    bool electronic_mapping::module_number_is_set() const
    {
      return _module_number_status_;
    }


    void electronic_mapping::initialize()
    {
      DT_THROW_IF(!geo_manager_is_set() && !module_number_is_set(), std::logic_error, "Geo manager or module number is not set ! ");
      _initialize();
      return;
    }

    bool electronic_mapping::is_initialized() const
    {
      return _initialized_;
    }
    
    void electronic_mapping::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Electronic mapping is not initialized, it can't be reset ! ");
      _geo_manager_ = 0;
      _initialized_ = false;
      return;
    }
    
    void electronic_mapping::convert_GID_to_EID(const bool tracker_trigger_mode_, 
						const geomtools::geom_id & geom_id_, 
						geomtools::geom_id & electronic_id_) const
    {
      const_cast<electronic_mapping *> (this) -> _convert_GID_to_EID(tracker_trigger_mode_,
								     geom_id_, 
								     electronic_id_);
      return;
    }

    void electronic_mapping::convert_EID_to_GID(const bool tracker_trigger_mode_, 
						const geomtools::geom_id & electronic_id_,
						geomtools::geom_id & geom_id_) const
    {
      const_cast<electronic_mapping *> (this) -> _convert_EID_to_GID(tracker_trigger_mode_,
								     electronic_id_,
								     geom_id_);
      return;
    }


    void electronic_mapping::_initialize()
    {
      _initialized_ = true;
      _ID_convertor_.initialize();
      for (std::set<int32_t>::iterator it = _pre_constructed_types_.begin(); it != _pre_constructed_types_.end(); it++)
	{
	  if (*it == mapping::GEIGER_CATEGORY_TYPE) _init_geiger();
	  else if (*it == mapping::CALO_MAIN_WALL_CATEGORY_TYPE) _init_mcalo();	    
	}
        
      return;
    }


    void electronic_mapping::_init_geiger()
    {
      geomtools::geom_id GID(mapping::GEIGER_CATEGORY_TYPE, 0, 0, 0, 0);
      geomtools::geom_id EID;
      
      for(int side = 0; side < mapping::NUMBER_OF_SIDES; side++)
	{
	  GID.set(mapping::SIDE_INDEX, side);
	  for(int layer = 0; layer < mapping::NUMBER_OF_LAYERS; layer++)
	    {
	      GID.set(mapping::LAYER_INDEX, layer);
	      for(int row = 0; row < mapping::NUMBER_OF_GEIGER_ROWS; row++)
		{
		  GID.set(mapping::ROW_INDEX, row);
		  EID = _ID_convertor_.convert_GID_to_EID(GID);
		  _geiger_id_bimap_.insert( ID_doublet(GID , EID) ) ;
		} // end of row loop
	    } // end of layer loop
	} // end of side loop
      return ;
    }

    void electronic_mapping::_init_mcalo()
    {
      geomtools::geom_id GID(mapping::CALO_MAIN_WALL_CATEGORY_TYPE, 0, 0, 0, 0);
      geomtools::geom_id EID;
      for(int side = 0; side < mapping::NUMBER_OF_SIDES; side++)
	{
	  GID.set(mapping::SIDE_INDEX, side);
	  for(int column = 0; column < mapping::NUMBER_OF_MAIN_CALO_COLUMNS; column++)
	    {
	      GID.set(mapping::COLUMN_INDEX, column);
	      for(int row = 0; row < mapping::NUMBER_OF_MAIN_CALO_ROWS; row++)
		{
		  GID.set(mapping::ROW_INDEX, row);
		  EID = _ID_convertor_.convert_GID_to_EID(GID);
		  _mcalo_id_bimap_.insert( ID_doublet(GID , EID) ) ;
		} // end of row loop
	    } // end of column loop
	} // end of side loop  
      return ;
    }
    
    void electronic_mapping::_init_x_wall()
    {
      geomtools::geom_id GID(mapping::CALORIMETER_X_WALL_CATEGORY_TYPE, 0, 0, 0, 0);
      geomtools::geom_id EID;
      for(int side = 0; side < mapping::NUMBER_OF_SIDES; side++)
	{
	  GID.set(mapping::SIDE_INDEX, side);
	  for (int wall = 0; wall < mapping::NUMBER_OF_WALLS; wall ++)
	    {
	      GID.set(mapping::WALL_INDEX, wall);
	      for(int column = 0; column < mapping::NUMBER_OF_X_CALO_COLUMNS; column++)
		{
		  GID.set(mapping::COLUMN_INDEX, column);
		  for(int row = 0; row < mapping::NUMBER_OF_X_CALO_ROWS; row++)
		    {
		      GID.set(mapping::ROW_INDEX, row);
		      EID = _ID_convertor_.convert_GID_to_EID(GID);
		      _mcalo_id_bimap_.insert( ID_doublet(GID , EID) ) ;
		    } // end of row loop
		} // end of column loop
	    } // end of wall loop
	} // end of side loop  
      return;
    }

    void electronic_mapping::_init_gveto()
    {
      geomtools::geom_id GID(mapping::CALORIMETER_GVETO_CATEGORY_TYPE, 0, 0, 0, 0);
      geomtools::geom_id EID;
      for(int side = 0; side < mapping::NUMBER_OF_SIDES; side++)
	{
	  GID.set(mapping::SIDE_INDEX, side);
	  for (int wall = 0; wall < mapping::NUMBER_OF_WALLS; wall ++)
	    {
	      GID.set(mapping::WALL_INDEX, wall);
	      for(int column = 0; column < mapping::NUMBER_OF_GVETO_COLUMNS; column++)
		{
		  GID.set(mapping::ROW_INDEX, column);
		  EID = _ID_convertor_.convert_GID_to_EID(GID);
		  _mcalo_id_bimap_.insert(ID_doublet(GID, EID));
		} // end of column loop
	    } // end of wall loop
	} // end of side loop  
      return;
    }

    void electronic_mapping::_convert_EID_to_GID(const bool tracker_trigger_mode_, 
						 const geomtools::geom_id & elec_id_, 
						 geomtools::geom_id & geom_id_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Electronic mapping is not initialized ! ");
      DT_THROW_IF(elec_id_.get_type() != mapping::FEB_CATEGORY_TYPE, std::logic_error, "elect_id incorrect type ! ");
      DT_THROW_IF(tracker_trigger_mode_ != mapping::THREE_WIRES_TRACKER_MODE, std::logic_error, " Give a correct traker trigger mode (Two wires mode is not supported yet) ! ");
      geom_id_.reset();
      ID_bimap::right_const_iterator right_iter ;
     
      switch (elec_id_.get(mapping::RACK_INDEX))
	{
	case mapping::GEIGER_RACK_ID :
	  right_iter = _geiger_id_bimap_.right.find(elec_id_);
	  if (right_iter != _geiger_id_bimap_.right.end() )
	    {
	      geom_id_ = right_iter->second;
	    }
	  else
	    {
	    }
	  break;

	case mapping::CALO_RACK_ID : 
	  right_iter = _mcalo_id_bimap_.right.find(elec_id_);
	  if (right_iter != _mcalo_id_bimap_.right.end() )
	    {
	      geom_id_ = right_iter->second;
	    }
	  else
	    {
	    }
	  break;

	default :
	  break;
	}    
      return;
    }

    void electronic_mapping::_convert_GID_to_EID(const bool tracker_trigger_mode_, 
						 const geomtools::geom_id & geom_id_, 
						 geomtools::geom_id & electronic_id_)
    {
      DT_THROW_IF(tracker_trigger_mode_ != mapping::THREE_WIRES_TRACKER_MODE, std::logic_error, " Give a correct traker trigger mode (Two wires mode is not supported yet) ! ");
      DT_THROW_IF(!is_initialized(), std::logic_error, "Electronic mapping is not initialized ! ");
      electronic_id_.reset();
      ID_bimap::left_const_iterator left_iter ;
  	    
      switch (geom_id_.get_type())
	{
	case mapping::GEIGER_CATEGORY_TYPE :
	  left_iter = _geiger_id_bimap_.left.find(geom_id_);
	  if (left_iter != _geiger_id_bimap_.left.end() )
	    {
	      electronic_id_ = left_iter->second;
	    }
	  else
	    {
	      electronic_id_ = _ID_convertor_.convert_GID_to_EID(geom_id_);
	      _geiger_id_bimap_.insert( ID_doublet(geom_id_ ,electronic_id_) );
	    }
	  break;

	case mapping::CALO_MAIN_WALL_CATEGORY_TYPE : 
	  left_iter = _mcalo_id_bimap_.left.find(geom_id_);
	  if (left_iter != _mcalo_id_bimap_.left.end() )
	    {
	      electronic_id_ = left_iter->second;
	    }
	  else
	    {
	      electronic_id_ = _ID_convertor_.convert_GID_to_EID(geom_id_);
	      _mcalo_id_bimap_.insert( ID_doublet(geom_id_ ,electronic_id_) );
	    }
	  break;

	case mapping::CALORIMETER_X_WALL_CATEGORY_TYPE :
	  left_iter = _xcalo_id_bimap_.left.find(geom_id_);
	  if (left_iter != _xcalo_id_bimap_.left.end() )
	    {
	      electronic_id_ = left_iter->second;
	    }
	  else
	    {
	      electronic_id_ = _ID_convertor_.convert_GID_to_EID(geom_id_);
	      _xcalo_id_bimap_.insert( ID_doublet(geom_id_ ,electronic_id_) );
	    }
	  break;

	case mapping::CALORIMETER_GVETO_CATEGORY_TYPE :
	  left_iter = _gveto_id_bimap_.left.find(geom_id_);
	  if (left_iter != _gveto_id_bimap_.left.end() )
	    {
	      electronic_id_ = left_iter->second;
	    }
	  else
	    {
	      electronic_id_ = _ID_convertor_.convert_GID_to_EID(geom_id_);
	      _gveto_id_bimap_.insert( ID_doublet(geom_id_ ,electronic_id_) );
	    }
	  break; 

	default :
	  break;
	}  
      return ;
    }
	       
  } // end of namespace digitization

} // end of namespace snemo
