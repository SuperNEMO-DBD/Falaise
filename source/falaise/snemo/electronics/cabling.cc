// snemo/electronics/cabling.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves :
#include <snemo/electronics/cabling.h>

// Standard library :
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/exception.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>

// This project :
#include <snemo/geometry/locator_plugin.h>

namespace snemo {

  namespace electronics {

    datatools::logger::priority cabling::get_logging() const
    {
      return _logging_;
    }

    void cabling::set_logging(const datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    const std::set<int32_t> &
    cabling::get_supported_geom_types() const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Not initialized.");
      return _supported_geom_types_;
    }

    void cabling::_init_supported_geom_types()
    {
      if (_gg_cat_info_ != nullptr) {
        _supported_geom_types_.insert(_gg_cat_info_->get_type());
      }
      if (_mcalo_cat_info_ != nullptr) {
        _supported_geom_types_.insert(_mcalo_cat_info_->get_type());
      }
      if (_xcalo_cat_info_ != nullptr) {
        _supported_geom_types_.insert(_xcalo_cat_info_->get_type());
      }
      if (_gveto_cat_info_ != nullptr) {
        _supported_geom_types_.insert(_gveto_cat_info_->get_type());
      }
      return;
    }

    void cabling::_set_defaults()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      _geo_manager_ = nullptr;
      _module_number_ = geomtools::geom_id::INVALID_ADDRESS;
      _mcalo_cat_info_  = nullptr;
      _xcalo_cat_info_  = nullptr;
      _gveto_cat_info_  = nullptr;
      _gg_cat_info_   = nullptr;
      _mcalo_locator_ = nullptr;
      _gveto_locator_ = nullptr;
      _xcalo_locator_ = nullptr;
      _gg_locator_    = nullptr;
      return;
    }

    cabling::cabling()
    {
      _initialized_ = false;
      _set_defaults();
      return;
    }

    cabling::~cabling()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void cabling::add_preconstructed_type(int type_)
    {
      // DT_THROW_IF(supported_geom_types().count(type_) == 0, std::logic_error, "Geometry category type ["<< type_ << "] is not supported ! ");
      _pre_constructed_types_.insert(type_);
      return;
    }

    void cabling::set_geo_manager(const geomtools::manager & mgr_ )
    {
      _geo_manager_ = & mgr_;
    }

    bool cabling::has_geo_manager() const
    {
      return _geo_manager_ != nullptr;
    }

    void cabling::set_module_number(int module_number_)
    {
      _module_number_ = module_number_;
      return;
    }

    bool cabling::has_module_number() const
    {
      return _module_number_ != geomtools::geom_id::INVALID_ADDRESS;
    }

    bool cabling::is_initialized() const
    {
      return _initialized_;
    }

