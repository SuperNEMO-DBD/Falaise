# -*- mode: conf-unix; -*-
# calorimeter_walls.geom


##########################################################################################
[name="calorimeter_module_11x8inch_column.model" type="geomtools::replicated_boxed_model"]

#@config A calorimeter main wall column of 11 optical modules (8" OMs)

replicated.axis            : string = "z"
replicated.number_of_items : integer = 11
replicated.model           : string = "calorimeter_module_8inch_rotated.model"
replicated.label           : string = "modules"

material.ref : string = "lab_air"

visibility.hidden           : boolean = false
visibility.envelop_hidden   : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

#@description Daughters mapping informations:
mapping.daughter_id.modules : string = "[calorimeter_optical_module:row+1]"


#################################################################
[name="calorimeter_column.model" type="geomtools::stacked_model"]

#@config A calorimeter main wall column of 13 optical modules (OMs)

#@description The stacking axis
stacked.axis : string = "z"

#@description The number of stacked volumes
stacked.number_of_items : integer = 3

#@description The name of the geometry model at stacking slot 0
stacked.model_0 : string = "calorimeter_module_5inch_rotated.model"

#@description The name of the geometry model at stacking slot 1
stacked.model_1 : string = "calorimeter_module_11x8inch_column.model"

#@description The name of the geometry model at stacking slot 2
stacked.model_2 : string = "calorimeter_module_5inch_rotated.model"

#@description The name of the volume at stacking slot 0
stacked.label_0 : string = "bottom_om"

#@description The name of the volume at stacking slot 1
stacked.label_1 : string = "middle_oms"

#@description The name of the volume at stacking slot 2
stacked.label_2 : string = "top_om"

#@description The name of the material that fills the mother box
material.ref            : string = "lab_air"

#@description Bottom OM mapping informations:
mapping.daughter_id.bottom_om : string = "[calorimeter_optical_module:row=0]"

#@description Top OM mapping informations:
mapping.daughter_id.top_om    : string = "[calorimeter_optical_module:row=12]"


############################################################################
[name="calorimeter_back_column.model" type="geomtools::rotated_boxed_model"]

#@config A calorimeter OMs column for the side=0 (Italy) main wall

rotated.axis          : string  = "z"
rotated.special_angle : string  = "0"
rotated.model         : string  = "calorimeter_column.model"

material.ref : string = "lab_air"


#############################################################################
[name="calorimeter_front_column.model" type="geomtools::rotated_boxed_model"]

#@config A calorimeter OMs column for the side=1 (France) main wall

rotated.axis          : string  = "z"
rotated.special_angle : string  = "180"
rotated.model         : string  = "calorimeter_column.model"

material.ref : string = "lab_air"


#############################################################################
[name="calorimeter_back_wall.model" type="geomtools::replicated_boxed_model"]

#@config A calorimeter main wall for the side=0 (Italy)

replicated.axis            : string = "y"
replicated.number_of_items : integer = 20
replicated.model           : string = "calorimeter_back_column.model"
replicated.label           : string = "columns"

material.ref : string = "lab_air"

visibility.hidden           : boolean = false
visibility.envelop_hidden   : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.columns : string = "[calorimeter_column:column+0]"


##############################################################################
[name="calorimeter_front_wall.model" type="geomtools::replicated_boxed_model"]

#@config A calorimeter main wall for the side=1 (France)

replicated.axis            : string = "y"
replicated.number_of_items : integer = 20
replicated.model           : string = "calorimeter_front_column.model"
replicated.label           : string = "columns"

material.ref : string = "lab_air"

visibility.hidden           : boolean = false
visibility.envelop_hidden   : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.columns : string = "[calorimeter_column:column+0]"


# end of calorimeter_walls.geom
