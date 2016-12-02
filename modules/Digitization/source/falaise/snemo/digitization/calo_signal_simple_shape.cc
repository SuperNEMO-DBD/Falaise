// snemo/digitization/calo_signal_simple_shape.cc
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_signal_simple_shape.h>

namespace snemo {
  
  namespace digitization {
    
    MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(calo_signal_simple_shape,
                                               "snemo::digitization::calo_signal_simple_shape")

    DATATOOLS_CLONEABLE_IMPLEMENTATION(calo_signal_simple_shape)

    void calo_signal_simple_shape::_set_defaults()
    {
      datatools::invalidate(_t0_);
      datatools::invalidate(_amplitude_);
      return;
    }

    calo_signal_simple_shape::calo_signal_simple_shape()
    {
      _set_defaults();
      return;
    }

    calo_signal_simple_shape::~calo_signal_simple_shape()
    {
      reset();
      return;
    }

    void calo_signal_simple_shape::reset()
    {
      _set_defaults();
      this->i_unary_function::reset();
      return;
    }
    
    bool calo_signal_simple_shape::is_initialized() const
    {
      return this->i_unary_function::is_initialized();
    }

    void calo_signal_simple_shape::initialize(const datatools::properties & config_,
					      mygsl::unary_function_dict_type & functors_)
    {
      this->mygsl::i_unary_function::_base_initialize(config_, functors_);
    }

  } // end of namespace digitization

} // end of namespace snemo
