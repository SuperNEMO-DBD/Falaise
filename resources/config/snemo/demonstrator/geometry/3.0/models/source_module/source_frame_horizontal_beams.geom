# -*- mode: conf-unix; -*-

#########################################################
#      Source frame horizontal (top/bottom) beams       #
#########################################################


######################################################################################
[name="source_horizontal_beam_main_plate.model" type="geomtools::simple_shaped_model"]
shape_type         : string = "box"
x                  : real   =   39.0 # mm
y                  : real   = 5430.0 # mm
z                  : real   =    9.5 # mm
length_unit        : string = "mm"
material.ref       : string = "std::iron"
visibility.hidden  : boolean = 0
visibility.color   : string  = "blue"


####################################################################################
[name="source_horizontal_beam_main_air.model" type="geomtools::simple_shaped_model"]
shape_type         : string = "box"
x                  : real   =   39.0 # mm
y                  : real   = 5300.0 # mm
z                  : real   =   50.5 # mm
length_unit        : string = "mm"
material.ref       : string = "lab_air"
visibility.hidden  : boolean = 1


###################################################################
[name="source_top_beam_main.model" type="geomtools::stacked_model"]
stacked.axis            : string = "z"
stacked.number_of_items : integer = 2
material.ref       : string = "lab_air"

stacked.model_1   : string  = "source_horizontal_beam_main_air.model"
stacked.label_1   : string  = "air"

stacked.model_0   : string  = "source_horizontal_beam_main_plate.model"
stacked.label_0   : string  = "plate"

visibility.hidden : boolean   = 0
visibility.color  : string    = "grey"


######################################################################
[name="source_bottom_beam_main.model" type="geomtools::stacked_model"]
stacked.axis            : string = "z"
stacked.number_of_items : integer = 2
material.ref       : string = "lab_air"

stacked.model_1   : string  = "source_horizontal_beam_main_plate.model"
stacked.label_1   : string  = "plate"

stacked.model_0   : string  = "source_horizontal_beam_main_air.model"
stacked.label_0   : string  = "air"

visibility.hidden : boolean   = 0
visibility.color  : string    = "grey"


######################################################################################
[name="source_horizontal_beam_side_plate.model" type="geomtools::simple_shaped_model"]
shape_type         : string = "box"
x                  : real   =   6.5 # mm
y                  : real   = 5430.0 # mm
z                  : real   =   60.0 # mm
length_unit        : string = "mm"
material.ref       : string = "std::iron"
visibility.hidden  : boolean = 0
visibility.color   : string  = "blue"


#######################################################################################
[name="source_horizontal_beam_interface_a.model" type="geomtools::simple_shaped_model"]
#@config Interface with the tracker frame
shape_type   : string  = "box"
lenght_unit  : string  = "mm"
x            : real    =    3
y            : real    = 5370
z            : real    =   30
material.ref : string  = "std::iron"
visibility.hidden  : boolean = 0
visibility.color   : string  = "blue"


#######################################################################################
[name="source_horizontal_beam_interface_b.model" type="geomtools::simple_shaped_model"]
shape_type   : string  = "box"
lenght_unit  : string  = "mm"
x            : real    =    3
y            : real    = 5410
z            : real    =   20
material.ref : string  = "RTV"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"


#######################################################################################
[name="source_horizontal_beam_interface_c.model" type="geomtools::simple_shaped_model"]
shape_type   : string  = "box"
lenght_unit  : string  = "mm"
x            : real    =  3
y            : real    = 20
z            : real    = 30
material.ref : string  = "RTV"
visibility.hidden  : boolean = 0
visibility.color   : string  = "magenta"


#######################################################################################
[name="source_horizontal_beam_interface_d.model" type="geomtools::simple_shaped_model"]
shape_type   : string  = "box"
lenght_unit  : string  = "mm"
x            : real    =    3
y            : real    = 5420
z            : real    =    5
material.ref : string  = "std::iron"
visibility.hidden  : boolean = 0
visibility.color   : string  = "blue"


#######################################################################################
[name="source_horizontal_beam_interface_e.model" type="geomtools::simple_shaped_model"]
shape_type   : string  = "box"
lenght_unit  : string  = "mm"
x            : real    =  3
y            : real    =  5
z            : real    = 50
material.ref : string  = "std::iron"
visibility.hidden  : boolean = 0
visibility.color   : string  = "blue"

#######################################################################################
[name="source_horizontal_beam_interface_f.model" type="geomtools::simple_shaped_model"]
shape_type   : string  = "box"
lenght_unit  : string  = "mm"
x            : real    =    3
y            : real    = 5430
z            : real    =    5
material.ref : string  = "SBR"
visibility.hidden  : boolean = 0
visibility.color   : string  = "orange"


#######################################################################################
[name="source_horizontal_beam_interface_g.model" type="geomtools::simple_shaped_model"]
shape_type   : string  = "box"
lenght_unit  : string  = "mm"
x            : real    =  3
y            : real    =  5
z            : real    = 55
material.ref : string  = "SBR"
visibility.hidden  : boolean = 0
visibility.color   : string  = "orange"


##############################################################################
[name="source_top_beam_interface.model" type="geomtools::simple_shaped_model"]
shape_type   : string  = "box"
lenght_unit  : string  = "mm"
x            : real    =    3
y            : real    = 5430
z            : real    =   60
material.ref : string  = "lab_air"

internal_item.labels  : string[10] = \
		      "a" "b" "d" "f" \
		      "c_left" "c_right" \
		      "e_left" "e_right" \
		      "g_left" "g_right"

