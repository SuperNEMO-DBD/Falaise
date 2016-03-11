# -*- mode: conf-unix; -*-
# source_frame_vertical_beams.geom

#########################################################
#      Source frame vertical (left/rigth) beams         #
#########################################################


####################################################################################
[name="source_vertical_beam_main_plate.model" type="geomtools::simple_shaped_model"]

shape_type         : string = "box"
x                  : real as length =    39   mm
y                  : real as length =     9.5 mm
z                  : real as length =  3410   mm

material.ref       : string = "basic::copper"

visibility.hidden  : boolean = false
visibility.color   : string = "red"


##################################################################################
[name="source_vertical_beam_main_air.model" type="geomtools::simple_shaped_model"]

shape_type         : string = "box"
x                  : real as length =   39   mm
y                  : real as length =   50.5 mm
z                  : real as length = 3410   mm

material.ref       : string = "lab_air"

visibility.hidden  : boolean = true


#####################################################################
[name="source_right_beam_main.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2
material.ref       : string = "lab_air"

stacked.model_1   : string = "source_vertical_beam_main_air.model"
stacked.label_1   : string = "air"

stacked.model_0   : string = "source_vertical_beam_main_plate.model"
stacked.label_0   : string = "plate"

visibility.hidden : boolean = false
visibility.color  : string = "grey"


####################################################################
[name="source_left_beam_main.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2
material.ref       : string = "lab_air"

stacked.model_1   : string = "source_vertical_beam_main_plate.model"
stacked.label_1   : string = "plate"

stacked.model_0   : string = "source_vertical_beam_main_air.model"
stacked.label_0   : string = "air"

visibility.hidden : boolean = false
visibility.color  : string = "grey"


####################################################################################
[name="source_vertical_beam_side_plate.model" type="geomtools::simple_shaped_model"]

shape_type         : string = "box"
x                  : real as length =    6.5 mm
y                  : real as length =   60   mm
z                  : real as length = 3410   mm

material.ref       : string = "basic::copper"

visibility.hidden  : boolean = false
visibility.color   : string = "red"


#####################################################################################
[name="source_vertical_beam_interface_a.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame

shape_type   : string = "box"
x            : real as length =    3 mm
y            : real as length =   30 mm
z            : real as length = 3410 mm

material.ref : string = "basic::copper"

visibility.hidden  : boolean = false
visibility.color   : string = "red"


#####################################################################################
[name="source_vertical_beam_interface_b.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame

shape_type   : string = "box"
x            : real as length =    3 mm
y            : real as length =   20 mm
z            : real as length = 3410 mm

material.ref : string = "RTV"

visibility.hidden  : boolean = false
visibility.color   : string = "magenta"


#####################################################################################
[name="source_vertical_beam_interface_d.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame
shape_type   : string = "box"
x            : real as length =    3 mm
y            : real as length =    5 mm
z            : real as length = 3410 mm

material.ref : string = "basic::copper"

visibility.hidden  : boolean = false
visibility.color   : string = "red"


#####################################################################################
[name="source_vertical_beam_interface_f.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame
shape_type   : string = "box"
x            : real as length =    3 mm
y            : real as length =    5 mm
z            : real as length = 3410 mm

material.ref : string = "SBR"

visibility.hidden  : boolean = false
visibility.color   : string = "orange"


################################################################################
[name="source_right_beam_interface.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame

shape_type   : string = "box"
x            : real as length =    3 mm
y            : real as length =   60 mm
z            : real as length = 3410 mm

material.ref : string = "lab_air"

internal_item.labels  : string[4] = \
		      "a" "b" "d" "f"

internal_item.placement.a : string = "0 -15 0 (mm)"
internal_item.model.a     : string = "source_vertical_beam_interface_a.model"

internal_item.placement.b : string = "0 +10 0 (mm)"
internal_item.model.b     : string = "source_vertical_beam_interface_b.model"

internal_item.placement.d : string = "0 +22.5 0 (mm)"
internal_item.model.d     : string = "source_vertical_beam_interface_d.model"

internal_item.placement.f : string = "0 +27.5 0  (mm)"
internal_item.model.f     : string = "source_vertical_beam_interface_f.model"

visibility.hidden  : boolean = false
visibility.color   : string = "grey"


###############################################################################
[name="source_left_beam_interface.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame

shape_type   : string = "box"
x            : real as length =    3 mm
y            : real as length =   60 mm
z            : real as length = 3410 mm

material.ref : string = "lab_air"

internal_item.labels  : string[4] = \
		      "a" "b" "d" "f"

internal_item.placement.a : string = "0 +15 0 (mm)"
internal_item.model.a     : string = "source_vertical_beam_interface_a.model"

internal_item.placement.b : string = "0 -10 0 (mm)"
internal_item.model.b     : string = "source_vertical_beam_interface_b.model"

internal_item.placement.d : string = "0 -22.5 0 (mm)"
internal_item.model.d     : string = "source_vertical_beam_interface_d.model"

internal_item.placement.f : string = "0 -27.5 0  (mm)"
internal_item.model.f     : string = "source_vertical_beam_interface_f.model"

visibility.hidden  : boolean = false
visibility.color   : string = "grey"


################################################################
[name="source_right_beam.model" type="geomtools::stacked_model"]

#@config Source beam (right)

stacked.axis            : string = "x"
stacked.number_of_items : integer = 5

stacked.model_4   : string = "source_right_beam_interface.model"
stacked.label_4   : string = "interface1"

stacked.model_3   : string = "source_vertical_beam_side_plate.model"
stacked.label_3   : string = "side1"

stacked.model_2   : string = "source_right_beam_main.model"
stacked.label_2   : string = "main"

stacked.model_1   : string = "source_vertical_beam_side_plate.model"
stacked.label_1   : string = "side0"

stacked.model_0   : string = "source_right_beam_interface.model"
stacked.label_0   : string = "interface0"

material.ref       : string = "basic::air"

visibility.hidden  : boolean = false


###############################################################
[name="source_left_beam.model" type="geomtools::stacked_model"]

#@config Source beam (left)

stacked.axis            : string = "x"
stacked.number_of_items : integer = 5

stacked.model_4   : string = "source_left_beam_interface.model"
stacked.label_4   : string = "interface1"

stacked.model_3   : string = "source_vertical_beam_side_plate.model"
stacked.label_3   : string = "side1"

stacked.model_2   : string = "source_left_beam_main.model"
stacked.label_2   : string = "main"

stacked.model_1   : string = "source_vertical_beam_side_plate.model"
stacked.label_1   : string = "side0"

stacked.model_0   : string = "source_left_beam_interface.model"
stacked.label_0   : string = "interface0"

material.ref       : string = "basic::air"

visibility.hidden  : boolean = false


# end of source_frame_vertical_beams.geom
