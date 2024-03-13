// falaise/snemo/datamodels/vertex_utils.cc

// Ourselves:
#include <falaise/snemo/datamodels/vertex_utils.h>

namespace snemo {

  namespace datamodel {

    // serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(vertex,
                                                      "snemo::datamodel::vertex")
    
    const std::string & vertex_undef_label()
    {
      static const std::string _label("undefined");
      return _label;
    }

    const std::string & vertex_on_wire_label()
    {
      static const std::string _label("wire");
      return _label;
    }

    const std::string & vertex_on_calibration_source_label()
    {
      static const std::string _label("calibration_source");
      return _label;
    }

    const std::string & vertex_on_source_foil_label()
    {
      static const std::string _label("foil");
      return _label;
    }

    const std::string & vertex_on_source_gap_label()
    {
      static const std::string _label("source_gap");
      return _label;
    }

    const std::string & vertex_on_main_calorimeter_label()
    {
      static const std::string _label("calo");
      return _label;
    }

    const std::string & vertex_on_x_calorimeter_label()
    {
      static const std::string _label("xcalo");
      return _label;
    }

    const std::string & vertex_on_gamma_veto_label()
    {
      static const std::string _label("gveto");
      return _label;
    }

    const std::string & to_string(const vertex_category_type category_)
    {
      switch (category_) {
      case VERTEX_CATEGORY_ON_SOURCE_FOIL:
        return vertex_on_source_foil_label();
      case VERTEX_CATEGORY_ON_MAIN_CALORIMETER:
        return vertex_on_main_calorimeter_label();
      case VERTEX_CATEGORY_ON_X_CALORIMETER:
        return vertex_on_x_calorimeter_label();
      case VERTEX_CATEGORY_ON_GAMMA_VETO:
        return vertex_on_gamma_veto_label();
      case VERTEX_CATEGORY_ON_WIRE:
        return vertex_on_wire_label();
      case VERTEX_CATEGORY_ON_CALIBRATION_SOURCE:
        return vertex_on_calibration_source_label();
      case VERTEX_CATEGORY_ON_SOURCE_GAP:
        return vertex_on_source_gap_label();
      default:
        break;
      }
      return vertex_undef_label();
    }

    vertex_category_type convert(const snemo::geometry::vertex_info::category_type gcat_)
    {
      switch(gcat_) {
      case snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_FOIL : return VERTEX_CATEGORY_ON_SOURCE_FOIL;
      case snemo::geometry::vertex_info::CATEGORY_ON_MAIN_CALORIMETER : return VERTEX_CATEGORY_ON_MAIN_CALORIMETER;
      case snemo::geometry::vertex_info::CATEGORY_ON_X_CALORIMETER : return VERTEX_CATEGORY_ON_X_CALORIMETER;
      case snemo::geometry::vertex_info::CATEGORY_ON_GAMMA_VETO : return VERTEX_CATEGORY_ON_GAMMA_VETO;
      case snemo::geometry::vertex_info::CATEGORY_ON_WIRE : return VERTEX_CATEGORY_ON_WIRE;
      case snemo::geometry::vertex_info::CATEGORY_ON_CALIBRATION_SOURCE : return VERTEX_CATEGORY_ON_CALIBRATION_SOURCE;
      case snemo::geometry::vertex_info::CATEGORY_ON_SOURCE_GAP : return VERTEX_CATEGORY_ON_SOURCE_GAP;
      default:
        break;
      }
      return VERTEX_CATEGORY_UNDEF;
    }
 
