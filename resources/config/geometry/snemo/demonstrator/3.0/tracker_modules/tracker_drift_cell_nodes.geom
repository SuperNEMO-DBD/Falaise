# -*- mode: conf-unix; -*-

##############################################################################
# A cell equipped with field wires sets on 4 sides:
#
#                    ^ y
#                    |
#                    |
#                    |
#                  right (3)
#          ......+-------+......
#          :     |o  o  o|     :
#          :   +-+-------+-+   :
#          +---+           +---+
#          | o |           | o |
# back (0) | o |     o     | o | front (1) - - - -> x
#          | o |           | o |
#          +---+           +---+
#          :   +-+-------+-+   :
#          :     |o  o  o|     :
#          :.....+-------+.....:
#                  left (2)
#
[name="drift_cell_node_0123.model" type="snemo::geometry::drift_cell_node_model"]
field_wires_set_model       : string    = "field_wires_set.model"
drift_cell_core_model       : string    = "drift_cell_core.model"
field_wires_sets            : string[4] = "front" "right"  "left"  "back"
visibility.hidden           : boolean   = 0
visibility.hidden_envelop   : boolean   = 0
visibility.daughters.hidden : boolean   = 0
visibility.color            : string    = "cyan"

# --> Daughters mapping informations:
mapping.daughter_id.field_wires_set_back  : string = "[drift_cell_field_wires_set:set=0]"
mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_left  : string = "[drift_cell_field_wires_set:set=2]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core                  : string = "[drift_cell_core]"


##############################################################################
# A cell equipped with field wires sets on 3 sides:
#
#                    ^ y
#                    |
#                    |
#                    |
#                  right (3)
#          ......+-------+......
#          :     |o  o  o|     :
#          :   +-+-------+-+   :
#          +---+           +---+
#          | o |           | o |
# back (0) | o |     o     | o | front (1) - - - -> x
#          | o |           | o |
#          +---+           +---+
#          :...+-----------+...:
#
[name="drift_cell_node_013.model" type="snemo::geometry::drift_cell_node_model"]
field_wires_set_model       : string    = "field_wires_set.model"
drift_cell_core_model       : string    = "drift_cell_core.model"
field_wires_sets            : string[3] = "front" "right"  "back"
visibility.hidden           : boolean   = 0
visibility.hidden_envelop   : boolean   = 1
visibility.daughters.hidden : boolean   = 0
visibility.color            : string    = "cyan"

# --> Daughters mapping informations:
mapping.daughter_id.field_wires_set_back  : string = "[drift_cell_field_wires_set:set=0]"
mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core                  : string = "[drift_cell_core]"


##############################################################################
# A cell equipped with field wires sets on 2 sides:
#
#                  ^ y
#                  |
#                  |
#                  |
#                right (3)
#            ..+-------+......
#            : |o  o  o|     :
#            +-+-------+-+   :
#            |           +---+
#            |           | o |
#            |     o     | o | front (1) - - - -> x
#            |           | o |
#            |           +---+
#            +-----------+...:
#
[name="drift_cell_node_13.model" type="snemo::geometry::drift_cell_node_model"]
field_wires_set_model       : string    = "field_wires_set.model"
drift_cell_core_model       : string    = "drift_cell_core.model"
field_wires_sets            : string[2] = "front" "right"
visibility.hidden           : boolean   = 0
visibility.hidden_envelop   : boolean   = 1
visibility.daughters.hidden : boolean   = 0
visibility.color            : string    = "cyan"

# --> Daughters mapping informations:
mapping.daughter_id.field_wires_set_front : string = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_right : string = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core                  : string = "[drift_cell_core]"


##############################################################################
# A cell equipped with field wires sets on 3 sides:
#
#                  ^ y
#                  |
#                  |
#                  |
#                right (3)
#            ..+-------+......
#            : |o  o  o|     :
#            +-+-------+-+   :
#            |           +---+
#            |           | o |
#            |     o     | o | front (1) - - - -> x
#            |           | o |
#            |           +---+
#            +-+-------+-+   :
#            : |o  o  o|     :
#            :.+-------+.....:
#                left (2)
#
 [name="drift_cell_node_123.model" type="snemo::geometry::drift_cell_node_model"]
field_wires_set_model       : string    = "field_wires_set.model"
drift_cell_core_model       : string    = "drift_cell_core.model"
field_wires_sets            : string[3] = "front" "right"  "left"
visibility.hidden           : boolean   = 0
visibility.hidden_envelop   : boolean   = 1
visibility.daughters.hidden : boolean   = 0
visibility.color            : string    = "cyan"

# --> Daughters mapping informations:
mapping.daughter_id.field_wires_set_front : string  = "[drift_cell_field_wires_set:set=1]"
mapping.daughter_id.field_wires_set_left  : string  = "[drift_cell_field_wires_set:set=2]"
mapping.daughter_id.field_wires_set_right : string  = "[drift_cell_field_wires_set:set=3]"
mapping.daughter_id.core                  : string  = "[drift_cell_core]"

# end of file
