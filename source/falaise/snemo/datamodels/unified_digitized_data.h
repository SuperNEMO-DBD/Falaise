// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/unified_digitized_data.h
/* Author(s) :    François Mauger <mauger@lal.in2p3.fr>
 * Creation date: 2016-12-06
 * Last modified: 2016-12-06
 *
 * License:
 *
 * Description:
 *
 *   SuperNEMO unified digitized data model
 *
 */

#ifndef FALAISE_SNEMO_DATAMODEL_UNIFIED_DIGITIZED_DATA_H
#define FALAISE_SNEMO_DATAMODEL_UNIFIED_DIGITIZED_DATA_H

// Standard library:
#include <string>
#include <vector>

namespace snemo {

  namespace datamodel {

    /// \brief The data structure that hosts information about unified digitized
    ///        data, both for real data and simulated data
    class unified_digitized_data
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
      , public datatools::i_clear
    {

    public:

      /// Constructor
      unified_digitized_data();

      /// Destructor
      virtual ~unified_digitized_data();

      /// Return the const container of auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /// Return the mutable container of auxiliary properties
      datatools::properties & grab_auxiliaries();

      /// Reset
      void reset();

      /// Clear attributes
      virtual void clear();

      /// Smart print
      virtual void tree_dump(std::ostream & a_out         = std::clog,
                             const std::string & a_title  = "",
                             const std::string & a_indent = "",
                             bool a_inherit          = false) const;


    private:

      datatools::properties _auxiliaries_; //!< Auxiliary properties

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::unified_digitized_data,
                        "snemo::datamodel::unified_digitized_data")

// // Class version:
// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(snemo::datamodel::unified_digitized_data, 1)

#endif // FALAISE_SNEMO_DATAMODEL_UNIFIED_DIGITIZED_DATA_H
