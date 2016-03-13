# -*- mode: conf-unix; -*-
# tracker_drift_cell.geom

#@description The geometry models that compose the drift cell (with field wires)
#@key_label   "name"
#@meta_label  "type"


###############################################################
[name="field_wire.model" type="geomtools::simple_shaped_model"]

#@config Geometric description of a drift cell field wire

#######################
# Geometry parameters #
#######################

#@description Shape
shape_type : string = "cylinder"

#@description Cylinder diameter
diameter : real as length = 50.0 um

#@description Cylinder height
z : real as length = 2920.0 mm

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "wire_metal"

#########################
# Visibility parameters #
#########################

visibility.hidden         : boolean = false
visibility.wired_cylinder : boolean = true
visibility.color          : string = "blue"


###################################################################
[name="x_field_wires_set.model" type="geomtools::replicated_model"]

#@config Geometric description of a set of 3 field wires on core cell X-side

# A set of field wires along the X axis:
#
#  wire #0      ^ y        wire #2
#     \         :         /
#    +-\-----------------/-+      ^
#  - | -o- - - -o- - - -o- |- - tiny - -> x
#    +-----------\---------+      v
#               : \
#                  wire #1
#       <- 18mm +23um ->
#

#######################
# Geometry parameters #
#######################

#@description The replication axis
replicated.axis : string = "x"

#@description The number of replicated volumes
replicated.number_of_items : integer = 3

#@description The model of the replicated volumes
replicated.model           : string = "field_wire.model"

#@description The label associated to the replicated volumes
replicated.label           : string = "field_wires"

#@description The step between replicated volumes
replicated.step            : real as length = 9.115 mm

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden            : boolean = false
visibility.hidden_envelope   : boolean = true
visibility.daughters.hidden  : boolean = true
visibility.color             : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires : string = "[drift_cell_field_wire:wire+0]"


###################################################################
[name="y_field_wires_set.model" type="geomtools::replicated_model"]

#@config Geometric description of a set of 3 field wires on core cell Y-side

# A set of field wires along the Y axis
#
#              ^ y
#              :
#            +---+
#            | : |
#        ^   | o-|----- wire #2
#        :   | : |
#        :   | : |         x
#  18 mm :- -|-o-|- - - ->
#  +     :   | :`|----- wire #1
#  23 um :   | : |
#        v   | o-|----- wire #0
#            | : |
#            +---+
#              :
#              :
#            <tiny>
#
#

#######################
# Geometry parameters #
#######################

#@description The replication axis
replicated.axis : string = "y"

#@description The number of replicated volumes
replicated.number_of_items : integer = 3

#@description The model of the replicated volumes
replicated.model : string = "field_wire.model"

#@description The label associated to the replicated volumes
replicated.label : string = "field_wires"

#@description The step between replicated volumes
replicated.step  : real as length = 9.115 mm

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = true
visibility.daughters.hidden : boolean = true
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires : string = "[drift_cell_field_wire:wire+0]"


############################################################################
[name="drift_cell_node_0123.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 4 sides
#
#                    y
#                     ^       'core cell' model
#           right (3) :        /
#              +-------------+/
#              |  o   o   o  / <-- X aligned 'set of field wires' model
#          +---+------------/+---+
#          | o |      :    / | o |
#          |   |      :      |   | front (1)
# back (0)-|-o |- - - o - - -|-o-|- - -> x
#          |   |      :      |   |
#          | o |      :      | o | <--- Y aligned 'set of field wires' model
#          +---+-------------+---+
#              |  o   o   o  |
#              +-------------+
#                     : left (2)
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core.model"
surrounded.label       : string = "core_cell"

surrounded.back_model  : string = "y_field_wires_set.model"
surrounded.back_label  : string = "field_wires_set_back"

surrounded.front_model : string = "y_field_wires_set.model"
surrounded.front_label : string = "field_wires_set_front"

surrounded.left_model  : string = "x_field_wires_set.model"
surrounded.left_label  : string = "field_wires_set_left"

surrounded.right_model : string = "x_field_wires_set.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires_set_back  : string = "[drift_cell_field_wires_set:set=0]"
mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_left  : string = "[drift_cell_field_wires_set:set=2]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"


######################################################################
[name="drift_cell_node_013.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 3 sides
#
#                    y
#                     ^       'core cell' model
#           right (3) :        /
#              +-------------+/
#              |  o   o   o  / <-- X aligned 'set of field wires' model
#          +---+------------/+---+
#          | o |      :    / | o |
#          |   |      :      |   | front (1)
# back (0)-|-o |- - - o - - -|-o-|- - -> x
#          |   |      :      |   |
#          | o |      :      | o | <--- Y aligned 'set of field wires' model
#          +---+-------------+---+
#                     :
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core.model"
surrounded.label       : string = "core_cell"

surrounded.back_model  : string = "y_field_wires_set.model"
surrounded.back_label  : string = "field_wires_set_back"

surrounded.front_model : string = "y_field_wires_set.model"
surrounded.front_label : string = "field_wires_set_front"

surrounded.right_model : string = "x_field_wires_set.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires_set_back  : string = "[drift_cell_field_wires_set:set=0]"
mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"


######################################################################
[name="drift_cell_node_13.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 3 sides
#
#                    y
#                     ^       'core cell' model
#           right (3) :        /
#              +-------------+/
#              |  o   o   o  / <-- X aligned 'set of field wires' model
#              +------------/+---+
#              |      :    / | o |
#              |      :      |   | front (1)
#              |- - - o - - -|-o-|- - -> x
#              |      :      |   |
#              |      :      | o | <--- Y aligned 'set of field wires' model
#              +-------------+---+
#                     :
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core.model"
surrounded.label       : string = "core_cell"

surrounded.front_model : string = "y_field_wires_set.model"
surrounded.front_label : string = "field_wires_set_front"

surrounded.right_model : string = "x_field_wires_set.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"


#####################################################################
[name="drift_cell_node_123.model" type="geomtools::surrounded_boxed_model"]

#@config A cell equipped with field wire sets on 3 sides
#
#                    y
#                     ^       'core cell' model
#           right (3) :        /
#              +-------------+/
#              |  o   o   o  / <-- X aligned 'set of field wires' model
#              +------------/+---+
#              |      :    / | o |
#              |      :      |   | front (1)
#              |- - - o - - -|-o-|- - -> x
#              |      :      |   |
#              |      :      | o | <--- Y aligned 'set of field wires' model
#              +-------------+---+
#              |  o   o   o  |
#              +-------------+
#                     : left (2)
#                     :
#

#######################
# Geometry parameters #
#######################

surrounded.centered_x  : boolean = false
surrounded.centered_y  : boolean = false
surrounded.centered_z  : boolean = false

surrounded.model       : string = "drift_cell_core.model"
surrounded.label       : string = "core_cell"

surrounded.front_model : string = "y_field_wires_set.model"
surrounded.front_label : string = "field_wires_set_front"

surrounded.left_model  : string = "x_field_wires_set.model"
surrounded.left_label  : string = "field_wires_set_left"

surrounded.right_model : string = "x_field_wires_set.model"
surrounded.right_label : string = "field_wires_set_right"

#######################
# Material parameters #
#######################

#@description Name of the material (may be a material alias)
material.ref : string = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = false
visibility.hidden_envelope  : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_left  : string = "[drift_cell_field_wires_set:set=2]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core_cell             : string = "[drift_cell_core]"


# end of tracker_drift_cell.geom