    vertex_category_type string_to_vertex_category(const std::string & label_)
    {
      if (label_ == to_string(VERTEX_CATEGORY_ON_SOURCE_FOIL)) return VERTEX_CATEGORY_ON_SOURCE_FOIL;
      if (label_ == to_string(VERTEX_CATEGORY_ON_MAIN_CALORIMETER)) return VERTEX_CATEGORY_ON_MAIN_CALORIMETER;
      if (label_ == to_string(VERTEX_CATEGORY_ON_X_CALORIMETER)) return VERTEX_CATEGORY_ON_X_CALORIMETER;
      if (label_ == to_string(VERTEX_CATEGORY_ON_GAMMA_VETO)) return VERTEX_CATEGORY_ON_GAMMA_VETO;
      if (label_ == to_string(VERTEX_CATEGORY_ON_WIRE)) return VERTEX_CATEGORY_ON_WIRE;
      if (label_ == to_string(VERTEX_CATEGORY_ON_CALIBRATION_SOURCE)) return VERTEX_CATEGORY_ON_CALIBRATION_SOURCE;
      if (label_ == to_string(VERTEX_CATEGORY_ON_SOURCE_GAP)) return VERTEX_CATEGORY_ON_SOURCE_GAP;
      return VERTEX_CATEGORY_UNDEF;
    }

    const std::string & to_string(const vertex_from_type from_)
    {
      static const std::string _fromFirst("first");
      static const std::string _fromLast("last");
      static const std::string _fromUndef("undefined");
      switch (from_) {
      case VERTEX_FROM_FIRST:
        return _fromFirst;
      case VERTEX_FROM_LAST:
        return _fromLast;
      default:
        break;
      }
      return _fromUndef;
    }

    vertex_from_type string_to_from(const std::string & label_)
    {
      if (label_ == to_string(VERTEX_FROM_FIRST)) return VERTEX_FROM_FIRST;
      if (label_ == to_string(VERTEX_FROM_LAST)) return VERTEX_FROM_LAST;
      return VERTEX_FROM_UNDEF;
    }

    const std::string & to_string(const vertex_extrapolation_type extrapolation_)
    {
      static const std::string _line("line");
      static const std::string _helix("helix");
      static const std::string _undef("undefined");
      switch (extrapolation_) {
      case VERTEX_EXTRAPOLATION_LINE:
        return _line;
      case VERTEX_EXTRAPOLATION_HELIX:
        return _helix;
      default:
        break;
      }
      return _undef;
    }

    vertex_extrapolation_type convert(const snemo::geometry::vertex_info::extrapolation_mode_type gext_)
    {
      switch(gext_) {
      case snemo::geometry::vertex_info::EXTRAPOLATION_LINE : return VERTEX_EXTRAPOLATION_LINE;
      case snemo::geometry::vertex_info::EXTRAPOLATION_HELIX : return VERTEX_EXTRAPOLATION_HELIX;
      default:
        break;
      }
      return VERTEX_EXTRAPOLATION_UNDEF;
    }

    vertex_extrapolation_type string_to_extrapolation(const std::string & label_)
    {
      if (label_ == to_string(VERTEX_EXTRAPOLATION_LINE)) return VERTEX_EXTRAPOLATION_LINE;
      if (label_ == to_string(VERTEX_EXTRAPOLATION_HELIX)) return VERTEX_EXTRAPOLATION_HELIX;
      return VERTEX_EXTRAPOLATION_UNDEF;
    }

    void vertex::reset()
    {
      _category_ = VERTEX_CATEGORY_UNDEF;
      _spot_.reset();
      _from_ = VERTEX_FROM_UNDEF;
      _extrapolation_ = VERTEX_EXTRAPOLATION_UNDEF;
      _distance_ = datatools::invalid_real();
      _distance_xy_ = datatools::invalid_real();
      _edge_ = false;
      _best_ = false;
      return;
    }
  
    vertex_category_type vertex::get_category() const
    {
      return _category_;
    }

    void vertex::set_category(const vertex_category_type category_)
    {
      _category_ = category_;
      return;
    }

    bool vertex::match_categories(const uint32_t category_flags_) const
    {
      if ((category_flags_ & _category_) != 0u) return true;
      return false;
    }
 
    bool vertex::is_on_source_foil() const
    {
      return _category_ == VERTEX_CATEGORY_ON_SOURCE_FOIL;
    }

