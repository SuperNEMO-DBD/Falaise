/// \file falaise/snemo/physics_model/activity_model_factory.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-05-24
 * Last modified: 2022-05-24
 *
 * Description: Factory of activity models
 */

#ifndef FALAISE_SNEMO_PHYSICS_MODEL_ACTIVITY_MODEL_FACTORY_H
#define FALAISE_SNEMO_PHYSICS_MODEL_ACTIVITY_MODEL_FACTORY_H

// Standard library:
#include <string>
#include <set>
#include <iostream>

// This project:
#include <falaise/snemo/physics_model/base_activity_model.h>

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/multi_properties.h>

namespace snemo {

  namespace physics_model {

    /// \brief Activity model factory
    class activity_model_factory
      : public datatools::i_tree_dumpable
    {
    public:

      activity_model_factory();
      
      virtual ~activity_model_factory();
      
      const activity_model_dict_type & get_models() const;

      activity_model_dict_type & grab_models();

      void build_model_names(std::set<std::string> & names_) const;

      bool has_model(const std::string & name_) const;
      
      const base_activity_model & get_model(const std::string & name_) const;
      
      activity_model_ptr_type get_model_handle(const std::string & name_) const;
      
      bool is_initialized() const;
      
      void initialize(const datatools::properties & config_);

      void reset();

      void load(const datatools::multi_properties & model_defs_);

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;
      
    private:

      bool _initialized_ = false;
      activity_model_dict_type _models_;      
      
    };
 
  }  // end of namespace physics_model

}  // end of namespace snemo

#endif // FALAISE_SNEMO_PHYSICS_MODEL_ACTIVOTY_MODEL_FACTORY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
