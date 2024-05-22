/// \file falaise/snemo/physics_model/tracker_drift_model.hpp
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2024-04-09
 * Last modified: 2024-04-11
 *
 * Description: Tracker drift model
 *
 * Source: 
 *  - SuperNEMO Drift Model, Betsy Landells (DocDB 5594)
 *  - SuperNEMO Drift Model, Betsy Landells (DocDB 5839)
 *
 */

#ifndef FALAISE_SNEMO_PHYSICS_MODEL_TRACKER_DRIFT_MODEL_H
#define FALAISE_SNEMO_PHYSICS_MODEL_TRACKER_DRIFT_MODEL_H

// Standard library:
#include <optional>
#include <tuple>
#include <limits>
#include <map>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/datatools/bit_mask.h>

// This project:
#include <falaise/snemo/time/time_utils.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include "falaise/snemo/services/service_traits.h"
#include <falaise/snemo/services/service_handle.h>
#include <falaise/snemo/services/tracker_cell_status_service.h>
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/services/services.h>

namespace snemo {

  namespace physics_model {

    /// \brief Cell quarter category
    ///
    /// A tracker cell is divided in 4 quarters (see DocDb #5839: p. 24, fig. 15)
    /// corresponding to regions with
    /// specific electric field conditions, mainly depending on the environnement
    /// of the cell: neighbouring cells with HV applied or not, edge of the tracking chamber...
		/// Each quarter is identified in a given cell with a conventional index from 0 to 3, with respect to
		/// the cell reference frame in the horizontal XY plane:
		///
		///               ^ Y+
		///               :
		///               :     a cell and its 4 quarters
    ///       +---------------+  
    ///       |       .       |
    ///       |   1   .   0   |
    ///       |       .       |
    ///   ----| - - - o - - - |-----> X+
    ///       |       .       |
    ///       |   2   .   3   |
    ///       |       .       |
    ///       +---------------+  
    ///               :
		///
    /// Four distinct categories of cell quarter are defined (see DocDB #5839):
    ///
    ///   - "centre" (ce) : a cell quarter with HV applied on both first neighbour cells
    ///     Example:
    ///
    ///       +-------+-------+--  
    ///       |HV any | HV on |
    ///       |   o   |   o   |
    ///       |       |       |
    ///       +-------+-------+-- 
    ///       | HV on |ce :   | 
    ///       |   o   |---o---| <--- a cell and 3 of its neighbour cells
    ///       |       |   :   |
    ///       +-------+-------+--
		///       |       |       |
    ///
    ///   - "edge" (ed) : a cell quarter with HV applied on only one neighbour cell, and not on the other one
    ///
    ///     Example:
    ///       +-------+-------+  
    ///       |HV any | HV on |
    ///       |   o   |   o   |
    ///       |       |       |
    ///       +-------+-------+  
    ///       | HV off|ed :   |
    ///       |   o   |---o---|
    ///       |       |   :   |
    ///       +-------+-------+  
    ///
    ///   - "corner" (co) : a cell quarter with HV not applied on both first neighbour cells
    ///
    ///     Example:
    ///       +-------+-------+  
    ///       |HV any | HV off|
    ///       |   o   |   o   |
    ///       |       |       |
    ///       +-------+-------+  
    ///       | HV off|co :   |
    ///       |   o   |---o---|
    ///       |       |   :   |
    ///       +-------+-------+  
    ///
    enum class cell_quarter_category {
      none,    ///< Undefined category
			centre,  ///< Centre quarter (both first neighbour cells with HV on)
			edge,    ///< Edge quarter (one neighbour cell with HV on, and one with HV off or missing)
			corner   ///< Corner quarter (both first neighbour cells with HV off or missing)
     };
    
		std::ostream & operator<<(std::ostream & out_, const cell_quarter_category cat_);