    bool vertex::is_on_source_gap() const
    {
      return _category_ == VERTEX_CATEGORY_ON_SOURCE_GAP;
    }

    bool vertex::is_on_main_calorimeter() const
    {
      return _category_ == VERTEX_CATEGORY_ON_MAIN_CALORIMETER;
    }

    bool vertex::is_on_x_calorimeter() const
    {
      return _category_ == VERTEX_CATEGORY_ON_X_CALORIMETER;
    }

    bool vertex::is_on_gamma_veto() const
    {
      return _category_ == VERTEX_CATEGORY_ON_GAMMA_VETO;
    }

    bool vertex::is_on_wire() const
    {
      return _category_ == VERTEX_CATEGORY_ON_WIRE;
    }

    bool vertex::is_on_calibration_source() const
    {
      return _category_ == VERTEX_CATEGORY_ON_CALIBRATION_SOURCE;
    }

    void vertex::set_spot(const geomtools::blur_spot & spot_)
    {
      _spot_ = spot_;
      return;
    }
  
    geomtools::blur_spot & vertex::get_spot()
    {
      return _spot_;
    }

    const geomtools::blur_spot & vertex::get_spot() const
    {
      return _spot_;
    }

    vertex_from_type vertex::get_from() const
    {
      return _from_;
    }

    void vertex::set_from(const vertex_from_type from_)
    {
      _from_ = from_;
      return;
    }
    
    bool vertex::is_edge() const
    {
      return _edge_;
    }
      
    void vertex::set_edge(const bool edge_)
    {
      _edge_ = edge_;
      return;
    }
    
    bool vertex::is_best() const
    {
      return _best_;
    }
      
    void vertex::set_best(const bool best_)
    {
      _best_ = best_;
      return;
    }

    vertex_extrapolation_type vertex::get_extrapolation() const
    {
      return _extrapolation_;
    }

    void vertex::set_extrapolation(const vertex_extrapolation_type extrapolation_)
    {
      _extrapolation_ = extrapolation_;
      return;
    }

    double vertex::get_distance() const
    {
      return _distance_;
    }

    void vertex::set_distance(const double distance_)
    {
      _distance_ = distance_;
      return;
    }

    double vertex::get_distance_xy() const
    {
      return _distance_xy_;
    }

    void vertex::set_distance_xy(const double distance_xy_)
    {
      _distance_xy_ = distance_xy_;
      return;
    }

    void vertex::print_tree(std::ostream & out_,
                            const boost::property_tree::ptree & options_) const
    {
      base_hit::print_tree(out_, base_print_options::force_inheritance(options_));
      base_print_options popts;
      popts.configure_from(options_);
      const std::string & indent = popts.indent;
      out_ << indent << tag << "Category      : '" << to_string(_category_) << "'" << std::endl;
      out_ << indent << tag << "From          : '" << to_string(_from_) << "'" << std::endl;
      out_ << indent << tag << "Extrapolation : '" << to_string(_extrapolation_) << "'" << std::endl;
      out_ << indent << tag << "Spot          : ";
      if (not _spot_.get_placement().is_valid()) {
        out_ << "invalid";
      }
      out_ << std::endl;
      if (_spot_.get_placement().is_valid()) {
        _spot_.tree_dump(out_, "", indent + tags::skip_item());
      }
      out_ << indent << tag << "Distance      : " << _distance_ / CLHEP::mm << " mm" << std::endl;
      out_ << indent << tag << "XY-distance   : " << _distance_xy_ / CLHEP::mm << " mm" << std::endl;
      out_ << indent << tag << "Edge          : " << std::boolalpha << _edge_  << std::endl;
      out_ << indent << inherit_tag(popts.inherit)
	   << "Best      : " << std::boolalpha << _best_ << std::endl;
      return;
    } 
 
  } // end of namespace datamodel

} // end of namespace snemo

// end 
