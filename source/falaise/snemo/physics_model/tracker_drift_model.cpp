/// \file falaise/snemo/physics_model/tracker_drift_model.cpp
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2024-04-09
/// Last modified: 2024-04-09
///

// Ourselves:
#include <falaise/snemo/physics_model/tracker_drift_model.hpp>

// Standard library:
#include <cmath>

// Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <falaise/snemo/services/tracker_cell_status_service.h>

// Bayeux:
#include <bayeux/geomtools/geometry_service.h>

namespace snemo {

  namespace physics_model {
  
    std::ostream & operator<<(std::ostream & out_, const cell_quarter_category cat_)
    {
      switch (cat_) {
      case cell_quarter_category::none : out_ << "none"; break;
      case cell_quarter_category::centre : out_ << "centre"; break;
      case cell_quarter_category::edge : out_ << "edge"; break;
      case cell_quarter_category::corner : out_ << "corner"; break;
      }
      return out_;
    }
 
    std::ostream & operator<<(std::ostream & out_, const cell_quarter_edge_direction dir_)
    {
      switch (dir_) {
      case cell_quarter_edge_direction::none : out_ << "none"; break;
      case cell_quarter_edge_direction::back : out_ << "back"; break;
      case cell_quarter_edge_direction::front : out_ << "front"; break;
      case cell_quarter_edge_direction::left : out_ << "left"; break;
      case cell_quarter_edge_direction::right : out_ << "right"; break;
      }
      return out_;
    }
    
    std::string format_ncop(const neighbour_cells_off_pattern ncop_)
    {
      std::ostringstream repr;
      if (ncop_ & off_cell_direction::back) {
	repr << 'b';
      } else {
	repr << '-';
      }
      if (ncop_ & off_cell_direction::front) {
	repr << 'f';
      } else {
	repr << '-';
      }
      if (ncop_ & off_cell_direction::left) {
	repr << 'l';
      } else {
	repr << '-';
      }
      if (ncop_ & off_cell_direction::right) {
	repr << 'r';
      } else {
	repr << '-';
      }	
      return repr.str();
    }
 
    std::ostream & operator<<(std::ostream & out_, const cell_quarter_info & cinfo_)
    {
      out_ << cinfo_.category;
      if (cinfo_.category == cell_quarter_category::edge) {
	out_ << " (missing '" << cinfo_.edge_dir << "' HV or cell)";
      }
      return out_;
    }

    void cell_efield_info::print_tree(std::ostream & out_, const boost::property_tree::ptree & options_) const
    {
      std::string indent = options_.get<std::string>("indent", "");
      out_ << indent << "|-- " << "Quarter #0 : " << qinfos[0] << '\n'; 
      out_ << indent << "|-- " << "Quarter #1 : " << qinfos[1] << '\n';  
      out_ << indent << "|-- " << "Quarter #2 : " << qinfos[2] << '\n'; 
      out_ << indent << "`-- " << "Quarter #3 : " << qinfos[3] << '\n'; 
      return;
    }

    void cell_info::print_tree(std::ostream & out_, const boost::property_tree::ptree & options_) const
    {
      std::string indent = options_.get<std::string>("indent", "");
      out_ << indent << "|-- " << "Off : " << std::boolalpha << off << '\n';
      out_ << indent << "`-- " << "E-field info : " << std::boolalpha << (efield_info != nullptr) << '\n';
      if (efield_info) {
	std::string indent2 = indent + "   ";
	boost::property_tree::ptree popts2;
	popts2.put("indent", indent2);
	efield_info->print_tree(out_, popts2);
      }
      return;
    }

