/// \file falaise/snemo/datamodels/reconstructed_particle.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2025-12-05
 * Last modified: 2025-12-05
 *
 * Description:  SuperNEMO reconstructed particle
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_RECONSTRUCTED_PARTICLE_H
#define FALAISE_SNEMO_DATAMODELS_RECONSTRUCTED_PARTICLE_H 1

// This project:
#include <falaise/snemo/datamodels/particle_track.h>

// Bayeux:
#include <bayeux/genbb_help/pdg_particle_tools.h>

namespace snemo {

  namespace datamodel {

		typedef genbb::pdg::particle::particle_code particle_code;
 
    /// \brief SuperNEMO reconstructed particle
    class reconstructed_particle
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
    {
    public:
      
      reconstructed_particle() = default;

      ~reconstructed_particle() override = default;

			bool is_gamma() const;

			void set_gamma();

			bool is_electron() const;

			void set_electron();

			bool is_positron() const;

			void set_positron();
	
			bool is_alpha() const;

			void set_alpha();

			bool is_muon() const;

			void set_muon_minus();

			void set_muon_plus();
			
			particle_code get_particle_code() const;

			void set_particle_code(const particle_code);

			void set_prompt();

			void set_delayed();
			
			bool is_delayed() const;

			bool is_prompt() const;

			// const ParticleHdl & get_track_handle() const;

			// const Particle & get_track() const;
	
			// void set_track_handle(const ParticleHdl & track_);
			
			/// Smart print
      void print_tree(std::ostream & out_ = std::clog,
											const boost::property_tree::ptree & options_ 
											/**/ = datatools::i_tree_dumpable::empty_options()) const override;
				
    private:

			particle_code _particle_code_ = genbb::pdg::particle::INVALID_CODE; ///< PDG particle code
			timestamp _start_timestamp_; ///< Reconstructed timestamp at particle's start of track
			timestamp _stop_timestamp_; ///< Reconstructed timestamp at particle's stop of track
			bool _delayed_ = false; ///< Delayed particle flag
			//ParticleHdl _track_handle_; ///< Handle to the particle's reconstructed track

			DATATOOLS_SERIALIZATION_DECLARATION()
			
    };

    /// Handles on reconstructed particle instances
    using ReconstructedParticle = reconstructed_particle;
    using ReconstructedParticleHdl = datatools::handle<ReconstructedParticle>;

    using ReconstructedParticleCollection = std::vector<ReconstructedParticle>;
    using ReconstructedParticleHdlCollection = std::vector<ReconstructedParticleHdl>;

  } // end of namespace datamodel

} // end of namespace snemo

// // Class version:
// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(snemo::datamodel::reconstructed_particle, 0)

#endif // FALAISE_SNEMO_DATAMODELS_RECONSTRUCTED_PARTICLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
