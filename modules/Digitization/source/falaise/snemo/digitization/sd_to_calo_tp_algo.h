// sd_to_calo_tp_algo.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SD_TO_CALO_TP_ALGO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SD_TO_CALO_TP_ALGO_H

// Standard library :
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>

// - Bayeux/datatools :
#include <datatools/handle.h>
#include <datatools/logger.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// This project :
#include <snemo/digitization/calo_tp_data.h>
#include <snemo/digitization/ID_convertor.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Algorithm processing. Take simulated datas and fill calo trigger primitive data object.
    class sd_to_calo_tp_algo : boost::noncopyable
    {
    public :

      /// Default constructor
      sd_to_calo_tp_algo();

      /// Destructor
      virtual ~sd_to_calo_tp_algo();
      
      /// Initializing
      void initialize(int32_t & clocktick_ref_,
											const ID_convertor & my_ID_convertor_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 

      /// Proces to fill a ctw data object from simulated data
      int process(const mctools::simulated_data & sd_,
									calo_tp_data & my_calo_tp_data_);

    protected: 
      
			// unsigned int _existing_same_electronic_id(const geomtools::geom_id & electronic_id_,
			// 																					calo_tp_data & my_calo_tp_data_);

			int _process(const mctools::simulated_data & sd_,
									 calo_tp_data & my_calo_tp_data_);

    private :
      
      bool _initialized_; //!< Initialization flag
      bool _active_main_wall_; //!< Main wall activation flag
      bool _active_xwall_; //!< X-wall activation flag
      bool _active_gveto_; //!< Gamma-veto activation flag

			int32_t _clocktick_ref_; //!< Clocktick reference of the algorithm
			const ID_convertor * _ID_convertor_; //!< Convert geometric ID into electronic ID

    };

  } // end of namespace digitization

} // end of namespace snemo


#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SD_TO_CALO_TP_ALGO_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