    void cell_info::export_viewer(std::ostream & out_, const double xcell_, const double ycell_, const double rcell_) const
    {
      out_ << xcell_ + rcell_ << ' ' << ycell_ + rcell_ << ' ' << 0 << '\n';
      out_ << xcell_ - rcell_ << ' ' << ycell_ + rcell_ << ' ' << 0 << '\n';
      out_ << xcell_ - rcell_ << ' ' << ycell_ - rcell_ << ' ' << 0 << '\n';
      out_ << xcell_ + rcell_ << ' ' << ycell_ - rcell_ << ' ' << 0 << '\n';
      out_ << xcell_ + rcell_ << ' ' << ycell_ + rcell_ << ' ' << 0 << '\n';
      double eps1 = 3 * CLHEP::mm;
      double eps2 = rcell_ - eps1;
      if (off) {
	out_ << '\n';
	out_ << xcell_ + eps2 << ' ' << ycell_ + eps2 << ' ' << -1 << '\n';
	out_ << xcell_ - eps2 << ' ' << ycell_ - eps2 << ' ' << -1 << '\n';
	out_ << '\n';
	out_ << xcell_ + eps2 << ' ' << ycell_ - eps2 << ' ' << -1 << '\n';
	out_ << xcell_ - eps2 << ' ' << ycell_ + eps2 << ' ' << -1 << '\n';
      } else {
	for (int i = 0; i < 4; i++) {
	  if (efield_info->qinfos[i].category == cell_quarter_category::centre) {
	    // nothing special
	  } else if (efield_info->qinfos[i].category == cell_quarter_category::corner) {
	    if (i == 0) {
	      out_ << '\n';
	      out_ << xcell_ + eps2 << ' ' << ycell_ + eps1 << ' ' << 1 << '\n';
	      out_ << xcell_ + eps2 << ' ' << ycell_ + eps2 << ' ' << 1 << '\n';
	      out_ << xcell_ + eps1 << ' ' << ycell_ + eps2 << ' ' << 1 << '\n';
	    } else if (i == 1) {
	      out_ << '\n';
	      out_ << xcell_ - eps2 << ' ' << ycell_ + eps1 << ' ' << 1 << '\n';
	      out_ << xcell_ - eps2 << ' ' << ycell_ + eps2 << ' ' << 1 << '\n';
	      out_ << xcell_ - eps1 << ' ' << ycell_ + eps2 << ' ' << 1 << '\n';
	    } else if (i == 2) {
	      out_ << '\n';
	      out_ << xcell_ - eps2 << ' ' << ycell_ - eps1 << ' ' << 1 << '\n';
	      out_ << xcell_ - eps2 << ' ' << ycell_ - eps2 << ' ' << 1 << '\n';
	      out_ << xcell_ - eps1 << ' ' << ycell_ - eps2 << ' ' << 1 << '\n';
	    } else if (i == 3) {
	      out_ << '\n';
	      out_ << xcell_ + eps2 << ' ' << ycell_ - eps1 << ' ' << 1 << '\n';
	      out_ << xcell_ + eps2 << ' ' << ycell_ - eps2 << ' ' << 1 << '\n';
	      out_ << xcell_ + eps1 << ' ' << ycell_ - eps2 << ' ' << 1 << '\n';
	    }
	  } else if (efield_info->qinfos[i].category == cell_quarter_category::edge) {
	    if (i == 0) {
	      if (efield_info->qinfos[i].edge_dir == cell_quarter_edge_direction::front) {
		out_ << '\n';
		out_ << xcell_ + eps2 << ' ' << ycell_ + eps1 << ' ' << 2 << '\n';
		out_ << xcell_ + eps2 << ' ' << ycell_ + eps2 << ' ' << 2 << '\n';
	      } else if (efield_info->qinfos[i].edge_dir == cell_quarter_edge_direction::right) {
		out_ << '\n';
		out_ << xcell_ + eps1 << ' ' << ycell_ + eps2 << ' ' << 2 << '\n';
		out_ << xcell_ + eps2 << ' ' << ycell_ + eps2 << ' ' << 2 << '\n';
	      } 
	    } else if (i == 1) {
	      if (efield_info->qinfos[i].edge_dir == cell_quarter_edge_direction::back) {
		out_ << '\n';
		out_ << xcell_ - eps2 << ' ' << ycell_ + eps1 << ' ' << 2 << '\n';
		out_ << xcell_ - eps2 << ' ' << ycell_ + eps2 << ' ' << 2 << '\n';
	      } else if (efield_info->qinfos[i].edge_dir == cell_quarter_edge_direction::right) {
		out_ << '\n';
		out_ << xcell_ - eps1 << ' ' << ycell_ + eps2 << ' ' << 2 << '\n';
		out_ << xcell_ - eps2 << ' ' << ycell_ + eps2 << ' ' << 2 << '\n';
	      } 
	    } else if (i == 2) {
	      if (efield_info->qinfos[i].edge_dir == cell_quarter_edge_direction::back) {
		out_ << '\n';
		out_ << xcell_ - eps2 << ' ' << ycell_ - eps1 << ' ' << 2 << '\n';
		out_ << xcell_ - eps2 << ' ' << ycell_ - eps2 << ' ' << 2 << '\n';
	      } else if (efield_info->qinfos[i].edge_dir == cell_quarter_edge_direction::left) {
		out_ << '\n';
		out_ << xcell_ - eps1 << ' ' << ycell_ - eps2 << ' ' << 2 << '\n';
		out_ << xcell_ - eps2 << ' ' << ycell_ - eps2 << ' ' << 2 << '\n';
	      } 
	    } else if (i == 3) {
	      if (efield_info->qinfos[i].edge_dir == cell_quarter_edge_direction::front) {
		out_ << '\n';
		out_ << xcell_ + eps2 << ' ' << ycell_ - eps1 << ' ' << 2 << '\n';
		out_ << xcell_ + eps2 << ' ' << ycell_ - eps2 << ' ' << 2 << '\n';
	      } else if (efield_info->qinfos[i].edge_dir == cell_quarter_edge_direction::left) {
		out_ << '\n';
		out_ << xcell_ + eps1 << ' ' << ycell_ - eps2 << ' ' << 2 << '\n';
		out_ << xcell_ + eps2 << ' ' << ycell_ - eps2 << ' ' << 2 << '\n';
	      } 
	    }
	  }	
	}
      }
      return;
    }

    const snemo::geometry::gg_locator &  tracker_drift_model::gg_locator() const
    {
      return *_gg_locator_;
    }

