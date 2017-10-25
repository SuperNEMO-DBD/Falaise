/// \file falaise/snemo/datamodels/topology_data.h
/* Author (s) : Steven Calvez  <calvez@lal.in2p3.fr>
 *              Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2015-03-31
 * Last modified: 2015-03-31
 *
 * Description:  SuperNEMO Topology data model
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TOPOLOGY_DATA_H
#define FALAISE_SNEMO_DATAMODELS_TOPOLOGY_DATA_H 1

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/i_clear.h>
#include <bayeux/datatools/handle.h>
#include <bayeux/datatools/properties.h>

// This project:
#include <falaise/snemo/datamodels/base_topology_pattern.h>

namespace snemo {

  namespace datamodel {

    /// \brief SuperNEMO topology data model
    class topology_data : public datatools::i_serializable,
                          public datatools::i_tree_dumpable,
                          public datatools::i_clear
    {
    public:

      /// Handle on topology pattern
      typedef datatools::handle<base_topology_pattern> handle_pattern;

      /// Default constructor
      topology_data();

      /// Destructor
      virtual ~topology_data();

      /// Check if the pattern is present
      bool has_pattern() const;

      /// Detach the pattern
      void detach_pattern();

      /// Attach a pattern by handle
      void set_pattern_handle(const handle_pattern & pattern_handle_);

      /// Return a mutable reference on the pattern handle
      handle_pattern & grab_pattern_handle();

      /// Return a non mutable reference on the pattern handle
      const handle_pattern & get_pattern_handle() const;

      /// Return a mutable reference on the pattern
      base_topology_pattern & grab_pattern();

      /// Return a non mutable reference on the pattern
      const base_topology_pattern & get_pattern() const;

      /// Check pattern data type
      template<class T>
      bool has_pattern_as() const
      {
        DT_THROW_IF(! has_pattern(),
                    std::logic_error,
                    "Topology data does not hold any topology pattern !");
        const std::type_info & ti = typeid(T);
        const std::type_info & tf = typeid(get_pattern());
        return ti == tf;
      }

      /// Get a non-mutable topology pattern of a given type
      template<class T>
      const T & get_pattern_as() const
      {
        DT_THROW_IF(! has_pattern_as<T>(),
                    std::logic_error,
                    "Invalid request on topology pattern type !");
        return dynamic_cast<const T&>(get_pattern());
      }

      /// Get a mutable topology pattern of a given type
      template<class T>
      T & grab_pattern_as()
      {
        DT_THROW_IF(! has_pattern_as<T>(),
                    std::logic_error,
                    "Invalid request on topology pattern type !");
        return dynamic_cast<T&>(grab_pattern());
      }

      /// Return a mutable reference on the container of auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /// Return a non mutable reference on the container of auxiliary properties
      datatools::properties & grab_auxiliaries();

      /// Reset the internals
      void reset();

      /// Clear the object
      virtual void clear();

      /// Smart print
      virtual void tree_dump(std::ostream      & out_    = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

    private :

      handle_pattern _pattern_;            //!< Handle to a topology pattern
      datatools::properties _auxiliaries_; //!< Auxiliary properties

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace datamodel

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::topology_data, "snemo::datamodel::topology_data")

#endif // FALAISE_SNEMO_DATAMODELS_TOPOLOGY_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
