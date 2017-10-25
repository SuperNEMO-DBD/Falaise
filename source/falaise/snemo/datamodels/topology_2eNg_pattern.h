/// \file falaise/snemo/datamodels/topology_2eNg_pattern.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-27
 *
 * Description: The 2eNg class of trajectory patterns
 */

#ifndef FALAISE_SNEMO_DATAMODEL_TOPOLOGY_2ENG_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_TOPOLOGY_2ENG_PATTERN_H 1

// This project:
#include <falaise/snemo/datamodels/topology_2e_pattern.h>
#include <falaise/snemo/datamodels/tof_measurement.h>
#include <falaise/snemo/datamodels/energy_measurement.h>

namespace snemo {

  namespace datamodel {

    /// \brief The 2 electron - N gammas class of reconstructed topology
    class topology_2eNg_pattern : public topology_2e_pattern
    {
    public:

      /// Typedef for TOF dictionnary
      typedef std::vector<snemo::datamodel::tof_measurement::probability_type> tof_collection_type;

      /// Typedef for energy dictionnary
      typedef std::vector<double> energy_collection_type;

      /// Static function to return pattern identifier of the pattern
      static const std::string & pattern_id();

      /// Return pattern identifier of the pattern
      virtual std::string get_pattern_id() const;

      /// Constructor
      topology_2eNg_pattern();

      /// Destructor
      virtual ~topology_2eNg_pattern();

      /// Check number of gammas validity
      bool has_number_of_gammas() const;

      /// Set number of gammas
      void set_number_of_gammas(const size_t ngammas_);

      /// Return internal probability
      size_t get_number_of_gammas() const;

      /// Check gammas energies existence
      bool has_gammas_energies() const;

      /// Fetch the gammas energies
      void fetch_gammas_energies(energy_collection_type & g_energies_) const;

      /// Check electrons-gammas TOF probabilities existence
      bool has_electrons_gammas_tof_probabilities() const;

      /// Fetch the electrons-gammas internal TOF probability
      void fetch_electrons_gammas_internal_probabilities(tof_collection_type & eg_pint_) const;

      /// Fetch the electrons-gammas external TOF probability
      void fetch_electrons_gammas_external_probabilities(tof_collection_type & eg_pext_) const;

      /// Check electron_min-gammas TOF probabilities existence
      bool has_electron_min_gammas_tof_probabilities() const;

      /// Fetch the electron_min-gammas internal TOF probability
      void fetch_electron_min_gammas_internal_probabilities(tof_collection_type & eg_pint_) const;

      /// Fetch the electron_min-gammas external TOF probability
      void fetch_electron_min_gammas_external_probabilities(tof_collection_type & eg_pext_) const;

      /// Check electron_max-gammas TOF probabilities existence
      bool has_electron_max_gammas_tof_probabilities() const;

      /// Fetch the electron_max-gammas internal TOF probability
      void fetch_electron_max_gammas_internal_probabilities(tof_collection_type & eg_pint_) const;

      /// Fetch the electron_max-gammas external TOF probability
      void fetch_electron_max_gammas_external_probabilities(tof_collection_type & eg_pext_) const;

    private:

      size_t _number_of_gammas_;//!< Number of gamma in the topology

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::topology_2eNg_pattern,
                        "snemo::datamodel::topology_2eNg_pattern")

#endif // FALAISE_SNEMO_DATAMODEL_TOPOLOGY_2ENG_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