internal_item.placement.a : string  = "0 0 -15 (mm)"
internal_item.model.a     : string  = "source_horizontal_beam_interface_a.model"

internal_item.placement.b : string  = "0 0 +10 (mm)"
internal_item.model.b     : string  = "source_horizontal_beam_interface_b.model"

internal_item.placement.d : string  = "0 0 +22.5 (mm)"
internal_item.model.d     : string  = "source_horizontal_beam_interface_d.model"

internal_item.placement.f : string  = "0 0 +27.5 (mm)"
internal_item.model.f     : string  = "source_horizontal_beam_interface_f.model"

internal_item.placement.c_left  : string  = "0 -2695 -15 (mm)"
internal_item.model.c_left      : string  = "source_horizontal_beam_interface_c.model"

internal_item.placement.c_right : string = "0 +2695 -15 (mm)"
internal_item.model.c_right     : string = "source_horizontal_beam_interface_c.model"

internal_item.placement.e_left  : string  = "0 -2707.5 -5 (mm)"
internal_item.model.e_left      : string  = "source_horizontal_beam_interface_e.model"

internal_item.placement.e_right : string = "0 +2707.5 -5 (mm)"
internal_item.model.e_right     : string = "source_horizontal_beam_interface_e.model"

internal_item.placement.g_left  : string  = "0 -2712.5 -2.5 (mm)"
internal_item.model.g_left      : string  = "source_horizontal_beam_interface_g.model"

internal_item.placement.g_right : string  = "0 +2712.5 -2.5 (mm)"
internal_item.model.g_right     : string  = "source_horizontal_beam_interface_g.model"

visibility.hidden  : boolean = 0
visibility.color   : string  = "grey"


#################################################################################
[name="source_bottom_beam_interface.model" type="geomtools::simple_shaped_model"]
shape_type   : string  = "box"
lenght_unit  : string  = "mm"
x            : real    =    3
y            : real    = 5430
z            : real    =   60
material.ref : string  = "lab_air"

internal_item.labels  : string[10] = \
		      "a" "b" "d" "f" \
		      "c_left" "c_right" \
		      "e_left" "e_right" \
		      "g_left" "g_right"

internal_item.placement.a : string  = "0 0 +15 (mm)"
internal_item.model.a     : string  = "source_horizontal_beam_interface_a.model"

internal_item.placement.b : string  = "0 0 -10 (mm)"
internal_item.model.b     : string  = "source_horizontal_beam_interface_b.model"

internal_item.placement.d : string  = "0 0 -22.5 (mm)"
internal_item.model.d     : string  = "source_horizontal_beam_interface_d.model"

internal_item.placement.f : string  = "0 0 -27.5 (mm)"
internal_item.model.f     : string  = "source_horizontal_beam_interface_f.model"

internal_item.placement.c_left  : string  = "0 -2695 +15 (mm)"
internal_item.model.c_left      : string  = "source_horizontal_beam_interface_c.model"

internal_item.placement.c_right : string = "0 +2695 +15 (mm)"
internal_item.model.c_right     : string = "source_horizontal_beam_interface_c.model"

internal_item.placement.e_left  : string  = "0 -2707.5 +5 (mm)"
internal_item.model.e_left      : string  = "source_horizontal_beam_interface_e.model"

internal_item.placement.e_right : string = "0 +2707.5 +5 (mm)"
internal_item.model.e_right     : string = "source_horizontal_beam_interface_e.model"

internal_item.placement.g_left  : string  = "0 -2712.5 +2.5 (mm)"
internal_item.model.g_left      : string  = "source_horizontal_beam_interface_g.model"

internal_item.placement.g_right : string  = "0 +2712.5 +2.5 (mm)"
internal_item.model.g_right     : string  = "source_horizontal_beam_interface_g.model"

visibility.hidden  : boolean = 0
visibility.color   : string  = "grey"


##############################################################
[name="source_top_beam.model" type="geomtools::stacked_model"]
#@config Source beam (top)
material.ref       : string = "std::air"
visibility.hidden  : boolean = 0

stacked.axis            : string = "x"
stacked.number_of_items : integer = 5

stacked.model_4   : string  = "source_top_beam_interface.model"
stacked.label_4   : string  = "interface1"

stacked.model_3   : string  = "source_horizontal_beam_side_plate.model"
stacked.label_3   : string  = "side1"

stacked.model_2   : string  = "source_top_beam_main.model"
stacked.label_2   : string  = "main"

stacked.model_1   : string  = "source_horizontal_beam_side_plate.model"
stacked.label_1   : string  = "side0"

stacked.model_0   : string  = "source_top_beam_interface.model"
stacked.label_0   : string  = "interface0"


#################################################################
[name="source_bottom_beam.model" type="geomtools::stacked_model"]
#@config Source beam (bottom)
material.ref       : string = "std::air"
visibility.hidden  : boolean = 0

stacked.axis            : string = "x"
stacked.number_of_items : integer = 5

stacked.model_4   : string  = "source_bottom_beam_interface.model"
stacked.label_4   : string  = "interface1"

stacked.model_3   : string  = "source_horizontal_beam_side_plate.model"
stacked.label_3   : string  = "side1"

stacked.model_2   : string  = "source_bottom_beam_main.model"
stacked.label_2   : string  = "main"

stacked.model_1   : string  = "source_horizontal_beam_side_plate.model"
stacked.label_1   : string  = "side0"

stacked.model_0   : string  = "source_bottom_beam_interface.model"
stacked.label_0   : string  = "interface0"


# end of file
