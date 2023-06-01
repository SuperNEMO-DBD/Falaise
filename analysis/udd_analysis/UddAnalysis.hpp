//! \file UddAnalysis
//! \brief User analysis module for flreconstruct
//! \details Process a things object and process UDD data bank.

#ifndef UDDANALYSIS_H
#define UDDANALYSIS_H

// - Bayeux
#include <bayeux/datatools/service_manager.h>
#include <bayeux/dpp/base_module.h>

// - Falaise
#include <falaise/snemo/datamodels/calibrated_data.h>

/// \brief UDD analysis module
class UddAnalysis
  : public dpp::base_module
{
public:
  
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

private:

  // Macro which automatically creates the interface needed
  // to enable the module to be loaded at runtime
  DPP_MODULE_REGISTRATION_INTERFACE(UddAnalysis)
  
};

#endif // UDDANALYSIS_H
