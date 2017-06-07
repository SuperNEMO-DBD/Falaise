# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/tracker_module/tracker_volumes.geom

#
# Pawel Guzowski 05 May 2013: Converted to "missing wires" models
#


###################################################################
#
#                           ^ y
#                           :
#                           :
#      :                 layers                   :
#      :    #0  #1  #2  #3  #4  #5  #6  #7  #8    :
#      +--+---+---+---+---+---+---+---+---+---+---+
#  S   |  | o | o | o | o | o | o | o | o | o |   | C
#  O   |  +---+---+---+---+---+---+---+---+---+   | A
#  U   |  | o | o | o | o | o | o | o | o | o |   | L
#  R   |  +---+---+---+---+---+---+---+---+---+   | O
#  C   :  : : : : : : : : : : : : : : : : : : :   : R
#  E   :  : : : : : : : : : : : : : : : : : : :   : I
#      :  : : : : : : : : : : : : : : : : : : :   : M
#  S   :  : : : : : : : : : : : : : : : : : : :   : E ----> x
#  U   |  +---+---+---+---+---+---+---+---+---+   | T
#  B   |  | o | o | o | o | o | o | o | o | o |   | E
#  M   |  +---+---+---+---+---+---+---+---+---+   | R
#  O   |  | o | o | o | o | o | o | o | o | o |   |
#  D   |  +---+---+---+---+---+---+---+---+---+   | S
#  U   |  | o | o | o | o | o | o | o | o | o |   | U
#  L   |  +---+---+---+---+---+---+---+---+---+   | B
#  E   |  | o | o | o | o | o | o | o | o | o |   | M
#      +--+---+---+---+---+---+---+---+---+---+---+
#      :source gap                        calo gap:
#      :                                          :
#
###################################################################
[name="tracker_front_volume.model" type="geomtools::stacked_model"]

#@config The tracking volume of the front tracker submodule

x : real as length =  405.0 mm
y : real as length = 5008.0 mm
z : real as length = 3030.0 mm

stacked.axis            : string  = "x"
stacked.number_of_items : integer = 11

stacked.model_0    : string  = "tracker_gap_source.model"
stacked.label_0    : string  = "source_gap"

#stacked.model_1    : string  = "tracker_layer_closing.model"
stacked.model_1    : string  = "tracker_layer_front_l0.model"
stacked.label_1    : string  = "layer_0"

stacked.model_2    : string  = "tracker_layer.model"
stacked.label_2    : string  = "layer_1"

stacked.model_3    : string  = "tracker_layer.model"
stacked.label_3    : string  = "layer_2"

stacked.model_4    : string  = "tracker_layer.model"
stacked.label_4    : string  = "layer_3"

#stacked.model_5    : string  = "tracker_layer.model"
stacked.model_5    : string  = "tracker_layer_front_l4.model"
stacked.label_5    : string  = "layer_4"

stacked.model_6    : string  = "tracker_layer.model"
stacked.label_6    : string  = "layer_5"

stacked.model_7    : string  = "tracker_layer.model"
stacked.label_7    : string  = "layer_6"

stacked.model_8    : string  = "tracker_layer.model"
stacked.label_8    : string  = "layer_7"

#stacked.model_9    : string  = "tracker_layer.model"
stacked.model_9    : string  = "tracker_layer_front_l8.model"
stacked.label_9    : string  = "layer_8"

stacked.model_10   : string  = "tracker_gap_calo.model"
stacked.label_10   : string  = "calo_gap"

material.ref : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "grey"

# Daughters mapping informations:
mapping.daughter_id.layer_0 : string = "[tracker_layer:layer=0]"
mapping.daughter_id.layer_1 : string = "[tracker_layer:layer=1]"
mapping.daughter_id.layer_2 : string = "[tracker_layer:layer=2]"
mapping.daughter_id.layer_3 : string = "[tracker_layer:layer=3]"
mapping.daughter_id.layer_4 : string = "[tracker_layer:layer=4]"
mapping.daughter_id.layer_5 : string = "[tracker_layer:layer=5]"
mapping.daughter_id.layer_6 : string = "[tracker_layer:layer=6]"
mapping.daughter_id.layer_7 : string = "[tracker_layer:layer=7]"
mapping.daughter_id.layer_8 : string = "[tracker_layer:layer=8]"