    /// \brief Cell quarter edge category
    ///
    /// This information encodes the direction of the neighbour cell with HV OFF in case of the 'edge' category.
    /// This direction is relative to the official reference frame in the horizontal (XY) plane. This information
    /// is important to properly take into account the asymmetry of the electric field for an 'edge' quarter. 
    ///
    /// Examples:
    ///
    /// * Edge category with "back" direction:
    ///
    ///                 ^ Y+
    ///                 :
    ///               right
    ///         +-------+-------+  
    ///         |       | HV on |
    ///         |   o   |   o   |
    ///         |       |       |
    ///   back  +-------+-------+ front -> X+
    ///         |HV off <ed :   |
    ///         |   o   |---o---|
    ///         |       |   :   |
    ///         +-------+-------+
    ///               left
    ///
    /// * Edge category with "right" direction:
    ///
    ///                 ^ Y+
    ///                 :
    ///               right
    ///         +-------+-------+  
    ///         |       | HV off|
    ///         |   o   |   o   |
    ///         |       |       |
    ///   back  +-------+-^-----+ front -> X+
    ///         |HV on  |ed :   |
    ///         |   o   |---o---|
    ///         |       |   :   |
    ///         +-------+-------+
    ///               left
    ///
    enum class cell_quarter_edge_direction {
      none,
      back,
      front,
      left,
      right
    };

		std::ostream & operator<<(std::ostream & out_, const cell_quarter_edge_direction dir_);

    /// This bit mask locate the position of n eighbourg cell with HV off with respect
    /// to a given cell used as a reference:
    ///
    /// Example: for a cell with two of its four neighbour cells with HV off, the associated 
    /// 'neighbour_cells_off_pattern' bitset is thus: ``off_cell_direction::front | off_cell_direction::right``
    ///
    ///                     ^ Y+
    ///                     :
    ///                   right
    ///         +-------+-------+-------+  
    ///         |       | HV off|       |
    ///         |   o   |   o   |   o   |
    ///         |       |       |       |
    ///         +-------+-------+-------+
    ///         | HV on |   :   | HV off|
    ///   back  |   o   |---o---|   o   | front -> X+
    ///         |       |   :   |       |
    ///         +-------+-------+-------+
    ///         |       | HV on |       |
    ///         |   o   |   o   |   .   |
    ///         |       |       |   o   |
    ///         +-------+-------+-------+
    ///                   left
    ///
    struct off_cell_direction {
      static const std::uint32_t back  = datatools::bit_mask::bit00; ///< 0001 == 000B
      static const std::uint32_t front = datatools::bit_mask::bit01; ///< 0010 == 00F0
      static const std::uint32_t left  = datatools::bit_mask::bit02; ///< 0100 == 0L00
      static const std::uint32_t right = datatools::bit_mask::bit03; ///< 1000 == R000
    };

    /// A cell OFF-pattern consists in a bitset of 4 bits using the conventional *RLFB* order.
		/// 
    /// Each bit corresponds to the E-field OFF status of some neighbour cells
    /// in a given direction with respect to a given cell.
    ///
    ///          right
    ///           ^ (Y+)
    ///           :
    ///  back --[cell]--> front  (X+)
    ///           :
    ///          left
    ///
    /// A "off_cell_direction" bit is set when the E-field is OFF or also if
    /// there is no cell in this direction (edge or corner of a tracker half-chamber)
    ///
    /// Examples:
		///
    ///   rlfb 
    ///   0010 : front cell's HV is OFF or (side=0,layer=0) or (side=1,layer=8)
    ///   0110 : front and left cells' HVs are OFF or (side=0,layer=0,row=0) or (side=1,layer=8,row=0)
    ///
    typedef std::uint32_t neighbour_cells_off_pattern;

		std::string format_ncop(const neighbour_cells_off_pattern ncop_);
		
    /// \brief Description of a cell quarter in terms of field layout
    ///
    /// This encodes the category of the cell quarter and the edge direction for 'edge' quarter.
    ///
    struct cell_quarter_info
    {
      cell_quarter_category category = cell_quarter_category::centre;
      cell_quarter_edge_direction edge_dir = cell_quarter_edge_direction::none;
    };

