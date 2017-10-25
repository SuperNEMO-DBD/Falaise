/// \file falaise/snemo/datamodels/topology_1e1Ng_pattern.h
/* Author(s) :    Steven Calvez <calvez@lal.in2p3.fr>
 * Creation date: 2015-10-24
 * Last modified: 2015-10-24
 *
 * Description: The 1e1Ng class of trajectory pattern
 */

#ifndef FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1ENG_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1ENG_PATTERN_H 1

// This project:
#include <falaise/snemo/datamodels/topology_1e_pattern.h>
#include <falaise/snemo/datamodels/tof_measurement.h>
#include <falaise/snemo/datamodels/energy_measurement.h>

namespace snemo {

  namespace datamodel {

    /// \brief The 1e1Ng class of reconstructed topology
    class topology_1eNg_pattern : public topology_1e_pattern
    {
    public:

      /// Typedef for TOF collection
      typedef std::vector<snemo::datamodel::tof_measurement::probability_type> tof_collection_type;

      /// Typedef for energy measurement collection
      typedef std::vector<double> energy_collection_type;

      /// Static function to return pattern identifier of the pattern
      static const std::string & pattern_id();

      /// Return pattern identifier of the pattern
      virtual std::string get_pattern_id() const;

      /// Constructor
      topology_1eNg_pattern();

      /// Destructor
      virtual ~topology_1eNg_pattern();

      /// Check number of gammas validity
      bool has_number_of_gammas() const;

      /// Set number of gammas
      void set_number_of_gammas(const size_t ngammas_);

      /// Return the number of gammas
      size_t get_number_of_gammas() const;

      /// Check gammas energy existence
      bool has_gammas_energies() const;

      /// Fetch gammas energies
      void fetch_gammas_energies(energy_collection_type & energies_) const;

      /// Check electron-gammas TOF probabilities existence
      bool has_electron_gammas_tof_probabilities() const;

      /// Fetch the electron-gammas internal TOF probability
      void fetch_electron_gammas_internal_probabilities(tof_collection_type & eg_pint_) const;

      /// Fetch the electron-gammas external TOF probability
      void fetch_electron_gammas_external_probabilities(tof_collection_type & eg_pext_) const;

    private:

      size_t _number_of_gammas_; //!< Number of gamma in the topology

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::topology_1eNg_pattern,
                        "snemo::datamodel::topology_1eNg_pattern")

#endif // FALAISE_SNEMO_DATAMODEL_TOPOLOGY_1ENG_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
