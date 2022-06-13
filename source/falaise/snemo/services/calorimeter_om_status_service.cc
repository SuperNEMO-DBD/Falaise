// -*- mode: c++ ; -*-
/** \file falaise/snemo/services/calorimeter_om_status_service.cc */

// Ourselves:
#include <falaise/snemo/services/calorimeter_om_status_service.h>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// Bayeux:
#include <bayeux/geomtools/geometry_service.h>

// This project:
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::calorimeter_om_status_service,
                                         "snemo::calorimeter_om_status_service")

namespace snemo {

  calorimeter_om_status_service::calorimeter_om_status_service()
    : datatools::base_service("CalorimeterOMStatusService",
                              "Calorimeter OM status service",
                              "Service publishing calorimeter OM status")
  {
    return;
  }

  calorimeter_om_status_service::~calorimeter_om_status_service()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }
  
  bool calorimeter_om_status_service::is_initialized() const
  {
    return _initialized_;
  }

  int calorimeter_om_status_service::initialize(const datatools::properties & config_,
                                                datatools::service_dict_type & services_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");

    if (config_.has_key("geometry_label")) {
      _geometry_label_ = config_.fetch_string("geometry_label");     
    }
    if (_geometry_label_.empty()) {
      _geometry_label_ = service_info::geometryServiceName();
    }
    if (datatools::has<geomtools::geometry_service>(services_, _geometry_label_)) {
      _geomgr_ = &datatools::get<geomtools::geometry_service>(services_, _geometry_label_).get_geom_manager();
    }
    DT_THROW_IF(_geomgr_ == nullptr, std::logic_error, "Missing geometry manager!");
    
    const geomtools::id_mgr & idMgr = _geomgr_->get_id_mgr();
    int caloOmType  = idMgr.categories_by_name().find("calorimeter_optical_module")->second.get_type();
    int xcaloOmType = idMgr.categories_by_name().find("xcalo_optical_module")->second.get_type();
    int gvetoOmType = idMgr.categories_by_name().find("gveto_optical_module")->second.get_type();
    _om_types_[0] = caloOmType;
    _om_types_[1] = xcaloOmType;
    _om_types_[2] = gvetoOmType;
    
    if (config_.has_key("mode")) {
      std::string modeLabel = config_.fetch_string("mode");
      if (modeLabel == "files") {
        _mode_ = MODE_FILES;
      } else if (modeLabel == "db") {
        _mode_ = MODE_DB;
      } else {
        DT_THROW(std::logic_error, "Invalid mode label '" << modeLabel << "'!");
      }
    }

    if (_mode_ == MODE_DB) {
      if (config_.has_key("db_label")) {
        _db_label_ = config_.fetch_string("db_label");     
      }
      if (_db_label_.empty()) {
        _db_label_ = service_info::dbServiceName();
      }
      if (datatools::has<snemo::db_service>(services_, _db_label_)) {
        _db_service_ = &datatools::get<snemo::db_service>(services_, _db_label_);
      }
      datatools::properties dbConfig;
      config_.export_and_rename_starting_with(dbConfig, "db.", "");
      _init_mode_db_(dbConfig);
    }

    if (_mode_ == MODE_FILES) {
      datatools::properties filesConfig;
      config_.export_and_rename_starting_with(filesConfig, "files.", "");
      _init_mode_files_(filesConfig);
    }
      
    _initialized_ = true;
    return 0;
  }

  int calorimeter_om_status_service::reset()
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    
    _initialized_ = false;
    if (_mode_ == MODE_DB) {
      _terminate_mode_db_();
    } else if (_mode_ == MODE_FILES) {
      _terminate_mode_files_();
    }
   
    _mode_ = MODE_DEFAULT;
    return 0;
  }

  calorimeter_om_status_service::mode_type calorimeter_om_status_service::get_mode() const
  {
    return _mode_;
  }

  bool calorimeter_om_status_service::has_geometry_manager() const
  {
    return _geomgr_ != nullptr;
  }

  void calorimeter_om_status_service::set_geometry_manager(const geomtools::manager & geomgr_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
    DT_THROW_IF(not geomgr_.is_initialized(), std::logic_error, "Geometry manager is not initialized!");
    _geomgr_ = &geomgr_;
    return;
  }

  void calorimeter_om_status_service::_init_mode_db_(const datatools::properties &)
  {
    DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    return;
  }

  void calorimeter_om_status_service::_terminate_mode_db_()
  {
    DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    return;
  }
  
  void calorimeter_om_status_service::_init_mode_files_(const datatools::properties & config_)
  {
    if (config_.has_key("om_maps")) {
      std::vector<std::string> omMaps;
      config_.fetch("om_maps", omMaps);
      for (const auto & omMap: omMaps) {
        load_om_status_map(omMap);
      }
    }
    DT_LOG_DEBUG(get_logging_priority(), "Number of histories : " << _histories_.size());
    for (const auto & h : _histories_) {
      std::clog << "[debug] " << "GID=" << h.first
                << " has " << h.second.records().size() << " status records"
                << '\n';
    }
    return;
  }

  void calorimeter_om_status_service::_terminate_mode_files_()
  {
    _histories_.clear();
    return;
  }

  bool calorimeter_om_status_service::has_om_history(const geomtools::geom_id & gid_) const
  {
    return _histories_.count(gid_) != 0;
  }

  snemo::rc::calorimeter_om_status_history &
  calorimeter_om_status_service::grab_om_history(const geomtools::geom_id & gid_)
  {
    if (not has_om_history(gid_)) {
      snemo::rc::calorimeter_om_status_history emptyHistory;
      _histories_[gid_] = emptyHistory;
    }
    history_type::iterator found = _histories_.find(gid_);
    return found->second;
  }

  const snemo::rc::calorimeter_om_status_history &
  calorimeter_om_status_service::get_om_history(const geomtools::geom_id & gid_) const
  {
    history_type::const_iterator found = _histories_.find(gid_);
    DT_THROW_IF(found == _histories_.end(), std::logic_error, "No history for OM '" << gid_ << "'!");
    return found->second;
  }

  const calorimeter_om_status_service::history_type &
  calorimeter_om_status_service::get_histories() const
  {
    return _histories_;
  }
   
  std::uint32_t calorimeter_om_status_service::get_om_status(const geomtools::geom_id & gid_,
                                                             const time::time_point & time_) const
  {
    std::uint32_t status = snemo::rc::calorimeter_om_status::OM_GOOD;
    if (_mode_ == MODE_DB) {
      DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    } else if (_mode_ == MODE_FILES) {
      history_type::const_iterator found = _histories_.find(gid_);
      if (found != _histories_.end()) {
        DT_LOG_DEBUG(get_logging_priority(), "Found history for om " << gid_);
        status = found->second.get_status(time_);
      } else {
        // DT_LOG_DEBUG(get_logging_priority(), "No history for om " << gid_);
      }
    }
    return status;
  }

  void calorimeter_om_status_service::load_om_status_map(const std::string & infile_)
  {
    const snemo::geometry::locator_plugin & locators
      = _geomgr_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
    const snemo::geometry::calo_locator & caloLocator = locators.caloLocator();
    const snemo::geometry::xcalo_locator & xcaloLocator = locators.xcaloLocator();
    const snemo::geometry::gveto_locator & gvetoLocator = locators.gvetoLocator();
    std::string filename = infile_;
    datatools::fetch_path_with_env(filename);
    std::ifstream fin(filename);
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open tracker om status file '" << filename << "'!");
    while (fin and not fin.eof()) {
      std::string line;
      std::getline(fin, line);
      DT_LOG_DEBUG(get_logging_priority(), "Line='" << line << "'");
      {
        std::istringstream checkss(line);
        std::string word;
        checkss >> word;
        if (word.size() == 0 or word[0] == '#') {
          continue;
        }
      }
      typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
      boost::char_separator<char> sep{";"};
      tokenizer tokens{line, sep};
      std::string geomIdRepr;
      std::string periodRepr;
      std::string statusRepr;
      int tkCount=0;
      for (std::string tk : tokens) {
        if (geomIdRepr.empty()) {
          DT_THROW_IF(tkCount != 0, std::logic_error, "Invalid formatted calorimeter OM status record!");
          geomIdRepr = tk;
        } else if (periodRepr.empty()) {
          DT_THROW_IF(tkCount != 1, std::logic_error, "Invalid formatted calorimeter OM status record!");
          periodRepr = tk;
        } else if (statusRepr.empty()) {
          DT_THROW_IF(tkCount != 2, std::logic_error, "Invalid formatted calorimeter OM status record!");
          statusRepr = tk;
        } else {
          DT_THROW(std::logic_error, "Invalid token '" << tk << "'!");
        }
        tkCount++;
      }
      geomtools::geom_id gid;
      boost::trim(geomIdRepr);
      DT_THROW_IF(geomIdRepr.empty(), std::logic_error, "Missing calorimeter OM geom ID!");
      {
        std::istringstream gidss(geomIdRepr);
        gidss >> gid;
        DT_THROW_IF(!gidss, std::logic_error, "Cannot decode missing calorimeter OM geom ID!");
        DT_THROW_IF(not gid.is_valid(), std::logic_error,
                    "Invalid calorimeter OM geom ID parsed from '" << geomIdRepr  << "'!");
        bool validType = false;
        if (gid.get_type() == _om_types_[0] or
            gid.get_type() == _om_types_[1] or
            gid.get_type() == _om_types_[2]) validType = true;
        DT_THROW_IF(not validType, std::logic_error,
                    "Invalid type for calorimeter OM geom ID '" << gid  << "'!");
        bool validGid = false;
        // XXX
        if (caloLocator.isCaloOM(gid) or
            xcaloLocator.isCaloOM(gid) or
            gvetoLocator.isCaloOM(gid)) validGid = true;
        DT_THROW_IF(not validGid, std::logic_error,
                    "Not a valid (x)calorimeter/gveto OM geom ID '" << gid  << "'!");
      }
      boost::trim(periodRepr);
      DT_THROW_IF(periodRepr.empty(), std::logic_error, "Missing period!");
      time::time_period period = time::time_period_from_string(periodRepr);
      DT_THROW_IF(not time::is_valid(period), std::logic_error,
                  "Invalid period parsed from '" << periodRepr  << "'!");
      boost::trim(statusRepr);
      std::uint32_t status = snemo::rc::calorimeter_om_status::status_from_string(statusRepr);
      if (status != snemo::rc::calorimeter_om_status::OM_GOOD) {
        snemo::rc::calorimeter_om_status_history & omHistory = grab_om_history(gid);
        DT_LOG_DEBUG(get_logging_priority(), "gid=" << gid << " period=" << time::to_string(period) << " status=" << status);
        omHistory.add(period, status);
      } else {
        DT_LOG_WARNING(get_logging_priority(), "Ignoring good status for OM " << gid << "");
      }
    }   
    fin.close();
    return;
  }

  /*
  void calorimeter_om_status_service::make_om_status_map(std::ostream & out_, const time::time_point & t_) const
  {
    const snemo::geometry::locator_plugin & locators = _geomgr_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
   
    const snemo::geometry::calo_locator & caloLocator = locators.caloLocator();
    const snemo::geometry::xcalo_locator & xcaloLocator = locators.xcaloLocator();
    const snemo::geometry::gveto_locator & gvetoLocator = locators.gvetoLocator();
    unsigned int moduleId = 0;
    unsigned int iSide = 0;
   
    // for (unsigned int iCol = 0; iCol < ggLocator.numberOfLayers(iSide); iCol++) {
    //   for (unsigned int iRow = 0; iRow < ggLocator.numberOfRows(iSide); iRow++) {
    //     geomtools::geom_id gid(_om_type_, moduleId, iSide, iLayer, iRow);
    //     // std::clog << "======> gid=" << gid << '\n';
    //     std::uint32_t status = get_om_status(gid, t_);
    //     geomtools::vector_3d omPos = ggLocator.getCellPosition(iSide, iLayer, iRow);
    //     out_ << cellPos.y() << ' ' << cellPos.x() << ' ' << status << '\n';
    //   }
    //   out_ << '\n';
    // }
    // out_ << '\n';
    // iSide = 1;
    // for (unsigned int iLayer = 0; iLayer < ggLocator.numberOfLayers(iSide); iLayer++) {
    //   for (unsigned int iRow = 0; iRow < ggLocator.numberOfRows(iSide); iRow++) {
    //     geomtools::geom_id gid(_cell_type_, moduleId, iSide, iLayer, iRow);
    //     // std::clog << "======> gid=" << gid << '\n';
    //     std::uint32_t status = get_cell_status(gid, t_);
    //     geomtools::vector_3d cellPos = ggLocator.getCellPosition(iSide, iLayer, iRow);
    //     out_ << cellPos.y() << ' ' << cellPos.x() << ' ' << status << '\n';
    //   }
    //   out_ << '\n';
    // } 
    // out_ << '\n';
    return;
  }
  */

}  // namespace snemo
