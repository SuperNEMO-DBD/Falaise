// -*- mode: c++ ; -*-
/* ocd_support.cc
 */

#include <falaise/ocd_support.h>

/*
// OCD support for class '::snemo::geometry::drift_cell_node_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::snemo::geometry::drift_cell_node_model,
                                ocd_)
{
  ocd_.set_class_name("snemo::geometry::drift_cell_node_model");
  ocd_.set_class_description("A geometry model implementing a SuperNEMO node cell");
  ocd_.set_class_library("falaise");
    ocd_.set_class_documentation("  \n"
    );

      //               ^
      //               |
      //               |
      //             BACK
      //             o o o
      //           o       o
      // ---- LEFT o   +   o RIGHT ----> y
      //           o       o
      //             o o o
      //             FRONT
      //               |
      //               |
      //               v x



  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("drift_cell_core_model")
      .set_terse_description("The geometry model name which corresponds to the core of the cell")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This property sets the drift cell core model name              \n"
                            "Example :                                                      \n"
                            "  |                                                            \n"
                            "  | drift_cell_core_model : string = \"drift_cell_core.model\" \n"
                            "  |                                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("field_wires_set_model")
      .set_terse_description("The geometry model name which corresponds to the field wires")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This property sets the field wires model name                  \n"
                            "Example :                                                      \n"
                            "  |                                                            \n"
                            "  | field_wires_set_model : string = \"field_wires_set.model\" \n"
                            "  |                                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("field_wires_set_model")
      .set_terse_description("The list of wire sides to be kept from the model")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("A list of side name within \"front\", \"right\", \n"
                            "\"left\", \"back\"                               \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | field_wires_sets : string[4] = \"front\" \\  \n"
                            "  |                                \"right\" \\  \n"
                            "  |                                \"left\"  \\  \n"
                            "  |                                \"back\"      \n"
                            "  |                                              \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("This model is configured through a configuration file that     \n"
                               "obeys the format of 'datatools::properties' setup file.        \n"
                               "Example :                                                      \n"
                               "  |                                                            \n"
                               "  | drift_cell_core_model : string = \"drift_cell_core.model\" \n"
                               "  | field_wires_set_model : string = \"field_wires_set.model\" \n"
                               "  | field_wires_sets      : string[3] = \"front\" \\           \n"
                               "  |                                     \"right\" \\           \n"
                               "  |                                     \"back\"               \n"
                               "  |                                                            \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::snemo::geometry::drift_cell_node_model,
                               "snemo::geometry::drift_cell_node_model")
*/

/*************************************************************************/

/*
// OCD support for class '::snemo::geometry::cathodic_ring_model' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::snemo::geometry::field_wires_set_model,
                                ocd_)
{
  ocd_.set_class_name("snemo::geometry::field_wires_set_model");
  ocd_.set_class_description("A geometry model implementing SuperNEMO field wires of the Geiger cells");
  ocd_.set_class_library("falaise");
  ocd_.set_class_documentation("  \n"
                               );

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("field_wire_model")
      .set_terse_description("The geometry model name which corresponds to a field wire")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This property sets the field wire model name         \n"
                            "Example :                                            \n"
                            "  |                                                  \n"
                            "  | field_wire_model : string = \"field_wire.model\" \n"
                            "  |                                                  \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("drift_cell_core_model")
      .set_terse_description("The geometry model name which corresponds to the drift cell core model")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("This property sets the drift cell core model name              \n"
                            "Example :                                                      \n"
                            "  |                                                            \n"
                            "  | drift_cell_core_model : string = \"drift_cell_core.model\" \n"
                            "  |                                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("number_of_field_wires")
      .set_terse_description("The number of field wires to be added")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(true)
      .set_long_description("This property sets the number of field wires \n"
                            "Example :                                    \n"
                            "  |                                          \n"
                            "  | number_of_field_wires : integer = 3      \n"
                            "  |                                          \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("distance_between_wires")
      .set_terse_description("The distance between field wires")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(true)
      .set_long_description("This property sets the distance between wires      \n"
                            "Example :                                          \n"
                            "  |                                                \n"
                            "  | distance_between_wires : real as length = 9 mm \n"
                            "  |                                                \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("This model is configured through a configuration file that       \n"
                               "obeys the format of 'datatools::properties' setup file.          \n"
                               "Example :                                                        \n"
                               "  |                                                              \n"
                               "  | field_wire_model       : string  = \"field_wire.model\"      \n"
                               "  | drift_cell_core_model  : string  = \"drift_cell_core.model\" \n"
                               "  | number_of_field_wires  : integer = 3                         \n"
                               "  | distance_between_wires : real as length = 9.11 mm            \n"
                               "  |                                                              \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::snemo::geometry::field_wires_set_model,
                               "snemo::geometry::field_wires_set_model")
*/

// end of ocd_support.cc
