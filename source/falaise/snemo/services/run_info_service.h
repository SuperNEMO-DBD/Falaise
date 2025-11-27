//! \file falaise/snemo/services/run_info_service.h
// Falaise service for run infos

#ifndef SNEMO_RUN_INFO_SERVICE_H
#define SNEMO_RUN_INFO_SERVICE_H

// Standard library:
#include <iostream>
#include <fstream>
#include <map>
#include <cstdint>
#include <vector>

// - Bayeux:
#include <bayeux/datatools/base_service.h>

// - Falaise:
#include "falaise/snemo/services/service_traits.h"
#include "falaise/snemo/rc/run_description.h"
#include "falaise/snemo/rc/run_list.h"
#include "falaise/snemo/rc/run_phase.h"
#include <falaise/snemo/services/db_service.h>

namespace snemo {

  class base_run_selector;
  
  /// \brief Run info service
  class run_info_service
    : public datatools::base_service
  {
  public:

    enum mode_type
      {
        MODE_DB = 0,
        MODE_FILES = 1,
        MODE_CSV_FILES = 2,
        MODE_DEFAULT = MODE_DB
      };

    run_info_service();

    virtual ~run_info_service() override;
    
    // Service interface:
    
    bool is_initialized() const override;

    int initialize(const datatools::properties &,
		   datatools::service_dict_type &) override;
    
    int reset() override;

    mode_type get_mode() const;
    
    const snemo::rc::run_list & get_run_list() const;
 
    snemo::rc::run_list & grab_run_list();
     
    bool has_run_info(const rc::run_id_type run_id_) const;

    bool has_run_phase(const std::uint32_t id_) const;
    
    /// Return the array of official run phases
    const std::vector<rc::run_phase> & run_phases() const;

    const rc::run_phase & get_run_phase(const std::uint32_t id_) const;
    
    const rc::run_description & get_run_info(const rc::run_id_type run_id_) const;
    
    /// Load run list
    void load_run_list(const std::string & infile_);
 
    /// Load run_list
    void load_run_csv_list(const std::string & infile_);

    //! Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;
   
  private:

    void _init_mode_db_run_phases_(const datatools::properties &);

    void _init_mode_db_run_infos_(const datatools::properties &);

    void _init_mode_db_(const datatools::properties &);

    void _terminate_mode_db_();

    void _init_mode_files_(const datatools::properties &);

    void _terminate_mode_files_();

    void _init_mode_csv_files_(const datatools::properties &);

    void _terminate_mode_csv_files_();

    // Attributes:
    bool _initialized_ = false; ///< Initializaion flag
    mode_type _mode_ = MODE_DEFAULT; ///< Mode
    std::string _db_label_; ///< Db service ID
    const snemo::db_service * _db_service_ = nullptr; ///< Handle the DB service
    rc::run_list _runs_; ///< List of run descriptions
    std::vector<rc::run_phase> _run_phases_; ///< List of run phases
    
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
