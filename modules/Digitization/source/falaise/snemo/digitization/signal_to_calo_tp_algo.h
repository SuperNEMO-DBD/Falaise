// snemo/digitization/signal_to_calo_tp_algo.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SIGNAL_TO_CALO_TP_ALGO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SIGNAL_TO_CALO_TP_ALGO_H

// Standard library :
#include <stdexcept>

// Third party:
// - Bayeux/datatools :
#include <datatools/logger.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// This project :
#include <snemo/digitization/calo_tp_data.h>
#include <snemo/digitization/signal_data.h>
#include <snemo/digitization/electronic_mapping.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Algorithm processing. Take simulated datas and fill calo trigger primitive data object.
    class signal_to_calo_tp_algo : boost::noncopyable
    {
    public :

      /// Default constructor
      signal_to_calo_tp_algo();

      /// Destructor
      virtual ~signal_to_calo_tp_algo();
      
      /// Initializing
      void initialize(electronic_mapping & _electronic_mapping_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 
			
		  /// Set the clocktick reference for the algorithm
			void set_clocktick_reference(int32_t clocktick_ref_);
			
		  /// Set the clocktick shift
			void set_clocktick_shift(double clocktick_shift_);

      /// Process to fill a calo tp data object from simulated data
			void process(const signal_data & signal_data_,
									 calo_tp_data & my_calo_tp_data_);

    protected:
      
			// unsigned int _existing_same_electronic_id(const geomtools::geom_id & electronic_id_,
			// 																					calo_tp_data & my_calo_tp_data_);

			///  Process to fill a calo tp data object from simulated data
			void _process(const signal_data & signal_data_,
										calo_tp_data & my_calo_tp_data_);

    private :
      
      bool _initialized_; //!< Initialization flag
      bool _active_main_wall_; //!< Main wall activation flag
      bool _active_xwall_; //!< X-wall activation flag
      bool _active_gveto_; //!< Gamma-veto activation flag

			int32_t _clocktick_ref_;   //!< Clocktick reference of the algorithm
			double  _clocktick_shift_; //!< Clocktick shift between [0:25]
			electronic_mapping * _electronic_mapping_; //!< Convert geometric ID into electronic ID
    };

  } // end of namespace digitization

} // end of namespace snemo


#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SIGNAL_TO_CALO_TP_ALGO_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
