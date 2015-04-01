// snemo/digitization/calo_tp_to_ctw_algo.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_TO_CTW_ALGO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_TO_CTW_ALGO_H

// Standard library :
#include <vector>

// This project :
#include <snemo/digitization/calo_tp_data.h>
#include <snemo/digitization/calo_ctw_data.h>

// Third party:
// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Algorithm processing. Take a calo_tp_data object (collection of calo_tp) in input used to fill a calo_ctw_data (collection of calo crate trigger word) in output.
    class calo_tp_to_ctw_algo
    {
    public : 
			/// Shift for board index because there is 10FEB/1CB/10FEB in one crate
      static const unsigned int BOARD_SHIFT_INDEX = -1;

      /// Default constructor
      calo_tp_to_ctw_algo();

      /// Destructor
      virtual ~calo_tp_to_ctw_algo();

			/// Check if the algorithm is initialized 
			bool is_initialized() const;

			/// Initializing
      void initialize(); 

			/// Reset the object
      void reset(); 

			/// Set the clocktick for one ctw
			void set_ctw_clocktick(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_);

			/// Set the ctw high threshold multiplicity for a given clocktick
			void set_ctw_htm(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_);

			/// Set the ctw zone bit for a given clocktick 
      void set_ctw_zone_bit(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_);

			/// Set the ctw low threshold only for a given clocktick 
			void set_ctw_lto(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_);

			/// General proces to fill a ctw data object from a list of calo tp
			void process(const calo_tp_data & tp_data_,  calo_ctw_data & ctw_data_);

		protected :

			/// Fill a calo ctw from a calo tp
			void _fill_a_calo_ctw(const calo_tp & my_calo_tp_, calo_ctw & a_calo_ctw_);

			/// Process to fill a ctw from a list of calo tp for a clocktick
	    void _process_for_a_ctw_for_a_clocktick(const std::vector<datatools::handle<calo_tp> > & my_list_of_calo_tp_,  calo_ctw & a_calo_ctw_);
			
    private :
      
			bool _initialized_; //!< Initialization flag
     
    };

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_TO_CTW_ALGO_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
