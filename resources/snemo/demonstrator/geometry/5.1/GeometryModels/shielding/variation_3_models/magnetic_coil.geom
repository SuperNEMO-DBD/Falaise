#/////////////////////////////Modified Mag Coil Model/////////////////////////////////


#///////////////Long Coil////////////////

[name="copper_coil_long_wire.model" type="geomtools::simple_boxed_model"]
  #@config Long copper wire (bar) for the coil
  shape_type : string = "box"
    x : real as length = 11.5 mm
    y : real as length = 5317 mm
    z : real as length = 11.5 mm
  
  material.ref    : string = "basic::copper"
  visibility.color: string  = "orange"


[name="copper_coil_long_space.model" type="geomtools::simple_boxed_model"]
  #@config Long air space between copper wires
  shape_type : string = "box"
    x : real as length = 13 mm
    y : real as length = 5317 mm
    z : real as length = 5.49 mm
  
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = true
  visibility.hidden_envelope  : boolean = true
  visibility.daughters.hidden : boolean = true
  visibility.color            : string  = "grey"


[name="copper_coil_wire_and_space_long.model" type="geomtools::stacked_model"]
  #@config Assembly of a long copper wire and air spacing above
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "copper_coil_long_wire.model"
    stacked.label_0 : string = "wire"
    stacked.model_1 : string = "copper_coil_long_space.model"
    stacked.label_1 : string = "space"
  material.ref : string = "lab_air"

  mapping.daughter_id.wire  : string = "[coil_wire]"
  mapping.daughter_id.space : string = "[coil_wire_space]"

 
[name="copper_coil_wire_block_long.model" type="geomtools::replicated_boxed_model"]
  #@config Long block of wires and spaces
  replicated.axis            : string = "z"
  replicated.number_of_items : integer = 200
  replicated.model           : string = "copper_coil_wire_and_space_long.model"
  replicated.label           : string = "wire_set"
  material.ref               : string = "lab_air"

  mapping.daughter_id.wire_set : string = "[coil_wire_set:wire+0]"


#///////////////Medium Coil////////////////

[name="copper_coil_medium_wire.model" type="geomtools::simple_boxed_model"]
  #@config Medium copper wire (bar) for the coil
  shape_type : string = "box"
    x : real as length = 620 mm
    y : real as length = 11.5 mm
    z : real as length = 11.5 mm
  
  material.ref : string = "basic::copper"
  visibility.color : string  = "orange"


[name="copper_coil_medium_space.model" type="geomtools::simple_boxed_model"]
  #@config Medium air space between copper wires
  shape_type : string = "box"
    x : real as length = 620 mm
    y : real as length = 13 mm
    z : real as length = 5.49 mm
  
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = true
  visibility.hidden_envelope  : boolean = true
  visibility.daughters.hidden : boolean = true
  visibility.color            : string  = "grey"


[name="copper_coil_wire_and_space_medium.model" type="geomtools::stacked_model"]
  #@config Assembly of a medium copper wire and air spacing above
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "copper_coil_medium_wire.model"
    stacked.label_0 : string = "wire"
    stacked.model_1 : string = "copper_coil_medium_space.model"
    stacked.label_1 : string = "space"
  material.ref : string = "lab_air"
 
  mapping.daughter_id.wire  : string = "[coil_wire]"
  mapping.daughter_id.space : string = "[coil_wire_space]"


[name="copper_coil_wire_block_medium.model" type="geomtools::replicated_boxed_model"]
  #@config Medium block of wires and spaces
  replicated.axis            : string = "z"
  replicated.number_of_items : integer = 200
  replicated.model           : string = "copper_coil_wire_and_space_medium.model"
  replicated.label           : string = "wire_set"
  material.ref : string = "lab_air"
  
  mapping.daughter_id.wire_set : string = "[coil_wire_set:wire+0]"


#///////////////Short Coil////////////////

[name="copper_coil_short_wire.model" type="geomtools::simple_boxed_model"]
  #@config Short copper wire (bar) for the coil
  shape_type : string = "box"
    x : real as length = 769 mm
    y : real as length = 11.5 mm
    z : real as length = 11.5 mm
  
  material.ref : string = "basic::copper"
  visibility.color : string  = "orange"


[name="copper_coil_short_space.model" type="geomtools::simple_boxed_model"]
  #@config Short air space between copper wires
  shape_type : string = "box"
    x : real as length = 769 mm
    y : real as length = 13 mm
    z : real as length = 5.49 mm
  
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = true
  visibility.hidden_envelope  : boolean = true
  visibility.daughters.hidden : boolean = true
  visibility.color            : string  = "grey"


