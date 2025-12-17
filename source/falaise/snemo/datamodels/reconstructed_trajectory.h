/// \file falaise/snemo/datamodels/reconstructed_trajectory.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2025-12-05
 * Last modified: 2025-12-05
 *
 * Description:  SuperNEMO Event Topology model
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_RECONSTRUCTED_TRAJECTORY_H
#define FALAISE_SNEMO_DATAMODELS_RECONSTRUCTED_TRAJECTORY_H 1

// Standard library:
#include <cstdint>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO reconstructed particle
    class reconstructed_trajectory
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
    {
    public:
      
      reconstructed_trajectory() = default;

      ~reconstructed_trajectory() override = default;

      std::size_t number_of_chained_tracks() const;
      
      void add_chained_track(const ParticleHdl & track_);
      
    private:

      ParticleHdlCollection _chained_tracks_;

      DATATOOLS_SERIALIZATION_DECLARATION()
			
    };

    /// Handles on reconstructed trajectory instances
    using ReconstructedTrajectory = reconstructed_trajectory;
    using ReconstructedTrajectoryHdl = datatools::handle<ReconstructedTrajectory>;

    using ReconstructedTrajectoryCollection = std::vector<ReconstructedTrajectory>;
    using ReconstructedTrajectoryHdlCollection = std::vector<ReconstructedTrajectoryHdl>;

  } // end of namespace datamodel

} // end of namespace snemo

// // Class version:
// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(snemo::datamodel::reconstructed_trajectory, 0)

#endif // FALAISE_SNEMO_DATAMODELS_RECONSTRUCTED_TRAJECTORY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