    const snemo::tracker_cell_status_service &  tracker_drift_model::tcss() const
    {
      return *_tcss_;
    }
    
    tracker_drift_model::tracker_drift_model(const datatools::logger::priority verbosity_)
    {
      _verbosity_ = verbosity_;
       
      return;
    }
 
    tracker_drift_model::~tracker_drift_model()
    {
      return;
    }
 
    datatools::logger::priority tracker_drift_model::get_verbosity() const
    {
      return _verbosity_;
    }
      
    void tracker_drift_model::set_verbosity(datatools::logger::priority verbosity_)
    {
      _verbosity_ = verbosity_;
      return;
    }
 
    void tracker_drift_model::initialize(const datatools::properties & config_,
                                         const datatools::service_dict_type & services_)
    {

      static const std::set<std::string> supportedModels = {"betsy"};
      if (config_.has_key("model")) {
        std::string modelName = config_.fetch_string("model");
        DT_THROW_IF(not supportedModels.count(modelName),
                    std::logic_error,
                    "Drift model '" << modelName << "' is not supported!");
        _model_ = modelName;
      }

      if (config_.has_key("version")) {
        unsigned int modelVersion = config_.fetch_positive_integer("version");
        DT_THROW_IF(modelVersion != 0,
                    std::logic_error,
                    "Drift model version " << modelVersion << " is not supported!");
        _version_ = modelVersion;
      }
      
      std::string geometryLabel = service_info::geometryServiceName();
      DT_THROW_IF(not datatools::has<geomtools::geometry_service>(services_, geometryLabel),
                  std::logic_error,
                  "No geometry service");
      _geomgr_ = &datatools::get<geomtools::geometry_service>(services_, geometryLabel).get_geom_manager();

      std::string dbLabel = service_info::dbServiceName();
      if (datatools::has<snemo::db_service>(services_, dbLabel)) {
	_db_ = &datatools::get<snemo::db_service>(services_, dbLabel);
      }
      DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
		     "Tracker drift model has no access to DB service");
      
      std::string tcssLabel = service_info::trackerCellStatusServiceName();
      DT_THROW_IF(not datatools::has<snemo::tracker_cell_status_service>(services_, tcssLabel),
                  std::logic_error,
                  "No tracker cell status service");
      _tcss_ = &datatools::get<snemo::tracker_cell_status_service>(services_, tcssLabel);

      const snemo::geometry::locator_plugin & locators
        = _geomgr_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
      _gg_locator_ = &locators.geigerLocator();

      if (_model_ == "betsy") {
        if (config_.has_key("default_gas_pressure")) {
          double P = config_.fetch_real_with_explicit_dimension("default_gas_pressure", "pressure");
          DT_THROW_IF(P < 835.0e-3 * CLHEP::bar or P > 925.0e-3 * CLHEP::bar,
                      std::domain_error,
                      "Unsupported gas pressure value");
          _default_gas_pressure_ = P;
        }
  
        if (config_.has_key("default_neighbour_cell_off_pattern")) {
          uint32_t cellOffPattern = config_.fetch_positive_integer("default_neighbour_cell_off_pattern");
          _default_neighbour_cells_off_pattern_ = cellOffPattern;
        }
      }
      
      return;
    }
 
    bool tracker_drift_model::has_default_gas_pressure() const
    {
      return _default_gas_pressure_.has_value();
    }

    bool tracker_drift_model::has_default_neighbour_cells_off_pattern() const
    {
      return _default_neighbour_cells_off_pattern_.has_value();
    }
 
    int tracker_drift_model::locate_cell_quarter(const geomtools::vector_3d & in_cell_position_) const
    {
      int quarterIndex = -1;
      double theta = std::atan2(in_cell_position_.y(), in_cell_position_.x());
      if (not std::isnan(theta)) {
        if (theta < 0.0) theta += 2 * M_PI;
        else if (theta < M_PI/2) quarterIndex = 0; 
        else if (theta < M_PI) quarterIndex = 1; 
        else if (theta < 1.5 * M_PI) quarterIndex = 2;
        else quarterIndex = 3;
      }
      return quarterIndex;
    }
  
    void tracker_drift_model::reset()
    {
      return;
    }
      
    tracker_gas_info
    tracker_drift_model::fetch_gas_info(const time::time_point & /* p_ */) const
    {
      tracker_gas_info tgi;
      if (has_default_gas_pressure()) {
        tgi.pressure = *_default_gas_pressure_;
      } else {
        // Here we should ask the DB, given the time point
        tgi.pressure = 880.0e-3 * CLHEP::bar ;
      }
      return tgi;
    }
         
