#/////////////////////////////Modified Mag Coil Model/////////////////////////////////




#///////////////Long Coil////////////////


 [name="copper_coil_long.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 11.5 mm
    y : real as length = 5317 mm
    z : real as length = 11.5 mm
  
  material.ref : string = "basic::copper"
  visibility.color            : string  = "orange"


 [name="copper_coil_long_space.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 13 mm
    y : real as length = 5317 mm
    z : real as length = 5.49 mm
  
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = true
  visibility.hidden_envelope  : boolean = true
  visibility.daughters.hidden : boolean = true
  visibility.color            : string  = "yellow"




 [name="copper_coil_wire_and_space_long.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "copper_coil_long.model"
    stacked.label_0 : string = "copper_coil_long"
    stacked.model_1 : string = "copper_coil_long_space.model"
    stacked.label_1 : string = "copper_coil_long_space"
  material.ref : string = "lab_air"


 # mapping.daughter_id.copper_coil_long   : string = "[shield_subsubsection:part=0]"
 # mapping.daughter_id.copper_coil_long_space   : string = "[shield_subsubsection:part=1]"

  

[name="copper_coil_wire_block_long.model" type="geomtools::replicated_boxed_model"]
  #@config New shielding wall
  replicated.axis            : string = "z"
  replicated.number_of_items : integer = 200
  replicated.model           : string = "copper_coil_wire_and_space_long.model"
  replicated.label           : string = "copper_coil_wire_and_space_long1"
  material.ref : string = "lab_air"

  #mapping.daughter_id.copper_coil_wire_and_space_long1   : string = "[shield_subsection:segment+0]"







#///////////////Medium Coil////////////////


 [name="copper_coil_medium.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 620 mm
    y : real as length = 11.5 mm
    z : real as length = 11.5 mm
  
  material.ref : string = "basic::copper"
  visibility.color            : string  = "orange"


 [name="copper_coil_medium_space.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 620 mm
    y : real as length = 13 mm
    z : real as length = 5.49 mm
  
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = true
  visibility.hidden_envelope  : boolean = true
  visibility.daughters.hidden : boolean = true
  visibility.color            : string  = "yellow"



 [name="copper_coil_wire_and_space_medium.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "copper_coil_medium.model"
    stacked.label_0 : string = "copper_coil_medium"
    stacked.model_1 : string = "copper_coil_medium_space.model"
    stacked.label_1 : string = "copper_coil_medium_space"
  material.ref : string = "lab_air"
 


[name="copper_coil_wire_block_medium.model" type="geomtools::replicated_boxed_model"]
  #@config New shielding wall
  replicated.axis            : string = "z"
  replicated.number_of_items : integer = 200
  replicated.model           : string = "copper_coil_wire_and_space_medium.model"
  replicated.label           : string = "copper_coil_wire_and_space_medium1"
  material.ref : string = "lab_air"
  





#///////////////Short Coil////////////////


 [name="copper_coil_short.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 769 mm
    y : real as length = 11.5 mm
    z : real as length = 11.5 mm
  
  material.ref : string = "basic::copper"
  visibility.color            : string  = "orange"


 [name="copper_coil_short_space.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 769 mm
    y : real as length = 13 mm
    z : real as length = 5.49 mm
  
  material.ref : string = "lab_air"
  visibility.hidden           : boolean = true
  visibility.hidden_envelope  : boolean = true
  visibility.daughters.hidden : boolean = true
  visibility.color            : string  = "yellow"



 [name="copper_coil_wire_and_space_short.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "copper_coil_short.model"
    stacked.label_0 : string = "copper_coil_short"
    stacked.model_1 : string = "copper_coil_short_space.model"
    stacked.label_1 : string = "copper_coil_short_space"
  material.ref : string = "lab_air"



[name="copper_coil_wire_block_short.model" type="geomtools::replicated_boxed_model"]
  #@config New shielding wall
  replicated.axis            : string = "z"
  replicated.number_of_items : integer = 200
  replicated.model           : string = "copper_coil_wire_and_space_short.model"
  replicated.label           : string = "copper_coil_wire_and_space_short1"
  material.ref : string = "lab_air"




#///////////////Iron Coil Support Fr-It////////////////


 [name="iron_coil_support.model" type="geomtools::simple_boxed_model"]
   #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 13 mm
    y : real as length = 333 mm
    z : real as length =  3397 mm
  material.ref : string = "basic::iron"
  visibility.color            : string  = "green"


[name="iron_coil_support_space.model" type="geomtools::simple_boxed_model"]
    #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 13 mm
    y : real as length = 100 mm
    z : real as length =  3397 mm
    material.ref : string = "lab_air"
   

 [name="iron_coil_support_and_space.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "iron_coil_support.model"
    stacked.label_0 : string = "iron_coil_support"
    stacked.model_1 : string = "iron_coil_support_space.model"
    stacked.label_1 : string = "iron_coil_support_space"
  material.ref : string = "lab_air"


[name="iron_support_inner_wall.model" type="geomtools::replicated_boxed_model"]
  #@config New shielding wall
  replicated.axis            : string = "y"
  replicated.number_of_items : integer = 12
  replicated.model           : string = "iron_coil_support_and_space.model"
  replicated.label           : string = "iron_coil_support_and_space"
  material.ref : string = "lab_air"
 

 [name="iron_coil_support_horizontal.model" type="geomtools::simple_boxed_model"]
   #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 13 mm
    y : real as length = 5317 mm
    z : real as length = 333 mm
  material.ref : string = "basic::iron"
  visibility.color            : string  = "green"


[name="iron_support_full_wall.model" type="geomtools::simple_shaped_model"]
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 26 mm
    y : real as length = 5317 mm
    z : real as length = 3397 mm
  material.ref : string = "lab_air"

      internal_item.labels : string[4] = \
      "iron_support_inner_wall1" \
      "iron_coil_support_horizontal1" \
      "iron_coil_support_horizontal2" \
      "iron_coil_support_horizontal3" 

      internal_item.model.iron_support_inner_wall1     : string = "iron_support_inner_wall.model"
      internal_item.placement.iron_support_inner_wall1 : string = "-6.5 0 0 (mm) "
      internal_item.model.iron_coil_support_horizontal1     : string = "iron_coil_support_horizontal.model"
      internal_item.placement.iron_coil_support_horizontal1 : string = "6.5 0 -1500 (mm)"
      internal_item.model.iron_coil_support_horizontal2     : string = "iron_coil_support_horizontal.model"
      internal_item.placement.iron_coil_support_horizontal2 : string = "6.5 0 0 (mm)"
      internal_item.model.iron_coil_support_horizontal3     : string = "iron_coil_support_horizontal.model"
      internal_item.placement.iron_coil_support_horizontal3 : string = "6.5 0 1500 (mm)"



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


[name="iron_support_inner_wall_mo_tu.model" type="geomtools::simple_shaped_model"]
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 800 mm
    y : real as length = 13 mm
    z : real as length = 3397 mm
  material.ref : string = "lab_air"

      internal_item.labels : string[2] = \
      "iron_coil_support1" \
      "iron_coil_support2" 
   
      internal_item.model.iron_coil_support1     : string = "iron_coil_support.model"
      internal_item.placement.iron_coil_support1 : string = "-180 0 0 (mm) / z 90 (degree)"
      internal_item.model.iron_coil_support2     : string = "iron_coil_support.model"
      internal_item.placement.iron_coil_support2 : string = "180 0 0 (mm) / z 90 (degree)"


[name="iron_support_full_wall_mo_tu.model" type="geomtools::simple_shaped_model"]
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 800 mm
    y : real as length = 26 mm
    z : real as length = 3397 mm
  material.ref : string = "lab_air"

      internal_item.labels : string[4] = \
      "iron_support_inner_wall_mo_tu1" \
      "iron_coil_support_horizontal_mo_tu1" \
      "iron_coil_support_horizontal_mo_tu2" \
      "iron_coil_support_horizontal_mo_tu3" 

      internal_item.model.iron_support_inner_wall_mo_tu1     : string = "iron_support_inner_wall_mo_tu.model"
      internal_item.placement.iron_support_inner_wall_mo_tu1 : string = "-6.5 0 0 (mm) "
      internal_item.model.iron_coil_support_horizontal_mo_tu1     : string = "iron_coil_support_horizontal_mo_tu.model"
      internal_item.placement.iron_coil_support_horizontal_mo_tu1 : string = "6.5 0 -1500 (mm)"
      internal_item.model.iron_coil_support_horizontal_mo_tu2     : string = "iron_coil_support_horizontal_mo_tu.model"
      internal_item.placement.iron_coil_support_horizontal_mo_tu2 : string = "6.5 0 0 (mm)"
      internal_item.model.iron_coil_support_horizontal_mo_tu3     : string = "iron_coil_support_horizontal_mo_tu.model"
      internal_item.placement.iron_coil_support_horizontal_mo_tu3 : string = "6.5 0 1500 (mm)"





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
    stacked.model_0 : string = "iron_support_full_wall.model"
    stacked.label_0 : string = "iron_support_full_wall"
    stacked.model_1 : string = "copper_coil_wire_block_long.model"
    stacked.label_1 : string = "copper_coil_wire_block_long"
    
  material.ref : string = "lab_air"

  mapping.daughter_id.copper_coil_wire_block_long   : string = "[shield_section:material_type=0]"
  mapping.daughter_id.iron_support_full_wall   : string = "[shield_section:material_type=1]"


[name="magnetic_coil_mo_tu_wall.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "iron_support_full_wall_mo_tu.model"
    stacked.label_0 : string = "iron_support_full_wall_mo_tu"
    stacked.model_1 : string = "copper_coil_wire_block_short.model"
    stacked.label_1 : string = "copper_coil_wire_block_short"
    
  material.ref : string = "lab_air"

  mapping.daughter_id.copper_coil_wire_block_short   : string = "[shield_section:material_type=0]"
  mapping.daughter_id.iron_support_full_wall_mo_tu   : string = "[shield_section:material_type=1]"



[name="magnetic_coil_tilted_wall.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "iron_coil_support_wide.model"
    stacked.label_0 : string = "iron_coil_support_wide"
    stacked.model_1 : string = "copper_coil_wire_block_medium.model"
    stacked.label_1 : string = "copper_coil_wire_block_medium"
  material.ref : string = "lab_air"





[name="combined_mag.model" type="geomtools::simple_shaped_model"]
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 3000 mm
    y : real as length = 6000 mm
    z : real as length = 5000 mm
  material.ref : string = "lab_air"

      internal_item.labels : string[8] = \
      "magnetic_coil_fr_wall" \
      "magnetic_coil_it_wall" \
      "magnetic_coil_tu_wall" \
      "magnetic_coil_mo_wall" \
      "corner1" \
      "corner2" \
      "corner3" \
      "corner4"

      internal_item.model.magnetic_coil_mo_wall     : string = "magnetic_coil_mo_tu_wall.model"
        internal_item.placement.magnetic_coil_mo_wall : string = "0 3025 0 (mm) / z 180 (degree)"
        internal_item.model.magnetic_coil_tu_wall     : string = "magnetic_coil_mo_tu_wall.model"
        internal_item.placement.magnetic_coil_tu_wall : string = "0 -3025 0 (mm)"

        internal_item.model.magnetic_coil_fr_wall     : string = "magnetic_coil_fr_it_wall.model"
        internal_item.placement.magnetic_coil_fr_wall : string = "-960 0 0 (mm)"
        internal_item.model.magnetic_coil_it_wall     : string = "magnetic_coil_fr_it_wall.model"
        internal_item.placement.magnetic_coil_it_wall : string = "960 0 0 (mm) / z 180 (degree)"

        internal_item.model.corner1     : string = "magnetic_coil_tilted_wall.model"
        internal_item.placement.corner1 : string = "680 2830 0 (mm) / z 140 (degree)"
        internal_item.model.corner2     : string = "magnetic_coil_tilted_wall.model"
        internal_item.placement.corner2 : string = "-680 -2830 0 (mm) / z 320 (degree)"
        internal_item.model.corner3     : string = "magnetic_coil_tilted_wall.model"
        internal_item.placement.corner3 : string = "680 -2830 0 (mm) / z 40 (degree)"
        internal_item.model.corner4     : string = "magnetic_coil_tilted_wall.model"
        internal_item.placement.corner4 : string = "-680 2830 0 (mm) / z 220 (degree)"