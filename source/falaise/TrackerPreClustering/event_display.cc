/// \file falaise/TrackerPreClustering/event_display.cc

// Ourselves:
#include <TrackerPreClustering/event_display.h>

// Standard library:
#include <cmath>

// Third party:
// - CLHEP:
#include <CLHEP/Units/SystemOfUnits.h>
// - Boost:
#include <boost/foreach.hpp>

namespace TrackerPreClustering {

  void event_display::set_cell_size(double cell_size_)
  {
    _cell_size_ = cell_size_;
    return;
  }

  void event_display::set_nb_layers(unsigned int nb_layers_)
  {
    _nb_layers_ = nb_layers_;
    return;
  }

  void event_display::set_nb_rows(unsigned int nb_rows_)
  {
    _nb_rows_ = nb_rows_;
    return;
  }

  event_display::event_display()
  {
    _nb_layers_ = 9;
    _nb_rows_   = 113;
    _cell_size_ = 44.0 * CLHEP::mm;
    return;
  }


  void event_display::plot_gg_hit(std::ostream & out_,
                                  int id_,
                                  double r_,
                                  double er_,
                                  double x_,
                                  double y_,
                                  double z_,
                                  double ez_,
                                  double delayed_time_) const
  {
    if (x_ != x_ || y_ != y_) {
      return;
    }
    out_ << "# hit id " << id_;
    if (delayed_time_ == delayed_time_) {
      out_ << "(delayed time=" << delayed_time_/CLHEP::microsecond <<  "us)";
    }
    out_ << std::endl;
    double r = r_;
    double er = er_;
    if (r != r) {
      r = 0.45 * _cell_size_;
      er = 0.1 * CLHEP::mm;
    }
    double x0 = x_;
    double y0 = y_;
    double z0 = z_;
    double ez0 = ez_;
    for (unsigned int j = 0; j <= 36; ++j){
      double theta = 2. * M_PI/36. * j;
      double x = x0 +(r+er) * std::cos(theta);
      double y = y0 +(r+er) * std::sin(theta);
      double z = z0;
      out_ << x << ' ' << y << ' ' << z << std::endl;
    }
    out_ << std::endl;
    for (unsigned int j = 0; j <= 36; ++j) {
      double theta = 2. * M_PI/36. * j;
      double x = x0 +(r-er) * std::cos(theta);
      double y = y0 +(r-er) * std::sin(theta);
      double z = z0;
      out_ << x << ' ' << y << ' ' << z << std::endl;
    }
    out_ << std::endl;
    out_ << x0 << ' ' << y0 << ' ' << z0 + ez0 << std::endl;
    out_ << x0 << ' ' << y0 << ' ' << z0 - ez0 << std::endl;
    out_ << std::endl;
    return;
  }


  void event_display::plot_clustered_item(std::ostream & out_,
                                          int /* id_ */,
                                          double x_,
                                          double y_,
                                          double z_,
                                          double ez_,
                                          double /* delayed_time_ */) const
  {
    static int count = 0;
    double s = 0.5 * _cell_size_;
    double x0 = x_;
    double y0 = y_;
    double z0 = z_;
    double dz0 = ez_;
    out_ << x0 + s  << ' ' << y0 + s << ' ' << z0 << std::endl;
    if (count % 2) {
      // gnuplot 3D-plot trick
      out_ << x0 + s  << ' ' << y0   << ' ' << z0+dz0 << std::endl;
    }
    out_ << x0 + s  << ' ' << y0 - s << ' ' << z0+dz0 << std::endl;
    out_ << x0 - s  << ' ' << y0 - s << ' ' << z0+dz0 << std::endl;
    out_ << x0 - s  << ' ' << y0 + s << ' ' << z0+dz0 << std::endl;
    out_ << x0 + s  << ' ' << y0 + s << ' ' << z0+dz0 << std::endl;
    out_ << std::endl;
    out_ << x0 + s  << ' ' << y0 - s << ' ' << z0-dz0 << std::endl;
    out_ << x0 - s  << ' ' << y0 - s << ' ' << z0-dz0 << std::endl;
    out_ << x0 - s  << ' ' << y0 + s << ' ' << z0-dz0 << std::endl;
    out_ << x0 + s  << ' ' << y0 + s << ' ' << z0-dz0 << std::endl;
    out_ << std::endl;
    out_ << x0 + s  << ' ' << y0 - s << ' ' << z0+dz0 << std::endl;
    out_ << x0 + s  << ' ' << y0 - s << ' ' << z0-dz0 << std::endl;
    out_ << std::endl;
    out_ << x0 - s  << ' ' << y0 - s << ' ' << z0+dz0 << std::endl;
    out_ << x0 - s  << ' ' << y0 - s << ' ' << z0-dz0 << std::endl;
    out_ << std::endl;
    out_ << x0 - s  << ' ' << y0 + s << ' ' << z0+dz0 << std::endl;
    out_ << x0 - s  << ' ' << y0 + s << ' ' << z0-dz0 << std::endl;
    out_ << std::endl;
    out_ << x0 + s  << ' ' << y0 + s << ' ' << z0+dz0 << std::endl;
    out_ << x0 + s  << ' ' << y0 + s << ' ' << z0-dz0 << std::endl;
    out_ << std::endl;
    return;
  }

} // end of namespace TrackerPreClustering
