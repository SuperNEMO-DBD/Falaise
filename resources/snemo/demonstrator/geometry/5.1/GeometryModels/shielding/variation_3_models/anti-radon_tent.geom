

#/////////////////////////////ART Model/////////////////////////////////


#///////////////Fr - It Walls////////////////

 [name="art_panel_fr_it.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 5 mm
    y : real as length = 1200 mm
    z : real as length = 4500 mm
  material.ref : string = "basic::polyethylene"
  visibility.color : string  = "blue"


 [name="art_panel_fr_it_short.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 5 mm
    y : real as length = 1000 mm
    z : real as length = 4500 mm
  material.ref : string = "basic::polyethylene"
  visibility.color : string  = "blue"


[name="art_fr_it_beam_vert.model" type="geomtools::simple_shaped_model"]
   #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 11.5 mm
    y : real as length = 40 mm
    z : real as length = 4500 mm
  material.ref : string = "basic::iron"
  visibility.color : string  = "orange"


[name="art_fr_it_beam_horz.model" type="geomtools::simple_shaped_model"]
   #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 40 mm
    y : real as length = 7300 mm
    z : real as length = 11.5 mm
  material.ref : string = "basic::iron"
  visibility.color            : string  = "orange"


 [name="art_panel_fr_it_panel_full_build.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_fr_it_beam_vert.model"
    stacked.label_0 : string = "component1"
    stacked.model_1 : string = "art_panel_fr_it.model"
    stacked.label_1 : string = "panel"
    stacked.model_2 : string = "art_fr_it_beam_vert.model"
    stacked.label_2 : string = "component2"
  material.ref : string = "lab_air"


[name="art_panel_fr_it_panel_full_build_short.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_fr_it_beam_vert.model"
    stacked.label_0 : string = "component1"
    stacked.model_1 : string = "art_panel_fr_it_short.model"
    stacked.label_1 : string = "panel"
    stacked.model_2 : string = "art_fr_it_beam_vert.model"
    stacked.label_2 : string = "component2"
  material.ref : string = "lab_air"


[name="art_panel_fr_it_wall.model" type="geomtools::replicated_boxed_model"]
  #@config New shielding wall
  replicated.axis            : string = "y"
  replicated.number_of_items : integer = 4
  replicated.model           : string = "art_panel_fr_it_panel_full_build.model"
  replicated.label           : string = "panels"
  material.ref : string = "lab_air"
  

[name="art_panel_fr_it_wall_full.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_panel_fr_it_panel_full_build_short.model"
    stacked.label_0 : string = "outer_part1"
    stacked.model_1 : string = "art_panel_fr_it_wall.model"
    stacked.label_1 : string = "inner_part"
    stacked.model_2 : string = "art_panel_fr_it_panel_full_build_short.model"
    stacked.label_2 : string = "outer_part2"
  material.ref : string = "lab_air"


 [name="art_panel_fr_it_wall_full_build.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_fr_it_beam_horz.model"
    stacked.label_0 : string = "component1"
    stacked.model_1 : string = "art_panel_fr_it_wall_full.model"
    stacked.label_1 : string = "panel"
    stacked.model_2 : string = "art_fr_it_beam_horz.model"
    stacked.label_2 : string = "component2"
  material.ref : string = "lab_air"


#///////////////Mo - Tu Walls////////////////

 [name="panel_mo_tu.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 1300 mm
    y : real as length = 5 mm
    z : real as length = 3600 mm
  material.ref : string = "basic::polyethylene"
  visibility.color : string  = "blue"


[name="ss_mo_tu_component_vert.model" type="geomtools::simple_shaped_model"]
   #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 40 mm
    y : real as length = 11.5 mm
    z : real as length = 3600 mm
  material.ref : string = "basic::iron"
  visibility.color : string  = "orange"


[name="ss_mo_tu_component_horz.model" type="geomtools::simple_shaped_model"]
   #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 2700 mm
    y : real as length = 11.5 mm
    z : real as length = 40 mm
  material.ref : string = "basic::iron"
  visibility.color : string  = "orange"
  

[name="art_panel_mo_tu_wall.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "x"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "ss_mo_tu_component_vert.model"
    stacked.label_0 : string = "component1"
    stacked.model_1 : string = "panel_mo_tu.model"
    stacked.label_1 : string = "panel"
    stacked.model_2 : string = "ss_mo_tu_component_vert.model"
    stacked.label_2 : string = "component2"
  material.ref : string = "lab_air"


[name="art_panel_mo_tu_wall_full.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "x"
  stacked.number_of_items : integer = 2
    stacked.model_0 : string = "art_panel_mo_tu_wall.model"
    stacked.label_0 : string = "panel_mo_tu_wall1"
    stacked.model_1 : string = "art_panel_mo_tu_wall.model"
    stacked.label_1 : string = "panel_mo_tu_wall2"
  material.ref : string = "lab_air"


[name="art_panel_mo_tu_wall_full_build.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "z"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "ss_mo_tu_component_horz.model"
    stacked.label_0 : string = "component1"
    stacked.model_1 : string = "art_panel_mo_tu_wall_full.model"
    stacked.label_1 : string = "panel"
    stacked.model_2 : string = "ss_mo_tu_component_horz.model"
    stacked.label_2 : string = "component2"
  material.ref : string = "lab_air"


#///////////////Ceiling Walls////////////////

[name="art_panel_ce.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 2700 mm
    y : real as length = 1350 mm
    z : real as length = 5 mm
  material.ref : string = "basic::polyethylene"
  visibility.color            : string  = "blue"


[name="art_ce_beam_horz.model" type="geomtools::simple_shaped_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 2700 mm
    y : real as length = 40 mm
    z : real as length = 11.5 mm
  material.ref : string = "basic::iron"
  visibility.color            : string  = "orange"


[name="art_ce_beam_horz_long.model" type="geomtools::simple_shaped_model"]
#@config New shielding wall
  shape_type : string = "box"
    x : real as length = 40 mm
    y : real as length = 7300 mm
    z : real as length = 11.5 mm
  material.ref : string = "basic::iron"
  visibility.color            : string  = "orange"


[name="art_panel_ce_full.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "y"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_ce_beam_horz.model"
    stacked.label_0 : string = "beam1"
    stacked.model_1 : string = "art_panel_ce.model"
    stacked.label_1 : string = "panel"
    stacked.model_2 : string = "art_ce_beam_horz.model"
    stacked.label_2 : string = "beam2"
  material.ref : string = "lab_air"


[name="art_panel_ce_panel_full.model" type="geomtools::replicated_boxed_model"]
  #@config New shielding wall
  replicated.axis            : string = "y"
  replicated.number_of_items : integer = 5
  replicated.model           : string = "art_panel_ce_full.model"
  replicated.label           : string = "panels"
  material.ref : string = "lab_air"


[name="art_panel_ce_panel_full_build.model" type="geomtools::stacked_model"]
  #@config Simplified demonstrator module
  stacked.axis : string = "x"
  stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_ce_beam_horz_long.model"
    stacked.label_0 : string = "beam1"
    stacked.model_1 : string = "art_panel_ce_panel_full.model"
    stacked.label_1 : string = "panel"
    stacked.model_2 : string = "art_ce_beam_horz_long.model"
    stacked.label_2 : string = "beam2"
  material.ref : string = "lab_air"


#///////////////Combined Model////////////////

[name="combined_art_model.model" type="geomtools::simple_shaped_model"]
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 3000 mm
    y : real as length = 6000 mm
    z : real as length = 5000 mm
   material.ref : string = "lab_air"
      internal_item.labels : string[6] = \
      "panel_fr_wall" \
      "panel_it_wall" \
      "panel_mo_wall" \
      "panel_tu_wall" \
      "panel_ce_wall" \
      "panel_floor_wall" 

      internal_item.model.panel_fr_wall     : string = "art_panel_fr_it_wall_full_build.model"
      internal_item.placement.panel_fr_wall : string = "1375 0 0 (mm) "
      internal_item.model.panel_it_wall     : string = "art_panel_fr_it_wall_full_build.model"
      internal_item.placement.panel_it_wall : string = "-1375 0 0 (mm) "

      internal_item.model.panel_mo_wall     : string = "art_panel_mo_tu_wall_full_build.model"
      internal_item.placement.panel_mo_wall : string = "0 3650 440 (mm) "
      internal_item.model.panel_tu_wall     : string = "art_panel_mo_tu_wall_full_build.model"
      internal_item.placement.panel_tu_wall : string = "0 -3650 440 (mm) "

      internal_item.model.panel_ce_wall     : string = "art_panel_ce_panel_full_build.model"
      internal_item.placement.panel_ce_wall : string = "0 0 2250 (mm) "
      internal_item.model.panel_floor_wall     : string = "art_panel_ce_panel_full_build.model"
      internal_item.placement.panel_floor_wall : string = "0 0 -2250 (mm) "

# end












