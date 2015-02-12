// calo_scin_box_model.cc

// Ourselves:
#include <falaise/calo_scin_box_model.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
// - Bayeux/geomtools:
#include <geomtools/polyline_3d.h>
#include <geomtools/gnuplot_draw.h>

namespace snemo {

  namespace geometry {

    // Registration :
    GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(calo_scin_box_model,
                                           "snemo::geometry::calo_scin_box_model");

    std::string calo_scin_box_model::get_model_id () const
    {
      return "snemo::geometry::calo_scin_box_model";
    }

    const std::string & calo_scin_box_model::get_material () const
    {
      return _material_;
    }

    const geomtools::subtraction_3d & calo_scin_box_model::get_solid () const
    {
      return _solid_;
    }

    calo_scin_box_model::calo_scin_box_model () :
      geomtools::i_model ("calo_scin_box_model")
    {
      _material_ = "";
      _optical_glue_thickness_ = 1.0 * CLHEP::mm;
      return;
    }

    calo_scin_box_model::~calo_scin_box_model ()
    {
      return;
    }

    void calo_scin_box_model::_at_construct (const std::string & name_,
                                             const datatools::properties & config_,
                                             geomtools::models_col_type * /*models_*/)
    {
      if (config_.has_flag ("devel"))
        {
          set_logging_priority (datatools::logger::PRIO_TRACE);
        }
      DT_LOG_TRACE (get_logging_priority (), "Entering...");

      set_name (name_);

      double lunit = CLHEP::mm;

      /*** parse properties ***/

      if (config_.has_key ("length_unit"))
        {
          const std::string lunit_str = config_.fetch_string ("length_unit");
          lunit = datatools::units::get_length_unit_from (lunit_str);
        }


      DT_THROW_IF (! config_.has_key ("material.ref"), std::logic_error, "Missing 'material.ref' property !");
      _material_ = config_.fetch_string ("material.ref");

      DT_THROW_IF (! config_.has_key ("x"), std::logic_error, "Missing 'x' property !");
      double x = config_.fetch_real ("x");

      DT_THROW_IF (! config_.has_key ("y"), std::logic_error, "Missing 'y' property !");
      double y = config_.fetch_real ("y");

      DT_THROW_IF (! config_.has_key ("z"), std::logic_error, "Missing 'z' property !");
      double z = config_.fetch_real ("z");

      DT_THROW_IF (! config_.has_key ("h"), std::logic_error, "Missing 'h' property !");
      double h = config_.fetch_real ("h");

      DT_THROW_IF (! config_.has_key ("r"), std::logic_error, "Missing 'r' property !");
      double r = config_.fetch_real ("r");

      double optical_glue_thickness = 1.0 * lunit;
      if (config_.has_key ("optical_glue_thickness"))
        {
          optical_glue_thickness = config_.fetch_real ("optical_glue_thickness");
        }
      else
        {
          //DT_THROW_IF (true, std::logic_error, "Missing 'optical_glue_thickness' property !");
          DT_LOG_WARNING (get_logging_priority (), "No 'optical_glue_thickness' property !");
        }
      DT_THROW_IF (optical_glue_thickness < 0.0,
                   std::underflow_error,
                   "Invalid value '" << optical_glue_thickness << "' (<0) for 'optical_glue_thickness' !");
      DT_LOG_TRACE (get_logging_priority (), "OGT=" << _optical_glue_thickness_);

      DT_LOG_TRACE (get_logging_priority (), "Properties are parsed !");

      x *= lunit;
      y *= lunit;
      z *= lunit;
      h *= lunit;
      r *= lunit;
      optical_glue_thickness *= lunit;

      DT_LOG_TRACE (get_logging_priority (), "x=" << x);
      DT_LOG_TRACE (get_logging_priority (), "y=" << y);
      DT_LOG_TRACE (get_logging_priority (), "z=" << z);
      DT_LOG_TRACE (get_logging_priority (), "r=" << r);
      DT_LOG_TRACE (get_logging_priority (), "h=" << h);

      _optical_glue_thickness_ = optical_glue_thickness;
      _h_ = h;

      _mother_box_.set (x, y, z);
      DT_THROW_IF (! _mother_box_.is_valid (),
                   std::logic_error,
                   "Invalid dimension(s) for the mother scintillator box !");

      _extrusion_.set (r);
      DT_THROW_IF (! _extrusion_.is_valid (),
                   std::logic_error,
                   "Invalid dimension(s) for the extrusion sphere !");

      DT_THROW_IF ((h < 0.0) || (h > 2 * z),
                   std::range_error,
                   "Invalid extrusion position !");

      const double ze = 0.5 * z - r - h;
      geomtools::placement extrusion_placement (0, 0, ze, 0, 0, 0);
      DT_LOG_TRACE (get_logging_priority (), "ze=" << ze / CLHEP::mm << " mm");
      _solid_.set_shapes (_mother_box_, _extrusion_, extrusion_placement);
      _solid_.grab_properties ().store ("h", h);
      _solid_.grab_properties ().store ("optical_glue_thickness", _optical_glue_thickness_);

      // Install proposed 'stackable data' pointer in the shape:
      {
        geomtools::stackable_data * sd_ptr = new geomtools::stackable_data;
        sd_ptr->xmin = -0.5 * x;
        sd_ptr->xmax = +0.5 * x;
        sd_ptr->ymin = -0.5 * y;
        sd_ptr->ymax = +0.5 * y;
        //sd_ptr->zmin = -0.5 * z;
        sd_ptr->zmin = +0.5 * z - h - _optical_glue_thickness_;
        //sd_ptr->zmin = +0.5 * z - h - 3 * CLHEP::mm;
        sd_ptr->zmax = +0.5 * z;
        _solid_.set_stackable_data (sd_ptr);
        DT_LOG_TRACE (get_logging_priority (), "Stackable data:");
        if (get_logging_priority () >= datatools::logger::PRIO_TRACE)
          {
            sd_ptr->tree_dump (std::cerr);
          }
      }

      // Install a dedicated drawer:
      _drawer_.reset(new wires_drawer(*this));
      _solid_.set_wires_drawer(*_drawer_);
      _solid_.lock();

      grab_logical ().set_name (i_model::make_logical_volume_name (name_));
      grab_logical ().set_shape (_solid_);
      grab_logical ().set_material_ref (_material_);

      DT_LOG_TRACE (get_logging_priority (), "Dumping '" << get_name () << "':");
      if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
        this->tree_dump (std::cerr);
      }
      DT_LOG_TRACE (get_logging_priority (), "Exiting.");
      return;
    }