    neighbour_cells_off_pattern
    tracker_drift_model::_fetch_default_neighbour_cells_off_pattern_(const geomtools::geom_id & gid_) const
    {
      DT_THROW_IF(_gg_locator_->cellGIDType() != gid_.get_type(),
                  std::logic_error,
                  "Not a valid cell geom ID");
      neighbour_cells_off_pattern offPattern = 0;
      uint32_t module = _gg_locator_->getModuleAddress(gid_);
      DT_THROW_IF(module != _gg_locator_->getModuleNumber(), std::logic_error,
		  "Unsupported module number [" << module << " != " << _gg_locator_->getModuleNumber() << "]");
      uint32_t side = _gg_locator_->getSideAddress(gid_);
      uint32_t layer = _gg_locator_->getLayerAddress(gid_);
      uint32_t row = _gg_locator_->getRowAddress(gid_);
      if (side == 0) {
        if (layer == 0) {
          offPattern |= off_cell_direction::front;
        } else if (layer == 8) {
          offPattern |= off_cell_direction::back;
        }
        if (row == 0) {
          offPattern |= off_cell_direction::left;  
        } else if (row == 112) {
          offPattern |= off_cell_direction::right;          
        }
      } else {
        if (layer == 0) {
          offPattern |= off_cell_direction::back;
        } else if (layer == 8) {
          offPattern |= off_cell_direction::front;
        }
        if (row == 0) {
          offPattern |= off_cell_direction::left;  
        } else if (row == 112) {
          offPattern |= off_cell_direction::right;          
        }
      }
      return offPattern;
    }
      
    neighbour_cells_off_pattern
    tracker_drift_model::fetch_cell_off_pattern(const geomtools::geom_id & gid_,
                                                const time::time_point & timepoint_) const
    {
      if (has_default_neighbour_cells_off_pattern()) {
        // Force a default cell OFF pattern for all cells:
        return *_default_neighbour_cells_off_pattern_;
      }
      // Fetch the cell OFF pattern from the geometry layout:
      neighbour_cells_off_pattern cellOffPattern = _fetch_default_neighbour_cells_off_pattern_(gid_);
      if (_tcss_) {
        // Apply tracker cell status informations:
        std::vector<geomtools::geom_id> neighbourGIDs = _gg_locator_->getNeighbourGIDs(gid_, false);
        for (auto iCell = 0u; iCell < neighbourGIDs.size(); iCell++) {
          const geomtools::geom_id & nGid = neighbourGIDs[iCell];
          std::uint32_t cellStatus = _tcss_->get_cell_status(nGid, timepoint_);
          if (snemo::rc::tracker_cell_status::is_off(cellStatus)) {
            // Detect OFF neighbour cells:
            auto dir = _gg_locator_->relativeDirection(gid_, nGid);
            if (dir == geometry::direction_t::LEFT)  cellOffPattern |= off_cell_direction::left;
            if (dir == geometry::direction_t::RIGHT) cellOffPattern |= off_cell_direction::right;
            if (dir == geometry::direction_t::BACK)  cellOffPattern |= off_cell_direction::back;
            if (dir == geometry::direction_t::FRONT) cellOffPattern |= off_cell_direction::front;
          }
        }
      }
      return cellOffPattern;
    }
   
    cell_info tracker_drift_model::fetch_cell_info(const geomtools::geom_id & gid_,
						   const time::time_point & timepoint_) const
    {
      neighbour_cells_off_pattern cellOffPattern = fetch_cell_off_pattern(gid_, timepoint_);
      cell_info cellInfo;
      cellInfo.off = false;
      if (_tcss_) {
	std::uint32_t cellStatus = _tcss_->get_cell_status(gid_, timepoint_);
	if (snemo::rc::tracker_cell_status::is_off(cellStatus)) {
	  cellInfo.off = true;
	}
      }
      cellInfo.efield_info = &supported_cell_efield_info(cellOffPattern);
      return cellInfo;
    }

    double lininterp2(double x1_, double x2_, double y1_, double y2_, double x_)
    {
      DT_THROW_IF(x2_ <= x1_, std::domain_error, "Invalid interpolation domain");
      DT_THROW_IF(x_ < x1_ - 0.5 * (x2_ - x1_), std::domain_error, "Out of supported interpolation domain");
      DT_THROW_IF(x_ > x2_ + 0.5 * (x2_ - x1_), std::domain_error, "Out of supported interpolation domain");
      return y1_ + (x_ - x1_) * (y2_ - y1_) / (x2_ - x1_);
    }

    double lininterp3(double x1_, double x2_, double x3_, double y1_, double y2_, double y3_, double x_)
    {
      DT_THROW_IF(x2_ <= x1_, std::domain_error, "Invalid interpolation domain");
      DT_THROW_IF(x3_ <= x2_, std::domain_error, "Invalid interpolation domain");
      DT_THROW_IF(x_ < x1_ - 0.5 * (x2_ - x1_), std::domain_error, "Out of supported interpolation domain");
      DT_THROW_IF(x_ > x3_ + 0.5 * (x3_ - x2_), std::domain_error, "Out of supported interpolation domain");
      if (x_ < x2_) {
        return y1_ + (x_ - x1_) * (y2_ - y1_) / (x2_ - x1_);
      }
      return y2_ + (x_ - x2_) * (y3_ - y2_) / (x3_ - x2_);
    }
    
