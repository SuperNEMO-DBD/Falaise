/// \file falaise/snemo/datamodels/event_topology.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2025-12-05
 * Last modified: 2025-12-05
 *
 * Description:  SuperNEMO Event Topology model
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_EVENT_TOPOLOGY_H
#define FALAISE_SNEMO_DATAMODELS_EVENT_TOPOLOGY_H 1

#include <vector>
#include <cstdint>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux:
#include <geomtools/base_hit.h>
#include <datatools/handle.h>

// This project:
#include <falaise/snemo/datamodels/timestamp.h>
#include <falaise/snemo/datamodels/vertex_utils.h>
#include <falaise/snemo/datamodels/reconstructed_particle.h>

namespace snemo {

  namespace datamodel {
 
    /// \brief SuperNEMO particle track
    class event_topology
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
    {
    public:
    
      event_topology() = default;

      ~event_topology() override = default;

			void set_origin_timestamp(const timestamp & origin_timestamp_);

			const timestamp & get_origin_timestamp() const;
			
			bool has_origin_vertex() const;

			const VertexHdl & get_origin_vertex_handle() const;

			const Vertex & get_origin_vertex() const;

			void set_origin_vertex(const VertexHdl & origin_vertex_handle_);

			std::size_t number_of_particles() const;

			void add_particle(const ReconstructedParticleHdl &);

			const ReconstructedParticleHdl & get_particle_handle(const std::uint32_t index_) const;

			const ReconstructedParticle & get_particle(const std::uint32_t index_) const;
			
			void set_likelihood(const double likelihood_);

			double get_likelihood() const;
			
			/// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ 
											/**/ = datatools::i_tree_dumpable::empty_options()) const override;
				
		private:

			timestamp _origin_timestamp_; ///< Reconstructed time of the origin location
			VertexHdl _origin_vertex_handle_; ///< Reconstructed origin location
			ReconstructedParticleHdlCollection _particles_; ///< Collection of reconstructed particles
			double _likelihood_ = datatools::invalid_real(); ///< Likelihood associated to the reconstructed topology
			
      DATATOOLS_SERIALIZATION_DECLARATION()

    };

    /// Handle on particle track
    using EventTopology = event_topology;
    using EventTopologyHdl = datatools::handle<EventTopology>;

    using EventTopologyCollection = std::vector<EventTopology>;
    using EventTopologyHdlCollection = std::vector<EventTopologyHdl>;


  } // end of namespace datamodel

} // end of namespace snemo

// // Class version:
// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(snemo::datamodel::event_topology, 0)

#endif // FALAISE_SNEMO_DATAMODELS_EVENT_TOPOLOGY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
