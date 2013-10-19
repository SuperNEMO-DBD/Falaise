# -*- mode: conf-unix; -*-

#@description The geometry models that compose the C module
#@key_label   "name"
#@meta_label  "type"


#######################################################
# C0 volume                                           #
#######################################################
[name="C0_volume.model" type="geomtools::stacked_model"]

#@config The C0 volume description

# The tracking volume of the C0 tracker part:
#
#                         Layers
#           #8  #7  #6  #5  #4  #3  #2  #1  #0
#          +---+---+---+---+---+---+---+---+---+
#  013 ----+-> |   |   |   |   |   |   |   | <-+-- 13
#          +---+---+---+---+---+---+---+---+---+
#  013 ----+-> |   |   |   |   |   |   |   | <-+-- 13
#          +---+---+---+---+---+---+---+---+---+
#          :   :   :   :   :   :   :   :   :   :
#   Calo   : : : : : : : : : : : : : : : : : : : Source
#          :   :   :   :   :   :   :   :   :   : -----> x
#          +---+---+---+---+---+---+---+---+---+
#  013 ----+-> |   |   |   |   |   |   |   | <-+-- 13
#          +---+---+---+---+---+---+---+---+---+
#  013 ----+-> |   |   |   |   |   |   |   | <-+-- 13
#          +---+---+---+---+---+---+---+---+---+
#  013 ----+-> |   |   |   |   |   |   |   | <-+-- 13
#          +---+---+---+---+---+---+---+---+---+
#          | ^ | ^ | ^ |   |   |   |   |   | ^ |
#          +/--+-|-+-|-+---+---+---+---+---+-|-+
#          /     |   |                       |
#        0123   123 123                     123
#

#######################
# Geometry parameters #
#######################

#@description The Z dimension of the mother volume
z : real as length = 3100.0 mm

#@description The label of the stacking axis
stacked.axis : string  = "x"

#@description The number of stacked volumes along the stacking axis
stacked.number_of_items : integer = 9

#@description The geometry model of the stacked layer volume #0
stacked.model_0 : string  = "C_shape_layer_closing.model"

#@description The label of the stacked layer volume #0
stacked.label_0 : string  = "layer_8"

#@description The geometry model of the stacked layer volume #1
stacked.model_1 : string  = "C_shape_layer.model"

#@description The label of the stacked layer volume #1
stacked.label_1 : string  = "layer_7"

#@description The geometry model of the stacked layer volume #2
stacked.model_2 : string  = "C_shape_layer.model"

#@description The label of the stacked layer volume #2
stacked.label_2 : string  = "layer_6"

#@description The geometry model of the stacked layer volume #3
stacked.model_3 : string  = "C_shape_layer.model"

#@description The label of the stacked layer volume #3
stacked.label_3 : string  = "layer_5"

#@description The geometry model of the stacked layer volume #4
stacked.model_4 : string  = "C_shape_layer.model"

#@description The label of the stacked layer volume #4
stacked.label_4 : string  = "layer_4"

#@description The geometry model of the stacked layer volume #5
stacked.model_5 : string  = "C_shape_layer.model"

#@description The label of the stacked layer volume #5
stacked.label_5 : string  = "layer_3"

#@description The geometry model of the stacked layer volume #6
stacked.model_6 : string  = "C_shape_layer.model"

#@description The label of the stacked layer volume #6
stacked.label_6 : string  = "layer_2"

#@description The geometry model of the stacked layer volume #7
stacked.model_7 : string  = "C_shape_layer.model"

#@description The label of the stacked layer volume #7
stacked.label_7 : string  = "layer_1"

#@description The geometry model of the stacked layer volume #8
stacked.model_8 : string  = "C_shape_layer.model"

#@description The label of the stacked layer volume #8
stacked.label_8 : string  = "layer_0"

#######################
# Material parameters #
#######################

#@description Material that fills the C0 tracking volume
material.ref             : string  = "tracking_gas"

#########################
# Visibility parameters #
#########################

#@description Hidden visibility flag
visibility.hidden        : boolean = 0

#@description Hidden envelope visibility flag
visibility.hidden_envelope : boolean = 1

#@description Hidden daughter volumes visibility flag
visibility.daughters.hidden : boolean = 0

#@description Display color
visibility.color         : string  = "grey"

######################
# Mapping parameters #
######################


#@description Daughter drift cell layer 0 mapping rule
mapping.daughter_id.layer_0 : string = "[tracker_layer:layer=0]"

#@description Daughter drift cell layer 1 mapping rule
mapping.daughter_id.layer_1 : string = "[tracker_layer:layer=1]"

#@description Daughter drift cell layer 2 mapping rule
mapping.daughter_id.layer_2 : string = "[tracker_layer:layer=2]"

#@description Daughter drift cell layer 3 mapping rule
mapping.daughter_id.layer_3 : string = "[tracker_layer:layer=3]"

