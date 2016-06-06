# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/source_module/source_layer.geom


#################################################
#  The assembly of all blocks of source strips  #
#################################################

##############################################################################
[name="source_layer_edge_spacing.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =    3.0 mm
z                  : real    = 3350.0 mm

material.ref       : string  = "tracking_gas"

visibility.color   : string  = "grey"


###########################################################################
#
#                                   ^ z
#                                   :
#             calib   calib   calib : calib   calib   calib
#             track   track   track : track   track   track
#    left       0       1       2   :   3       4       5       right
#     gap                           :                           gap
#     +--+-----+-+-----+-+-----+-+-----+-+-----+-+-----+-+-----+--+
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
# ----|--|-----|-|-----|-|-----|-|--+--|-|-----|-|-----|-|-----|--|-----> y
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     |  |     | |     | |     | |  :  | |     | |     | |     |  |
#     +--+-----+-+-----+-+-----+-+-----+-+-----+-+-----+-+-----+--+
#         block   block   block   block   block   block   block
#           0       1       2       3       4       5       6
#                                   :
#                                   :
#
###########################################################
[name="source_layer.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 15

stacked.model_0   : string = "source_layer_edge_spacing.model"
stacked.label_0   : string = "source_gap_left"

stacked.model_1   : string = "source_strip_block_0.model"
stacked.label_1   : string = "source_block_0"

stacked.model_2   : string = "source_calibration_track.model"
stacked.label_2   : string = "calibration_0"

stacked.model_3   : string = "source_strip_block_1.model"
stacked.label_3   : string = "source_block_1"

stacked.model_4   : string = "source_calibration_track.model"
stacked.label_4   : string = "calibration_1"

stacked.model_5   : string = "source_strip_block_2.model"
stacked.label_5   : string = "source_block_2"

stacked.model_6   : string = "source_calibration_track.model"
stacked.label_6   : string = "calibration_2"

stacked.model_7   : string = "source_strip_block_3.model"
stacked.label_7   : string = "source_block_3"

stacked.model_8   : string = "source_calibration_track.model"
stacked.label_8   : string = "calibration_3"

stacked.model_9   : string = "source_strip_block_4.model"
stacked.label_9   : string = "source_block_4"

stacked.model_10  : string = "source_calibration_track.model"
stacked.label_10  : string = "calibration_4"

stacked.model_11  : string = "source_strip_block_5.model"
stacked.label_11  : string = "source_block_5"

stacked.model_12  : string = "source_calibration_track.model"
stacked.label_12  : string = "calibration_5"

stacked.model_13  : string = "source_strip_block_6.model"
stacked.label_13  : string = "source_block_6"

stacked.model_14  : string = "source_layer_edge_spacing.model"
stacked.label_14  : string = "source_gap_right"

material.ref : string = "tracking_gas"

visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

mapping.daughter_id.calibration_0 : string = "[source_calibration_track:track=0]"
mapping.daughter_id.calibration_1 : string = "[source_calibration_track:track=1]"
mapping.daughter_id.calibration_2 : string = "[source_calibration_track:track=2]"
mapping.daughter_id.calibration_3 : string = "[source_calibration_track:track=3]"
mapping.daughter_id.calibration_4 : string = "[source_calibration_track:track=4]"
mapping.daughter_id.calibration_5 : string = "[source_calibration_track:track=5]"


# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/source_module/source_layer.geom
