# calorimeter_walls.geom
[name="calorimeter_module_11x8inch_column.model" type="geomtools::replicated_boxed_model"]
  #@config A calorimeter main wall column of 11 optical modules (8" OMs)
  replicated.axis            : string = "z"
  replicated.number_of_items : integer = 11
  replicated.model           : string = "calorimeter_module_8inch_rotated.model"
  replicated.label           : string = "modules"
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = true
  visibility.daughters.hidden : boolean = false
  visibility.color            : string = "grey"
  mapping.daughter_id.modules : string = "[calorimeter_optical_module:row+1]"


[name="calorimeter_column.model" type="geomtools::stacked_model"]
  #@config A calorimeter main wall column of 13 optical modules (OMs)
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "calorimeter_module_5inch_rotated.model"
    stacked.label_0 : string = "bottom_om"
    stacked.model_1 : string = "calorimeter_module_11x8inch_column.model"
    stacked.label_1 : string = "middle_oms"
    stacked.model_2 : string = "calorimeter_module_5inch_rotated.model"
    stacked.label_2 : string = "top_om"
  material.ref : string = "lab_air"
  mapping.daughter_id.bottom_om : string = "[calorimeter_optical_module:row=0]"
  mapping.daughter_id.top_om    : string = "[calorimeter_optical_module:row=12]"


[name="calorimeter_back_column.model" type="geomtools::rotated_boxed_model"]
  #@config A calorimeter OMs column for the side=0 (Italy) main wall
  rotated.axis          : string  = "z"
  rotated.special_angle : string  = "0"
  rotated.model         : string  = "calorimeter_column.model"
  material.ref : string = "lab_air"


[name="calorimeter_front_column.model" type="geomtools::rotated_boxed_model"]
  #@config A calorimeter OMs column for the side=1 (France) main wall
  rotated.axis          : string  = "z"
  rotated.special_angle : string  = "180"
  rotated.model         : string  = "calorimeter_column.model"
  material.ref : string = "lab_air"


[name="calorimeter_back_wall.model" type="geomtools::replicated_boxed_model"]
  #@config A calorimeter main wall for the side=0 (Italy)
  replicated.axis            : string = "y"
  replicated.number_of_items : integer = 20
  replicated.model           : string = "calorimeter_back_column.model"
  replicated.label           : string = "columns"
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = true
  visibility.daughters.hidden : boolean = false
  visibility.color            : string = "grey"
  mapping.daughter_id.columns : string = "[calorimeter_column:column+0]"


[name="calorimeter_front_wall.model" type="geomtools::replicated_boxed_model"]
  #@config A calorimeter main wall for the side=1 (France)
  replicated.axis            : string = "y"
  replicated.number_of_items : integer = 20
  replicated.model           : string = "calorimeter_front_column.model"
  replicated.label           : string = "columns"
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = true
  visibility.daughters.hidden : boolean = false
  visibility.color            : string = "grey"
  mapping.daughter_id.columns : string = "[calorimeter_column:column+0]"

