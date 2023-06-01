/// \file falaise/snemo/datamodels/vertex_utils.h
/* Author (s) : Mathieu Bongrand <bongrand@lal.in2p3.fr>
 *              Xavier Garrido <garrido@lal.in2p3.fr>
 *              Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-17
 * Last modified: 2022-12-13
 *
 * Description: Some utility to handle vertexes associated to particle tracks
 */

#ifndef FALAISE_SNEMO_DATAMODELS_VERTEX_UTILS_H
#define FALAISE_SNEMO_DATAMODELS_VERTEX_UTILS_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/geomtools:
#include <bayeux/datatools/handle.h>
#include <bayeux/geomtools/base_hit.h>
#include <bayeux/geomtools/blur_spot.h>

// This project:
#include <falaise/snemo/datamodels/calibrated_calorimeter_hit.h>
#include <falaise/snemo/datamodels/tracker_trajectory.h>
#include <falaise/snemo/geometry/utils.h>

namespace snemo {

  namespace datamodel {

    /// Supported categories of vertex
    enum vertex_category_type {
      VERTEX_CATEGORY_UNDEF = 0x0, ///< Undefined category
      VERTEX_CATEGORY_ON_SOURCE_FOIL = datatools::bit_mask::bit00, ///< Maybe a source strip, pad or pad bulk physica volume
      VERTEX_CATEGORY_ON_MAIN_CALORIMETER = datatools::bit_mask::bit01, ///< The entrance window of a main calorimeter block (front)
      VERTEX_CATEGORY_ON_X_CALORIMETER = datatools::bit_mask::bit02, ///< The entrance window of a main X-calorimeter block
      VERTEX_CATEGORY_ON_GAMMA_VETO = datatools::bit_mask::bit03, ///< The bulk volume of a gamma veto block
      VERTEX_CATEGORY_ON_WIRE = datatools::bit_mask::bit04, ///< The vicinity of a wire in the tracking chamber
      VERTEX_CATEGORY_ON_CALIBRATION_SOURCE = datatools::bit_mask::bit05, ///< A calibration source
      VERTEX_CATEGORY_ON_SOURCE_GAP = datatools::bit_mask::bit06 ///< A gap between source pads
    };

    vertex_category_type convert(const snemo::geometry::vertex_info::category_type cat_);
    
    /// Associated 'VERTEX_UNDEF_NONE' flag
    const std::string & vertex_undef_label();

    /// Associated 'VERTEX_CATEGORY_ON_SOURCE_FOIL' flag 
    const std::string & vertex_on_source_foil_label();

    /// Associated 'VERTEX_CATEGORY_ON_MAIN_CALORIMETER' flag
    const std::string & vertex_on_main_calorimeter_label();

    /// Associated 'VERTEX_CATEGORY_ON_X_CALORIMETER' flag
    const std::string & vertex_on_x_calorimeter_label();

    /// Associated 'VERTEX_CATEGORY_ON_GAMMA_CALORIMETER' flag
    const std::string & vertex_on_gamma_veto_label();

    /// Associated 'VERTEX_CATEGORY_ON_WIRE' flag
    const std::string & vertex_on_wire_label();

    /// Associated 'VERTEX_CATEGORY_ON_CALIBRATION_SOURCE' flag
    const std::string & vertex_on_calibration_source_label();

    /// Associated 'VERTEX_CATEGORY_ON_SOURCE_GAP' flag
    const std::string & vertex_on_source_gap_label();

    /// Conversion to a string
    const std::string & to_string(const vertex_category_type);
    
    /// Conversion to a vertex category
    vertex_category_type string_to_vertex_category(const std::string &);

    /// From end extrapolation
    enum vertex_from_type {
      VERTEX_FROM_UNDEF = 0,
      VERTEX_FROM_FIRST = 1,
      VERTEX_FROM_LAST  = 2
    };

    /// Conversion to a string
    const std::string & to_string(const vertex_from_type);
    
    /// Conversion to a vertex from
    vertex_from_type string_to_from(const std::string &);

