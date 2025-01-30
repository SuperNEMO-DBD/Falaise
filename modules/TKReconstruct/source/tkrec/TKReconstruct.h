#ifndef FALAISE_TKRECONSTRUCT_TKRECONSTRUCT_H
#define FALAISE_TKRECONSTRUCT_TKRECONSTRUCT_H

// Interface from Falaise
#include "bayeux/dpp/base_module.h"
#include "bayeux/mctools/simulated_data.h"

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/geomtools/line_3d.h>
#include <bayeux/geomtools/polyline_3d.h>

#include "falaise/snemo/processing/module.h"
#include "falaise/snemo/datamodels/calibrated_data.h"
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/event_header.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include "falaise/snemo/datamodels/tracker_clustering_solution.h"
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_solution.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>
#include <falaise/snemo/datamodels/polyline_trajectory_pattern.h>
#include "falaise/snemo/datamodels/particle_track_data.h"

#include "tkrec/TKEvent.h"

namespace tkrec {

  class TKReconstruct
    : public dpp::base_module
  {
  public:

    static const double invalid_distance;

    /// \brief Configuration parameters
    struct config_type
    {
      datatools::logger::priority verbosity = datatools::logger::PRIO_FATAL;
      double trkhit_default_sigma_r = 2.0; // mm
      double chi_square_threshold = 5.0;
      TKEventRecConfig recConfig;
    };
    
    ////////////////////////////////////////////////
    // The following PUBLIC methods MUST be defined!
    // Default constructor
    TKReconstruct();

    // Default destructor
    virtual ~TKReconstruct();

    void read_config(const datatools::properties& config_);

    //! Configure the module
    virtual void initialize(const datatools::properties &myConfig,
			    datatools::service_manager &flServices,
			    dpp::module_handle_dict_type &what);

    //! Reset the module
    virtual void reset();

    // Process event
    virtual dpp::base_module::process_status process(datatools::things &workItem);

    void populate_working_event(const datatools::things &workItem);
    
    void fill_TCD_bank(snemo::datamodel::calibrated_data& falaiseCDbank,
		       snemo::datamodel::tracker_clustering_data& the_tracker_clustering_data) const;

    void fill_TTD_bank(snemo::datamodel::tracker_clustering_data& the_tracker_clustering_data,
		       snemo::datamodel::tracker_trajectory_data& the_tracker_trajectory_data) const;
    
    void line_to_verteces(const TKtrack* track, geomtools::line_3d & line_3d);

  private:

    // Configuration parameters:
    config_type _config_;

    // Working event:
    TKEvent _wrkevent_;
    
    DPP_MODULE_REGISTRATION_INTERFACE(TKReconstruct)
    
  };
  
} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKRECONSTRUCT_H
