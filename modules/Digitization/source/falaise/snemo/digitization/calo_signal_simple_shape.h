// snemo/digitization/calo_signal_simple_shape.h
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_SIGNAL_SIMPLE_SHAPE_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_SIGNAL_SIMPLE_SHAPE_H
// Standard library :

// Third party:
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>
// - Bayeux/mctools :
#include <mctools/signal/utils.h>
// - Bayeux/datatools :
#include <datatools/clhep_units.h>
#include <datatools/i_cloneable.h>

namespace snemo {
  
  namespace digitization {

    class calo_signal_simple_shape : public mygsl::i_unary_function,
				     public datatools::i_cloneable
    {
    public : 

      //! Check the validity
      bool is_valid() const;

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

      //! Set the polarity of the signal
      void set_polarity(mctools::signal::polarity_type);

      //! Return the polarity of the signal
      mctools::signal::polarity_type get_polarity() const;

      //! Reset the polarity of the signal
      void reset_polarity();

      //! Evaluation from parameters
      double _eval(double x_) const;

    protected:

      
      //! Set default attributes
      void _set_defaults();

    private :

      // Configuration:
      mctools::signal::polarity_type _polarity_ = mctools::signal::POLARITY_UNKNOWN; //!< Polarity of the signal
      double _t0_;         //!< Start time
      double _t1_;         //!< Stop time
      double _amplitude_;  //!< Amplitude of the signal
      double _param_0_ = -0.004047;
      double _param_1_ = 48.67;
      double _param_2_ = 3.754;
      double _param_3_ = -24.31;
      double _param_4_ = 1048;

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
