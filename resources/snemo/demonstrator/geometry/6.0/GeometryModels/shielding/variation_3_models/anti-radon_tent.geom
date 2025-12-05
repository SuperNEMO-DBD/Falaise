#/////////////////////////////ART Model/////////////////////////////////


#///////////////Fr - It Walls////////////////

[name="art_panel_fr_it.model" type="geomtools::simple_boxed_model"]
#@config Art subpanel (fr/it)
shape_type : string = "box"
    x : real as length = 5 mm
    y : real as length = 1200 mm
    z : real as length = 4500 mm
material.ref : string = "basic::polyethylene"
visibility.color : string  = "grey"


[name="art_panel_fr_it_short.model" type="geomtools::simple_boxed_model"]
#@config Art subpanel (short) (fr/it)
shape_type : string = "box"
    x : real as length = 5 mm
    y : real as length = 1000 mm
    z : real as length = 4500 mm
material.ref : string = "basic::polyethylene"
visibility.color : string  = "grey"


[name="art_fr_it_beam_vert.model" type="geomtools::simple_shaped_model"]
#@config Art subpanel vertical beam (fr/it)
shape_type : string = "box"
    x : real as length = 11.5 mm
    y : real as length = 40 mm
    z : real as length = 4500 mm
material.ref : string = "basic::iron"
visibility.color : string  = "cyan"


[name="art_fr_it_beam_horz.model" type="geomtools::simple_shaped_model"]
#@config Art subpanel horizontal beam (fr/it)
shape_type : string = "box"
    x : real as length = 40 mm
    y : real as length = 7300 mm
    z : real as length = 11.5 mm
material.ref : string = "basic::iron"
visibility.color : string  = "cyan"


[name="art_panel_fr_it_panel_full_build.model" type="geomtools::stacked_model"]
#@config Art subpanel assembly (fr/it)
stacked.axis : string = "y"
stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_fr_it_beam_vert.model"
    stacked.label_0 : string = "beam0"
    stacked.model_1 : string = "art_panel_fr_it.model"
    stacked.label_1 : string = "window"
    stacked.model_2 : string = "art_fr_it_beam_vert.model"
    stacked.label_2 : string = "beam1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.beam0  : string = "[art_xbeam:beam=0]"
mapping.daughter_id.window : string = "[art_window]"
mapping.daughter_id.beam1  : string = "[art_xbeam:beam=1]"


[name="art_panel_fr_it_panel_full_build_short.model" type="geomtools::stacked_model"]
#@config Art subpanel assembly (short) (fr/it)
stacked.axis : string = "y"
stacked.number_of_items : integer = 3
stacked.model_0 : string = "art_fr_it_beam_vert.model"
stacked.label_0 : string = "beam0"
stacked.model_1 : string = "art_panel_fr_it_short.model"
stacked.label_1 : string = "window"
stacked.model_2 : string = "art_fr_it_beam_vert.model"
stacked.label_2 : string = "beam1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.beam0  : string = "[art_xbeam:beam=0]"
mapping.daughter_id.window : string = "[art_window]"
mapping.daughter_id.beam1  : string = "[art_xbeam:beam=1]"


[name="art_panel_fr_it_wall.model" type="geomtools::replicated_boxed_model"]
#@config Art panel internal assembly (fr/it)
replicated.axis            : string = "y"
replicated.number_of_items : integer = 4
replicated.model           : string = "art_panel_fr_it_panel_full_build.model"
replicated.label           : string = "panels"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"
  
mapping.daughter_id.panels  : string = "[art_panel:panel+1]"

[name="art_panel_fr_it_wall_full.model" type="geomtools::stacked_model"]
#@config Art panel full assembly (fr/it)
stacked.axis : string = "y"
stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_panel_fr_it_panel_full_build_short.model"
    stacked.label_0 : string = "outer_part0"
    stacked.model_1 : string = "art_panel_fr_it_wall.model"
    stacked.label_1 : string = "inner_part"
    stacked.model_2 : string = "art_panel_fr_it_panel_full_build_short.model"
    stacked.label_2 : string = "outer_part1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.outer_part0 : string = "[art_panel:panel=0]"
mapping.daughter_id.outer_part1 : string = "[art_panel:panel=5]"

