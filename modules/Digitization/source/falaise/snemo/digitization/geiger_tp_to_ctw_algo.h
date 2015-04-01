// snemo/digitization/geiger_tp_to_ctw_algo.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_TO_CTW_ALGO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_TO_CTW_ALGO_H

// Standard library :
#include <vector>

// This project :
#include <snemo/digitization/geiger_tp_data.h>
#include <snemo/digitization/geiger_ctw_data.h>

// Third party:
// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Algorithm processing. Take a geiger_tp_data object (collection of geiger_tp) in input used to fill a geiger_ctw_data (collection of geiger crate trigger word) in output.
    class geiger_tp_to_ctw_algo
    {
    public : 
      /// Shift for board index because there is 10FEB/1CB/10FEB in one crate
      static const unsigned int BOARD_SHIFT_INDEX = -1;

      /// Default constructor
      geiger_tp_to_ctw_algo();

      /// Destructor
      virtual ~geiger_tp_to_ctw_algo();

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Initializing
      void initialize(); 

      /// Reset the object
      void reset(); 

      /// Return the position of the 100 bitset word in the 2000 bitset word
      unsigned int board_to_index_mapping(uint32_t board_id_);

      // /// Set the clocktick for one ctw
      // void set_ctw_clocktick(int32_t a_clocktick_, geiger_ctw & a_geiger_ctw_);

      /// General proces to fill a ctw data object from a list of geiger tp
      void process(const geiger_tp_data & tp_data_,  geiger_ctw_data & ctw_data_);

    protected :

      /// Fill a geiger ctw from a geiger tp
      void _fill_a_geiger_ctw(const geiger_tp & my_geiger_tp_, geiger_ctw & a_geiger_ctw_);
      
      /// Process to fill a ctw from a list of geiger tp for a clocktick
      void _process_for_a_ctw_for_a_clocktick(const std::vector<datatools::handle<geiger_tp> > & my_list_of_geiger_tp_,  geiger_ctw & a_geiger_ctw_);
			
    private :
      
      bool _initialized_; //!< Initialization flag
    };

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_TO_CTW_ALGO_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
