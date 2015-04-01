// snemo/digitization/sd_to_geiger_signal_algo.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SD_TO_GEIGER_SIGNAL_ALGO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SD_TO_GEIGER_SIGNAL_ALGO_H

// Standard library :
#include <stdexcept>

// Third party:
// - Bayeux/datatools :
#include <datatools/handle.h>
#include <datatools/logger.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

// This project :
#include <snemo/digitization/geiger_signal.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Algorithm processing. Take simulated datas and fill geiger signal objects.
    class sd_to_geiger_signal_algo : boost::noncopyable
    {
    public :

      /// Default constructor
      sd_to_geiger_signal_algo();

      /// Destructor
      virtual ~sd_to_geiger_signal_algo();
      
      /// Initializing
      void initialize();

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 

      int process(const mctools::simulated_data & sd_);

    protected: 
      
      ///  Process to fill a ctw data object from simulated data

      int _process(const mctools::simulated_data & sd_);

    private :
      
      bool _initialized_; //!< Initialization flag


    };

  } // end of namespace digitization

} // end of namespace snemo


#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SD_TO_GEIGER_SIGNAL_ALGO_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