    cell_drift_fit_parameters tracker_drift_model::fetch_cell_fit_params(const cell_quarter_category cqc_,
                                                                         const double pressure_,
                                                                         bool large_radius_) const
    {
      // DocDB5839, page 42, Tables 2-3
      // Unit fix from Kaela Albert (2024-04-23)
      DT_THROW_IF(pressure_ < 840e-3 * CLHEP::bar, std::domain_error, "Unsupported gas pressure");
      DT_THROW_IF(pressure_ > 920e-3 * CLHEP::bar, std::domain_error, "Unsupported gas pressure");
      static const double P1 = 850.;
      static const double P2 = 880.;
      static const double P3 = 910.;
      cell_drift_fit_parameters cdfp;
      double P = pressure_ / (1e-3 * CLHEP::bar);
      if (cqc_ == cell_quarter_category::corner) {
        cdfp.x = 21.5 * CLHEP::mm;
        if (not large_radius_) {
          cdfp.tx  = lininterp3(P1, P2, P3, 3.34, 3.45, 3.59, P) * CLHEP::microsecond;
          cdfp.a   = lininterp3(P1, P2, P3, 7.66, 7.92, 8.16, P) * 1e-1 * CLHEP::microsecond / CLHEP::cm;
          cdfp.b   = lininterp3(P1, P2, P3, 0.87, 0.87, 0.87, P);
          cdfp.alo = lininterp3(P1, P2, P3, 8.79, 9.13, 9.36, P) * 1e-1 * CLHEP::microsecond / CLHEP::cm;
          cdfp.blo = lininterp3(P1, P2, P3, 0.89, 0.88, 0.90, P);
          cdfp.ahi = lininterp3(P1, P2, P3, 6.77, 6.95, 7.20, P) * 1e-1 * CLHEP::microsecond / CLHEP::cm;
          cdfp.bhi = lininterp3(P1, P2, P3, 0.85, 0.86, 0.85, P);
        } else {
          cdfp.tx  = lininterp3(P1, P2, P3, 3.34, 3.45, 3.59, P) * CLHEP::microsecond;
          cdfp.a   = lininterp3(P1, P2, P3, 5.18, 4.94, 5.25, P) * 1e-1 * CLHEP::microsecond / CLHEP::cm;
          cdfp.b   = lininterp3(P1, P2, P3, 1.40, 1.48, 1.45, P);
          cdfp.alo = lininterp3(P1, P2, P3, 5.16, 4.91, 5.17, P) * 1e-1 * CLHEP::microsecond / CLHEP::cm;
          cdfp.blo = lininterp3(P1, P2, P3, 1.61, 1.69, 1.68, P);
          cdfp.ahi = lininterp3(P1, P2, P3, 5.17, 4.93, 5.28, P) * 1e-1 * CLHEP::microsecond / CLHEP::cm;
          cdfp.bhi = lininterp3(P1, P2, P3, 1.23, 1.31, 1.27, P);
        }
      } else if (cqc_ == cell_quarter_category::edge) {
        cdfp.x = 22.0 * CLHEP::mm;
        if (not large_radius_) {
          cdfp.tx  = lininterp3(P1, P2, P3, 3.73, 4.15, 4.12, P) * CLHEP::microsecond;
          cdfp.a   = lininterp3(P1, P2, P3, 8.05, 8.35, 8.56, P) * 1e-1 * CLHEP::microsecond;
          cdfp.b   = lininterp3(P1, P2, P3, 0.90, 0.92, 0.90, P);
          cdfp.alo = lininterp3(P1, P2, P3, 9.32, 9.62, 9.88, P) * 1e-1 * CLHEP::microsecond;
          cdfp.blo = lininterp3(P1, P2, P3, 0.91, 0.95, 0.92, P);
          cdfp.ahi = lininterp3(P1, P2, P3, 7.05, 7.37, 7.53, P) * 1e-1 * CLHEP::microsecond;
          cdfp.bhi = lininterp3(P1, P2, P3, 0.88, 0.88, 0.87, P);
        } else {
          cdfp.tx  = lininterp3(P1, P2, P3, 3.73, 4.15, 4.12, P) * CLHEP::microsecond;
          cdfp.a   = lininterp3(P1, P2, P3, 3.34, 3.39, 4.03, P) * 1e-1 * CLHEP::microsecond;
          cdfp.b   = lininterp3(P1, P2, P3, 2.04, 2.07, 1.91, P);
          cdfp.alo = lininterp3(P1, P2, P3, 3.29, 3.29, 3.89, P) * 1e-1 * CLHEP::microsecond;
          cdfp.blo = lininterp3(P1, P2, P3, 2.30, 2.34, 2.20, P);
          cdfp.ahi = lininterp3(P1, P2, P3, 3.42, 3.48, 4.14, P) * 1e-1 * CLHEP::microsecond;
          cdfp.bhi = lininterp3(P1, P2, P3, 1.82, 1.84, 1.69, P);
        }
      } else if (cqc_ == cell_quarter_category::centre) {
        cdfp.x = 19.0 * CLHEP::mm;
        if (not large_radius_) {
          cdfp.tx  = lininterp3(P1, P2, P3, 2.95, 2.97, 3.06, P) * CLHEP::microsecond;
          cdfp.a   = lininterp3(P1, P2, P3, 8.28, 8.53, 8.77, P) * 1e-1 * CLHEP::microsecond;
          cdfp.b   = lininterp3(P1, P2, P3, 0.90, 0.90, 0.90, P);
          cdfp.alo = lininterp3(P1, P2, P3, 9.61, 9.88, 10.2, P) * 1e-1 * CLHEP::microsecond;
          cdfp.blo = lininterp3(P1, P2, P3, 0.93, 0.93, 0.93, P);
          cdfp.ahi = lininterp3(P1, P2, P3, 7.25, 7.48, 7.69, P) * 1e-1 * CLHEP::microsecond;
          cdfp.bhi = lininterp3(P1, P2, P3, 0.88, 0.87, 0.87, P);
        } else {
          cdfp.tx  = lininterp3(P1, P2, P3, 2.95, 2.97, 3.06, P) * CLHEP::microsecond;
          cdfp.a   = lininterp3(P1, P2, P3, 3.86, 4.19, 4.55, P) * 1e-1 * CLHEP::microsecond;
          cdfp.b   = lininterp3(P1, P2, P3, 1.99, 1.93, 1.90, P);
          cdfp.alo = lininterp3(P1, P2, P3, 4.18, 4.33, 4.86, P) * 1e-1 * CLHEP::microsecond;
          cdfp.blo = lininterp3(P1, P2, P3, 2.19, 2.17, 2.10, P);
          cdfp.ahi = lininterp3(P1, P2, P3, 3.80, 4.16, 4.50, P) * 1e-1 * CLHEP::microsecond;
          cdfp.bhi = lininterp3(P1, P2, P3, 1.79, 1.72, 1.68, P);
        }
      }
      return cdfp;
    }