    /*** dump ***/

    void calo_scin_box_model::tree_dump (std::ostream & out_,
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

    void calo_scin_box_model::gnuplot_draw_user_function (std::ostream & out_,
                                                          const geomtools::vector_3d & position_,
                                                          const geomtools::rotation_3d & rotation_,
                                                          const geomtools::i_object_3d & obj_,
                                                          void *)

      calo_scin_box_model::wires_drawer::wires_drawer(const calo_scin_box_model & model_)
    {
      _model_ = &model_;
      return;
    }

    calo_scin_box_model::wires_drawer::~wires_drawer()
    {
      return;
    }

    void calo_scin_box_model::wires_drawer::generate_wires(std::ostream & out_,
                                                           const geomtools::vector_3d & position_,
                                                           const geomtools::rotation_3d & rotation_)
    {
      datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
      DT_LOG_TRACE (local_priority, "Entering...");
      const geomtools::subtraction_3d & solid = _model_->get_solid();

      const geomtools::i_composite_shape_3d::shape_type & s1 = solid.get_shape1 ();
      const geomtools::i_composite_shape_3d::shape_type & s2 = solid.get_shape2 ();
      const geomtools::i_shape_3d & sh1 = s1.get_shape ();
      const geomtools::i_shape_3d & sh2 = s2.get_shape ();

      // extract useful stuff (shapes and properties):
      const geomtools::box & mother_box = dynamic_cast<const geomtools::box &> (sh1);
      const geomtools::sphere & extrusion_sphere = dynamic_cast<const geomtools::sphere &> (sh2);

      const double h = _model_->_h_;

      const bool draw_scin_block = true;
      if (draw_scin_block) {
        // draw first shape:
        geomtools::placement mother_world_placement;
        mother_world_placement.set_translation (position_);
        mother_world_placement.set_orientation (rotation_);

        geomtools::placement world_item_placement;
        mother_world_placement.child_to_mother (s1.get_placement (),
                                                world_item_placement);
        const geomtools::vector_3d   & sh1_pos = world_item_placement.get_translation ();
        const geomtools::rotation_3d & sh1_rot = world_item_placement.get_rotation ();
        geomtools::gnuplot_draw::draw_box (out_,
                                           sh1_pos,
                                           sh1_rot,
                                           mother_box);
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
          const double r = extrusion_sphere.get_r ();
          const double rho = r - c;
          const double r1 = sqrt (r *r - rho * rho);

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

        // extrusion arcs:
        {
          const double z = mother_box.get_zmin ();
          const double c = mother_box.get_z () - h;
          const double r = extrusion_sphere.get_r ();
          const double rho = r - c;

          geomtools::gnuplot_draw::polyline_type arc1;
          geomtools::gnuplot_draw::polyline_type arc2;
          const double theta0 = acos (rho / r);
          const size_t nsamples = 20;
          const double dt = 2 * theta0 / nsamples;
          const double z0 = z + c - r;
          //vector_3d Q (0.0, 0.0, z0);
          //arc.push_back (Q);
          for (unsigned int i = 0; i <= nsamples; i++) {
            const double theta = -theta0 + i * dt;
            const double dz = r * cos (theta);
            const double dt = r * sin (theta);
            const geomtools::vector_3d P1 (dt, 0.0, z0 + dz);
            arc1.push_back (P1);
            const geomtools::vector_3d P2 (0.0, dt, z0 + dz);
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
          const double r = extrusion_sphere.get_r ();
          const double rho = r - c2;
          const double r1 = sqrt (r *r - rho * rho);

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
          const double r = extrusion_sphere.get_r ();
          const double rho = r - c2;
          const double r1 = sqrt (r *r - rho * rho);

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
      return;
    }

  } // end of namespace geometry

} // end of namespace snemo
