// calo_tapered_scin_box_model.cc

// Ourselves:
#include <falaise/bipo3/geometry/calo_tapered_scin_box_model.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iomanip>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
// - Bayeux/geomtools:
#include <geomtools/polyline_3d.h>
#include <geomtools/gnuplot_draw.h>

namespace snemo {

  namespace geometry {

    // Registration :
    GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(calo_tapered_scin_box_model,
                                           "snemo::geometry::calo_tapered_scin_box_model");

    std::string calo_tapered_scin_box_model::get_model_id () const
    {
      return "snemo::geometry::calo_tapered_scin_box_model";
    }

    const geomtools::intersection_3d & calo_tapered_scin_box_model::get_solid () const
    {
      return _solid_;
    }

    calo_tapered_scin_box_model::calo_tapered_scin_box_model () :
      geomtools::i_model ("calo_tapered_scin_box_model")
    {
      _material_ = "";
      _optical_glue_thickness_ = 1.0 * CLHEP::mm;
      datatools::invalidate (_w_);
      datatools::invalidate (_x_);
      datatools::invalidate (_y_);
      datatools::invalidate (_h_);
      datatools::invalidate (_z_);
      datatools::invalidate (_r_);
      datatools::invalidate (_tapered_r_);
      datatools::invalidate (_tapered_angle_);
      return;
    }

    calo_tapered_scin_box_model::~calo_tapered_scin_box_model ()
    {
      return;
    }