    double cell_drift_fit_parameters::raw_eval(const double & radius_, const int pos_) const
    {
      double t = std::numeric_limits<double>::quiet_NaN();
      double ae = a; 
      double be = b;
      if (pos_ < 0) {
        ae = alo; 
        be = blo; 
      } else if (pos_ > 0) {
        ae = ahi; 
        be = bhi; 
      }
      t = ae * std::pow(radius_ / CLHEP::cm, 1. + be);
      return t;
    }

    std::tuple<double,double, double>
    tracker_drift_model::compute_drift_time(const cell_quarter_category cqc_,
                                            const double pressure_,
                                            const double radius_) const
    {
      cell_drift_fit_parameters cdfpSmall = fetch_cell_fit_params(cqc_, pressure_, false);
      cell_drift_fit_parameters cdfpLarge = fetch_cell_fit_params(cqc_, pressure_, true);
      double tmin = std::numeric_limits<double>::quiet_NaN();
      double tmed = std::numeric_limits<double>::quiet_NaN();
      double tmax = std::numeric_limits<double>::quiet_NaN();
      static const double dr = 1 * CLHEP::mm;
      static const double max_radius = std::max(32.0 * CLHEP::mm,
						_gg_locator_->cellRadius() * M_SQRT2); 
      DT_THROW_IF(radius_ >= max_radius,
                  std::domain_error,
                  "Invalid cell radial distance");
      if (radius_ <= cdfpSmall.x - dr) {
        tmin = cdfpSmall.raw_eval(radius_, -1);
        tmed = cdfpSmall.raw_eval(radius_, 0);
        tmax = cdfpSmall.raw_eval(radius_, +1);
      } else if (radius_ >= cdfpSmall.x + dr) {
        tmin = cdfpLarge.raw_eval(radius_, -1);
        tmed = cdfpLarge.raw_eval(radius_, 0);
        tmax = cdfpLarge.raw_eval(radius_, +1);
      } else {
        double r1 = radius_ - dr;
        double r2 = radius_ + dr;
        double tmin1 = cdfpSmall.raw_eval(r1, -1);
        double tmed1 = cdfpSmall.raw_eval(r1, 0);
        double tmax1 = cdfpSmall.raw_eval(r1, +1);
        double tmin2 = cdfpLarge.raw_eval(r2, -1);
        double tmed2 = cdfpLarge.raw_eval(r2, 0);
        double tmax2 = cdfpLarge.raw_eval(r2, +1);
        tmin = lininterp2(r1, r2, tmin1, tmin2, radius_);
        tmed = lininterp2(r1, r2, tmed1, tmed2, radius_);
        tmax = lininterp2(r1, r2, tmax1, tmax2, radius_);
      }
      return std::make_tuple(tmin, tmed, tmax);
    }

