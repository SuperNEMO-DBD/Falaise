// snemo/digitization/sd_to_calo_signal_algo.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SD_TO_CALO_SIGNAL_ALGO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SD_TO_CALO_SIGNAL_ALGO_H

// Standard library :
#include <stdexcept>

// Third party:
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/manager.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

// This project :
#include <snemo/digitization/signal_data.h>

namespace snemo {

namespace digitization {

/// \brief Algorithm processing. Take simulated datas and fill calo signal objects.
class sd_to_calo_signal_algo : boost::noncopyable {
 public:
  /// Default constructor
  sd_to_calo_signal_algo();

  // Constructor by initialisation
  sd_to_calo_signal_algo(const geomtools::manager& mgr_);

  /// Destructor
  virtual ~sd_to_calo_signal_algo();

  /// Initializing
  void initialize();

  /// Check if the algorithm is initialized
  bool is_initialized() const;

  /// Reset the object
  void reset();

  /// Set the geometry manager
  void set_geo_manager(const geomtools::manager& mgr_);

  /// General process
  void process(const mctools::simulated_data& sd_, signal_data& signal_data_);

 protected:
  /// Convert energy to amplitude (1 MeV <=> 300 mV). Approximation for the moment
  double _convert_energy_to_amplitude(const double energy_);

  ///  Process to fill a signal data object from simulated data
  int _process(const mctools::simulated_data& sd_, signal_data& signal_data_);

 private:
  bool _initialized_;                       //!< Initialization flag
  const geomtools::manager* _geo_manager_;  //!< Geometry manager
};

}  // end of namespace digitization

}  // end of namespace snemo

#endif  // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SD_TO_CALO_SIGNAL_ALGO_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
