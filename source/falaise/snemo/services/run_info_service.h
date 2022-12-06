//! \file falaise/snemo/services/run_info_service.h
// Falaise service for run infos

#ifndef SNEMO_RUN_INFO_SERVICE_H
#define SNEMO_RUN_INFO_SERVICE_H

// Standard library:
#include <iostream>
#include <fstream>
#include <map>

// - Bayeux:
#include <bayeux/datatools/base_service.h>

// - Falaise:
#include "falaise/snemo/services/service_traits.h"
#include "falaise/snemo/rc/run_description.h"
#include "falaise/snemo/rc/run_list.h"
#include <falaise/snemo/services/db_service.h>

namespace snemo {

  /// \brief Run info service
  class run_info_service
    : public datatools::base_service
  {
  public:

    enum mode_type
      {
        MODE_DB = 0,
        MODE_FILES = 1,
        MODE_DEFAULT = MODE_FILES
      };

    run_info_service();

    virtual ~run_info_service() override;
    
    // Service interface:
    
    bool is_initialized() const override;

    int initialize(const datatools::properties &, datatools::service_dict_type &) override;
    
    int reset() override;

    mode_type get_mode() const;
    
    const snemo::rc::run_list & get_run_list() const;
 
    snemo::rc::run_list & grab_run_list();
     
    bool has_run_info(const rc::run_id_type run_id_) const;
   
    const rc::run_description & get_run_info(const rc::run_id_type run_id_) const;
 
    /// Load run_list
    void load_run_list(const std::string & infile_);

    //! Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;
   
  private:

    void _init_mode_db_(const datatools::properties &);

    void _terminate_mode_db_();

    void _init_mode_files_(const datatools::properties &);

    void _terminate_mode_files_();

    // Attributes:
    bool _initialized_ = false;
    mode_type _mode_ = MODE_DEFAULT;
    std::string _db_label_;
    const snemo::db_service * _db_service_ = nullptr;
    rc::run_list _runs_;   
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(run_info_service)
      
  };

  //! Falaise service template  
  template <>
  struct service_traits<run_info_service>
  {
    using label_type = BOOST_METAPARSE_STRING("runInfo");
    using service_type = run_info_service;
    using instance_type = service_type;
    static instance_type * get(service_type & sm) { return &sm; }
  };

} // namespace snemo

#endif // SNEMO_RUN_INFO_SERVICE_H