    void calo_tapered_scin_box_model::_at_construct (const std::string & name_,
                                                     const datatools::properties & config_,
                                                     geomtools::models_col_type * /*models_*/)
    {
      if (config_.has_flag("devel")) {
        set_logging_priority(datatools::logger::PRIO_TRACE);
      }
      DT_LOG_TRACE (get_logging_priority (), "Entering...");

      set_name(name_);

      double lunit = CLHEP::mm;
      double aunit = CLHEP::degree;

      /*** parse properties ***/

      if (config_.has_key ("length_unit")) {
        const std::string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
        DT_LOG_TRACE (get_logging_priority (), "length unit = '" << lunit_str << "'");
      }

      if (config_.has_key ("angle_unit")) {
        const std::string aunit_str = config_.fetch_string ("angle_unit");
        aunit = datatools::units::get_angle_unit_from (aunit_str);
        DT_LOG_TRACE (get_logging_priority (), "angle_unit = '" << aunit_str << "'");
      }

      DT_THROW_IF (! config_.has_key ("material.ref"), std::logic_error, "Missing 'material.ref' property !");
      _material_ = config_.fetch_string ("material.ref");
      DT_LOG_TRACE (get_logging_priority (), "material = '" << _material_ << "'");

      DT_THROW_IF (! config_.has_key ("width"), std::logic_error, "Missing 'width' property !");
      double w = config_.fetch_real ("width");
      if (! config_.has_explicit_unit ("width")) {
        w *= lunit;
      }
      DT_LOG_TRACE (get_logging_priority (), "w = '" << w << "'");

      DT_THROW_IF (! config_.has_key ("z"), std::logic_error, "Missing 'z' property !");
      double z = config_.fetch_real ("z");
      if (! config_.has_explicit_unit ("z")) {
        z *= lunit;
      }
      DT_LOG_TRACE (get_logging_priority (), "z = '" << z << "'");

      DT_THROW_IF (! config_.has_key ("h"), std::logic_error, "Missing 'h' property !");
      double h = config_.fetch_real ("h");
      if (! config_.has_explicit_unit ("h")) {
        h *= lunit;
      }
      DT_LOG_TRACE (get_logging_priority (), "h = '" << h << "'");

      DT_THROW_IF (! config_.has_key ("r"), std::logic_error, "Missing 'r' property !");
      double r = config_.fetch_real ("r");
      if (! config_.has_explicit_unit ("r")) {
        r *= lunit;
      }
      DT_LOG_TRACE (get_logging_priority (), "r = '" << r << "'");

      DT_THROW_IF (! config_.has_key ("tapered_r"), std::logic_error, "Missing 'tapered_r' property !");
      double tapered_r = config_.fetch_real ("tapered_r");
      if (! config_.has_explicit_unit ("tapered_r")) {
        r *= lunit;
      }
      DT_LOG_TRACE (get_logging_priority (), "Tapered r = '" << tapered_r << "'");

      DT_THROW_IF (! config_.has_key ("tapered_angle"), std::logic_error, "Missing 'tapered_angle' property !");
      const double tapered_angle = config_.fetch_real ("tapered_angle");
      if (! config_.has_explicit_unit ("tapered_angle")) {
        r *= aunit;
      }
      DT_LOG_TRACE (get_logging_priority (), "Tapered angle = '" << tapered_angle << "'");

      double optical_glue_thickness = 1.0 * lunit;
      if (config_.has_key ("optical_glue_thickness"))
        {
          optical_glue_thickness = config_.fetch_real ("optical_glue_thickness");
          if (! config_.has_explicit_unit ("optical_glue_thickness")) {
            optical_glue_thickness *= lunit;
          }
          DT_LOG_TRACE (get_logging_priority (), "optical glue thickness = '" << optical_glue_thickness << "'");
        }
      else
        {
          //DT_THROW_IF (true, std::logic_error, "Missing 'optical_glue_thickness' property !");
          DT_LOG_TRACE (get_logging_priority (), "No 'optical_glue_thickness' property !");
        }
      DT_THROW_IF (optical_glue_thickness < 0.0,
                   std::underflow_error,
                   "Invalid value '" << optical_glue_thickness << "' (<0) for 'optical_glue_thickness' !");

      DT_LOG_TRACE (get_logging_priority (), "Properties are parsed !");

      _w_ = w;
      _x_ = w;
      _y_ = w;
      _z_ = z;
      _h_ = h;
      _r_ = r;
      _tapered_r_ = tapered_r;
      _tapered_angle_ = tapered_angle;
      _optical_glue_thickness_ = optical_glue_thickness;

      DT_LOG_TRACE (get_logging_priority (), "x=" << _x_);
      DT_LOG_TRACE (get_logging_priority (), "y=" << _y_);
      DT_LOG_TRACE (get_logging_priority (), "z=" << _z_);
      DT_LOG_TRACE (get_logging_priority (), "r=" << _r_);
      DT_LOG_TRACE (get_logging_priority (), "h=" << _h_);
      DT_LOG_TRACE (get_logging_priority (), "tapered_r=" << _tapered_r_);
      DT_LOG_TRACE (get_logging_priority (), "tapered_angle=" << _tapered_angle_);
      DT_LOG_TRACE (get_logging_priority (), "OGT=" << _optical_glue_thickness_);

      _mother_box_.set (_x_, _y_, _z_);
      DT_THROW_IF (! _mother_box_.is_valid (),
                   std::logic_error,
                   "Invalid dimension(s) for the mother scintillator box !");

      _extrusion_.set (_r_);
      DT_THROW_IF (! _extrusion_.is_valid (),
                   std::logic_error,
                   "Invalid dimension(s) for the extrusion sphere !");

      DT_THROW_IF ((_h_ < 0.0) || (_h_ > 2 * _z_),
                   std::range_error,
                   "Invalid extrusion position !");

      const double ze = 0.5 * _z_ - _r_ - _h_;
      const double angle_e = acos (std::abs (ze + 0.5 * _z_) / _r_);
      const double re = _r_ * std::sin (angle_e);
      DT_THROW_IF (re > 0.5 * _w_,
                   std::logic_error,
                   "Radius of extrusion is too large (" << re / CLHEP::mm
                   << " mm) compared to the dimension of the block ("
                   << 0.5 * _w_ / CLHEP::mm << " mm) !");

      const double rt = _tapered_r_;
      DT_THROW_IF (rt < re,
                   std::logic_error,
                   "Tapered radius (" << rt / CLHEP::mm
                   << " mm) is too small (should be >= "
                   << re / CLHEP::mm << " mm) !");

      const double angle_t = _tapered_angle_;
      DT_THROW_IF ((angle_t < 0. * CLHEP::degree) && (angle_t > 90. * CLHEP::degree),
                   std::range_error,
                   "Tapered angle (" << angle_t / CLHEP::degree
                   << " degree) must be within [0;90] degree range !");

      const double lt = re / tan (angle_t);
      const double zt = -0.5 * _z_ - lt;
      DT_LOG_TRACE (get_logging_priority (), "re=" << re);
      DT_LOG_TRACE (get_logging_priority (), "zt=" << zt);
      DT_LOG_TRACE (get_logging_priority (), "rt=" << rt);
      DT_LOG_TRACE (get_logging_priority (), "lt=" << lt);

      // check:
      {
        const double alpha = 45.0 * CLHEP::degree;
        const double x1 = std::sqrt (2) * 0.5 * w * std::cos (alpha);
        const double y1 = std::sqrt (2) * 0.5 * w * std::sin (alpha);
        const double z1 = 0.5 * z;
        const double z2 = hypot (x1, y1) / tan (angle_t) + zt;
        DT_LOG_TRACE (get_logging_priority (), "z1=" << z1 / CLHEP::mm << " mm");
        DT_LOG_TRACE (get_logging_priority (), "z2=" << z2 / CLHEP::mm << " mm");
        DT_THROW_IF (z2 > z1,
                     std::logic_error,
                     "Tapered angle (" <<  _tapered_angle_ / CLHEP::degree
                     << " degree) or tapered radius (" <<  _tapered_r_ / CLHEP::mm
                     << " mm) is too small  !");
      }

      geomtools::placement extrusion_placement (0, 0, ze, 0, 0, 0);
      DT_LOG_TRACE (get_logging_priority (), "ze=" << ze / CLHEP::mm << " mm");
      _solid_0_.set_shapes (_mother_box_, _extrusion_, extrusion_placement);

      // should use some intrinsic skin value:
      const double eps = 0.01 * _z_;
      _removed_cone_.add (zt, 0.0, false);
      _removed_cone_.add (0.5 *_z_ + eps,
                          (0.5 * _z_ - zt) * tan (angle_t), true);

      geomtools::placement cone_placement;
      _solid_.set_shapes (_solid_0_, _removed_cone_, cone_placement);

      _zt_ = zt;
      _ze_ = ze;
      _re_ = re;
      _angle_e_ = angle_e;
      // _solid_.grab_properties ().store ("tapered_r", _tapered_r_);
      // _solid_.grab_properties ().store ("tapered_angle", _tapered_angle_);
      // _solid_.grab_properties ().store ("optical_glue_thickness", _optical_glue_thickness_);

      // Install default 'stackable data' pointer in the shape:
      {
        double x = 0.0, y = 0.0;
        geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
        sd_ptr->xmin = -0.5 * x;
        sd_ptr->xmax = +0.5 * x;
        sd_ptr->ymin = -0.5 * y;
        sd_ptr->ymax = +0.5 * y;
        sd_ptr->zmin = +0.5 * z - h - _optical_glue_thickness_;
        sd_ptr->zmax = +0.5 * z;
        _solid_.set_stackable_data (sd_ptr);
        DT_LOG_TRACE (get_logging_priority (), "Stackable data:");
        if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
          sd_ptr->tree_dump (std::cerr);
        }
      }

