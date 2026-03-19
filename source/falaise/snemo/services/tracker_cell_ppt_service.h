// -*- mode: c++ ; -*-
//! \file falaise/snemo/services/tracker_cell_ppt_service.h
// Falaise service for cell PPTs in SuperNEMO tracker (Plasma Propagation Time)

#ifndef SNEMO_TRACKER_CELL_PPT_SERVICE_H
#define SNEMO_TRACKER_CELL_PPT_SERVICE_H

// Standard library:
#include <iostream>
#include <fstream>
#include <map>

// - Bayeux:
#include <bayeux/datatools/base_service.h>
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/manager.h>

// - Falaise:
#include "falaise/snemo/services/service_traits.h"
#include "falaise/snemo/rc/tracker_cell_status.h"
#include "falaise/snemo/rc/tracker_cell_ppt.h"
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/services/tracker_cell_status_service.h>

namespace snemo {
 
  /// \brief Tracker cell PPT service
  class tracker_cell_ppt_service
    : public datatools::base_service
  {
  public:

    enum mode_type
      {
	MODE_DB = 0, ///< Using Database 'GG_PPT' table
	MODE_FILES = 1, ///< Using local file with PPT records for all cells
	MODE_FALLBACK = 2, ///< Using a fallback mock file (not for production)
	MODE_DEFAULT = MODE_FILES
      };
   
    tracker_cell_ppt_service();
    
    virtual ~tracker_cell_ppt_service() override;
   
    // Service interface:
    
    bool is_initialized() const override;

    int initialize(const datatools::properties &,
		   datatools::service_dict_type &) override;
    
    int reset() override;
    
    bool has_cell_history(const geomtools::geom_id & gid_) const;
    
    const snemo::rc::tracker_cell_ppt_history &
    get_cell_history(const geomtools::geom_id & gid_) const;
 
    snemo::rc::tracker_cell_ppt_history &
    grab_cell_history(const geomtools::geom_id & gid_);

    const snemo::rc::tracker_cell_ppt_history_map & get_histories() const;

    // Main method:   
    snemo::rc::tracker_cell_ppt_info get_cell_ppt_info(const geomtools::geom_id & gid_,
						       const time::time_point & time_) const;

    //! Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;
    
    /// Load cell PPT map from a CSV formatted file
    void load_cell_ppt_map(const std::string & infile_);

  private:

    /// Load cell PPT map from a CSV formatted file (format 1)
    void _load_cell_ppt_map_1_(const std::string & infile_);

    void _init_mode_db_(const datatools::properties &);

    void _terminate_mode_db_();

    void _init_mode_files_(const datatools::properties &);

    void _terminate_mode_files_();

  private:
    
    bool _initialized_ = false;
    mode_type _mode_ = MODE_DEFAULT;
    std::string _tracker_cell_status_label_;
    const snemo::tracker_cell_status_service * _tracker_cell_status_service_ = nullptr; ///< Handle the tracker cell status service
    std::string _db_label_;
    const snemo::db_service * _db_service_ = nullptr; ///< Handle the DB service
    std::string _file_format_ = "csv-1";
    std::uint32_t _cell_type_ = 1203;
    std::uint32_t _cell_core_type_ = 1204;
    snemo::rc::tracker_cell_ppt_history_map _histories_;

    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(tracker_cell_ppt_service)
      
  };

} // namespace snemo

#endif // SNEMO_TRACKER_CELL_PPT_SERVICE_H