    void cabling::initialize()
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Electronic cabling is already initialized ! ");
      DT_THROW_IF(!has_geo_manager(), std::logic_error, "Geo manager is not set ! ");
      DT_THROW_IF(!has_module_number(), std::logic_error, "Module number is not set ! ");
      _initialize();
      _initialized_ = true;
      return;
    }

    void cabling::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Electronic cabling is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _geiger_id_bimap_.clear();
      _mcalo_id_bimap_.clear();
      _xcalo_id_bimap_.clear();
      _gveto_id_bimap_.clear();
      _supported_geom_types_.clear();
      _pre_constructed_types_.clear();
      _set_defaults();
      return;
    }

    void cabling::convert_GID_to_EID(const geomtools::geom_id & geom_id_,
                                                 geomtools::geom_id & electronic_id_) const
    {
      const_cast<cabling *>(this)->_convert_GID_to_EID(geom_id_,
                                                                   electronic_id_);
      return;
    }

    void cabling::convert_EID_to_GID(const geomtools::geom_id & electronic_id_,
                                                 geomtools::geom_id & geom_id_) const
    {
      const_cast<cabling *> (this) -> _convert_EID_to_GID(electronic_id_,
                                                                      geom_id_);
      return;
    }

    void cabling::_initialize()
    {
      const geomtools::id_mgr & idmgr = _geo_manager_->get_id_mgr();

      if (idmgr.has_category_info("drift_cell_core")) {
        _gg_cat_info_ = &idmgr.get_category_info("drift_cell_core");
        _supported_geom_types_.insert(_gg_cat_info_->get_type());
      }

      if (idmgr.has_category_info("calorimeter_block")) {
        _mcalo_cat_info_ = &idmgr.get_category_info("calorimeter_block");
        _supported_geom_types_.insert(_mcalo_cat_info_->get_type());
      }

      if (idmgr.has_category_info("xcalo_block")) {
        _xcalo_cat_info_ = &idmgr.get_category_info("xcalo_block");
        _supported_geom_types_.insert(_xcalo_cat_info_->get_type());
      }

      if (idmgr.has_category_info("gveto_block")) {
        _gveto_cat_info_ = &idmgr.get_category_info("gveto_block");
        _supported_geom_types_.insert(_gveto_cat_info_->get_type());
      }

      if (_geo_manager_->is_plugin_a<snemo::geometry::locator_plugin>("locators_driver")) {
        const snemo::geometry::locator_plugin & lp
          = _geo_manager_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
        if (lp.has_gg_locator()) {
          _gg_locator_ = &lp.get_gg_locator();
        }
        if (lp.has_calo_locator()) {
          _mcalo_locator_ = &lp.get_calo_locator();
        }
        if (lp.has_xcalo_locator()) {
          _xcalo_locator_ = &lp.get_xcalo_locator();
        }
        if (lp.has_gveto_locator()) {
          _gveto_locator_ = &lp.get_gveto_locator();
        }
      }
      // const geomtools::cabling & gmap = _geo_manager_->
      // _ID_convertor_.set_module_number(module_number_);
      // _ID_convertor_.set_geo_manager(*_geo_manager_);
      // _ID_convertor_.initialize();
      for (std::set<int32_t>::iterator it = _pre_constructed_types_.begin();
           it != _pre_constructed_types_.end();
           it++) {
        if (*it == _gg_cat_info_->get_type()) _init_geiger();
        else if (*it == _mcalo_cat_info_->get_type()) _init_mcalo();
        else if (*it == _xcalo_cat_info_->get_type()) _init_xcalo();
        else if (*it == _gveto_cat_info_->get_type()) _init_gveto();
      }
      return;
    }


    void cabling::_init_geiger()
    {
      geomtools::geom_id GID;
      _gg_cat_info_->create(GID);
      GID.set(_gg_cat_info_->get_subaddress_index("module"), _module_number_);
      for(unsigned int side = 0; side < _gg_locator_->get_number_of_sides(); side++) {
        GID.set(_gg_cat_info_->get_subaddress_index("side"), side);
        for(unsigned int layer = 0; layer < _gg_locator_->get_number_of_layers(side); layer++) {
          GID.set(_gg_cat_info_->get_subaddress_index("layer"), layer);
          for(unsigned int row = 0; row < _gg_locator_->get_number_of_rows(side); row++) {
            GID.set(_gg_cat_info_->get_subaddress_index("row"), row);
            geomtools::geom_id EID;
            // XXX  EID = _ID_convertor_.convert_GID_to_EID(GID);
            _geiger_id_bimap_.insert( ID_doublet(GID , EID) ) ;
          } // end of row loop
        } // end of layer loop
      } // end of side loop
      return ;
    }

    void cabling::_init_mcalo()
    {
      geomtools::geom_id GID;
      _mcalo_cat_info_->create(GID);
      GID.set(_mcalo_cat_info_->get_subaddress_index("module"), _module_number_);
      for(unsigned int side = 0; side < _mcalo_locator_->get_number_of_sides(); side++) {
        GID.set(_mcalo_cat_info_->get_subaddress_index("side"), side);
        for(unsigned int column = 0; column < _mcalo_locator_->get_number_of_columns(side); column++) {
          GID.set(_mcalo_cat_info_->get_subaddress_index("column"), column);
          for(unsigned int row = 0; row < _mcalo_locator_->get_number_of_rows(side); row++) {
            GID.set(_mcalo_cat_info_->get_subaddress_index("row"), row);
            geomtools::geom_id EID;
            // XXX EID = _ID_convertor_.convert_GID_to_EID(GID);
            _mcalo_id_bimap_.insert( ID_doublet(GID , EID) ) ;
          } // end of row loop
        } // end of column loop
      } // end of side loop
      return ;
    }

    void cabling::_init_xcalo()
    {
      geomtools::geom_id GID;
      _xcalo_cat_info_->create(GID);
      GID.set(_xcalo_cat_info_->get_subaddress_index("module"), _module_number_);
      for(unsigned int side = 0; side < _xcalo_locator_->get_number_of_sides(); side++) {
        GID.set(_xcalo_cat_info_->get_subaddress_index("side"), side);
        for(unsigned int wall = 0; wall < _xcalo_locator_->get_number_of_walls(); wall++) {
          GID.set(_xcalo_cat_info_->get_subaddress_index("wall"),wall);
          for(unsigned int column = 0; column < _xcalo_locator_->get_number_of_columns(side, wall); column++) {
            GID.set(_xcalo_cat_info_->get_subaddress_index("column"), column);
            for(unsigned int row = 0; row < _xcalo_locator_->get_number_of_rows(side, wall); row++) {
              GID.set(_xcalo_cat_info_->get_subaddress_index("row"), row);
              geomtools::geom_id EID;
              // XXX EID = _ID_convertor_.convert_GID_to_EID(GID);
              _mcalo_id_bimap_.insert( ID_doublet(GID , EID) ) ;
            } // end of row loop
          } // end of column loop
        } // end of wall loop
      } // end of side loop
      return ;
    }

    void cabling::_init_gveto()
    {
      geomtools::geom_id GID;
      _gveto_cat_info_->create(GID);
      GID.set(_gveto_cat_info_->get_subaddress_index("module"), _module_number_);
      for(unsigned int side = 0; side < _gveto_locator_->get_number_of_sides(); side++) {
        GID.set(_gveto_cat_info_->get_subaddress_index("side"), side);
        for(unsigned int wall = 0; wall < _gveto_locator_->get_number_of_walls(); wall++) {
          GID.set(_gveto_cat_info_->get_subaddress_index("wall"), wall);
          for(unsigned int column = 0; column < _gveto_locator_->get_number_of_columns(side, wall); column++) {
            GID.set(_gveto_cat_info_->get_subaddress_index("column"), column);
            geomtools::geom_id EID;
            // XXX EID = _ID_convertor_.convert_GID_to_EID(GID);
            _gveto_id_bimap_.insert( ID_doublet(GID , EID) ) ;
          } // end of row loop
        } // end of wall loop
      } // end of side loop
      return;
    }

    void cabling::_convert_EID_to_GID(const geomtools::geom_id & elec_id_,
                                                  geomtools::geom_id & geom_id_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Electronic cabling is not initialized ! ");
      // DT_THROW_IF(elec_id_.get_type() != cabling::FEB_CATEGORY_TYPE, std::logic_error, "elect_id incorrect type ! ");
      geom_id_.reset();



      /*
      ID_bimap::right_const_iterator right_iter ;

      switch (elec_id_.get(cabling::RACK_INDEX)) {
      case cabling::GEIGER_RACK_ID :
        right_iter = _geiger_id_bimap_.right.find(elec_id_);
        if (right_iter != _geiger_id_bimap_.right.end() ) {
          geom_id_ = right_iter->second;
        }
        break;

      case cabling::CALO_RACK_ID :
        right_iter = _mcalo_id_bimap_.right.find(elec_id_);
        if (right_iter != _mcalo_id_bimap_.right.end() ) {
          geom_id_ = right_iter->second;
        } else {
        }
        break;

      default :
        break;
      }
      */

      return;
    }

    void cabling::_convert_GID_to_EID(const geomtools::geom_id & geom_id_,
                                                  geomtools::geom_id & electronic_id_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Electronic cabling is not initialized ! ");
      electronic_id_.reset();
      ID_bimap::left_const_iterator left_iter ;
      if (_gg_cat_info_ != nullptr && geom_id_.get_type() == _gg_cat_info_->get_type()) {
        ID_bimap::left_const_iterator left_iter = _geiger_id_bimap_.left.find(geom_id_);
        if (left_iter != _geiger_id_bimap_.left.end() )  {
          electronic_id_ = left_iter->second;
        }
      } else if (_mcalo_cat_info_ != nullptr && geom_id_.get_type() == _mcalo_cat_info_->get_type()) {
        ID_bimap::left_const_iterator left_iter = _mcalo_id_bimap_.left.find(geom_id_);
        if (left_iter != _mcalo_id_bimap_.left.end() )  {
          electronic_id_ = left_iter->second;
        }
      } else if (_xcalo_cat_info_ != nullptr && geom_id_.get_type() == _xcalo_cat_info_->get_type()) {
        ID_bimap::left_const_iterator left_iter = _xcalo_id_bimap_.left.find(geom_id_);
        if (left_iter != _xcalo_id_bimap_.left.end() )  {
          electronic_id_ = left_iter->second;
        }
      } else if (_gveto_cat_info_ != nullptr && geom_id_.get_type() == _gveto_cat_info_->get_type()) {
        ID_bimap::left_const_iterator left_iter = _gveto_id_bimap_.left.find(geom_id_);
        if (left_iter != _gveto_id_bimap_.left.end() )  {
          electronic_id_ = left_iter->second;
        }
      }
      return ;
    }

  } // end of namespace electronics

} // end of namespace snemo
