/// \file falaise/snemo/datamodels/raw_trigger_infos.h
/* Author(s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-03-19
 * Last modified: 2014-01-28
 *
 * Description: The SuperNEMO raw trigger informations
 */

#ifndef FALAISE_SNEMO_DATAMODEL_RAW_TRIGGER_INFOS_H
#define FALAISE_SNEMO_DATAMODEL_RAW_TRIGGER_INFOS_H 1

// Standard library:
#include <bitset>

// Third party:
// - Boost :
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>

namespace snemo {

  namespace datamodel {

    /// \brief The data structure that hosts information about the raw trigger informations
    class raw_trigger_infos : public datatools::i_serializable,
                              public datatools::i_tree_dumpable
    {
    public:

      static const unsigned int NUMBER_OF_SIDES       = 2;
      static const unsigned int NUMBER_OF_ROWS        = 113;
      static const unsigned int NUMBER_OF_LAYERS      = 9;
      static const unsigned int NUMBER_OF_CALO_ZONES  = 10;
      static const unsigned int NUMBER_OF_GVETO_ZONES = 3;

      /// Set the trigger number
      void set_trigger_number(uint32_t);

      /// Return the trigger number
      uint32_t get_trigger_number() const;

      /// Return a const reference to the SVTZW for a given side
      const std::bitset<3> & get_vtzw(int side_) const;

      /// Return a mutable reference to the SVTZW for a given side
      std::bitset<3> & grab_vtzw(int side_);

      /// Return a const reference to the SCTZW for a given side
     const std::bitset<10> & get_ctzw(int side_) const;

      /// Return a mutable reference to the SCTZW for a given side
      std::bitset<10> & grab_ctzw(int side_);

      /// Return a const reference to the STTP for a given side
      const std::bitset<9> & get_ttp(int side_, int row_) const;

      std::bitset<9> & grab_ttp(int side_, int row_);

      /// Constructor
      raw_trigger_infos();

      /// Destructor
      virtual ~raw_trigger_infos();

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream & a_out         = std::clog,
                             const std::string & a_title  = "",
                             const std::string & a_indent = "",
                             bool a_inherit               = false) const;

    private:

      uint32_t _trigger_number_; //!< The trigger number

      // Trigger primitives:
      std::bitset<NUMBER_OF_GVETO_ZONES> _VTZW_[NUMBER_OF_SIDES]; //!< Gamma-veto trigger zoning words per side
      std::bitset<NUMBER_OF_CALO_ZONES>  _CTZW_[NUMBER_OF_SIDES]; //!< Calorimeter trigger zoning words per side
      std::bitset<NUMBER_OF_LAYERS>      _TTP_[NUMBER_OF_SIDES][NUMBER_OF_ROWS]; //!< Tracker trigger primitives per side and per row

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODEL_RAW_TRIGGER_INFOS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
