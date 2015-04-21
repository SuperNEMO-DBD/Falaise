// snemo/digitization/trigger_algorithm.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_ALGORITHM_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_ALGORITHM_H

// This project :
#include <snemo/digitization/geiger_ctw_data.h>
#include <snemo/digitization/calo_ctw_data.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Trigger algorithm general process
    class trigger_algorithm
    {
    public : 

      /// Default constructor
      trigger_algorithm();

      /// Destructor
      virtual ~trigger_algorithm();

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Initializing
      void initialize(); 

      /// Reset the object
      void reset(); 

      /// General process
      void process();

    protected :
			
    private :
      
      bool _initialized_; //!< Initialization flag
      bool _geiger_matrix_[2][9][113]; //!< Geiger cells matrix [side][layer][row]
    };

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRIGGER_ALGORITHM_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
