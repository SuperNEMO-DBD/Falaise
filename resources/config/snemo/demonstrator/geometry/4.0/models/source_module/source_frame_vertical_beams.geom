# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/source_module/source_frame_vertical_beams.geom

#########################################################
#    Source frame vertical (left=y<0/right=y>0) beams   #
#########################################################


#############################################################################
[name="source_vertical_beam_rod.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "cylinder"
z                 : real as length = 39.0 mm
r                 : real as length =  6.0 mm

material.ref      : string = "basic::copper"

visibility.hidden : boolean = false
visibility.color  : string = "red"


###################################################################################
[name="source_vertical_beam_boxed_rod.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length = 39.0 mm
y                 : real as length = 20.0 mm
z                 : real as length = 20.0 mm

material.ref      : string = "lab_air"

visibility.hidden : boolean = false

internal_item.labels        : string[1] = "rod"
internal_item.placement.rod : string    = "0 0 0 (mm) / y 90.0 (deg)"
internal_item.model.rod     : string    = "source_vertical_beam_rod.model"


##################################################################################
[name="source_vertical_beam_multirod.model" type="geomtools::replicated_model"]

#@description The stacking axis
replicated.axis : string = "z"

#@description The number of stacked volumes
replicated.number_of_items : integer = 28

#@description The model of replicated volumes
replicated.model        : string = "source_vertical_beam_boxed_rod.model"

#@description The label of replicated volumes
replicated.label : string = "pads"

#@description The replicated step
replicated.step  : real as length = 121.78 mm

# #@description Thickness of the mother box
# x                : real as length = 39.0 mm

# #@description Thickness of the mother box
# y                : real as length = 20.0 mm

#@description Height of the mother box
z                 : real as length = 3410.0 mm

material.ref      : string = "lab_air"

visibility.hidden : boolean = false


##################################################################################
[name="source_vertical_beam_main_air.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =   39.0 mm
y                 : real as length =   30.5 mm
z                 : real as length = 3410.0 mm

material.ref      : string = "lab_air"

visibility.hidden : boolean = true


####################################################################################
[name="source_vertical_beam_main_plate.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =   39.0 mm
y                 : real as length =    9.5 mm
z                 : real as length = 3410.0 mm

material.ref      : string = "basic::copper"

visibility.hidden : boolean = false
visibility.color  : string = "red"


#############################################################################
[name="source_yneg_vertical_beam_main.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_vertical_beam_multirod.model"
stacked.label_0   : string = "rods"

stacked.model_1   : string = "source_vertical_beam_main_air.model"
stacked.label_1   : string = "air"

stacked.model_2   : string = "source_vertical_beam_main_plate.model"
stacked.label_2   : string = "plate"

material.ref      : string = "lab_air"

visibility.hidden : boolean = false
visibility.color  : string = "grey"


#############################################################################
[name="source_ypos_vertical_beam_main.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 3

stacked.model_0   : string = "source_vertical_beam_main_plate.model"
stacked.label_0   : string = "plate"

stacked.model_1   : string = "source_vertical_beam_main_air.model"
stacked.label_1   : string = "air"

stacked.model_2   : string = "source_vertical_beam_multirod.model"
stacked.label_2   : string = "rods"

material.ref      : string = "lab_air"

visibility.hidden : boolean = false
visibility.color  : string = "grey"


####################################################################################
[name="source_vertical_beam_side_plate.model" type="geomtools::simple_shaped_model"]

shape_type        : string = "box"
x                 : real as length =    6.5 mm
y                 : real as length =   60.0 mm
z                 : real as length = 3410.0 mm

material.ref      : string = "basic::copper"

visibility.hidden : boolean = false
visibility.color  : string = "red"


#############################################################################################
[name="source_vertical_beam_tracker_interface_a.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame

shape_type   : string = "box"
x            : real as length =    3.0 mm
y            : real as length =   30.0 mm
z            : real as length = 3410.0 mm

material.ref : string = "basic::copper"

visibility.hidden : boolean = false
visibility.color  : string = "red"


#############################################################################################
[name="source_vertical_beam_tracker_interface_b.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame

shape_type   : string = "box"
x            : real as length =    3.0 mm
y            : real as length =   20.0 mm
z            : real as length = 3410.0 mm

material.ref : string = "RTV"

visibility.hidden : boolean = false
visibility.color  : string = "magenta"


#############################################################################################
[name="source_vertical_beam_tracker_interface_c.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame
shape_type   : string = "box"
x            : real as length =    3.0 mm
y            : real as length =    5.0 mm
z            : real as length = 3410.0 mm

material.ref : string = "basic::copper"

visibility.hidden  : boolean = false
visibility.color   : string = "red"


#############################################################################################
[name="source_vertical_beam_tracker_interface_d.model" type="geomtools::simple_shaped_model"]

#@config Interface with the tracker frame
shape_type   : string = "box"
x            : real as length =    3.0 mm
y            : real as length =    5.0 mm
z            : real as length = 3410.0 mm

material.ref : string = "SBR"

visibility.hidden  : boolean = false
visibility.color   : string = "orange"


##########################################################################################
[name="source_yneg_vertical_beam_tracker_interface.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 4

stacked.model_0   : string = "source_vertical_beam_tracker_interface_d.model"
stacked.label_0   : string = "interface_3"

stacked.model_1   : string = "source_vertical_beam_tracker_interface_c.model"
stacked.label_1   : string = "interface_2"

stacked.model_2   : string = "source_vertical_beam_tracker_interface_b.model"
stacked.label_2   : string = "interface_1"

stacked.model_3   : string = "source_vertical_beam_tracker_interface_a.model"
stacked.label_3   : string = "interface_0"

material.ref      : string = "lab_air"


##########################################################################################
[name="source_ypos_vertical_beam_tracker_interface.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 4

stacked.model_0   : string = "source_vertical_beam_tracker_interface_a.model"
stacked.label_0   : string = "interface_0"

stacked.model_1   : string = "source_vertical_beam_tracker_interface_b.model"
stacked.label_1   : string = "interface_1"

stacked.model_2   : string = "source_vertical_beam_tracker_interface_c.model"
stacked.label_2   : string = "interface_2"

stacked.model_3   : string = "source_vertical_beam_tracker_interface_d.model"
stacked.label_3   : string = "interface_3"

material.ref      : string = "lab_air"


########################################################################
[name="source_yneg_vertical_beam.model" type="geomtools::stacked_model"]

#@config Source vertical beam (y<0)

stacked.axis            : string = "x"
stacked.number_of_items : integer = 5

stacked.model_0   : string = "source_yneg_vertical_beam_tracker_interface.model"
stacked.label_0   : string = "interface0"

stacked.model_1   : string = "source_vertical_beam_side_plate.model"
stacked.label_1   : string = "side0"

stacked.model_2   : string = "source_yneg_vertical_beam_main.model"
stacked.label_2   : string = "main"

stacked.model_3   : string = "source_vertical_beam_side_plate.model"
stacked.label_3   : string = "side1"

stacked.model_4   : string = "source_yneg_vertical_beam_tracker_interface.model"
stacked.label_4   : string = "interface1"

material.ref      : string = "lab_air"

visibility.hidden : boolean = false


########################################################################
[name="source_ypos_vertical_beam.model" type="geomtools::stacked_model"]

#@config Source vertical beam (y>0)

stacked.axis            : string = "x"
stacked.number_of_items : integer = 5

stacked.model_0   : string = "source_ypos_vertical_beam_tracker_interface.model"
stacked.label_0   : string = "interface0"

stacked.model_1   : string = "source_vertical_beam_side_plate.model"
stacked.label_1   : string = "side0"

stacked.model_2   : string = "source_ypos_vertical_beam_main.model"
stacked.label_2   : string = "main"

stacked.model_3   : string = "source_vertical_beam_side_plate.model"
stacked.label_3   : string = "side1"

stacked.model_4   : string = "source_ypos_vertical_beam_tracker_interface.model"
stacked.label_4   : string = "interface1"

material.ref      : string = "lab_air"

visibility.hidden : boolean = false


# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/source_module/source_frame_vertical_beams.geom