		std::ostream & operator<<(std::ostream & out_, const cell_quarter_info & cinfo_);

    /// \brief Electric field description of a cell with its 4 quarters
    struct cell_efield_info
    {
      cell_quarter_info qinfos[4];
			void print_tree(std::ostream & out_, const boost::property_tree::ptree & options_) const;
    };

    /// \brief General informations about a cell
    struct cell_info
    {
			bool off = false;
			const cell_efield_info * efield_info = nullptr;
			void print_tree(std::ostream & out_, const boost::property_tree::ptree & options_) const;
			void export_viewer(std::ostream & out_, const double xcell_, const double ycell_, const double rcell_) const;
		};
			
    /// \brief Description of the tracker gas
    struct tracker_gas_info
    {
      /// Tracker gas pressure
      double pressure = 880.0e-3 * CLHEP::bar;
    };

    /// \brief Encode informations usable by the drift model about the tracker
    struct tracker_info
    {
			time::time_point timestamp;
      tracker_gas_info gas_info;
			std::map<geomtools::geom_id,cell_info> cell_infos;
    };

    /// \brief Drift parameters associated to a cell quarter (see DocDb #5839, page 42, tables 2-3)
    struct cell_drift_fit_parameters
    {
      double x   = std::numeric_limits<double>::quiet_NaN(); ///< Radius cut
      double tx  = std::numeric_limits<double>::quiet_NaN(); ///< Drift time cut
      // Fit parameters
      double alo = std::numeric_limits<double>::quiet_NaN();
      double blo = std::numeric_limits<double>::quiet_NaN();
      double a   = std::numeric_limits<double>::quiet_NaN();
      double b   = std::numeric_limits<double>::quiet_NaN();
      double ahi = std::numeric_limits<double>::quiet_NaN();
      double bhi = std::numeric_limits<double>::quiet_NaN();
      
      /// Evaluate the drift time associated to a given radius using formula in DocDb #5839, p. 25, eq. 16.
      /// The model does not take into account possible angular assymmetry (see DocDb #5839, section p.28-30,
      /// fig. 24).
      ///
      /// @arg radius_ The radial distance to the central anode wire
      /// @arg error_ The error mode (0 : central value, -1 : -1 sigma value, +1 : +1 sigma value)
      /// @return the drift value associated to the error mode (for details see see DocDb #5839:
      /// p. 23, fig. 14; p. 25, fig. 17; p. 28, fig. 21-22)
      double raw_eval(const double & radial_distance_, const int error_ = 0) const;
    };

    /// \brief Tracker drift model
		///
		/// This model implements large parts of the approach described in
		/// DocDb #5839 (Betsy Landells' MSc Project), namely the so-called "betsy" model.
    class tracker_drift_model
    {
    public:

      tracker_drift_model(const datatools::logger::priority verbosity_ = datatools::logger::PRIO_FATAL);
 
      virtual ~tracker_drift_model();

      datatools::logger::priority get_verbosity() const;
      
      void set_verbosity(datatools::logger::priority);

			/// Initialize the tracker drift model
      void initialize(const datatools::properties & config_,
                      const datatools::service_dict_type & services_);

			/// Reset/terminate the tracker drift model
      void reset();

			/// Return a reference to the geometry tracker/Geiger cell locator			
			const snemo::geometry::gg_locator & gg_locator() const;

			/// Return a reference to the tracker cell status service			
			const snemo::tracker_cell_status_service & tcss() const;
			
			/// Check if a default gas pressure is applied (regardless the effective real gas pressure of the experiment)
      bool has_default_gas_pressure() const;
 
			/// Check if a default neighbour cell OFF pattern is applied (regardless the effective pattern associate to each cells during the lifetime of the experiment)
      bool has_default_neighbour_cells_off_pattern() const;

      /// Return the quarter index from a position relative to the center of a cell
      int locate_cell_quarter(const geomtools::vector_3d & in_cell_position_) const;

