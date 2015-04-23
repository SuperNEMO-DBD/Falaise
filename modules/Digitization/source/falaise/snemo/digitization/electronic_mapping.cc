// snemo/digitization/electronic_mapping.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library :
#include<string>

// - Bayeux/datatools:
#include <datatools/exception.h>

// Ourselves :
#include <snemo/digitization/electronic_mapping.h>

namespace snemo {
  
  namespace digitization {

    electronic_mapping::electronic_mapping()
    {
      _initialized_ = false;
      _geo_manager_ = 0;
    }

    electronic_mapping::~electronic_mapping()
    { 
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void electronic_mapping::initialize()
    {
      _initialized_ = true;
      _ID_convertor_.initialize();
      return;

    }
    void electronic_mapping::initialize(std::string type)
    {
      _ID_convertor_.initialize();

      if (type == mapping::geiger_type())
	{
	  init_geiger();
	}

      if (type == mapping::main_calo_type())
	{
	  init_mcalo();
	}
      _initialized_ = true;
      return;
    }
    
    void electronic_mapping::initialize(std::string type1, std::string type2)
    {
      // _ID_convertor_.initialize()
      // _initialized = true;
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
	    std::clog << "DEBUG : insert unknown electronic ID = " << electronic_id_ << std::endl;
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
	    std::clog << "DEBUG : insert unknown electronic ID = " << electronic_id_ << std::endl;
	    _mcalo_id_bimap_.insert( ID_doublet(geom_id_ ,electronic_id_) );
	    }
	  break;

	default :
	  break;
	}  
      return ;
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
	      std::clog << "DEBUG : unknown electronic ID = " << elec_id_ << std::endl;
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
	      std::clog << "DEBUG : unknown electronic ID = " << elec_id_ << std::endl;
	    }
	  break;

	default :
	  break;
	}
      
      return;
    }

    void electronic_mapping::init_geiger()
    {
      geomtools::geom_id GID(mapping::GEIGER_CATEGORY_TYPE, 0, 0, 0, 0);
      geomtools::geom_id EID;
      
      for(int side = 0; side < mapping::MAX_NUMBER_OF_SIDE; side++)
	{
	  GID.set(mapping::SIDE_INDEX, side);
	  for(int layer = 0; layer < mapping::MAX_NUMBER_OF_LAYER; layer++)
	    {
	      GID.set(mapping::LAYER_INDEX, layer);
	      for(int row = 0; row < mapping::MAX_NUMBER_OF_GEIGER_ROW; row++)
		{
		  GID.set(mapping::ROW_INDEX, row);
		  EID = _ID_convertor_.convert_GID_to_EID(GID);
		  _geiger_id_bimap_.insert( ID_doublet(GID , EID) ) ;
		} // end of row loop
	    } // end of layer loop
	} // end of side loop
      return ;
    }

    void electronic_mapping::init_mcalo()
    {
      geomtools::geom_id GID(mapping::CALO_MAIN_WALL_CATEGORY_TYPE, 0, 0, 0, 0);
      geomtools::geom_id EID;
      for(int side = 0; side < mapping::MAX_NUMBER_OF_SIDE; side++)
	{
	  GID.set(mapping::SIDE_INDEX, side);
	  for(int column = 0; column < mapping::MAX_NUMBER_OF_CALO_ROW; column++)
	    {
	      GID.set(mapping::COLUMN_INDEX, column);
	      for(int row = 0; row < mapping::MAX_NUMBER_OF_CALO_ROW; row++)
		{
		  GID.set(mapping::ROW_INDEX, row);
		  EID = _ID_convertor_.convert_GID_to_EID(GID);
		  _mcalo_id_bimap_.insert( ID_doublet(GID , EID) ) ;
		} // end of row loop
	    } // end of column loop
	} // end of side loop  
      return ;
    }

    void electronic_mapping::set_geo_manager(const geomtools::manager & mgr_ )
    {
      _geo_manager_ = & mgr_;
      _ID_convertor_.set_geo_manager(mgr_);
    }
    
    void electronic_mapping::set_module_number(int module_number_)
    {
      _module_number_ = module_number_;
      _ID_convertor_.set_module_number(module_number_);
      return;
    }


    bool electronic_mapping::is_initialized()
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
	       
  } // end of namespace digitization

} // end of namespace snemo