[name="art_panel_fr_it_wall_full_build.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "z"
stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_fr_it_beam_horz.model"
    stacked.label_0 : string = "beam0"
    stacked.model_1 : string = "art_panel_fr_it_wall_full.model"
    stacked.label_1 : string = "panels"
    stacked.model_2 : string = "art_fr_it_beam_horz.model"
    stacked.label_2 : string = "beam1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.beam0  : string = "[art_beam:beam=0]"
mapping.daughter_id.panels : string = "[art_panel_set]"
mapping.daughter_id.beam1  : string = "[art_beam:beam=1]"


#///////////////Mo - Tu Walls////////////////

[name="art_panel_mo_tu.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
    x : real as length = 1300 mm
    y : real as length = 5 mm
    z : real as length = 3600 mm
material.ref : string = "basic::polyethylene"
visibility.color : string  = "grey"


[name="art_ss_mo_tu_beam_vert.model" type="geomtools::simple_shaped_model"]
#@config New shielding wall
shape_type : string = "box"
    x : real as length = 40 mm
    y : real as length = 11.5 mm
    z : real as length = 3600 mm
material.ref : string = "basic::iron"
visibility.color : string  = "cyan"


[name="art_ss_mo_tu_beam_horz.model" type="geomtools::simple_shaped_model"]
#@config New shielding wall
shape_type : string = "box"
    # x : real as length = 2700 mm
    x : real as length = 2760 mm
    y : real as length = 11.5 mm
    z : real as length = 40 mm
material.ref : string = "basic::iron"
visibility.color : string  = "cyan"
  

[name="art_panel_mo_tu_wall.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "x"
stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_ss_mo_tu_beam_vert.model"
    stacked.label_0 : string = "beam0"
    stacked.model_1 : string = "art_panel_mo_tu.model"
    stacked.label_1 : string = "window"
    stacked.model_2 : string = "art_ss_mo_tu_beam_vert.model"
    stacked.label_2 : string = "beam1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.beam1  : string = "[art_xbeam:beam=0]"
mapping.daughter_id.window : string = "[art_window]"
mapping.daughter_id.beam2  : string = "[art_xbeam:beam=1]"

[name="art_panel_mo_tu_wall_full1.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "x"
stacked.number_of_items : integer = 2
    stacked.model_0 : string = "art_panel_mo_tu_wall.model"
    stacked.label_0 : string = "panel0"
    stacked.model_1 : string = "art_panel_mo_tu_wall.model"
    stacked.label_1 : string = "panel1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.panel0 : string = "[art_panel:panel=0]"
mapping.daughter_id.panel1 : string = "[art_panel:panel=1]"
 

[name="art_patch_panel_mo_tu.model" type="geomtools::simple_boxed_model"]
#@config Cable patch panel
shape_type : string = "box"
    x : real as length = 1380 mm
    y : real as length = 5 mm
    z : real as length = 850 mm
material.ref : string = "basic::iron"
visibility.color : string  = "blue"


[name="art_patch_panel_mo_tu_wall_full1.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "x"
stacked.number_of_items : integer = 2
    stacked.model_0 : string = "art_patch_panel_mo_tu.model"
    stacked.label_0 : string = "panel0"
    stacked.model_1 : string = "art_patch_panel_mo_tu.model"
    stacked.label_1 : string = "panel1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.panel0 : string = "[art_patch_panel:panel=0]"
mapping.daughter_id.panel1 : string = "[art_patch_panel:panel=1]"


[name="art_panel_mo_tu_wall_full_build.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "z"
stacked.number_of_items : integer = 4
    stacked.model_0 : string = "art_patch_panel_mo_tu_wall_full1.model"
    stacked.label_0 : string = "patch_panels"
    stacked.model_1 : string = "art_ss_mo_tu_beam_horz.model"
    stacked.label_1 : string = "beam0"
    stacked.model_2 : string = "art_panel_mo_tu_wall_full1.model"
    stacked.label_2 : string = "panels"
    stacked.model_3 : string = "art_ss_mo_tu_beam_horz.model"
    stacked.label_3 : string = "beam1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.beam0  : string = "[art_beam:beam=0]"
mapping.daughter_id.panels : string = "[art_panel_set]"
mapping.daughter_id.beam1  : string = "[art_beam:beam=1]"


#///////////////Ceiling Walls////////////////

[name="art_panel_ce.model" type="geomtools::simple_boxed_model"]
#@config New shielding wall
shape_type : string = "box"
    x : real as length = 2700 mm
    y : real as length = 1350 mm
    z : real as length = 5 mm
material.ref : string = "basic::polyethylene"
visibility.color : string  = "grey"


[name="art_ce_beam_horz.model" type="geomtools::simple_shaped_model"]
#@config New shielding wall
shape_type : string = "box"
    x : real as length = 2700 mm
    y : real as length = 40 mm
    z : real as length = 11.5 mm
material.ref : string = "basic::iron"
visibility.color : string  = "cyan"


[name="art_ce_beam_horz_long.model" type="geomtools::simple_shaped_model"]
#@config New shielding wall
shape_type : string = "box"
    x : real as length = 40 mm
    # y : real as length = 7300 mm
    y : real as length = 7150 mm
    z : real as length = 11.5 mm
material.ref : string = "basic::iron"
visibility.color : string  = "cyan"


[name="art_panel_ce_full.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "y"
stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_ce_beam_horz.model"
    stacked.label_0 : string = "beam0"
    stacked.model_1 : string = "art_panel_ce.model"
    stacked.label_1 : string = "window"
    stacked.model_2 : string = "art_ce_beam_horz.model"
    stacked.label_2 : string = "beam1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.beam0  : string = "[art_xbeam:beam=0]"
mapping.daughter_id.window : string = "[art_window]"
mapping.daughter_id.beam1  : string = "[art_xbeam:beam=1]"


[name="art_panel_ce_panel_full.model" type="geomtools::replicated_boxed_model"]
#@config New shielding wall
replicated.axis            : string = "y"
  replicated.number_of_items : integer = 5
  replicated.model           : string = "art_panel_ce_full.model"
  replicated.label           : string = "panels"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.panels : string = "[art_panel:panel+0]"
 

[name="art_panel_ce_panel_full_build.model" type="geomtools::stacked_model"]
#@config Simplified demonstrator module
stacked.axis : string = "x"
stacked.number_of_items : integer = 3
    stacked.model_0 : string = "art_ce_beam_horz_long.model"
    stacked.label_0 : string = "beam0"
    stacked.model_1 : string = "art_panel_ce_panel_full.model"
    stacked.label_1 : string = "panels"
    stacked.model_2 : string = "art_ce_beam_horz_long.model"
    stacked.label_2 : string = "beam1"
material.ref : string = "lab_air"
visibility.color : string  = "transparent"

mapping.daughter_id.beam0  : string = "[art_beam:beam=0]"
mapping.daughter_id.panels : string = "[art_panel_set]"
mapping.daughter_id.beam1  : string = "[art_beam:beam=1]"


#///////////////Combined Model////////////////
#
# [name="combined_art_model.model" type="geomtools::simple_shaped_model"]
# #@config Simplified demonstrator module (for test only, not used in production Falaise's geometry)
#   
# shape_type : string = "box"
#     x : real as length = 3000 mm
#     y : real as length = 7500 mm
#     z : real as length = 5000 mm
# material.ref : string = "lab_air"
# visibility.color : string  = "transparent"
#
# internal_item.labels : string[10] = \
#       "panel_fr_wall" \
#       "panel_it_wall" \
#       "panel_mo_wall" \
#       "panel_tu_wall" \
#       "panel_ce_wall" \
#       "panel_floor_wall"  \
#       "patch_panel_tu_fr"  \
#       "patch_panel_tu_it" \
#       "patch_panel_mo_fr"  \
#       "patch_panel_mo_it" 
#
#       internal_item.model.panel_fr_wall     : string = "art_panel_fr_it_wall_full_build.model"
#       internal_item.placement.panel_fr_wall : string = "1375 0 0 (mm) "
#       internal_item.model.panel_it_wall     : string = "art_panel_fr_it_wall_full_build.model"
#       internal_item.placement.panel_it_wall : string = "-1375 0 0 (mm) "
#
#       internal_item.model.panel_mo_wall     : string = "art_panel_mo_tu_wall_full_build.model"
#       internal_item.placement.panel_mo_wall : string = "0 3650 440 (mm) "
#       internal_item.model.panel_tu_wall     : string = "art_panel_mo_tu_wall_full_build.model"
#       internal_item.placement.panel_tu_wall : string = "0 -3650 440 (mm) "
#
#       internal_item.model.panel_ce_wall     : string = "art_panel_ce_panel_full_build.model"
#       internal_item.placement.panel_ce_wall : string = "0 0 2250 (mm) "
#       internal_item.model.panel_floor_wall     : string = "art_panel_ce_panel_full_build.model"
#       internal_item.placement.panel_floor_wall : string = "0 0 -2250 (mm) "
#
#       internal_item.model.patch_panel_tu_fr     : string = "art_patch_panel_mo_tu.model"
#       internal_item.placement.patch_panel_tu_fr : string = "+675 3650 -1825 (mm) "
#
#       internal_item.model.patch_panel_tu_it     : string = "art_patch_panel_mo_tu.model"
#       internal_item.placement.patch_panel_tu_it : string = "-675 3650 -1825 (mm) "
#
#       internal_item.model.patch_panel_mo_fr     : string = "art_patch_panel_mo_tu.model"
#       internal_item.placement.patch_panel_mo_fr : string = "+675 -3650 -1825 (mm) "
#
#       internal_item.model.patch_panel_mo_it     : string = "art_patch_panel_mo_tu.model"
#       internal_item.placement.patch_panel_mo_it : string = "-675 -3650 -1825 (mm) "
#

# end