			/// Return information about the tracker gas at given time
      tracker_gas_info fetch_gas_info(const time::time_point & p_) const;

			/// Return the *neighbour cells OFF pattern* associated to given cell and time
      neighbour_cells_off_pattern fetch_cell_off_pattern(const geomtools::geom_id & gid_,
                                                         const time::time_point & timepoint_) const;

      /// \brief Return the cell info object associated to a given tracker cell and timepoint.
      ///
      /// The *cell info* object depends on the placement of the cell in the tracker geometry
      /// (central cell, cell near an edge, cell at a corner) but also, after the "betsy" drift model,
      /// on the HV status of its first neighbourg cells (back, front, left, right).
      /// The geometry layout of the cell is of course not mutable. However, the HV status (on/off) of neighbourg cells 
      /// may change during the lifetime of the experiment. As a consequence, the shape of the electric field
      /// in some cell quarters can be modified. A request to the *tracker cell status service* can be done
      /// to update the effective cell info at a given timepoint,
      /// taking into account possible changes of some quarter's categories with respect to their nominal value.
      cell_info fetch_cell_info(const geomtools::geom_id & gid_,
																const time::time_point & timepoint_) const;

      /// Return the drift model parameters associated to given quarter category and pressure.
      /// Two radial distance regions are defined (small: typically r < 22mm, and large: typically r > 22 mm).
      /// For details, see DocDb #5839, section *Optimal transition region*, p30-32.
      cell_drift_fit_parameters fetch_cell_fit_params(const cell_quarter_category cqc_,
                                                      const double pressure_,
                                                      bool large_radius_) const;

      /// Compute a triplet of the predicted drift times (-1 sigma, central, +1 sigma) associated
      /// to given cell quarter category, gas pressure and radial distance
      ///
      /// 2024-04-11, FM: Should we add support for the polar angle too ?
      std::tuple<double,double, double>
      compute_drift_time(const cell_quarter_category cqc_,
                         const double pressure_,
                         const double radial_distance_) const;

      /// Return the list of 16 supported cell info patterns, after the "betsy" model
      static const std::vector<cell_efield_info> & supported_cell_efield_infos();

      /// Return the cell information associated to a given cell OFF pattern, after the "betsy" model
      static const cell_efield_info & supported_cell_efield_info(const neighbour_cells_off_pattern off_pattern_);

			/// Populate a tracker info object for a given timepoint
			void build_tracker_info(const time::time_point & timepoint_,
															tracker_info & trkinfo_) const;
			
    private:

      /// Return the default *geometric" cell off pattern associated to a given tracker cell
      ///
      /// This bitset may be updated later, taking into account the status of the HV of neighbourg cells.
      neighbour_cells_off_pattern _fetch_default_neighbour_cells_off_pattern_(const geomtools::geom_id & gid_) const;

    private:

      datatools::logger::priority _verbosity_ = datatools::logger::PRIO_FATAL; ///< Verbosity
      std::string _model_ = "betsy"; ///< Drift model name
      unsigned int _version_ = 0; ///< Drift model version

      // Internal services and tools:
      const geomtools::manager * _geomgr_ = nullptr; ///< Geometry manager
      const snemo::geometry::gg_locator * _gg_locator_ = nullptr; ///< Geometry tracker/Geiger cell locator
      const snemo::db_service * _db_ = nullptr; ///< Service for database access
      const snemo::tracker_cell_status_service * _tcss_ = nullptr; ///< Service for dynamic tracker cell status

      // For 'betsy' model: 
      std::optional<double> _default_gas_pressure_; ///< Example: 880 millibar
      std::optional<neighbour_cells_off_pattern> _default_neighbour_cells_off_pattern_; ///< Example: Cell with all neighbour cells ON = 0000
    
    };

  } // end of namespace physics_model

} // end of namespace snemo

#endif // FALAISE_SNEMO_PHYSICS_MODEL_TRACKER_DRIFT_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/



