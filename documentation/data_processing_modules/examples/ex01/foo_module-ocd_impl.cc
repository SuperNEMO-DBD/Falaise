// -*- mode: c++; -*-

// Ourselves
#include "foo_module.h"

// OCD support for class 'falaise::example::foo_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(falaise::example::foo_module, ocd_)
{
  ocd_.set_class_name ("falaise::example::foo_module");
  ocd_.set_class_library ("foo_module");
  ocd_.set_class_description ("A module which adds a bank with a ``datatools::properties`` instance in the event record and adds a boolean flag in it");
  ocd_.set_class_documentation ("This is an example of a data processing module that   \n"
                                "illustrates the interface of a data processing module \n"
                                "as used in the SuperNEMO pipeline system.             \n"
                                );

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("Logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This tag sets the logging priority threshold.  \n"
                            "Default value is \"warning\".                  \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "  logging.priority : string = \"warning\"      \n"
                            "                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("foo_bank_key")
      .set_terse_description("The name/key of the data bank to be added in the event record.")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Default value is : ``\"Foo\"``.                \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "  foo_bank_key : string = \"Bar\"              \n"
                            "                                               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("foo_flag_key")
      .set_terse_description("The name/label of the flag to be added in the properties bank.")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Default value is : ``\"Test\"``.               \n"
                            "                                               \n"
                            "Example::                                      \n"
                            "                                               \n"
                            "  foo_flag_key : string = \"Dummy\"            \n"
                            "                                               \n"
                            )
      ;
  }

  ocd_.set_configuration_hints ("A ``falaise::example::foo_module`` object can be setup with the      \n"
                                "following syntax in a ``datatools::multi_properties`` configuration  \n"
                                "file, typically from a module manager object.                        \n"
                                "                                                                     \n"
                                "Example::                                                            \n"
                                "                                                                     \n"
                                "  #@description A list of data processing modules                    \n"
                                "  #@key_label   \"name\"                                             \n"
                                "  #@meta_label  \"type\"                                             \n"
                                "                                                                     \n"
                                "  [name=\"dummy\" type=\"falaise::foo_module\"]                      \n"
                                "  #@config A dummy module                                            \n"
                                "  logging.priority : string = \"warning\"                            \n"
                                "  foo_bank_key : string = \"Bar\"    # default is \"Foo\"            \n"
                                "  foo_flag_key : string = \"Dummy\"  # default is \"Test\"           \n"
                                "                                                                     \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::falaise::example::foo_module,"falaise::example::foo_module")