    // static
    const std::vector<cell_efield_info> &
    tracker_drift_model::supported_cell_efield_infos()
    {
      static std::vector<cell_efield_info> _scis;
      if (_scis.empty()) {
        size_t number_of_off_patterns = 16;
        // Allocate memory:
        _scis.assign(number_of_off_patterns, cell_efield_info());
        for (uint32_t offPattern = 0U; offPattern < number_of_off_patterns; offPattern++) {
          cell_efield_info cinfo; // Default (all 'centre' quarters)
          //   v
          // v + v = 0000 = 0
          //   v
          // Process 15 more cases:
          if (offPattern == off_cell_direction::back) {
            //   v
            // x + v = 0001 = 1
            //   v
            cinfo.qinfos[1].category = cell_quarter_category::edge;
            cinfo.qinfos[1].edge_dir = cell_quarter_edge_direction::back;
            cinfo.qinfos[2].category = cell_quarter_category::edge;
            cinfo.qinfos[2].edge_dir = cell_quarter_edge_direction::back;
          } else if (offPattern == off_cell_direction::front) {
            //   v
            // v + x = 0010 = 2
            //   v
            cinfo.qinfos[0].category = cell_quarter_category::edge;
            cinfo.qinfos[0].edge_dir = cell_quarter_edge_direction::front;
            cinfo.qinfos[3].category = cell_quarter_category::edge;
            cinfo.qinfos[3].edge_dir = cell_quarter_edge_direction::front;          
          } else if (offPattern == off_cell_direction::left) {
            //   v
            // v + v = 0100 = 4
            //   x
            cinfo.qinfos[2].category = cell_quarter_category::edge;
            cinfo.qinfos[2].edge_dir = cell_quarter_edge_direction::left;
            cinfo.qinfos[3].category = cell_quarter_category::edge;
            cinfo.qinfos[3].edge_dir = cell_quarter_edge_direction::left;
          } else if (offPattern == off_cell_direction::right) {
            //   x
            // v + v = 1000 = 8
            //   v
            cinfo.qinfos[0].category = cell_quarter_category::edge;
            cinfo.qinfos[0].edge_dir = cell_quarter_edge_direction::right;
            cinfo.qinfos[1].category = cell_quarter_category::edge;
            cinfo.qinfos[1].edge_dir = cell_quarter_edge_direction::right;   
          } else if (offPattern == ( \
				    off_cell_direction::front | \
				    off_cell_direction::left | \
				    off_cell_direction::right)) {
            //   x
            // v + x = 1110 = 14
            //   x
            cinfo.qinfos[0].category = cell_quarter_category::corner;
            cinfo.qinfos[1].category = cell_quarter_category::edge;
            cinfo.qinfos[1].edge_dir = cell_quarter_edge_direction::right;
            cinfo.qinfos[2].category = cell_quarter_category::edge;
            cinfo.qinfos[2].edge_dir = cell_quarter_edge_direction::left;
            cinfo.qinfos[3].category = cell_quarter_category::corner;
          } else if (offPattern == ( \
				    off_cell_direction::back | \
				    off_cell_direction::front |\
				    off_cell_direction::right)) {
            //   x
            // x + x = 1011 = 11
            //   v
            cinfo.qinfos[0].category = cell_quarter_category::corner;
            cinfo.qinfos[1].category = cell_quarter_category::corner;
            cinfo.qinfos[2].category = cell_quarter_category::edge;
            cinfo.qinfos[2].edge_dir = cell_quarter_edge_direction::back;
            cinfo.qinfos[3].category = cell_quarter_category::edge;
            cinfo.qinfos[3].edge_dir = cell_quarter_edge_direction::front;
          } else if (offPattern == ( \
				    off_cell_direction::back | \
				    off_cell_direction::left | \
				    off_cell_direction::right)) {
            //   x
            // x + v = 1101 = 13
            //   x
            cinfo.qinfos[0].category = cell_quarter_category::edge;
            cinfo.qinfos[0].edge_dir = cell_quarter_edge_direction::right;
            cinfo.qinfos[1].category = cell_quarter_category::corner;
            cinfo.qinfos[2].category = cell_quarter_category::corner;
            cinfo.qinfos[3].category = cell_quarter_category::edge;
            cinfo.qinfos[3].edge_dir = cell_quarter_edge_direction::left;
          } else if (offPattern == ( \
				    off_cell_direction::back | \
				    off_cell_direction::left | \
				    off_cell_direction::front)) {
            //   v
            // x + x = 0111 = 7
            //   x
            cinfo.qinfos[0].category = cell_quarter_category::edge;
            cinfo.qinfos[0].edge_dir = cell_quarter_edge_direction::front;
            cinfo.qinfos[1].category = cell_quarter_category::corner;
            cinfo.qinfos[1].edge_dir = cell_quarter_edge_direction::back;
            cinfo.qinfos[2].category = cell_quarter_category::corner;
            cinfo.qinfos[3].category = cell_quarter_category::corner;
          } else if (offPattern == ( \
				    off_cell_direction::back | \
				    off_cell_direction::front | \
				    off_cell_direction::left | \
				    off_cell_direction::front)) {
            //   x
            // x + x = 1111 = 15
            //   x
            cinfo.qinfos[0].category = cell_quarter_category::corner;
            cinfo.qinfos[1].category = cell_quarter_category::corner;
            cinfo.qinfos[2].category = cell_quarter_category::corner;
            cinfo.qinfos[3].category = cell_quarter_category::corner;
          } else if (offPattern == ( \
                     off_cell_direction::back | \
                     off_cell_direction::left)) {
            //   v
            // x + v = 0101 = 5
            //   x
            cinfo.qinfos[1].category = cell_quarter_category::edge;
            cinfo.qinfos[1].edge_dir = cell_quarter_edge_direction::back;
            cinfo.qinfos[2].category = cell_quarter_category::corner;
            cinfo.qinfos[3].category = cell_quarter_category::edge;
            cinfo.qinfos[3].edge_dir = cell_quarter_edge_direction::left;
          } else if (offPattern == ( \
				    off_cell_direction::front | \
				    off_cell_direction::left)) {
            //   v
            // v + x = 0110 = 6
            //   x
            cinfo.qinfos[0].category = cell_quarter_category::edge;
            cinfo.qinfos[0].edge_dir = cell_quarter_edge_direction::front;
            cinfo.qinfos[2].category = cell_quarter_category::edge;
            cinfo.qinfos[2].edge_dir = cell_quarter_edge_direction::left;
            cinfo.qinfos[3].category = cell_quarter_category::corner;
          } else if (offPattern == ( \
				    off_cell_direction::front | \
				    off_cell_direction::right)) {
            //   x
            // v + x = 1010 = 10
            //   v
            cinfo.qinfos[0].category = cell_quarter_category::corner;
            cinfo.qinfos[1].category = cell_quarter_category::edge;
            cinfo.qinfos[1].edge_dir = cell_quarter_edge_direction::right;
            cinfo.qinfos[3].category = cell_quarter_category::edge;
            cinfo.qinfos[3].edge_dir = cell_quarter_edge_direction::front;
          } else if (offPattern == ( \
				    off_cell_direction::back | \
				    off_cell_direction::right)) {
            //   x
            // x + v = 1001 = 9
            //   v
            cinfo.qinfos[0].category = cell_quarter_category::edge;
            cinfo.qinfos[0].edge_dir = cell_quarter_edge_direction::right;
            cinfo.qinfos[1].category = cell_quarter_category::corner;
            cinfo.qinfos[2].category = cell_quarter_category::edge;
            cinfo.qinfos[2].edge_dir = cell_quarter_edge_direction::back;
          } else if (offPattern == ( \
				    off_cell_direction::left | \
				    off_cell_direction::right)) {
            //   x
            // v + v = 1100 = 12
            //   x
            cinfo.qinfos[0].category = cell_quarter_category::edge;
            cinfo.qinfos[0].edge_dir = cell_quarter_edge_direction::right;
            cinfo.qinfos[1].category = cell_quarter_category::edge;
            cinfo.qinfos[1].edge_dir = cell_quarter_edge_direction::right;
            cinfo.qinfos[2].category = cell_quarter_category::edge;
            cinfo.qinfos[2].edge_dir = cell_quarter_edge_direction::left;
            cinfo.qinfos[3].category = cell_quarter_category::edge;
            cinfo.qinfos[3].edge_dir = cell_quarter_edge_direction::left;
          } else if (offPattern == ( \
				    off_cell_direction::back | \
				    off_cell_direction::front)) {
            //   v
            // x + x = 0011 = 3
            //   v
            cinfo.qinfos[0].category = cell_quarter_category::edge;
            cinfo.qinfos[0].edge_dir = cell_quarter_edge_direction::front;
            cinfo.qinfos[1].category = cell_quarter_category::edge;
            cinfo.qinfos[1].edge_dir = cell_quarter_edge_direction::back;
            cinfo.qinfos[2].category = cell_quarter_category::edge;
            cinfo.qinfos[2].edge_dir = cell_quarter_edge_direction::back;
            cinfo.qinfos[3].category = cell_quarter_category::edge;
            cinfo.qinfos[3].edge_dir = cell_quarter_edge_direction::front;
          }
          _scis[offPattern] = cinfo;
        }
      }
      return _scis;
    }
                
