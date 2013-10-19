# -*- mode: conf-unix; -*-
# calorimeter_walls.geom

##########################################################################
[name="calorimeter_column.model" type="geomtools::replicated_boxed_model"]
material.ref              : string = "lab_air"

replicated.axis            : string = "z"
replicated.number_of_items : integer = 13
replicated.model           : string = "calorimeter_module_rotated.model"
replicated.label           : string = "modules"

visibility.hidden           : boolean = 0
visibility.envelop_hidden   : boolean = 1
visibility.daughters.hidden : boolean = 0
visibility.color            : string = "grey"

# --> Daughters mapping informations:
mapping.daughter_id.modules : string = "[calorimeter_optical_module:row+0]"


############################################################################
[name="calorimeter_back_column.model" type="geomtools::rotated_boxed_model"]
material.ref              : string = "lab_air"

rotated.axis              : string  = "z"
rotated.special_angle     : string  = "0"
rotated.model             : string  = "calorimeter_column.model"


#############################################################################
[name="calorimeter_front_column.model" type="geomtools::rotated_boxed_model"]
material.ref              : string = "lab_air"

rotated.axis              : string  = "z"
rotated.special_angle     : string  = "180"
rotated.model             : string  = "calorimeter_column.model"


#############################################################################
[name="calorimeter_back_wall.model" type="geomtools::replicated_boxed_model"]
material.ref               : string = "lab_air"

replicated.axis            : string = "y"
replicated.number_of_items : integer = 20
replicated.model           : string = "calorimeter_back_column.model"
replicated.label           : string = "columns"

visibility.hidden           : boolean = 0
visibility.envelop_hidden   : boolean = 1
visibility.daughters.hidden : boolean = 0
visibility.color            : string = "grey"

# --> Daughters mapping informations:
mapping.daughter_id.columns : string = "[calorimeter_column:column+0]"


##############################################################################
[name="calorimeter_front_wall.model" type="geomtools::replicated_boxed_model"]
material.ref               : string = "lab_air"

replicated.axis            : string = "y"
replicated.number_of_items : integer = 20
replicated.model           : string = "calorimeter_front_column.model"
replicated.label           : string = "columns"

visibility.hidden           : boolean = 0
visibility.envelop_hidden   : boolean = 1
visibility.daughters.hidden : boolean = 0
visibility.color            : string = "grey"

# --> Daughters mapping informations:
mapping.daughter_id.columns : string = "[calorimeter_column:column+0]"


# end of calorimeter_walls.geom