[name="copper_coil_wire_and_space_short.model" type="geomtools::stacked_model"]
  #@config Assembly of a short copper wire and air spacing above
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "copper_coil_short_wire.model"
    stacked.label_0 : string = "wire"
    stacked.model_1 : string = "copper_coil_short_space.model"
    stacked.label_1 : string = "space"
  material.ref : string = "lab_air"

  mapping.daughter_id.wire  : string = "[coil_wire]"
  mapping.daughter_id.space : string = "[coil_wire_space]"


[name="copper_coil_wire_block_short.model" type="geomtools::replicated_boxed_model"]
  #@config Short block of wires and spaces
  replicated.axis            : string = "z"
  replicated.number_of_items : integer = 200
  replicated.model           : string = "copper_coil_wire_and_space_short.model"
  replicated.label           : string = "wire_set"
  material.ref : string = "lab_air"

  mapping.daughter_id.wire_set : string = "[coil_wire_set:wire+0]"


#///////////////Iron Coil Support Fr-It////////////////

[name="iron_coil_support_plate.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 13 mm
    y : real as length = 333 mm
    z : real as length = 3397 mm
  material.ref : string = "basic::iron"
  visibility.color : string  = "green"


[name="iron_coil_support_space.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 13 mm
    y : real as length = 100 mm
    z : real as length = 3397 mm
    material.ref : string = "lab_air"
   

[name="iron_coil_support_plate_and_space.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "iron_coil_support_plate.model"
    stacked.label_0 : string = "plate"
    stacked.model_1 : string = "iron_coil_support_space.model"
    stacked.label_1 : string = "space"
  material.ref : string = "lab_air"


[name="iron_coil_support_inner_wall.model" type="geomtools::replicated_boxed_model"]
  #@config New shielding wall
  replicated.axis            : string = "y"
  replicated.number_of_items : integer = 12
  replicated.model           : string = "iron_coil_support_plate_and_space.model"
  replicated.label           : string = "support_set"
  material.ref : string = "lab_air"
 

[name="iron_coil_support_horizontal.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 13 mm
    y : real as length = 5317 mm
    z : real as length = 333 mm
  material.ref : string = "basic::iron"
  visibility.color : string  = "green"


[name="iron_coil_support_full_wall.model" type="geomtools::simple_shaped_model"]
  #@config Simplified demonstrator module
   
  shape_type : string = "box"
    x : real as length = 26 mm
    y : real as length = 5317 mm
    z : real as length = 3397 mm
  material.ref : string = "lab_air"

      internal_item.labels : string[4] = \
      "inner_wall" \
      "horizontal_support_1" \
      "horizontal_support_2" \
      "horizontal_support_3" 

      internal_item.model.inner_wall     : string = "iron_coil_support_inner_wall.model"
      internal_item.placement.inner_wall : string = "-6.5 0 0 (mm) "
      internal_item.model.horizontal_support_1     : string = "iron_coil_support_horizontal.model"
      internal_item.placement.horizontal_support_1 : string = "6.5 0 -1500 (mm)"
      internal_item.model.horizontal_support_2     : string = "iron_coil_support_horizontal.model"
      internal_item.placement.horizontal_support_2 : string = "6.5 0 0 (mm)"
      internal_item.model.horizontal_support_3     : string = "iron_coil_support_horizontal.model"
      internal_item.placement.horizontal_support_3 : string = "6.5 0 1500 (mm)"


#///////////////Iron Coil Support Mo-Tu////////////////

[name="iron_coil_support_horizontal_mo_tu.model" type="geomtools::simple_boxed_model"]
   #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 800 mm
    y : real as length = 13 mm
    z : real as length = 333 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "green"


[name="iron_coil_support_inner_wall_mo_tu.model" type="geomtools::simple_shaped_model"]
  #@config Simplified demonstrator module
   
  shape_type : string = "box"
    x : real as length = 800 mm
    y : real as length = 13 mm
    z : real as length = 3397 mm
  material.ref : string = "lab_air"

      internal_item.labels : string[2] = \
      "support1" \
      "support2" 
   
      internal_item.model.support1     : string = "iron_coil_support_plate.model"
      internal_item.placement.support1 : string = "-180 0 0 (mm) / z 90 (degree)"
      internal_item.model.support2     : string = "iron_coil_support_plate.model"
      internal_item.placement.support2 : string = "180 0 0 (mm) / z 90 (degree)"


[name="iron_coil_support_full_wall_mo_tu.model" type="geomtools::simple_shaped_model"]
  #@config Simplified demonstrator module
   
  shape_type : string = "box"
    x : real as length = 800 mm
    y : real as length = 26 mm
    z : real as length = 3397 mm
  material.ref : string = "lab_air"

      internal_item.labels : string[4] = \
      "inner_wall" \
      "horizontal_support_1" \
      "horizontal_support_2" \
      "horizontal_support_3" 

      internal_item.model.inner_wall               : string = "iron_coil_support_inner_wall_mo_tu.model"
      internal_item.placement.inner_wall           : string = "-6.5 0 0 (mm) "
      internal_item.model.horizontal_support_1     : string = "iron_coil_support_horizontal_mo_tu.model"
      internal_item.placement.horizontal_support_1 : string = "6.5 0 -1500 (mm)"
      internal_item.model.horizontal_support_2     : string = "iron_coil_support_horizontal_mo_tu.model"
      internal_item.placement.horizontal_support_2 : string = "6.5 0 0 (mm)"
      internal_item.model.horizontal_support_3     : string = "iron_coil_support_horizontal_mo_tu.model"
      internal_item.placement.horizontal_support_3 : string = "6.5 0 1500 (mm)"


#///////////////Iron Coil Corner Walls////////////////

[name="iron_coil_support_wide.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 620 mm
    y : real as length = 13 mm
    z : real as length =  3397 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "green"


#///////////////Combined (Fe+Cu) Coil////////////////

[name="magnetic_coil_fr_it_wall.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "x"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "iron_coil_support_full_wall.model"
    stacked.label_0 : string = "support"
    stacked.model_1 : string = "copper_coil_wire_block_long.model"
    stacked.label_1 : string = "coil"
    
  material.ref : string = "lab_air"

  mapping.daughter_id.coil    : string = "[coil_wall_wire_block]"
  mapping.daughter_id.support : string = "[coil_wall_support]"


[name="magnetic_coil_mo_tu_wall.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "iron_coil_support_full_wall_mo_tu.model"
    stacked.label_0 : string = "support"
    stacked.model_1 : string = "copper_coil_wire_block_short.model"
    stacked.label_1 : string = "coil"
    
  material.ref : string = "lab_air"

  mapping.daughter_id.coil    : string = "[coil_wall_wire_block]"
  mapping.daughter_id.support : string = "[coil_wall_support]"


[name="magnetic_coil_tilted_wall.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "iron_coil_support_wide.model"
    stacked.label_0 : string = "support"
    stacked.model_1 : string = "copper_coil_wire_block_medium.model"
    stacked.label_1 : string = "coil"
  material.ref : string = "lab_air"

  mapping.daughter_id.coil    : string = "[coil_wall_wire_block]"
  mapping.daughter_id.support : string = "[coil_wall_support]"


# [name="combined_mag.model" type="geomtools::simple_shaped_model"]
#   #@config Simplified demonstrator module
   
#    shape_type : string = "box"
#     x : real as length = 3000 mm
#     y : real as length = 6000 mm
#     z : real as length = 5000 mm
#   material.ref : string = "lab_air"

#       internal_item.labels : string[8] = \
#       "magnetic_coil_fr_wall" \
#       "magnetic_coil_it_wall" \
#       "magnetic_coil_tu_wall" \
#       "magnetic_coil_mo_wall" \
#       "corner1" \
#       "corner2" \
#       "corner3" \
#       "corner4"

#       internal_item.model.magnetic_coil_mo_wall     : string = "magnetic_coil_mo_tu_wall.model"
#         internal_item.placement.magnetic_coil_mo_wall : string = "0 3025 0 (mm) / z 180 (degree)"
#         internal_item.model.magnetic_coil_tu_wall     : string = "magnetic_coil_mo_tu_wall.model"
#         internal_item.placement.magnetic_coil_tu_wall : string = "0 -3025 0 (mm)"

#         internal_item.model.magnetic_coil_fr_wall     : string = "magnetic_coil_fr_it_wall.model"
#         internal_item.placement.magnetic_coil_fr_wall : string = "-960 0 0 (mm)"
#         internal_item.model.magnetic_coil_it_wall     : string = "magnetic_coil_fr_it_wall.model"
#         internal_item.placement.magnetic_coil_it_wall : string = "960 0 0 (mm) / z 180 (degree)"

#         internal_item.model.corner1     : string = "magnetic_coil_tilted_wall.model"
#         internal_item.placement.corner1 : string = "680 2830 0 (mm) / z 140 (degree)"
#         internal_item.model.corner2     : string = "magnetic_coil_tilted_wall.model"
#         internal_item.placement.corner2 : string = "-680 -2830 0 (mm) / z 320 (degree)"
#         internal_item.model.corner3     : string = "magnetic_coil_tilted_wall.model"
#         internal_item.placement.corner3 : string = "680 -2830 0 (mm) / z 40 (degree)"
#         internal_item.model.corner4     : string = "magnetic_coil_tilted_wall.model"
#         internal_item.placement.corner4 : string = "-680 2830 0 (mm) / z 220 (degree)"

# end