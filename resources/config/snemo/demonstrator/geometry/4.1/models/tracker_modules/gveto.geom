# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.1/models/tracker_module/gveto.geom


#######################################################################
[name="gveto_line_left.model" type="geomtools::replicated_boxed_model"]

replicated.axis            : string  = "y"
replicated.number_of_items : integer = 8
replicated.model           : string  = "gveto_module.model"
replicated.label           : string  = "module"

material.ref : string  = "tracking_gas"

visibility.hidden_envelope  : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# --> Daughters mapping informations:
mapping.daughter_id.module : string = "[gveto_optical_module:column+0]"


########################################################################
[name="gveto_line_right.model" type="geomtools::replicated_boxed_model"]

replicated.axis            : string  = "y"
replicated.number_of_items : integer = 8
replicated.model           : string  = "gveto_module.model"
replicated.label           : string  = "module"

material.ref : string  = "tracking_gas"

visibility.hidden_envelope  : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# --> Daughters mapping informations:
mapping.daughter_id.module : string = "[gveto_optical_module:column+8]"


###########################################################################
[name="gveto_line_stock_plate.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length = 292.0 mm
y          : real as length =  10.5 mm
z          : real as length =   6.5 mm

material.ref : string  = "basic::iron"

visibility.color  : string  = "red"


###########################################################################
[name="gveto_line_stock_outer.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length = 292.0 mm
y          : real as length =  10.5 mm
z          : real as length = 318.5 mm

material.ref : string  = "lab_air"

visibility.color  : string  = "grey"


###########################################################################
[name="gveto_line_stock_inner.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length = 292.0 mm
y          : real as length =  10.5 mm
z          : real as length = 154.0 mm

material.ref : string  = "tracking_gas"

visibility.color  : string  = "cyan"


###############################################################
[name="gveto_line_stock.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 3
stacked.model_0   : string  = "gveto_line_stock_outer.model"
stacked.label_0   : string  = "outer"
stacked.model_1   : string  = "gveto_line_stock_plate.model"
stacked.label_1   : string  = "plate"
stacked.model_2   : string  = "gveto_line_stock_inner.model"
stacked.label_2   : string  = "inner"

material.ref : string  = "lab_air"


#################################################################################
[name="gveto_line_ender_stock_plate.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length =  292.0 mm
y          : real as length =   6.75 mm
z          : real as length =    6.5 mm

material.ref : string  = "basic::iron"

visibility.color  : string  = "red"


#################################################################################
[name="gveto_line_ender_stock_outer.model" type="geomtools::simple_shaped_model"]

shape_type  : string = "box"
x           : real as length = 292.0 mm
y           : real as length =   6.75 mm
z           : real as length = 318.5 mm

material.ref      : string  = "lab_air"

visibility.color  : string  = "grey"


#################################################################################
[name="gveto_line_ender_stock_inner.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length = 292.0 mm
y          : real as length =   6.75 mm
z          : real as length = 154.0 mm

material.ref : string = "tracking_gas"

visibility.color  : string  = "cyan"


#####################################################################
[name="gveto_line_ender_stock.model" type="geomtools::stacked_model"]

stacked.axis            : string = "z"
stacked.number_of_items : integer = 3
stacked.model_0   : string  = "gveto_line_ender_stock_outer.model"
stacked.label_0   : string  = "outer"
stacked.model_1   : string  = "gveto_line_ender_stock_plate.model"
stacked.label_1   : string  = "plate"
stacked.model_2   : string  = "gveto_line_ender_stock_inner.model"
stacked.label_2   : string  = "inner"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"


#########################################################
[name="gveto_line.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 5
stacked.model_0   : string  = "gveto_line_ender_stock.model"
stacked.label_0   : string  = "left_ender_stock"
stacked.model_1   : string  = "gveto_line_left.model"
stacked.label_1   : string  = "left_line"
stacked.model_2   : string  = "gveto_line_stock.model"
stacked.label_2   : string  = "middle_stock"
stacked.model_3   : string  = "gveto_line_right.model"
stacked.label_3   : string  = "right_line"
stacked.model_4   : string  = "gveto_line_ender_stock.model"
stacked.label_4   : string  = "right_ender_stock"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"


###################################################################
[name="gveto_line_top.model" type="geomtools::rotated_boxed_model"]

rotated.axis          : string  = "y"
rotated.special_angle : string  = "180"
rotated.model         : string  = "gveto_line.model"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = true
visibility.color          : string  = "grey"


######################################################################
[name="gveto_line_bottom.model" type="geomtools::rotated_boxed_model"]

rotated.axis          : string  = "y"
rotated.special_angle : string  = "0"
rotated.model         : string  = "gveto_line.model"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

### TO BE MODIFY : PRELIMINARY FEEDTHROUGHS MODEL :

####################################################################################
[name="tracker_feedthroughs_pins.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length = 70.0 mm
y          : real as length = 40.0 mm
z          : real as length =  2.0 mm

material.ref : string  = "snemo::beryllium-copper"

visibility.color  : string  = "blue"


#####################################################################################
[name="tracker_feedthroughs_pins_top.model" type="geomtools::replicated_model"]

replicated.axis            : string  = "y"
replicated.number_of_items : integer = 56
replicated.model           : string  = "tracker_feedthroughs_pins.model"
replicated.label           : string  = "pins_top"

#@description The replicated step
replicated.step         : real as length = 85.0 mm

#@description Height of the mother box
y                       : real as length = 5008.0 mm

material.ref : string  = "tracking_gas"

visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Mapping informations for daughter volumes:
mapping.daughter_id.pins_top  : string  = "[gveto_tracker_feedthrough_pins:element+0]"


#####################################################################################
[name="tracker_feedthroughs_pins_bottom.model" type="geomtools::replicated_model"]

replicated.axis            : string  = "y"
replicated.number_of_items : integer = 29
replicated.model           : string  = "tracker_feedthroughs_pins.model"
replicated.label           : string  = "pins_bottom"

#@description The replicated step
replicated.step         : real as length = 170.0 mm

#@description Height of the mother box
y                       : real as length = 5008.0 mm

material.ref : string  = "tracking_gas"

visibility.hidden_envelope  : boolean = false
visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Mapping informations for daughter volumes:
mapping.daughter_id.pins_bottom  : string  = "[gveto_tracker_feedthrough_pins:element+0]"

#######################################################################################
[name="tracker_horizontal_beam_side_plate.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length =    6.5 mm
y          : real as length = 5008.0 mm
z          : real as length =  318.5 mm

material.ref : string = "basic::iron"

visibility.color  : string  = "red"


########################################################################################
[name="tracker_horizontal_beam_outer_plate.model" type="geomtools::simple_shaped_model"]
shape_type : string = "box"
x          : real as length =   84.5 mm
y          : real as length = 5008.0 mm
z          : real as length =    6.5 mm

material.ref : string = "basic::iron"

visibility.color  : string  = "red"


########################################################################################
[name="tracker_horizontal_beam_inner_plate.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length   =   28.5 mm
y          : real as length   = 5008.0 mm
z          : real as length   =    6.5 mm

material.ref : string = "basic::iron"

visibility.color  : string  = "red"


############################################################################################
[name="tracker_horizontal_beam_outer_gas_stock.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length =   84.5 mm
y          : real as length = 5008.0 mm
z          : real as length =  154.0 mm

material.ref : string = "tracking_gas"

visibility.color  : string  = "cyan"


############################################################################################
[name="tracker_horizontal_beam_inner_gas_stock.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length =   28.5 mm
y          : real as length = 5008.0 mm
z          : real as length =  154.0 mm

material.ref : string  = "tracking_gas"

visibility.color  : string  = "cyan"


##################################################################################
[name="tracker_horizontal_beam_inner.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length =   28.5 mm
y          : real as length = 5008.0 mm
z          : real as length =  479.0 mm

material.ref : string  = "lab_air"

visibility.color  : string  = "grey"

internal_item.labels  : string[3] = "beam_side" "beam_outer" "gas"

internal_item.placement.beam_side  : string  = "-11 0 -80.25 (mm)"
internal_item.model.beam_side      : string  = "tracker_horizontal_beam_side_plate.model"

internal_item.placement.beam_outer : string  = "0 0 82.25 (mm)"
internal_item.model.beam_outer     : string  = "tracker_horizontal_beam_inner_plate.model"

internal_item.placement.gas        : string  = "0 0 162.5 (mm)"
internal_item.model.gas            : string  = "tracker_horizontal_beam_inner_gas_stock.model"

##################################################################################
[name="tracker_horizontal_beam_outer_bottom.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length =   84.5 mm
y          : real as length = 5008.0 mm
z          : real as length =  479.0 mm

material.ref : string  = "lab_air"

visibility.color  : string  = "grey"

internal_item.labels  : string[4] = "beam_side" "feedthroughs_pins" "beam_inner" "gas"

internal_item.placement.beam_side         : string  = "39.0 0 -80.25 (mm)"
internal_item.model.beam_side             : string  = "tracker_horizontal_beam_side_plate.model"

internal_item.placement.feedthroughs_pins : string  = "0 0 78 (mm)"
internal_item.model.feedthroughs_pins     : string  = "tracker_feedthroughs_pins_bottom.model"

internal_item.placement.beam_inner        : string  = "0 0 82.25 (mm)"
internal_item.model.beam_inner            : string  = "tracker_horizontal_beam_outer_plate.model"

internal_item.placement.gas               : string  = "0 0 162.5 (mm)"
internal_item.model.gas                   : string  = "tracker_horizontal_beam_outer_gas_stock.model"

######################################################################################
[name="tracker_horizontal_beam_outer_top.model" type="geomtools::simple_shaped_model"]

shape_type : string = "box"
x          : real as length =   84.5 mm
y          : real as length = 5008.0 mm
z          : real as length =  479.0 mm

material.ref : string  = "lab_air"

visibility.color  : string  = "grey"

internal_item.labels  : string[4] = "beam_side" "feedthroughs_pins" "beam_inner" "gas"

internal_item.placement.beam_side         : string  = "39.0 0 -80.25 (mm)"
internal_item.model.beam_side             : string  = "tracker_horizontal_beam_side_plate.model"

internal_item.placement.feedthroughs_pins : string  = "0 0 78 (mm)"
internal_item.model.feedthroughs_pins     : string  = "tracker_feedthroughs_pins_top.model"

internal_item.placement.beam_inner        : string  = "0 0 82.25 (mm)"
internal_item.model.beam_inner            : string  = "tracker_horizontal_beam_outer_plate.model"

internal_item.placement.gas               : string  = "0 0 162.5 (mm)"
internal_item.model.gas                   : string  = "tracker_horizontal_beam_outer_gas_stock.model"


###############################################################################################
[name="tracker_horizontal_beam_outer_front_bottom.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string  = "x"
rotated.special_angle     : string  = "0"
rotated.model             : string  = "tracker_horizontal_beam_outer_bottom.model"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = false


############################################################################################
[name="tracker_horizontal_beam_outer_front_top.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string  = "x"
rotated.special_angle     : string  = "180"
rotated.model             : string  = "tracker_horizontal_beam_outer_top.model"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = false


##############################################################################################
[name="tracker_horizontal_beam_outer_back_bottom.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string  = "z"
rotated.special_angle     : string  = "180"
rotated.model             : string  = "tracker_horizontal_beam_outer_bottom.model"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = false


###########################################################################################
[name="tracker_horizontal_beam_outer_back_top.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string  = "y"
rotated.special_angle     : string  = "180"
rotated.model             : string  = "tracker_horizontal_beam_outer_top.model"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = false


###############################################################################################
[name="tracker_horizontal_beam_inner_front_bottom.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string  = "x"
rotated.special_angle     : string  = "0"
rotated.model             : string  = "tracker_horizontal_beam_inner.model"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = false


############################################################################################
[name="tracker_horizontal_beam_inner_front_top.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string  = "x"
rotated.special_angle     : string  = "180"
rotated.model             : string  = "tracker_horizontal_beam_inner.model"

visibility.hidden_envelope : boolean = false

material.ref : string = "lab_air"


##############################################################################################
[name="tracker_horizontal_beam_inner_back_bottom.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string  = "z"
rotated.special_angle     : string  = "180"
rotated.model             : string  = "tracker_horizontal_beam_inner.model"

visibility.hidden_envelope : boolean = false

material.ref : string = "lab_air"


###########################################################################################
[name="tracker_horizontal_beam_inner_back_top.model" type="geomtools::rotated_boxed_model"]

rotated.axis              : string  = "y"
rotated.special_angle     : string  = "180"
rotated.model             : string  = "tracker_horizontal_beam_inner.model"

visibility.hidden_envelope : boolean = false

material.ref : string = "lab_air"


###################################################################################
[name="tracker_horizontal_beam_front_bottom.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0   : string  = "tracker_horizontal_beam_inner_front_bottom.model"
stacked.label_0   : string  = "inner"
stacked.model_1   : string  = "gveto_line_bottom.model"
stacked.label_1   : string  = "gveto"
stacked.model_2   : string  = "tracker_horizontal_beam_outer_front_bottom.model"
stacked.label_2   : string  = "outer"

material.ref : string = "lab_air"


################################################################################
[name="tracker_horizontal_beam_front_top.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0   : string  = "tracker_horizontal_beam_inner_front_top.model"
stacked.label_0   : string  = "inner"
stacked.model_1   : string  = "gveto_line_top.model"
stacked.label_1   : string  = "gveto"
stacked.model_2   : string  = "tracker_horizontal_beam_outer_front_top.model"
stacked.label_2   : string  = "outer"

material.ref : string = "lab_air"


##################################################################################
[name="tracker_horizontal_beam_back_bottom.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0   : string  = "tracker_horizontal_beam_outer_back_bottom.model"
stacked.label_0   : string  = "outer"
stacked.model_1   : string  = "gveto_line_bottom.model"
stacked.label_1   : string  = "gveto"
stacked.model_2   : string  = "tracker_horizontal_beam_inner_back_bottom.model"
stacked.label_2   : string  = "inner"

material.ref : string = "lab_air"

###############################################################################
[name="tracker_horizontal_beam_back_top.model" type="geomtools::stacked_model"]

stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0   : string  = "tracker_horizontal_beam_outer_back_top.model"
stacked.label_0   : string  = "outer"
stacked.model_1   : string  = "gveto_line_top.model"
stacked.label_1   : string  = "gveto"
stacked.model_2   : string  = "tracker_horizontal_beam_inner_back_top.model"
stacked.label_2   : string  = "inner"

material.ref : string = "lab_air"


# end of @falaise:config/snemo/demonstrator/geometry/4.1/models/tracker_module/gveto.geom
