# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.1/models/source_module/source_frame_horizontal_beams.geom

#########################################################
#  Source frame horizontal (top=z<0/bottom=z>0) beams   #
#########################################################


###############################################################################
[name="source_horizontal_beam_rod.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "cylinder"
z                 : real as length = 39.0 mm
r                 : real as length =  6.0 mm

material.ref      : string = "basic::iron"


visibility.color  : string  = "blue"


#####################################################################################
[name="source_horizontal_beam_boxed_rod.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 39.0 mm
y                 : real as length = 20.0 mm
z                 : real as length = 20.0 mm

material.ref      : string = "lab_air"



internal_item.labels        : string[1] = "rod"
internal_item.placement.rod : string    = "0 0 0 (mm) / y 90.0 (deg)"
internal_item.model.rod     : string    = "source_horizontal_beam_rod.model"


#################################################################################
[name="source_horizontal_beam_multirod.model" type="geomtools::replicated_model"]

#@description The stacking axis
replicated.axis : string = "y"

#@description The number of stacked volumes
replicated.number_of_items : integer = 42

#@description The model of replicated volumes
replicated.model        : string = "source_horizontal_beam_boxed_rod.model"

#@description The label of replicated volumes
replicated.label : string = "pads"

#@description The replicated step
replicated.step  : real as length = 129.28 mm

# #@description Thickness of the mother box
# x                : real as length = 39.0 mm

# #@description Thickness of the mother box
# z                : real as length = 20.0 mm

#@description Height of the mother box
y                 : real as length = 5430.0 mm

material.ref      : string = "lab_air"




####################################################################################
[name="source_horizontal_beam_main_air.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =   39.0 mm
y                 : real as length = 5430.0 mm
z                 : real as length =   30.5 mm

material.ref      : string = "lab_air"


######################################################################################
[name="source_horizontal_beam_main_plate.model" type="geomtools::simple_shaped_model"]

shape_type         : string = "box"
x                  : real as length =   39.0 mm
y                  : real as length = 5430.0 mm
z                  : real as length =    9.5 mm

material.ref       : string = "basic::iron"

visibility.color   : string  = "blue"


###############################################################################
[name="source_zneg_horizontal_beam_main.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_horizontal_beam_multirod.model"
stacked.label_0   : string = "rods"

stacked.model_1   : string = "source_horizontal_beam_main_air.model"
stacked.label_1   : string = "air"

stacked.model_2   : string = "source_horizontal_beam_main_plate.model"
stacked.label_2   : string = "plate"

material.ref      : string = "lab_air"


visibility.color  : string = "grey"


###############################################################################
[name="source_zpos_horizontal_beam_main.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_horizontal_beam_main_plate.model"
stacked.label_0   : string = "plate"

stacked.model_1   : string = "source_horizontal_beam_main_air.model"
stacked.label_1   : string = "air"

stacked.model_2   : string = "source_horizontal_beam_multirod.model"
stacked.label_2   : string = "rods"

material.ref      : string = "lab_air"


visibility.color  : string = "grey"


######################################################################################
[name="source_horizontal_beam_side_plate.model" type="geomtools::simple_shaped_model"]

shape_type         : string = "box"
x                  : real as length =    6.5 mm
y                  : real as length = 5430.0 mm
z                  : real as length =   60.0 mm

material.ref       : string = "basic::iron"


visibility.color   : string  = "blue"


###############################################################################################
[name="source_horizontal_beam_tracker_interface_a.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame

shape_type   : string  = "box"
x            : real as length =    3.0 mm
y            : real as length = 5400.0 mm
z            : real as length =   30.0 mm

material.ref : string  = "basic::iron"


visibility.color   : string  = "blue"


###############################################################################################
[name="source_horizontal_beam_tracker_interface_b.model" type="geomtools::simple_shaped_model"]

shape_type   : string  = "box"
x            : real as length =    3.0 mm
y            : real as length = 5410.0 mm
z            : real as length =   20.0 mm

material.ref : string  = "RTV"


visibility.color   : string  = "magenta"


###############################################################################################
[name="source_horizontal_beam_tracker_interface_c.model" type="geomtools::simple_shaped_model"]

shape_type   : string  = "box"

x            : real as length =    3.0 mm
y            : real as length = 5420.0 mm
z            : real as length =    5.0 mm

material.ref : string  = "basic::iron"


visibility.color   : string  = "blue"


###############################################################################################
[name="source_horizontal_beam_tracker_interface_d.model" type="geomtools::simple_shaped_model"]

shape_type   : string  = "box"
x            : real as length =    3.0 mm
y            : real as length = 5430.0 mm
z            : real as length =    5.0 mm

material.ref : string  = "SBR"


visibility.color   : string  = "orange"


###############################################################################################
[name="source_horizontal_beam_tracker_interface_e.model" type="geomtools::simple_shaped_model"]

shape_type   : string  = "box"

x            : real as length =  3.0 mm
y            : real as length =  5.0 mm
z            : real as length = 30.0 mm

material.ref : string  = "RTV"


visibility.color   : string  = "magenta"


###############################################################################################
[name="source_horizontal_beam_tracker_interface_f.model" type="geomtools::simple_shaped_model"]

shape_type   : string  = "box"
x            : real as length =  3.0 mm
y            : real as length =  5.0 mm
z            : real as length = 50.0 mm

material.ref : string  = "basic::iron"


visibility.color   : string  = "blue"


###############################################################################################
[name="source_horizontal_beam_tracker_interface_g.model" type="geomtools::simple_shaped_model"]

shape_type   : string  = "box"
x            : real as length =  3.0 mm
y            : real as length =  5.0 mm
z            : real as length = 55.0 mm

material.ref : string  = "SBR"


visibility.color   : string  = "orange"


##################################################################################################
[name="source_zpos_horizontal_beam_tracker_interface.model" type="geomtools::simple_shaped_model"]

shape_type   : string  = "box"
x            : real as length =    3.0 mm
y            : real as length = 5430.0 mm
z            : real as length =   60.0 mm

material.ref : string  = "lab_air"

internal_item.labels  : string[10] = \
		      "a" "b" "c" "d" \
		      "e_left" "e_right" \
		      "f_left" "f_right" \
		      "g_left" "g_right"

internal_item.placement.a       : string  = "0 0 -15 (mm)"
internal_item.model.a           : string  = "source_horizontal_beam_tracker_interface_a.model"

internal_item.placement.b       : string  = "0 0 +10 (mm)"
internal_item.model.b           : string  = "source_horizontal_beam_tracker_interface_b.model"

internal_item.placement.c       : string  = "0 0 +22.5 (mm)"
internal_item.model.c           : string  = "source_horizontal_beam_tracker_interface_c.model"

internal_item.placement.d       : string  = "0 0 +27.5 (mm)"
internal_item.model.d           : string  = "source_horizontal_beam_tracker_interface_d.model"

internal_item.placement.e_left  : string = "0 -2702.5 -15 (mm)"
internal_item.model.e_left      : string = "source_horizontal_beam_tracker_interface_e.model"

internal_item.placement.e_right : string = "0 -2702.5 -15 (mm)"
internal_item.model.e_right     : string = "source_horizontal_beam_tracker_interface_e.model"

internal_item.placement.f_left  : string = "0 -2707.5 -5 (mm)"
internal_item.model.f_left      : string = "source_horizontal_beam_tracker_interface_f.model"

internal_item.placement.f_right : string = "0 +2707.5 -5 (mm)"
internal_item.model.f_right     : string = "source_horizontal_beam_tracker_interface_f.model"

internal_item.placement.g_left  : string = "0 -2712.5 -2.5 (mm)"
internal_item.model.g_left      : string = "source_horizontal_beam_tracker_interface_g.model"

internal_item.placement.g_right : string = "0 +2712.5 -2.5 (mm)"
internal_item.model.g_right     : string = "source_horizontal_beam_tracker_interface_g.model"


visibility.color   : string  = "grey"


##################################################################################################
[name="source_zneg_horizontal_beam_tracker_interface.model" type="geomtools::simple_shaped_model"]

shape_type   : string  = "box"
x            : real as length =    3.0 mm
y            : real as length = 5430.0 mm
z            : real as length =   60.0 mm

material.ref : string  = "lab_air"

internal_item.labels  : string[10] = \
		      "a" "b" "c" "d" \
		      "e_left" "e_right" \
		      "f_left" "f_right" \
		      "g_left" "g_right"

internal_item.placement.a       : string = "0 0 +15 (mm)"
internal_item.model.a           : string = "source_horizontal_beam_tracker_interface_a.model"

internal_item.placement.b       : string = "0 0 -10 (mm)"
internal_item.model.b           : string = "source_horizontal_beam_tracker_interface_b.model"

internal_item.placement.c       : string = "0 0 -22.5 (mm)"
internal_item.model.c           : string = "source_horizontal_beam_tracker_interface_c.model"

internal_item.placement.d       : string = "0 0 -27.5 (mm)"
internal_item.model.d           : string = "source_horizontal_beam_tracker_interface_d.model"

internal_item.placement.e_left  : string = "0 -2707.5 +15 (mm)"
internal_item.model.e_left      : string = "source_horizontal_beam_tracker_interface_e.model"

internal_item.placement.e_right : string = "0 +2707.5 +15 (mm)"
internal_item.model.e_right     : string = "source_horizontal_beam_tracker_interface_e.model"

internal_item.placement.f_left  : string = "0 -2707.5 +5 (mm)"
internal_item.model.f_left      : string = "source_horizontal_beam_tracker_interface_f.model"

internal_item.placement.f_right : string = "0 +2707.5 +5 (mm)"
internal_item.model.f_right     : string = "source_horizontal_beam_tracker_interface_f.model"

internal_item.placement.g_left  : string = "0 -2712.5 +2.5 (mm)"
internal_item.model.g_left      : string = "source_horizontal_beam_tracker_interface_g.model"

internal_item.placement.g_right : string = "0 +2712.5 +2.5 (mm)"
internal_item.model.g_right     : string = "source_horizontal_beam_tracker_interface_g.model"


visibility.color   : string  = "grey"


##########################################################################
[name="source_zneg_horizontal_beam.model" type="geomtools::stacked_model"]

#@config Source horizontal beam (z<0)

stacked.axis            : string = "x"
stacked.number_of_items : integer = 5

stacked.model_0   : string = "source_zneg_horizontal_beam_tracker_interface.model"
stacked.label_0   : string = "interface0"

stacked.model_1   : string = "source_horizontal_beam_side_plate.model"
stacked.label_1   : string = "side0"

stacked.model_2   : string = "source_zneg_horizontal_beam_main.model"
stacked.label_2   : string = "main"

stacked.model_3   : string = "source_horizontal_beam_side_plate.model"
stacked.label_3   : string = "side1"

stacked.model_4   : string = "source_zneg_horizontal_beam_tracker_interface.model"
stacked.label_4   : string = "interface1"

material.ref      : string = "lab_air"




##########################################################################
[name="source_zpos_horizontal_beam.model" type="geomtools::stacked_model"]

#@config Source horizontal beam (z>0)

stacked.axis            : string = "x"
stacked.number_of_items : integer = 5

stacked.model_0   : string = "source_zpos_horizontal_beam_tracker_interface.model"
stacked.label_0   : string = "interface0"

stacked.model_1   : string = "source_horizontal_beam_side_plate.model"
stacked.label_1   : string = "side0"

stacked.model_2   : string = "source_zpos_horizontal_beam_main.model"
stacked.label_2   : string = "main"

stacked.model_3   : string = "source_horizontal_beam_side_plate.model"
stacked.label_3   : string = "side1"

stacked.model_4   : string = "source_zpos_horizontal_beam_tracker_interface.model"
stacked.label_4   : string = "interface1"

material.ref      : string = "lab_air"


# end of @falaise:config/snemo/demonstrator/geometry/4.1/models/source_module/source_frame_horizontal_beams.geom