    // static
    const cell_efield_info &
    tracker_drift_model::supported_cell_efield_info(const neighbour_cells_off_pattern off_pattern_) 
    {
      static const std::vector<cell_efield_info> & scis =  supported_cell_efield_infos();
      DT_THROW_IF(off_pattern_ >= scis.size(), std::domain_error,
                  "Unsupported cell E-field pattern");
      return scis[off_pattern_];
    }

    void tracker_drift_model::build_tracker_info(const time::time_point & timepoint_,
						 tracker_info & trkinfo_) const
    {
      trkinfo_.timestamp = timepoint_;
      trkinfo_.gas_info = this->fetch_gas_info(timepoint_);
      geomtools::geom_id allCellsGidPattern(_gg_locator_->cellGIDType(),
					    _gg_locator_->getModuleNumber(),
					    geomtools::geom_id::ANY_ADDRESS,
					    geomtools::geom_id::ANY_ADDRESS,
					    geomtools::geom_id::ANY_ADDRESS);
      std::set<geomtools::geom_id> allCellsGids;
      _gg_locator_->buildGeigerCells(allCellsGidPattern, allCellsGids);
      for (const auto & cellGid : allCellsGids) {
	auto cellInfo = this->fetch_cell_info(cellGid, timepoint_);
	trkinfo_.cell_infos[cellGid] = cellInfo;
      }
      return;
    }
    
  } // end of namespace physics_model

} // end of namespace snemo