#@description Daughter drift cell layer 4 mapping rule
mapping.daughter_id.layer_4 : string = "[tracker_layer:layer=4]"

#@description Daughter drift cell layer 5 mapping rule
mapping.daughter_id.layer_5 : string = "[tracker_layer:layer=5]"

#@description Daughter drift cell layer 6 mapping rule
mapping.daughter_id.layer_6 : string = "[tracker_layer:layer=6]"

#@description Daughter drift cell layer 7 mapping rule
mapping.daughter_id.layer_7 : string = "[tracker_layer:layer=7]"

#@description Daughter drift cell layer 8 mapping rule
mapping.daughter_id.layer_8 : string = "[tracker_layer:layer=8]"


#####################################################################
# C0 closing plate                                                  #
#####################################################################
[name="C0_closing_plate.model" type="geomtools::simple_shaped_model"]

#@config The C0 closing plate volume description

#######################
# Geometry parameters #
#######################

#@description the shape of the plate
shape_type   : string = "box"

#@description Default length unit
length_unit  : string = "mm"

#@description X dimension of the plate
x            : real   = 1.0 mm

#@description Y dimension of the plate
y            : real   = 3500.0 mm

#@description Z dimension of the plate
z            : real   = 3500.0 mm

#######################
# Material parameters #
#######################

#@description Material the plate is made of
material.ref : string = "std::aluminium"

#########################
# Visibility parameters #
#########################

#@description Hidden visibility flag
visibility.hidden           : boolean = 0

#@description Hidden envelope visibility flag
visibility.hidden_envelope   : boolean = 0

#@description Hidden daughter volumes visibility flag
visibility.daughters.hidden : boolean = 0

#@description Display color
visibility.color            : string  = "orange"


####################################################################
# C0 submodule volume                                              #
####################################################################
[name="C0_submodule.model" type="geomtools::surrounded_boxed_model"]

#@config The C0 submodule volume description

#######################
# Geometry parameters #
#######################

#@description Centering on X axis flag
surrounded.centered_x : boolean = 1

#@description Centering on Y axis flag
surrounded.centered_y : boolean = 1

#@description Centering on Z axis flag
surrounded.centered_z : boolean = 1

#@description The geometry model of the volume to be surrounded
surrounded.model : string = "C0_volume.model"

#@description The label of the volume to be surrounded
surrounded.label : string = "C0_volume"

#######################
# Material parameters #
#######################

#@description Material that fills the C0 tracking volume
material.ref : string  = "tracking_gas"

######################
# Mapping parameters #
######################

#@description Daughter setup volume mapping rule
mapping.daughter_id.C0_volume : string = "[tracker_volume]"

#########################
# Visibility parameters #
#########################

#@description Hidden visibility flag
visibility.hidden           : boolean = 0

#@description Hidden envelope visibility flag
visibility.hidden_envelope   : boolean = 0

#@description Hidden daughter volumes visibility flag
visibility.daughters.hidden : boolean = 0

#@description Display color
visibility.color            : string  = "green"


########################################################
# C0 module                                            #
########################################################
[name="C0_module.model" type="geomtools::stacked_model"]

#@config The C0 module description

#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit : string  = "mm"

#@description X dimension of the C0 module
x   : real   = 1000.0 mm

#@description Y dimension of the C0 module
y   : real   = 5000.0 mm

#@description Z dimension of the C0 module
z   : real   = 4000.0 mm

#@description The label of the stacking axis
stacked.axis            : string  = "x"

#@description The number of the stacked volumes
stacked.number_of_items : integer = 3

#@description The geometry model of the first stacked volume
stacked.model_0   : string = "C0_closing_plate.model"

#@description The geometry model of the second stacked volume
stacked.model_1   : string =     "C0_submodule.model"

#@description The geometry model of the third stacked volume
stacked.model_2   : string = "C0_closing_plate.model"

#@description The label of the first stacked volume
stacked.label_0   : string = "bottom_closing"

#@description The label of the second stacked volume
stacked.label_1   : string = "C0_submodule"

#@description The label of the third stacked volume
stacked.label_2   : string = "top_closing"

#######################
# Material parameters #
#######################

#@description Material the C0 module envelope is filled
material.ref    : string = "lab_air"

#########################
# Visibility parameters #
#########################

#@description Hidden visibility flag
visibility.hidden           : boolean = 0

#@description Hidden envelope visibility flag
visibility.hidden_envelope  : boolean = 0

#@description Hidden daughter volumes visibility flag
visibility.daughters.hidden : boolean = 0

#@description Display color
visibility.color            : string  = "grey"

######################
# Mapping parameters #
######################

#@description Daughter C0 submodule mapping rule
mapping.daughter_id.C0_submodule : string  = "[tracker_submodule:side=0]"

# end of C0_module.geom
