// snemo/digitization/calo_signal_simple_shape.h
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_SIGNAL_SIMPLE_SHAPE_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_SIGNAL_SIMPLE_SHAPE_H
// Standard library :

// Third party:
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>
#include <mygsl/tabulated_function.h>
// - Bayeux/mctools :
#include <mctools/signal/utils.h>
// - Bayeux/datatools :
#include <datatools/clhep_units.h>
#include <datatools/i_cloneable.h>

namespace snemo {
  
  namespace digitization {

    class calo_signal_simple_shape : public mygsl::tabulated_function
		{
    public : 

      //! Default constructor
      calo_signal_simple_shape();

      //! Destructor
      virtual ~calo_signal_simple_shape();

      //! Initialization
      virtual void initialize(const datatools::properties & config_,
                              mygsl::unary_function_dict_type & functors_);

      //! Reset
      virtual void reset();

      //! Check initialization status
      virtual bool is_initialized() const;
			
    protected:
      
      //! Set default attributes
      void _set_defaults();

    private :

      // Configuration:
      double _t0_;         //!< Start time
      double _amplitude_;  //!< Amplitude of the signal

      // Working data:

      //! Registration of the functor class
      MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(calo_signal_simple_shape)

      //! Cloneable interface
      DATATOOLS_CLONEABLE_DECLARATION(calo_signal_simple_shape)

    };
  

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_SIGNAL_SIMPLE_SHAPE_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
