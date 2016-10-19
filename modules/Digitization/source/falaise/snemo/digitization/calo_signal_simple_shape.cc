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
      _polarity_ = mctools::signal::POLARITY_INVALID;
      datatools::invalidate(_t0_);
      datatools::invalidate(_t1_);
      datatools::invalidate(_amplitude_);
      datatools::invalidate(_param_0_);
      datatools::invalidate(_param_1_);
      datatools::invalidate(_param_2_);
      datatools::invalidate(_param_3_);
      datatools::invalidate(_param_4_);
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

    bool calo_signal_simple_shape::is_valid() const
    {
      return datatools::is_valid(_t0_)
        && datatools::is_valid(_t1_)
        && datatools::is_valid(_amplitude_)
        && polarity_is_set(_polarity_);
    }

    void calo_signal_simple_shape::reset()
    {
      reset_polarity();
      _set_defaults();
      this->i_unary_function::reset();
      return;
    }

    void calo_signal_simple_shape::reset_polarity()
    {
      _polarity_ = mctools::signal::POLARITY_INVALID;
      return;
    }
    
    bool calo_signal_simple_shape::is_initialized() const
    {
      return this->i_unary_function::is_initialized()
        && is_valid();
    }

    void calo_signal_simple_shape::set_polarity(mctools::signal::polarity_type p_)
    {
      DT_THROW_IF(p_ == mctools::signal::POLARITY_UNKNOWN, std::logic_error, "Invalid polarity value!");
      _polarity_ = p_;
      return;
    }
  
    mctools::signal::polarity_type calo_signal_simple_shape::get_polarity() const
    {
      return _polarity_;
    }

    void calo_signal_simple_shape::initialize(const datatools::properties & config_,
					      mygsl::unary_function_dict_type & functors_)
    {
      this->mygsl::i_unary_function::_base_initialize(config_, functors_);
    }

    double calo_signal_simple_shape::_eval(double x_) const
    {
      double res = 0.0;
      res = _param_0_ * exp((_param_1_ - x_)/_param_2_) * exp(-exp(_param_1_ - x_)/(_param_2_)) * exp (std::pow(_param_3_ - x_,2)/_param_4_);
	/*
      if (_polarity_ == mctools::signal::POLARITY_NEGATIVE) {
        res *= -1.0;
	}*/
      return res;
    }
    

  } // end of namespace digitization

} // end of namespace snemo
