// -*- mode: c++ ; -*-
/** \file falaise/snemo/datamodels/calibrated_tracker_hit.cc
 */

// Ourselves
#include <falaise/snemo/datamodels/calibrated_tracker_hit.h>

// Third party
// - Bayeux/datatools
#include <datatools/clhep_units.h>
#include <datatools/utils.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::serialization::i_serializable interface :
    // DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_tracker_hit, "snemo::core::model::calibrated_tracker_hit")
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calibrated_tracker_hit, "snemo::datamodel::calibrated_tracker_hit")

    const std::string calibrated_tracker_hit::ANODE_TIME_KEY          = "anode_time";
    const std::string calibrated_tracker_hit::BOTTOM_CATHODE_TIME_KEY = "bottom_cathode_time";
    const std::string calibrated_tracker_hit::TOP_CATHODE_TIME_KEY    = "top_cathode_time";

    bool calibrated_tracker_hit::_get_trait_bit (uint32_t mask_) const
    {
      return _traits_ & mask_;
    }

    void calibrated_tracker_hit::_set_trait_bit (bool value_, uint32_t mask_)
    {
      if (value_) _traits_ |= mask_;
      else _traits_ &= ~mask_;
      return;
    }

    bool calibrated_tracker_hit::is_delayed () const
    {
      return _get_trait_bit (delayed);
    }

    bool calibrated_tracker_hit::is_prompt () const
    {
      return ! is_delayed ();
    }

    bool calibrated_tracker_hit::is_peripheral () const
    {
      return _get_trait_bit (peripheral);
    }

    bool calibrated_tracker_hit::is_noisy () const
    {
      return _get_trait_bit (noisy);
    }

    bool calibrated_tracker_hit::is_sterile () const
    {
      return _get_trait_bit (sterile);
    }

    bool calibrated_tracker_hit::is_fake () const
    {
      return _get_trait_bit (fake);
    }

    void calibrated_tracker_hit::set_fake (bool fake_)
    {
      _set_trait_bit (fake_, fake);
      return;
    }

    void calibrated_tracker_hit::set_sterile (bool sterile_)
    {
      _set_trait_bit (sterile_, sterile);
      return;
    }

    void calibrated_tracker_hit::set_delayed (bool delayed_)
    {
      _set_trait_bit (delayed_, delayed);
      if (! delayed_) {
        datatools::invalidate(_delayed_time_);
        datatools::invalidate(_delayed_time_error_);
      }
      return;
    }

    void calibrated_tracker_hit::set_delayed_time (double delayed_time_,
                                                   double delayed_time_error_)
    {
      set_delayed (true);
      _delayed_time_ = delayed_time_;
      _delayed_time_error_ = delayed_time_error_;
      return;
    }

    bool calibrated_tracker_hit::has_delayed_time () const
    {
      return datatools::is_valid (_delayed_time_);
    }

    /// Return the delayed reference time of the hit
    double calibrated_tracker_hit::get_delayed_time () const
    {
      return _delayed_time_;
    }

    bool calibrated_tracker_hit::has_delayed_time_error () const
    {
      return datatools::is_valid (_delayed_time_error_);
    }

    /// Return the delayed reference time of the hit
    double calibrated_tracker_hit::get_delayed_time_error () const
    {
      return _delayed_time_error_;
    }

    void calibrated_tracker_hit::set_bottom_cathode_missing (bool missing_)
    {
      _set_trait_bit (missing_, missing_bottom_cathode);
      return;
    }

    void calibrated_tracker_hit::set_top_cathode_missing (bool missing_)
    {
      _set_trait_bit (missing_, missing_top_cathode);
      return;
    }

    void calibrated_tracker_hit::set_noisy (bool noisy_)
    {
      _set_trait_bit (noisy_, noisy);
      return;
    }

    void calibrated_tracker_hit::set_peripheral (bool peripheral_)
    {
      _set_trait_bit (peripheral_, peripheral);
      return;
    }

    bool calibrated_tracker_hit::has_anode_time () const
    {
      bool ok = false;
      if (get_auxiliaries ().has_key (ANODE_TIME_KEY))
        {
          if (get_auxiliaries ().is_real (ANODE_TIME_KEY))
            {
              ok = true;
            }
        }
      return ok;
    }

    void calibrated_tracker_hit::set_anode_time (double anode_time_)
    {
      grab_auxiliaries ().update (ANODE_TIME_KEY, anode_time_);
      return;
    }

    double calibrated_tracker_hit::get_anode_time () const
    {
      double anode_time;
      datatools::invalidate (anode_time);
      if (get_auxiliaries ().has_key ("anode_time"))
        {
          if (get_auxiliaries ().is_real ("anode_time"))
            {
              anode_time = get_auxiliaries ().fetch_real ("anode_time");
            }
        }
      return anode_time;
    }

    bool calibrated_tracker_hit::has_xy () const
    {
      return _get_trait_bit (xy);
    }

    void calibrated_tracker_hit::set_xy (double x_, double y_)
    {
      _set_trait_bit (true, xy);
      _set_x (x_);
      _set_y (y_);
      return;
    }

    double calibrated_tracker_hit::get_x () const
    {
      return _x_;
    }

    void calibrated_tracker_hit::_set_x (double x_)
    {
      _x_ = x_;
      return;
    }

    double calibrated_tracker_hit::get_y () const
    {
      return _y_;
    }

    void calibrated_tracker_hit::_set_y (double y_)
    {
      _y_ = y_;
      return;
    }

    double calibrated_tracker_hit::get_z () const
    {
      return _z_;
    }

    void calibrated_tracker_hit::set_z (double z_)
    {
      _z_ = z_;
      return;
    }

    double calibrated_tracker_hit::get_sigma_z () const
    {
      return _sigma_z_;
    }

    void calibrated_tracker_hit::set_sigma_z (double sigma_z_)
    {
      _sigma_z_ = sigma_z_;
      return;
    }

    double calibrated_tracker_hit::get_r () const
    {
      return _r_;
    }

    void calibrated_tracker_hit::set_r (double r_)
    {
      _r_ = r_;
      return;
    }

    double calibrated_tracker_hit::get_sigma_r () const
    {
      return _sigma_r_;
    }

    void calibrated_tracker_hit::set_sigma_r (double sigma_r_)
    {
      _sigma_r_ = sigma_r_;
      return;
    }

    void calibrated_tracker_hit::invalidate_xy ()
    {
      datatools::invalidate(_x_);
      datatools::invalidate(_y_);
      _set_trait_bit (true, xy);
      return;
    }

    void calibrated_tracker_hit::invalidate_positions ()
    {
      datatools::invalidate(_r_);
      datatools::invalidate(_sigma_r_);
      datatools::invalidate(_z_);
      datatools::invalidate(_sigma_z_);
      return;
    }

    // ctor:
    calibrated_tracker_hit::calibrated_tracker_hit () : base_hit ()
    {
      _traits_  = 0x0;
      datatools::invalidate(_r_);
      datatools::invalidate(_sigma_r_);
      datatools::invalidate(_z_);
      datatools::invalidate(_sigma_z_);
      datatools::invalidate(_x_);
      datatools::invalidate(_y_);
      datatools::invalidate(_delayed_time_);
      datatools::invalidate(_delayed_time_error_);
      return;
    }

    // dtor:
    calibrated_tracker_hit::~calibrated_tracker_hit ()
    {
      return;
    }

    bool calibrated_tracker_hit::is_valid () const
    {
      return this->base_hit::is_valid () && std::isnormal (_z_) && std::isnormal (_r_);
    }

    void calibrated_tracker_hit::invalidate ()
    {
      this->base_hit::invalidate ();
      invalidate_positions ();
      invalidate_xy ();
      datatools::invalidate(_delayed_time_);
      datatools::invalidate(_delayed_time_error_);
      _traits_ = 0x0;
      return;
    }

    void calibrated_tracker_hit::clear ()
    {
      calibrated_tracker_hit::invalidate ();
      return;
    }

    bool calibrated_tracker_hit::is_bottom_cathode_missing () const
    {
      return _get_trait_bit (missing_bottom_cathode);
    }

    bool calibrated_tracker_hit::is_top_cathode_missing () const
    {
      return _get_trait_bit (missing_top_cathode);
    }

    bool calibrated_tracker_hit::are_both_cathodes_missing () const
    {
      return is_bottom_cathode_missing () && is_top_cathode_missing ();
    }

    void calibrated_tracker_hit::tree_dump (std::ostream & out_,
                                            const std::string & title_,
                                            const std::string & indent_,
                                            bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      base_hit::tree_dump (out_, title_, indent_, true);

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Traits        : " << _traits_ << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Delayed        : " << is_delayed () << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Noisy        : " << is_noisy () << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Peripheral        : " << is_peripheral () << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Bottom cathode missing : " << is_bottom_cathode_missing () << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Top cathode missing : " << is_top_cathode_missing () << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Sterile        : " << is_sterile () << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Fake        : " << is_fake () << std::endl;

      if (has_xy ())
        {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "x        : " << _x_ / CLHEP::mm << " mm" << std::endl;

          out_ << indent << datatools::i_tree_dumpable::tag
               << "y        : " << _y_ / CLHEP::mm << " mm" << std::endl;
        }

      if (is_delayed ())
        {
          out_ << indent << datatools::i_tree_dumpable::tag
               << "delayed_time       : " << _delayed_time_ / CLHEP::microsecond << " us" << std::endl;

          out_ << indent << datatools::i_tree_dumpable::tag
               << "delayed_time_error : " << _delayed_time_error_ / CLHEP::microsecond << " us" << std::endl;
        }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "r        : " << _r_ / CLHEP::mm << " mm" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "sigma(r) : " << _sigma_r_ / CLHEP::mm << " mm" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::tag
           << "z        : " << _z_ / CLHEP::cm << " cm" << std::endl;

      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "sigma(z) : " << _sigma_z_ / CLHEP::cm << " cm" << std::endl;

      return;
    }

    void calibrated_tracker_hit::dump () const
    {
      tree_dump (std::clog, "snemo::core::model::calibrated_tracker_hit");
      return;
    }

    int32_t calibrated_tracker_hit::get_id () const
    {
      return get_hit_id ();
    }

    int32_t calibrated_tracker_hit::get_module () const
    {
      return get_geom_id ().get (0);
    }

    int32_t calibrated_tracker_hit::get_side () const
    {
      return get_geom_id ().get (1);
    }

    int32_t calibrated_tracker_hit::get_layer () const
    {
      return get_geom_id ().get (2);
    }

    int32_t calibrated_tracker_hit::get_row () const
    {
      return get_geom_id ().get (3);
    }

    bool compare_tracker_hit_by_delayed_time::operator ()
      (const calibrated_tracker_hit & hit_i_,
       const calibrated_tracker_hit & hit_j_) const
    {
      double dti = 0.0;
      if (hit_i_.is_delayed ())
        {
          dti = hit_i_.get_delayed_time ();
        }
      double dtj = 0.0;
      if (hit_j_.is_delayed ())
        {
          dtj = hit_j_.get_delayed_time ();
        }
      return (dti < dtj);
    }

  } // end of namespace datamodel

} // end of namespace snemo

// end of falaise/snemo/datamodels/calibrated_tracker_hit.cc
