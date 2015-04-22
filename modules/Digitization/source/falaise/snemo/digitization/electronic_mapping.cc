// snemo/digitization/electronic_mapping.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#include<string>

// - Bayeux/datatools:
#include <datatools/exception.h>

// Ourselves
#include <snemo/digitization/electronic_mapping.h>

namespace snemo {
  
  namespace digitization {

    electronic_mapping::electronic_mapping()
    {
      _initialized_ = false;
      //_ID_convertor = 0;
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
      _ID_convertor.initialize();
      return;

    }
    void electronic_mapping::initialize(std::string type)
    {

      _ID_convertor.initialize();


      if(type == "geiger"){
	init_geiger();

      }
      if(type == "mcalo"){
	init_mcalo();
      }

      _initialized_ = true;      


      return;
    }
    
    // void electronic_mapping::initialize(std::string type1, std::string type2)
    // {
    // }
    
    
    geomtools::geom_id electronic_mapping::convert_GID_to_EID(const geomtools::geom_id & geom_id_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Electronic mapping is not initialized ! ");
      geomtools::geom_id electronic_id;
      ID_bimap::left_const_iterator left_iter ;
      // std::clog <<"**** DEBUG : geom id type = " << geom_id_.get_type() << std::endl;
      //std::clog <<"**** DEBUG : geom id  = " << geom_id_ << std::endl;
	    
      switch (geom_id_.get_type())
	{
	case 1204 :
	  /*  for( ID_bimap::const_iterator iter = _geiger_id_bimap.begin(), iend = _geiger_id_bimap.end();  iter != iend; ++iter )
	    {
	      std::cout << iter->left << " <--> " << iter->right << std::endl;
	    }
	  */
	  //std::clog <<"DEBUG : Enter the 1204 " << std::endl;
	  left_iter = _geiger_id_bimap.left.find(geom_id_);
	  if (left_iter != _geiger_id_bimap.left.end() ){
	    //std::clog <<"DEBUG : Enter the if condition " << std::endl;
	    //std::cout <<"GID : "<<  left_iter->first<<std::endl;
	    //std::cout <<"EID : "<<  left_iter->second<<std::endl;
	    electronic_id = left_iter->second;
	  }else{
	    /*
	      std::clog <<"DEBUG : Enter the else condition " << std::endl;
	    std::cout<<"GID not found in geiger id bimap ! "<< std::endl;
	    std::clog << "DEBUG : geom id = " << geom_id_ << std::endl;
	    */
	    electronic_id = _ID_convertor.convert_GID_to_EID(geom_id_);	
	    std::clog << "DEBUG : insert unknown electronic ID = " << electronic_id << std::endl;
	    _geiger_id_bimap.insert( ID_doublet(geom_id_ ,electronic_id) );
	  }
	  break;

	case 1302 : 
	  left_iter = _mcalo_id_bimap.left.find(geom_id_);
	  if (left_iter != _mcalo_id_bimap.left.end() ){
	    //std::cout <<"GID : "<<  left_iter->first<<std::endl;
	    //std::cout <<"EID : "<<  left_iter->second<<std::endl;
	    electronic_id = left_iter->second;
	  }else{
	    //std::cout<<"GID not found ! "<< std::endl;
	    electronic_id = _ID_convertor.convert_GID_to_EID(geom_id_);	
	    std::clog << "DEBUG : insert unknown electronic ID = " << electronic_id << std::endl;
	    _mcalo_id_bimap.insert( ID_doublet(geom_id_ ,electronic_id) );

	  }
	  break;

	default :
	  break;

	}
      
      return electronic_id;
    }
    

