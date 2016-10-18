// snemo/digitization/geiger_signal.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_SIGNAL_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_SIGNAL_H

// Standard library :
#include <bitset>

// Third party:
// - Bayeux/geomtools :
#include <bayeux/geomtools/base_hit.h>

namespace snemo {
  
  namespace digitization {

    /// \brief The geiger tracker signal
    class geiger_signal : public geomtools::base_hit
    {
    public : 

      /// Default constructor
      geiger_signal();

      /// Destructor
      virtual ~geiger_signal();
      
			/// Set the header with valid values
			void set_header(int32_t geiger_signal_hit_id_,
											const geomtools::geom_id & electronic_id_);

			/// Set the data with values
			void set_data(const double & anode_avalanche_time_);
			
      /// Return the const time of avalanche on the anode
      double get_anode_avalanche_time() const;
     
      // Set the time of avalanche on the anode
      void set_anode_avalanche_time(const double & anode_avalanche_time_);

      /// Return the const time of plasma on top cathode
      double get_plasma_top_time_() const;

      /// Set the time plasma on top cathode
      void set_plasma_top_time_(const double & plasma_top_time_);

      /// Return the const time of plasma on bottom cathode
      double get_plasma_bottom_time_() const;

      /// Set the time plasma on bottom cathode 
			void set_plasma_bottom_time_(const double & plasma_bottom_time_);

      /// Check if the internal data of geiger signal is valid
      bool is_valid() const;

      /// Reset the internal data of geiger signal
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
														 const std::string & a_title  = "",
														 const std::string & a_indent = "",
														 bool a_inherit               = false) const;

    private : 

			double _anode_avalanche_time_; //!< Time of the anode avalanche
			double _plasma_top_time_; //!< Time of plasma on top cathode
      double _plasma_bottom_time_; //!< Time of plasma on bottom cathode

			// DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_SIGNAL_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