      //_solid_.set_user_draw ((void *) &calo_tapered_scin_box_model::gnuplot_draw_user_function);

      // Install a dedicated drawer:
      _drawer_.reset(new wires_drawer(*this));
      _solid_.set_wires_drawer(*_drawer_);
      _solid_.lock();

      grab_logical ().set_name (i_model::make_logical_volume_name (name_));
      grab_logical ().set_shape (_solid_);
      grab_logical ().set_material_ref (_material_);

      DT_LOG_TRACE (get_logging_priority (), "Dumping '" << get_name () << "':");
      if (get_logging_priority () >= datatools::logger::PRIO_TRACE)
        {
          this->tree_dump (std::cerr);
        }
      DT_LOG_TRACE (get_logging_priority (), "Exiting.");
      return;
    }

    /*** dump ***/

    void calo_tapered_scin_box_model::tree_dump (std::ostream & out_,
                                                 const std::string & title_,
                                                 const std::string & indent_,
                                                 bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      i_model::tree_dump (out_, title_, indent, true);

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Material : '" << _material_ << "'" << std::endl;

      {
        // Mother scintillator box:
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Mother box : " << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _mother_box_.tree_dump (out_,"",indent_oss.str ());
        }
      }

      {
        // Spherical extrusion:
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Spherical extrusion : " << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
          _extrusion_.tree_dump (out_,"",indent_oss.str ());
        }
      }

      {
        // Solid:
        out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
             << "Solid : " << std::endl;
        {
          std::ostringstream indent_oss;
          indent_oss << indent;
          indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
          _solid_.tree_dump (out_,"",indent_oss.str ());
        }
      }

      return;
    }

    calo_tapered_scin_box_model::wires_drawer::wires_drawer(const calo_tapered_scin_box_model & model_)
    {
      _model_ = &model_;
      return;
    }

    calo_tapered_scin_box_model::wires_drawer::~wires_drawer()
    {
      return;
    }

    void calo_tapered_scin_box_model::wires_drawer::generate_wires(std::ostream & out_,
                                                                   const geomtools::vector_3d & position_,
                                                                   const geomtools::rotation_3d & rotation_)
    {
      datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
      DT_LOG_TRACE (local_priority, "Entering...");
      const geomtools::intersection_3d & solid = _model_->get_solid();

      const geomtools::i_composite_shape_3d::shape_type & s1 = solid.get_shape1 ();
      const geomtools::i_composite_shape_3d::shape_type & s2 = solid.get_shape2 ();
      const geomtools::i_shape_3d & sh1 = s1.get_shape ();
      const geomtools::i_shape_3d & sh2 = s2.get_shape ();

      // extract useful stuff (shapes and properties):
      const geomtools::subtraction_3d & sub = dynamic_cast<const geomtools::subtraction_3d &> (sh1);
      DT_LOG_TRACE (local_priority, "Sub. shape @ address " << &sub << " is ok !");

      const geomtools::polycone & pol = dynamic_cast<const geomtools::polycone &> (sh2);
      DT_LOG_TRACE (local_priority, "Polycone shape @ address " << &pol << " is ok !");

      const geomtools::i_shape_3d & s3 = sub.get_shape1().get_shape();
      const geomtools::i_shape_3d & s4 = sub.get_shape2().get_shape();
      DT_LOG_TRACE (local_priority, "Dumping '" << s3.get_shape_name () << "':");
      if (local_priority >= datatools::logger::PRIO_TRACE) {
        s3.tree_dump (std::cerr);
      }

      const geomtools::box & mother_box = dynamic_cast<const geomtools::box &> (s3);
      DT_LOG_TRACE (local_priority, "Mother box shape @ address " << &mother_box << " is ok !");

      const geomtools::sphere & extrusion_sphere = dynamic_cast<const geomtools::sphere &> (s4);
      DT_LOG_TRACE (local_priority, "Extruded sphere shape @ address " << &extrusion_sphere << " is ok !");

      const double w       = _model_->_w_;
      const double z       = _model_->_z_;
      const double h       = _model_->_h_;
      const double zt      = _model_->_zt_;
      const double rt      = _model_->_tapered_r_;
      const double angle_t = _model_->_tapered_angle_;
      // const double optical_glue_thickness = _model_->_optical_glue_thickness_;

      double zlim1;
      datatools::invalidate (zlim1);
      double zlim2;
      datatools::invalidate (zlim2);

      const bool draw_scin_block = true;
      if (draw_scin_block) {
        // draw box top face:
        geomtools::placement mother_world_placement;
        mother_world_placement.set_translation (position_);
        mother_world_placement.set_orientation (rotation_);

        geomtools::placement world_item_placement;
        mother_world_placement.child_to_mother (s1.get_placement (),
                                                world_item_placement);
        const geomtools::vector_3d   & sh1_pos = world_item_placement.get_translation ();
        const geomtools::rotation_3d & sh1_rot = world_item_placement.get_rotation ();
        geomtools::polyline_3d top_face_pl;
        top_face_pl.add (geomtools::vector_3d ( 0.5  * w, 0.5 * w, 0.5 * z));
        top_face_pl.add (geomtools::vector_3d (-0.5  * w, 0.5 * w, 0.5 * z));
        top_face_pl.add (geomtools::vector_3d (-0.5  * w, -0.5 * w, 0.5 * z));
        top_face_pl.add (geomtools::vector_3d ( 0.5  * w, -0.5 * w, 0.5 * z));
        top_face_pl.set_closed (true);

        geomtools::gnuplot_draw::draw_polyline (out_,
                                                sh1_pos,
                                                sh1_rot,
                                                top_face_pl);

        for (unsigned int i = 0; i < 4; i++) {
          const double alpha = (45.0 + i * 90.0) * CLHEP::degree;
          const double x1 = std::sqrt (2) * 0.5 * w * std::cos (alpha);
          const double y1 = std::sqrt (2) * 0.5 * w * std::sin (alpha);
          const double z1 = 0.5 * z;
          const double z2 = hypot (x1, y1) / tan (angle_t) + zt;
          if (! datatools::is_valid (zlim1)) {
            zlim1 = z2;
          }
          geomtools::polyline_3d side_pl;
          side_pl.add (geomtools::vector_3d (x1, y1, z1));
          side_pl.add (geomtools::vector_3d (x1, y1, z2));
          const double x3 = rt * std::cos (alpha);
          const double y3 = rt * std::sin (alpha);
          const double z3 = -0.5 * z;
          side_pl.add (geomtools::vector_3d (x3, y3, z3));
          side_pl.set_closed (false);
          geomtools::gnuplot_draw::draw_polyline (out_,
                                                  sh1_pos,
                                                  sh1_rot,
                                                  side_pl);
        }

        for (unsigned int i = 0; i < 4; i++) {
          const double alpha = (i * 90) * CLHEP::degree;
          const double x2 = 0.5 * w * std::cos (alpha);
          const double y2 = 0.5 * w * std::sin (alpha);
          const double z2 = hypot (x2, y2) / tan (angle_t) + zt;
          if (! datatools::is_valid (zlim2)) {
            zlim2 = z2;
          }
          geomtools::polyline_3d side_pl;
          side_pl.add (geomtools::vector_3d (x2, y2, z2));
          const double x3 = rt * std::cos (alpha);
          const double y3 = rt * std::sin (alpha);
          const double z3 = -0.5 * z;
          side_pl.add (geomtools::vector_3d (x3, y3, z3));
          side_pl.set_closed (false);
          geomtools::gnuplot_draw::draw_polyline (out_,
                                                  sh1_pos,
                                                  sh1_rot,
                                                  side_pl);
        }

        for (unsigned int i = 0; i < 4; i++) {
          geomtools::polyline_3d cut_pl;
          const size_t n = 20;
          const double dt = w / n;
          for (unsigned int j = 0; j <= n; j++) {
            double x1 = 0.0;
            double y1 = 0.0;
            if (i == 0) {
              x1 = 0.5 * w;
              y1 = - 0.5 * w + dt * j;
            }
            if (i == 2) {
              x1 = -0.5 * w;
              y1 = - 0.5 * w + dt * j;
            }
            if (i == 1) {
              y1 = 0.5 * w;
              x1 = - 0.5 * w + dt * j;
            }
            if (i == 3) {
              y1 = -0.5 * w;
              x1 = - 0.5 * w + dt * j;
            }
            const double z1 = hypot (x1, y1) / tan (angle_t) + zt;
            cut_pl.add (geomtools::vector_3d (x1, y1, z1));
          }
          cut_pl.set_closed (false);
          geomtools::gnuplot_draw::draw_polyline (out_,
                                                  sh1_pos,
                                                  sh1_rot,
                                                  cut_pl);
        }

      }

      const bool draw_extrusion = true;
      if (draw_extrusion) {
        // draw extrusion limit:
        geomtools::placement mother_world_placement;
        mother_world_placement.set_translation (position_);
        mother_world_placement.set_orientation (rotation_);

        // surface extrusion circle:
        {
          const double z = mother_box.get_zmin ();
          const double c = mother_box.get_z () - h;
          const double rx = extrusion_sphere.get_r ();
          const double rho = rx - c;
          const double r1 = std::sqrt (rx * rx - rho * rho);

          geomtools::placement c1_plcmt;
          c1_plcmt.set (0., 0., z, 0. , 0., 0.);
          geomtools::placement world_item_placement;
          mother_world_placement.child_to_mother (c1_plcmt,
                                                  world_item_placement);
          const geomtools::vector_3d   & sh2_pos = world_item_placement.get_translation ();
          const geomtools::rotation_3d & sh2_rot = world_item_placement.get_rotation ();
          geomtools::gnuplot_draw::draw_circle (out_,
                                                sh2_pos,
                                                sh2_rot,
                                                r1);
          geomtools::gnuplot_draw::draw_circle (out_,
                                                sh2_pos,
                                                sh2_rot,
                                                rt);
        }

        // extrusion arcs:
        {
          const double z = mother_box.get_zmin ();
          const double c = mother_box.get_z () - h;
          const double rx = extrusion_sphere.get_r ();
          const double rho = rx - c;

          geomtools::gnuplot_draw::polyline_type arc1;
          geomtools::gnuplot_draw::polyline_type arc2;
          const double theta0 = acos (rho / rx);
          const size_t nsamples = 20;
          const double dt = 2 * theta0 / nsamples;
          const double z0 = z + c - rx;
          //geomtools::vector_3d Q (0.0, 0.0, z0);
          //arc.push_back (Q);
          for (unsigned int i = 0; i <= nsamples; i++) {
            const double theta = -theta0 + i * dt;
            const double dz = rx * std::cos (theta);
            const double dt = rx * std::sin (theta);
            geomtools::vector_3d P1 (dt, 0.0, z0 + dz);
            arc1.push_back (P1);
            geomtools::vector_3d P2 (0.0, dt, z0 + dz);
            arc2.push_back (P2);
          }
          geomtools::gnuplot_draw::draw_polyline (out_,
                                                  position_,
                                                  rotation_,
                                                  arc1);
          geomtools::gnuplot_draw::draw_polyline (out_,
                                                  position_,
                                                  rotation_,
                                                  arc2);
        }

        {
          const double c = mother_box.get_z () - h;
          const double c2 = 0.5 * c;
          const double z = mother_box.get_zmin () + c - c2;
          const double rx = extrusion_sphere.get_r ();
          const double rho = rx - c2;
          const double r1 = std::sqrt (rx *rx - rho * rho);

          geomtools::placement c1_plcmt;
          c1_plcmt.set (0., 0., z, 0. , 0., 0.);
          geomtools::placement world_item_placement;
          mother_world_placement.child_to_mother (c1_plcmt,
                                                  world_item_placement);
          const geomtools::vector_3d   & sh2_pos = world_item_placement.get_translation ();
          const geomtools::rotation_3d & sh2_rot = world_item_placement.get_rotation ();
          geomtools::gnuplot_draw::draw_circle (out_,
                                                sh2_pos,
                                                sh2_rot,
                                                r1);
        }

        {
          const double c = mother_box.get_z () - h;
          const double c2 = 0.25 * c;
          const double z = mother_box.get_zmin () + c - c2;
          const double rx = extrusion_sphere.get_r ();
          const double rho = rx - c2;
          const double r1 = std::sqrt(rx * rx - rho * rho);

          geomtools::placement c1_plcmt;
          c1_plcmt.set (0., 0., z, 0. , 0., 0.);
          geomtools::placement world_item_placement;
          mother_world_placement.child_to_mother (c1_plcmt,
                                                  world_item_placement);
          const geomtools::vector_3d   & sh2_pos = world_item_placement.get_translation ();
          const geomtools::rotation_3d & sh2_rot = world_item_placement.get_rotation ();
          geomtools::gnuplot_draw::draw_circle (out_,
                                                sh2_pos,
                                                sh2_rot,
                                                r1);
        }
      }
      DT_LOG_TRACE (local_priority, "Exiting.");
      return;
    }

  } // end of namespace geometry

} // end of namespace snemo
