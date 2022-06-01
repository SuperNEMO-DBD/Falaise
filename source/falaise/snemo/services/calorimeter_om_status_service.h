//! \file falaise/snemo/services/calorimeter_om_status_service.h
// Falaise service for OM status in SuperNEMO calorimeter(s)

#ifndef SNEMO_CALORIMETER_OM_STATUS_SERVICE_H
#define SNEMO_CALORIMETER_OM_STATUS_SERVICE_H

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
#include "falaise/snemo/rc/calorimeter_om_status.h"
#include <falaise/snemo/services/db_service.h>

namespace snemo {

  /// \brief Calorimeter OM status service
  class calorimeter_om_status_service
    : public datatools::base_service
  {
  public:

    enum mode_type
      {
       MODE_DB = 0,
       MODE_FILES = 1,
       MODE_DEFAULT = MODE_FILES
      };

    calorimeter_om_status_service();

    virtual ~calorimeter_om_status_service() override;
    
    // Service interface:
    
    bool is_initialized() const override;

    int initialize(const datatools::properties &, datatools::service_dict_type &) override;
    
    int reset() override;

    mode_type get_mode() const;

    bool has_geometry_manager() const;

    void set_geometry_manager(const geomtools::manager & geomgr_);
    
    // Mode 'files':
    
    bool has_om_history(const geomtools::geom_id & gid_) const;
    
    const snemo::rc::calorimeter_om_status_history &
    get_om_history(const geomtools::geom_id & gid_) const;
 
    snemo::rc::calorimeter_om_status_history &
    grab_om_history(const geomtools::geom_id & gid_);

    // Main method:
    
    std::uint32_t get_om_status(const geomtools::geom_id & gid_,
                                  const time::time_point & t_) const;

    /// Load om status map from a CSV formatted file
    void load_om_status_map(const std::string & infile_);

    // void make_om_status_map(std::ostream & out_, const time::time_point & t_) const;

    typedef std::map<geomtools::geom_id, snemo::rc::calorimeter_om_status_history> history_type;
    
    const history_type & get_histories() const;
    
  private:

    void _init_mode_db_(const datatools::properties &);

    void _terminate_mode_db_();

    void _init_mode_files_(const datatools::properties &);

    void _terminate_mode_files_();

    bool _initialized_ = false;
    mode_type _mode_ = MODE_DEFAULT;
    std::string _geometry_label_;
    std::string _db_label_;
    const geomtools::manager * _geomgr_ = nullptr;
    std::uint32_t _om_types_[3] = { 1301, 1231, 1251 };
    const snemo::db_service * _db_service_ = nullptr;
    history_type _histories_;
    
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(calorimeter_om_status_service)
      
  };

  //! Falaise service template  
  template <>
  struct service_traits<calorimeter_om_status_service>
  {
    using label_type = BOOST_METAPARSE_STRING("calorimeterOmStatus");
    using service_type = calorimeter_om_status_service;
    using instance_type = service_type;
    static instance_type * get(service_type & sm) { return &sm; }
  };

} // namespace snemo

#endif  // SNEMO_CALORIMETER_OM_STATUS_SERVICE_H