##################################################################
#
#                           ^ y
#                           :
#                           :
#      :                  layers                   :
#      :    #8  #7  #6  #5  #4  #3  #2  #1  #0     :
#      +---+---+---+---+---+---+---+---+---+---+---+
#  C   |   | o | o | o | o | o | o | o | o | o |   | S
#  A   |   +---+---+---+---+---+---+---+---+---+   | O
#  L   |   | o | o | o | o | o | o | o | o | o |   | U
#  O   |   +---+---+---+---+---+---+---+---+---+   | R
#  R   :   : : : : : : : : : : : : : : : : : : :   : C
#  I   :   : : : : : : : : : : : : : : : : : : :   : E
#  M   :   : : : : : : : : : : : : : : : : : : :   :
#  E   :   : : : : : : : : : : : : : : : : : : :   : S ----> x
#  T   |   +---+---+---+---+---+---+---+---+---+   | U
#  E   |   | o | o | o | o | o | o | o | o | o |   | B
#  R   |   +---+---+---+---+---+---+---+---+---+   | M
#      |   | o | o | o | o | o | o | o | o | o |   | O
#  S   |   +---+---+---+---+---+---+---+---+---+   | D
#  U   |   | o | o | o | o | o | o | o | o | o |   | U
#  B   |   +---+---+---+---+---+---+---+---+---+   | L
#  M   |   | o | o | o | o | o | o | o | o | o |   | E
#      +---+---+---+---+---+---+---+---+---+---+---+
#      :calo gap                         source gap:
#      :                                           :
#
##################################################################
[name="tracker_back_volume.model" type="geomtools::stacked_model"]

#@config The tracking volume of the back tracker submodule

x : real as length =  405.0 mm
y : real as length = 5008.0 mm
z : real as length = 3030.0 mm

stacked.axis                : string  = "x"
stacked.number_of_items     : integer = 11

stacked.model_0    : string  = "tracker_gap_calo.model"
stacked.label_0    : string  = "calo_gap"

stacked.model_1    : string  = "tracker_layer_closing.model"
stacked.label_1    : string  = "layer_8"

stacked.model_2    : string  = "tracker_layer.model"
stacked.label_2    : string  = "layer_7"

stacked.model_3    : string  = "tracker_layer.model"
stacked.label_3    : string  = "layer_6"

#stacked.model_4    : string  = "tracker_layer.model"
stacked.model_4    : string  = "tracker_layer_back_l5.model"
stacked.label_4    : string  = "layer_5"

stacked.model_5    : string  = "tracker_layer.model"
stacked.label_5    : string  = "layer_4"

#stacked.model_6    : string  = "tracker_layer.model"
stacked.model_6    : string  = "tracker_layer_back_l3.model"
stacked.label_6    : string  = "layer_3"

stacked.model_7    : string  = "tracker_layer.model"
stacked.label_7    : string  = "layer_2"

stacked.model_8    : string  = "tracker_layer.model"
stacked.label_8    : string  = "layer_1"

stacked.model_9    : string  = "tracker_layer.model"
stacked.label_9    : string  = "layer_0"

stacked.model_10   : string  = "tracker_gap_source.model"
stacked.label_10   : string  = "source_gap"

material.ref                : string  = "tracking_gas"

visibility.hidden_envelope  : boolean = true
visibility.daughters.hidden : boolean = false
visibility.color            : string  = "grey"

# Daughters mapping informations:
mapping.daughter_id.layer_0 : string = "[tracker_layer:layer=0]"
mapping.daughter_id.layer_1 : string = "[tracker_layer:layer=1]"
mapping.daughter_id.layer_2 : string = "[tracker_layer:layer=2]"
mapping.daughter_id.layer_3 : string = "[tracker_layer:layer=3]"
mapping.daughter_id.layer_4 : string = "[tracker_layer:layer=4]"
mapping.daughter_id.layer_5 : string = "[tracker_layer:layer=5]"
mapping.daughter_id.layer_6 : string = "[tracker_layer:layer=6]"
mapping.daughter_id.layer_7 : string = "[tracker_layer:layer=7]"
mapping.daughter_id.layer_8 : string = "[tracker_layer:layer=8]"


##########################################################################
[name="tracker_chamber_ender.model" type="geomtools::simple_shaped_model"]

#@config Top/bottom gap

shape_type : string = "box"
x : real as length =  405. mm
y : real as length = 4998. mm
z : real as length =   34. mm

material.ref : string  = "tracking_gas"

visibility.color  : string  = "cyan"


####################################################################
[name="tracker_front_chamber.model" type="geomtools::stacked_model"]

#@config Front tracking chamber

stacked.axis            : string  = "z"
stacked.number_of_items : integer = 3
stacked.model_0         : string  = "tracker_chamber_ender.model"
stacked.label_0         : string  = "bottom_gas_gap"
stacked.model_1         : string  = "tracker_front_volume.model"
stacked.label_1         : string  = "tracking_volume"
stacked.model_2         : string  = "tracker_chamber_ender.model"
stacked.label_2         : string  = "top_gas_gap"

material.ref : string  = "tracking_gas"

visibility.color  : string  = "cyan"


###################################################################
[name="tracker_back_chamber.model" type="geomtools::stacked_model"]

#@config Back tracking chamber

stacked.axis            : string  = "z"
stacked.number_of_items : integer = 3
stacked.model_0         : string  = "tracker_chamber_ender.model"
stacked.label_0         : string  = "bottom_gas_gap"
stacked.model_1         : string  = "tracker_back_volume.model"
stacked.label_1         : string  = "tracking_volume"
stacked.model_2         : string  = "tracker_chamber_ender.model"
stacked.label_2         : string  = "top_gas_gap"

material.ref : string  = "tracking_gas"

visibility.color  : string  = "cyan"


# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/tracker_module/tracker_volumes.geom