  geomtools::geom_id electronic_mapping::convert_EID_to_GID(const geomtools::geom_id & elec_id_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Electronic mapping is not initialized ! ");
      DT_THROW_IF(elec_id_.get_type() != 666, std::logic_error, "elect_id incorrect type ! ");
	    
      geomtools::geom_id geom_id;
      ID_bimap::right_const_iterator right_iter ;
     
      switch (elec_id_.get(mapping::RACK_INDEX))
	{
	case mapping::GEIGER_RACK_ID :
	  right_iter = _geiger_id_bimap.right.find(elec_id_);
	  if (right_iter != _geiger_id_bimap.right.end() ){
	    geom_id = right_iter->second;
	  }else{
	    //geom_id = _ID_convertor.convert_EID_to_GID(elec_id_);	
	    std::clog << "DEBUG : unknown electronic ID = " << elec_id_ << std::endl;
	    // _geiger_id_bimap.insert( ID_doublet(elec_id_ ,geom_id) );
	  }
	  break;

	case  mapping::CALO_RACK_ID : 
	  right_iter = _mcalo_id_bimap.right.find(elec_id_);
	  if (right_iter != _mcalo_id_bimap.right.end() ){
	    geom_id = right_iter->second;
	  }else{
	    //geom_id = _ID_convertor.convert_GID_to_EID(elec_id_);
	    std::clog << "DEBUG : unknown electronic ID = " << elec_id_ << std::endl;
	    //_mcalo_id_bimap.insert( ID_doublet(elec_id_ ,geom_id) );

	  }
	  break;

	default :
	  break;

	}
      
      return geom_id;
    }




    void electronic_mapping::init_geiger()
    {
      geomtools::geom_id GID(1204,0,0,0,0);
      geomtools::geom_id EID;
      

      for(int side=0;side<2;side++){
	GID.set(1, side);
	for(int layer=0;layer<9;layer++){
	  GID.set(2, layer);
	  for(int row=0;row<113;row++){
	    GID.set(3, row);
	    //    std::clog<<"DEBUG : GID : "<<GID <<std::endl;
	    EID = _ID_convertor.convert_GID_to_EID(GID);
	    
	    //  std::clog<<"DEBUG : EID : "<<EID <<std::endl;
	    _geiger_id_bimap.insert( ID_doublet(GID , EID) ) ;
	  }
	}
      }
      return ;
    }

    void electronic_mapping::init_mcalo()
    {
      geomtools::geom_id GID(1302,0,0,0,0);
      geomtools::geom_id EID;
      for(int side=0;side<2;side++){
	GID.set(1, side);
	for(int column=0;column<20;column++){
	  GID.set(2, column);
	  for(int row=0;row<13;row++){
	    GID.set(3, row);
	    //    std::clog<<"DEBUG : GID : "<<GID <<std::endl;
	    EID = _ID_convertor.convert_GID_to_EID(GID);
	    
	    //  std::clog<<"DEBUG : EID : "<<EID <<std::endl;
	    _mcalo_id_bimap.insert( ID_doublet(GID , EID) ) ;
	  }
	}
      }






      
      return ;
    }

    void electronic_mapping::set_geo_manager(const geomtools::manager & mgr_ ){
      //DT_LOG_DEBUG(_logging_, "Set geometry manager ");
      //std::clog << "DEBUG : Before fill _geo_manager_ " << std::endl;
      _geo_manager_ = & mgr_;
      //std::clog << "DEBUG : After fill _geo_manager_ " << std::endl;
      _ID_convertor.set_geo_manager(mgr_);
      //  std::clog << "DEBUG : After ID_Convertor " << std::endl;
    }
    
    void electronic_mapping::set_module_number(int module_number_)
    {
      _module_number_ = module_number_;
      _ID_convertor.set_module_number(module_number_);
      return;
    }


    bool electronic_mapping::is_initialized()
    {
      return _initialized_;
    }
    
    void electronic_mapping::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Electronic mapping is not initialized, it can't be reset ! ");
      //_ID_convertor = 0;
      _geo_manager_ = 0;
      _initialized_ = false;
      return;
    }

	       
  } // end of namespace digitization

} // end of namespace snemo
