/// \file falaise/TrackFit/gg_hit.cc

// Ourselves:
#include <TrackFit/gg_hit.h>

// Standard library:
#include <limits>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_draw.h>

namespace TrackFit {

  const std::string & gg_hit::prompt_flag()
  {
    static const std::string flag("prompt");
    return flag;
  }

  const std::string & gg_hit::delayed_flag()
  {
    static const std::string flag("delayed");
    return flag;
  }

  const std::string & gg_hit::last_flag()
  {
    static std::string flag = "last_hit";
    return flag;
  }

  const std::string & gg_hit::first_flag()
  {
    static std::string flag = "first_hit";
    return flag;
  }

  int gg_hit::get_id() const
  {
    return _id_;
  }

  void gg_hit::set_id(int id_)
  {
    _id_ = id_;
    return;
  }

  bool gg_hit::has_phi_ref() const
  {
    return datatools::is_valid(_phi_ref_);
  }

  void gg_hit::reset_phi_ref()
  {
    datatools::invalidate(_phi_ref_);
    return;
  }

  double gg_hit::get_phi_ref() const
  {
    return _phi_ref_;
  }

  void gg_hit::set_phi_ref(double phi_ref_)
  {
    _phi_ref_ = phi_ref_;
    return;
  }

  bool gg_hit::is_last() const
  {
    return _last_;
  }

  bool gg_hit::is_first() const
  {
    return _first_;
  }

  void gg_hit::set_last(bool last_)
  {
    _last_ = last_;
    return;
  }

  void gg_hit::set_first(bool first_)
  {
    _first_ = first_;
    return;
  }

  double gg_hit::get_x() const
  {
    return _x_;
  }

  void gg_hit::set_x(double new_value_)
  {
    _x_ = new_value_;
    return;
  }

  double gg_hit::get_y() const
  {
    return _y_;
  }

  void gg_hit::set_y(double new_value_)
  {
    _y_ = new_value_;
    return;
  }

  double gg_hit::get_z() const
  {
    return _z_;
  }

  void gg_hit::set_z(double new_value_)
  {
    _z_ = new_value_;
    return;
  }

  double gg_hit::get_sigma_z() const
  {
    return _sigma_z_;
  }

  void gg_hit::set_sigma_z(double new_value_)
  {
    _sigma_z_ = new_value_;
    return;
  }

  double gg_hit::get_t() const
  {
    return _t_;
  }

  void gg_hit::set_t(double new_value_)
  {
    _t_ = new_value_;
    return;
  }

  double gg_hit::get_rmax() const
  {
    return _rmax_;
  }

  void gg_hit::set_rmax(double new_value_)
  {
    _rmax_ = new_value_;
    return;
  }

  double gg_hit::get_max_length() const
  {
    return _max_length_;
  }

  void gg_hit::set_max_length(double new_value_)
  {
    _max_length_ = new_value_;
    return;
  }

  double gg_hit::get_r() const
  {
    return _r_;
  }

  void gg_hit::set_r(double new_value_)
  {
    _r_ = new_value_;
    return;
  }

  double gg_hit::get_sigma_r() const
  {
    return _sigma_r_;
  }

  void gg_hit::set_sigma_r(double new_value_)
  {
    _sigma_r_ = new_value_;
    return;
  }

  const datatools::properties & gg_hit::get_properties() const
  {
    return _properties_;
  }

  datatools::properties & gg_hit::grab_properties()
  {
    return _properties_;
  }

  gg_hit::gg_hit()
  {
    _set_defaults();
    return;
  }

  gg_hit::~gg_hit()
  {
    return;
  }

  void gg_hit::reset()
  {
    _set_defaults();
    return;
  }

  void gg_hit::_set_defaults()
  {
    _id_         = -1;
    _x_          = std::numeric_limits<double>::quiet_NaN();
    _y_          = std::numeric_limits<double>::quiet_NaN();
    _z_          = std::numeric_limits<double>::quiet_NaN();
    _sigma_z_    = std::numeric_limits<double>::infinity();
    _r_          = std::numeric_limits<double>::quiet_NaN();
    _sigma_r_    = std::numeric_limits<double>::infinity();
    _t_          = std::numeric_limits<double>::quiet_NaN();
    _rmax_       = std::numeric_limits<double>::quiet_NaN();
    _max_length_ = std::numeric_limits<double>::quiet_NaN();
    _phi_ref_    = std::numeric_limits<double>::quiet_NaN();
    _first_      = false;
    _last_       = false;
    _properties_.clear();
    return;
  }

  void gg_hit::tree_dump(std::ostream      & out_,
                         const std::string & title_,
                         const std::string & indent_,
                         bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) {
      indent = indent_;
    }
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "id         = " << _id_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "x          = " << _x_ / CLHEP::cm << " cm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "y          = " << _y_ / CLHEP::cm << " cm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "z          = " << _z_ / CLHEP::cm << " cm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "sigma_z    = " << _sigma_z_ / CLHEP::cm << " cm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "t          = " << _t_ / CLHEP::ns << " ns" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "rmax       = " << _rmax_ / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "max length = " << _max_length_ / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "r          = " << _r_ / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "sigma_r    = " << _sigma_r_ / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "first      = " << _first_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "last       = " << _last_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Properties : ";
    if (_properties_.empty()) {
      out_ << "<empty>";
    }
    out_ << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::last_skip_tag;
      _properties_.tree_dump(out_, "", indent_oss.str());
    }
    return;
  }

  void gg_hit::dump() const
  {
    tree_dump(std::clog, "TrackFit::gg_hit:");
    return;
  }


  void draw_hits(std::ostream & out_, const gg_hits_col & hits_)
  {
    for (gg_hits_col::const_iterator iter = hits_.begin();
         iter != hits_.end();
         iter++) {
      const gg_hit & the_hit = *iter;
      geomtools::vector_3d pos(the_hit.get_x(),
                               the_hit.get_y(),
                               the_hit.get_z());
      geomtools::rotation rot;
      if (the_hit.get_r() - the_hit.get_sigma_r() > 0.0) {
        geomtools::gnuplot_draw::draw_circle(out_, pos, rot, the_hit.get_r() - the_hit.get_sigma_r());
      }
      geomtools::gnuplot_draw::draw_circle(out_, pos, rot, the_hit.get_r() + the_hit.get_sigma_r());

    }
    return;
  }

} // end of namespace TrackFit
