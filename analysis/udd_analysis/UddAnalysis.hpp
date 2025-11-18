//! \file UddAnalysis
//! \brief User analysis module for flreconstruct
//! \details Process a things object and process UDD data bank.

#ifndef UDDANALYSIS_H
#define UDDANALYSIS_H

// Standard library:
#include <iostream>

// - Bayeux
#include <bayeux/datatools/service_manager.h>
#include <bayeux/dpp/base_module.h>

// - Falaise
#include <falaise/snemo/datamodels/calibrated_data.h>
#include <falaise/snemo/datamodels/timestamp.h>

void __attribute__((constructor)) udd_analysis_init();
void __attribute__((destructor))  udd_analysis_fini();

/// \brief UDD analysis module
class UddAnalysis
  : public dpp::base_module
{
public:

  static const uint32_t CLOCK_TICK_NS = 25;
  static const int PICOTOMICRO = 1000000;

  struct om_record
  {
    int om_id = -1;
    uint32_t status = 0u;
    uint32_t ncounts;
    uint32_t ncountsHT;
    uint32_t ncountsLTO;
    uint32_t ncountsNoT;
    void print(std::ostream & out_) const;
    void print_csv(std::ostream & out_) const;
  };

  struct gg_record
  {
    int gg_id = -1;
    uint32_t status = 0u;
    uint32_t ncountsR0 = 0u;
    uint32_t ncountsR1 = 0u;
    uint32_t ncountsR2 = 0u;
    uint32_t ncountsR3 = 0u;
    uint32_t ncountsR4 = 0u;
    uint32_t ncountsR5 = 0u;
    uint32_t ncountsR6 = 0u;
    void print_csv(std::ostream & out_) const;
  };
  
  
  struct stats_data
  {
    uint32_t eventCounter = 0u;
    snemo::datamodel::timestamp firstEventTimeStamp;
    int64_t runRefTs = 0;
    int64_t lastRefTs = 0;
    std::map<int, om_record> om_records;
    std::map<int, gg_record> gg_records;
    stats_data();
    ~stats_data();
    
  };
  
  //! Construct module
  UddAnalysis();

  //! Destructor
  virtual ~UddAnalysis();

  //! Configure the module
  void initialize(const datatools::properties & config_,
		  datatools::service_manager & services_,
		  dpp::module_handle_dict_type & modules_) override;

  //! Reset the module
  void reset() override;

  //! Process supplied data record
  dpp::base_module::process_status process(datatools::things & event_record_) override;

  void print_report() const;

  void build_report();

  void build_report_oms();

  void build_report_ggs();
  
private:

  // Config:
  bool _print_udd_data_ = false;

  // Working data:
  stats_data _stats_;
  
  // Macro which automatically creates the interface needed
  // to enable the module to be loaded at runtime
  DPP_MODULE_REGISTRATION_INTERFACE(UddAnalysis)
  
};

#endif // UDDANALYSIS_H
