# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/source_module/source_module.geom
# Author  : F. Mauger
# Date    : 2012-04-03
# Updated : 2016-04-19
#
# Description :
#
#   Description of the SuperNEMO demonstrator source frame module
#
#   - depends on :
#     source_frame_horizontal_beams.geom
#     source_frame_vertical_beams.geom
#     source_strips.geom
#

################################################################################
[name="source_strip_block_edge_gap.model" type="geomtools::simple_shaped_model"]

shape_type         : string  = "box"
x                  : real as length =   58.0 mm
y                  : real as length =    3.0 mm
z                  : real as length = 3350.0 mm

material.ref       : string  = "tracking_gas"

visibility.color   : string  = "cyan"


##########################################################################
[name="source_strip_block_assembly.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 15

stacked.model_0   : string = "source_strip_block_edge_gap.model"
stacked.label_0   : string = "source_gap_left"

stacked.model_1   : string = "source_strip_block_0.model"
stacked.label_1   : string = "source_block_0"

stacked.model_2   : string = "source_calibration_track.model"
stacked.label_2   : string = "calibration_track_0"

stacked.model_3   : string = "source_strip_block_1.model"
stacked.label_3   : string = "source_block_1"

stacked.model_4   : string = "source_calibration_track.model"
stacked.label_4   : string = "calibration_track_1"

stacked.model_5   : string = "source_strip_block_2.model"
stacked.label_5   : string = "source_block_2"

stacked.model_6   : string = "source_calibration_track.model"
stacked.label_6   : string = "calibration_track_2"

stacked.model_7   : string = "source_strip_block_3.model"
stacked.label_7   : string = "source_block_3"

stacked.model_8   : string = "source_calibration_track.model"
stacked.label_8   : string = "calibration_track_3"

stacked.model_9   : string = "source_strip_block_4.model"
stacked.label_9   : string = "source_block_4"

stacked.model_10  : string = "source_calibration_track.model"
stacked.label_10  : string = "calibration_track_4"

stacked.model_11  : string = "source_strip_block_5.model"
stacked.label_11  : string = "source_block_5"

stacked.model_12  : string = "source_calibration_track.model"
stacked.label_12  : string = "calibration_track_5"

stacked.model_13  : string = "source_strip_block_6.model"
stacked.label_13  : string = "source_block_6"

stacked.model_14  : string = "source_strip_block_edge_gap.model"
stacked.label_14  : string = "source_gap_right"

material.ref : string = "tracking_gas"

visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

# Daughters mapping informations:
mapping.daughter_id.calibration_track_0 : string = "[source_calibration_track:track=0]"
mapping.daughter_id.calibration_track_1 : string = "[source_calibration_track:track=1]"
mapping.daughter_id.calibration_track_2 : string = "[source_calibration_track:track=2]"
mapping.daughter_id.calibration_track_3 : string = "[source_calibration_track:track=3]"
mapping.daughter_id.calibration_track_4 : string = "[source_calibration_track:track=4]"
mapping.daughter_id.calibration_track_5 : string = "[source_calibration_track:track=5]"


#####################
#  The core volume  #
#####################

#################################################################
[name="source_core_volume.model" type="geomtools::stacked_model"]

#@config Source core volume

x : real as length =   58.0 mm
y : real as length = 5000.0 mm
z : real as length = 3410.0 mm

stacked.axis            : string  = "z"
stacked.number_of_items : integer =  5

stacked.model_0 : string = "source_foil_bottom_gas_gap.model"
stacked.label_0 : string = "bottom_gas_gap"

stacked.model_1 : string = "source_foil_bottom_support.model"
stacked.label_1 : string = "bottom_foil_support"

stacked.model_2 : string = "source_strip_block_assembly.model"
stacked.label_2 : string = "sources"

stacked.model_3 : string = "source_foil_top_support.model"
stacked.label_3 : string = "top_foil_support"

stacked.model_4 : string = "source_foil_top_gas_gap.model"
stacked.label_4 : string = "top_gas_gap"

material.ref : string = "tracking_gas"

visibility.hidden_envelope : boolean = true
visibility.color           : string  = "cyan"


#############################################################################
[name="source_side_beam_gas_gap.model" type="geomtools::simple_shaped_model"]

#@config Side beam gas gap

shape_type : string = "box"
x : real as length =   58.0 mm
y : real as length =  155.0 mm
z : real as length = 3410.0 mm

material.ref : string  = "tracking_gas"

visibility.color  : string  = "cyan"


#################################################################################
[name="source_yneg_vertical_beam_with_gap.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2

stacked.model_0 : string = "source_yneg_vertical_beam.model"
stacked.label_0 : string = "beam"

stacked.model_1 : string = "source_side_beam_gas_gap.model"
stacked.label_1 : string = "gas_gap"

material.ref : string = "tracking_gas"

visibility.hidden_envelope : boolean = true
visibility.color          : string  = "grey"


#################################################################################
[name="source_ypos_vertical_beam_with_gap.model" type="geomtools::stacked_model"]

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2

stacked.model_0 : string = "source_side_beam_gas_gap.model"
stacked.label_0 : string = "gas_gap"

stacked.model_1 : string = "source_ypos_vertical_beam.model"
stacked.label_1 : string = "beam"

material.ref : string = "tracking_gas"

visibility.hidden_envelope : boolean = true
visibility.color          : string  = "grey"


##########################
#  The source submodule  #
##########################

########################################################################
#
#                            ^ z
#                            :
#          +------------------------------------+
#          | top beam        :                  |
#          +---+----------------------------+---+
#    left  |   | core        :              |   | right
#    beam  |   | volume      :              |   | beam
#          |   |             :              |   |
#          |   |             :              |   |
#  --------|---|-------------x--------------|---|--------> y
#          |   |             :              |   |
#          |   |             :              |   |
#          |   |             :              |   |
#          |   |             :              |   |
#          +---+----------------------------+---+
#          | bottom beam     :                  |
#          +------------------------------------+
#                            :
#
########################################################################
[name="source_submodule.model" type="geomtools::surrounded_boxed_model"]

#@config Source submodule

surrounded.centered_x   : boolean = false
surrounded.centered_y   : boolean = false
surrounded.centered_z   : boolean = false

surrounded.model        : string = "source_core_volume.model"
surrounded.label        : string = "source_core_volume"

surrounded.bottom_model : string = "source_zneg_horizontal_beam.model"
surrounded.top_model    : string = "source_zpos_horizontal_beam.model"
surrounded.left_model   : string = "source_yneg_vertical_beam_with_gap.model"
surrounded.right_model  : string = "source_ypos_vertical_beam_with_gap.model"

surrounded.bottom_label : string = "bottom_beam"
surrounded.top_label    : string = "top_beam"
surrounded.left_label   : string = "left_beam"
surrounded.right_label  : string = "right_beam"

material.ref : string = "lab_air"

visibility.hidden_envelope : boolean = true
visibility.color          : string  = "grey"


# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/source_module/source_module.gom
