// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/particle_track.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_IPP
#define FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/particle_track.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools:
#include <geomtools/blur_spot.ipp>

// This project:
#include <falaise/snemo/geometry/utils.h>
#include <falaise/snemo/datamodels/boost_io/tracker_trajectory.ipp>
#include <falaise/snemo/datamodels/boost_io/vertex_utils.ipp>

namespace snemo {

  namespace datamodel {

    namespace io_details {

      typedef datatools::handle<geomtools::blur_spot> handle_spot;
      typedef std::vector<handle_spot> vertex_collection_type;
      void convert_vertexes(const vertex_collection_type & old_spots_, VertexHdlCollection & new_vtx_);

    }
 
    template <class Archive>
    void particle_track::serialize(Archive & ar_ , const unsigned int version_)
    {
      ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_hit);
      ar_ & boost::serialization::make_nvp("charge_from_source", charge_from_source_);
      ar_ & boost::serialization::make_nvp("trajectory", trajectory_);
      if (version_ >= 1) {
        ar_ & boost::serialization::make_nvp("vertices", vertices_);
      } else {
        // 2022-12-14 FM : Original version of the vertex collection used a vectopr of handles on datatools::blur_spot:
        if (Archive::is_loading::value) {
          io_details::vertex_collection_type old_vertices;
          ar_ & boost::serialization::make_nvp("vertices", old_vertices);
          io_details::convert_vertexes(old_vertices, vertices_); 
        }
      }
      ar_ & boost::serialization::make_nvp("associated_calorimeter_hits", associated_calorimeters_);
      
      return;
    }

    namespace io_details {
     
      void convert_vertexes(const vertex_collection_type & old_spots_, VertexHdlCollection & new_vtx_)
      {
        new_vtx_.clear();
        new_vtx_.reserve(old_spots_.size());
        for (const auto & spot : old_spots_) {
          VertexHdl hVertex = datatools::make_handle<Vertex>();
          hVertex->set_hit_id(spot->get_hit_id());
          hVertex->set_geom_id(spot->get_geom_id());
          hVertex->set_spot(*spot);
          
          if (spot->get_auxiliaries().has_key("vertex.from")) {
            std::string from = spot->get_auxiliaries().fetch_string("vertex.from");
            if (from == snemo::geometry::vertex_info::from_to_label(snemo::geometry::vertex_info::FROM_FIRST)) {
              hVertex->set_from(VERTEX_FROM_FIRST);
            }
            if (from == snemo::geometry::vertex_info::from_to_label(snemo::geometry::vertex_info::FROM_LAST)) {
              hVertex->set_from(VERTEX_FROM_LAST);
            }
          }

          if (spot->get_auxiliaries().has_key("vertex.extrapolation_mode")) {
            std::string extrapolation_mode = spot->get_auxiliaries().fetch_string("vertex.extrapolation_mode");
            if (extrapolation_mode == snemo::geometry::vertex_info::to_label(snemo::geometry::vertex_info::EXTRAPOLATION_LINE)) {
              hVertex->set_extrapolation(VERTEX_EXTRAPOLATION_LINE);
            }
            if (extrapolation_mode == snemo::geometry::vertex_info::to_label(snemo::geometry::vertex_info::EXTRAPOLATION_HELIX)) {
              hVertex->set_extrapolation(VERTEX_EXTRAPOLATION_HELIX);
            }
          }

          if (spot->get_auxiliaries().has_key("vertex.distance")) {
            double distance = spot->get_auxiliaries().fetch_real_with_explicit_dimension("vertex.distance", "length");
            hVertex->set_distance(distance);
          }

          if (spot->get_auxiliaries().has_key("vertex.distance_xy")) {
            double distance_xy = spot->get_auxiliaries().fetch_real_with_explicit_dimension("vertex.distance_xy", "length");
            hVertex->set_distance_xy(distance_xy);
          }

          if (spot->get_auxiliaries().has_key("vertex.type")) {
            std::string type = spot->get_auxiliaries().fetch_string("vertex.type");
            vertex_category_type cat = string_to_vertex_category(type);
            hVertex->set_category(cat);
          }
          
          new_vtx_.push_back(hVertex);
        }
        return;
      }

    } // end of namespace io_details
    
  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_PARTICLE_TRACK_IPP