    /// Vertex extrapolation mode
    enum vertex_extrapolation_type {
      VERTEX_EXTRAPOLATION_UNDEF = 0,
      VERTEX_EXTRAPOLATION_LINE  = 1,
      VERTEX_EXTRAPOLATION_HELIX = 2
    };

    vertex_extrapolation_type convert(const snemo::geometry::vertex_info::extrapolation_mode_type gext_);
 
    /// Conversion to a string
    const std::string & to_string(const vertex_extrapolation_type);

    /// Conversion to a vertex extrapolation
    vertex_extrapolation_type string_to_extrapolation(const std::string &);
  
    /// \brief Vertex
    class vertex
      : public geomtools::base_hit
    {
    public:
     
      /// Default constructor
      vertex() = default;

      // Destructor
      virtual ~vertex() override = default;

      // Copy constructor
      vertex(const vertex &) = default;

      // Copy assignment
      vertex & operator=(const vertex &) = default;

      // Move constructor
      vertex(vertex &&);

      // Move assignment
      vertex & operator=(vertex &&) = default;

      /// Reset
      void reset();

      /// Return the vertex category
      vertex_category_type get_category() const;

      /// Set the vertex category
      void set_category(const vertex_category_type);

      /// Check if the vertex category match a category mask
      bool match_categories(const uint32_t category_flags_) const;
      
      /// Check a vertex on source foil
      bool is_on_source_foil() const;
      
      /// Check a vertex on main calorimeter
      bool is_on_main_calorimeter() const;

      /// Check a vertex on X calorimeter
      bool is_on_x_calorimeter() const;

      /// Check a vertex on gamma veto
      bool is_on_gamma_veto() const;

      /// Check a vertex on wire
      bool is_on_wire() const;

      /// Check a vertex on calibration source
      bool is_on_calibration_source() const;

      /// Check a vertex on calibration source
      bool is_on_source_gap() const;

      /// Set the spot
      void set_spot(const geomtools::blur_spot &);

      /// Return a reference to a mutable spot
      geomtools::blur_spot & get_spot();

      /// Return a reference to a non mutable spot
      const geomtools::blur_spot & get_spot() const;

      /// Return the from tag
      vertex_from_type get_from() const;

      /// Set the from tag
      void set_from(const vertex_from_type);

      /// Check the edge flag
      bool is_edge() const;
      
      /// Set the edge tag
      void set_edge(const bool);

      /// Check the best flag
      bool is_best() const;
      
      /// Set the best tag
      void set_best(const bool);

      /// Return the from tag
      vertex_extrapolation_type get_extrapolation() const;

      /// Set the extrapolation tag
      void set_extrapolation(const vertex_extrapolation_type);

      /// Return extrapolation distance
      double get_distance() const;

      /// Set extrapolation distance
      void set_distance(const double distance_);

      /// Return extrapolation distance in the XY plane
      double get_distance_xy() const;

      /// Set extrapolation distance in the XY plane
      void set_distance_xy(const double distance_xy_);
 
      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ 
                              /**/ = datatools::i_tree_dumpable::empty_options()) const override;
      
    private:

      vertex_category_type _category_ = VERTEX_CATEGORY_UNDEF; ///< Vertex category
      vertex_from_type _from_ = VERTEX_FROM_UNDEF; ///< From flag
      vertex_extrapolation_type _extrapolation_ = VERTEX_EXTRAPOLATION_UNDEF; ///< Extrapolation mode
      geomtools::blur_spot _spot_; ///< Vertex spot
      double _distance_ = datatools::invalid_real(); ///< 3D extrapolation distance from the end of the track to the spot
      double _distance_xy_ = datatools::invalid_real(); ///< XY extrapolation distance from the end of the track to the spot
      bool _edge_ = false; ///< Edge flag: set if the spot is near the edge of the target volume/surface
      bool _best_ = false; ///< Best flag
      
      DATATOOLS_SERIALIZATION_DECLARATION()
     
    };

    /// Handle on particle track
    using Vertex = vertex;
    using VertexHdl = datatools::handle<Vertex>;
    using VertexCollection = std::vector<Vertex>;
    using VertexHdlCollection = std::vector<VertexHdl>;

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_VERTEX_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
