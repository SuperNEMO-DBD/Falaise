# -*- mode: conf-unix; -*-
# source_module.geom
# Author  : F. Mauger
# Date    : 2012-04-03
# Updated : 2016-03-10
#
# Description :
#
#   Description of the SuperNEMO demonstrator source frame module
#
#   - depends on :
#     source_frame_horizontal_beams.geom
#     source_frame_vertical_beams.geom
#     source_strips.geom


#############################
#  Blocks of source strips  #
#############################

###################################################################
[name="source_strip_block_0.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 5

stacked.model_0   : string = "source_external_full_strip.model"
stacked.label_0   : string = "strip_0"

stacked.model_1   : string = "source_strip_spacing.model"
stacked.label_1   : string = "spacing_0_1"

stacked.model_2   : string = "source_internal_full_strip.model"
stacked.label_2   : string = "strip_1"

stacked.model_3   : string = "source_strip_spacing.model"
stacked.label_3   : string = "spacing_1_2"

stacked.model_4   : string = "source_internal_full_strip.model"
stacked.label_4   : string = "strip_2"

visibility.hidden          : boolean = false
visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

mapping.daughter_id.strip_0 : string = "[source_strip_path: strip=0]"
mapping.daughter_id.strip_1 : string = "[source_strip_path: strip=1]"
mapping.daughter_id.strip_2 : string = "[source_strip_path: strip=2]"


###################################################################
[name="source_strip_block_1.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 11

stacked.model_0   : string = "source_internal_full_strip.model"
stacked.label_0   : string = "strip_3"

stacked.model_1   : string = "source_strip_spacing.model"
stacked.label_1   : string = "spacing_3_4"

stacked.model_2   : string = "source_internal_full_strip.model"
stacked.label_2   : string = "strip_4"

stacked.model_3   : string = "source_strip_spacing.model"
stacked.label_3   : string = "spacing_4_5"

stacked.model_4   : string = "source_internal_full_strip.model"
stacked.label_4   : string = "strip_5"

stacked.model_5   : string = "source_strip_spacing.model"
stacked.label_5   : string = "spacing_5_6"

stacked.model_6   : string = "source_internal_full_strip.model"
stacked.label_6   : string = "strip_6"

stacked.model_7   : string = "source_strip_spacing.model"
stacked.label_7   : string = "spacing_6_7"

stacked.model_8   : string = "source_internal_full_strip.model"
stacked.label_8   : string = "strip_7"

stacked.model_9   : string = "source_strip_spacing.model"
stacked.label_9   : string = "spacing_7_8"

stacked.model_10  : string = "source_internal_full_strip.model"
stacked.label_10  : string = "strip_8"

visibility.hidden          : boolean = false
visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

mapping.daughter_id.strip_3 : string = "[source_strip_path: strip=3]"
mapping.daughter_id.strip_4 : string = "[source_strip_path: strip=4]"
mapping.daughter_id.strip_5 : string = "[source_strip_path: strip=5]"
mapping.daughter_id.strip_6 : string = "[source_strip_path: strip=6]"
mapping.daughter_id.strip_7 : string = "[source_strip_path: strip=7]"
mapping.daughter_id.strip_8 : string = "[source_strip_path: strip=8]"


###################################################################
[name="source_strip_block_2.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 11

stacked.model_0   : string = "source_internal_full_strip.model"
stacked.label_0   : string = "strip_9"

stacked.model_1   : string = "source_strip_spacing.model"
stacked.label_1   : string = "spacing_9_10"

stacked.model_2   : string = "source_internal_full_strip.model"
stacked.label_2   : string = "strip_10"

stacked.model_3   : string = "source_strip_spacing.model"
stacked.label_3   : string = "spacing_10_11"

stacked.model_4   : string = "source_internal_full_strip.model"
stacked.label_4   : string = "strip_11"

stacked.model_5   : string = "source_strip_spacing.model"
stacked.label_5   : string = "spacing_11_12"

stacked.model_6   : string = "source_internal_full_strip.model"
stacked.label_6   : string = "strip_12"

stacked.model_7   : string = "source_strip_spacing.model"
stacked.label_7   : string = "spacing_12_13"

stacked.model_8   : string = "source_internal_full_strip.model"
stacked.label_8   : string = "strip_13"

stacked.model_9   : string = "source_strip_spacing.model"
stacked.label_9   : string = "spacing_13_14"

stacked.model_10  : string = "source_internal_full_strip.model"
stacked.label_10  : string = "strip_14"

visibility.hidden          : boolean = false
visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

mapping.daughter_id.strip_9  : string = "[source_strip_path: strip=9]"
mapping.daughter_id.strip_10 : string = "[source_strip_path: strip=10]"
mapping.daughter_id.strip_11 : string = "[source_strip_path: strip=11]"
mapping.daughter_id.strip_12 : string = "[source_strip_path: strip=12]"
mapping.daughter_id.strip_13 : string = "[source_strip_path: strip=13]"
mapping.daughter_id.strip_14 : string = "[source_strip_path: strip=14]"


###################################################################
[name="source_strip_block_3.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 11

stacked.model_0   : string = "source_internal_full_strip.model"
stacked.label_0   : string = "strip_15"

stacked.model_1   : string = "source_strip_spacing.model"
stacked.label_1   : string = "spacing_15_16"

stacked.model_2   : string = "source_internal_full_strip.model"
stacked.label_2   : string = "strip_16"

stacked.model_3   : string = "source_strip_spacing.model"
stacked.label_3   : string = "spacing_16_17"

stacked.model_4   : string = "source_internal_full_strip.model"
stacked.label_4   : string = "strip_17"

stacked.model_5   : string = "source_strip_spacing.model"
stacked.label_5   : string = "spacing_17_18"

stacked.model_6   : string = "source_internal_full_strip.model"
stacked.label_6   : string = "strip_18"

stacked.model_7   : string = "source_strip_spacing.model"
stacked.label_7   : string = "spacing_18_19"

stacked.model_8   : string = "source_internal_full_strip.model"
stacked.label_8   : string = "strip_19"

stacked.model_9   : string = "source_strip_spacing.model"
stacked.label_9   : string = "spacing_19_20"

stacked.model_10  : string = "source_internal_full_strip.model"
stacked.label_10  : string = "strip_20"

visibility.hidden          : boolean = false
visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

mapping.daughter_id.strip_15 : string = "[source_strip_path: strip=15]"
mapping.daughter_id.strip_16 : string = "[source_strip_path: strip=16]"
mapping.daughter_id.strip_17 : string = "[source_strip_path: strip=17]"
mapping.daughter_id.strip_18 : string = "[source_strip_path: strip=18]"
mapping.daughter_id.strip_19 : string = "[source_strip_path: strip=19]"
mapping.daughter_id.strip_20 : string = "[source_strip_path: strip=20]"


###################################################################
[name="source_strip_block_4.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 11

stacked.model_0   : string = "source_internal_full_strip.model"
stacked.label_0   : string = "strip_21"

stacked.model_1   : string = "source_strip_spacing.model"
stacked.label_1   : string = "spacing_21_22"

stacked.model_2   : string = "source_internal_full_strip.model"
stacked.label_2   : string = "strip_22"

stacked.model_3   : string = "source_strip_spacing.model"
stacked.label_3   : string = "spacing_22_23"

stacked.model_4   : string = "source_internal_full_strip.model"
stacked.label_4   : string = "strip_23"

stacked.model_5   : string = "source_strip_spacing.model"
stacked.label_5   : string = "spacing_23_24"

stacked.model_6   : string = "source_internal_full_strip.model"
stacked.label_6   : string = "strip_24"

stacked.model_7   : string = "source_strip_spacing.model"
stacked.label_7   : string = "spacing_24_25"

stacked.model_8   : string = "source_internal_full_strip.model"
stacked.label_8   : string = "strip_25"

stacked.model_9   : string = "source_strip_spacing.model"
stacked.label_9   : string = "spacing_25_26"

stacked.model_10  : string = "source_internal_full_strip.model"
stacked.label_10  : string = "strip_26"

visibility.hidden          : boolean = false
visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

mapping.daughter_id.strip_21 : string = "[source_strip_path: strip=21]"
mapping.daughter_id.strip_22 : string = "[source_strip_path: strip=22]"
mapping.daughter_id.strip_23 : string = "[source_strip_path: strip=23]"
mapping.daughter_id.strip_24 : string = "[source_strip_path: strip=24]"
mapping.daughter_id.strip_25 : string = "[source_strip_path: strip=25]"
mapping.daughter_id.strip_26 : string = "[source_strip_path: strip=26]"


###################################################################
[name="source_strip_block_5.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 11

stacked.model_0   : string = "source_internal_full_strip.model"
stacked.label_0   : string = "strip_27"

stacked.model_1   : string = "source_strip_spacing.model"
stacked.label_1   : string = "spacing_27_28"

stacked.model_2   : string = "source_internal_full_strip.model"
stacked.label_2   : string = "strip_28"

stacked.model_3   : string = "source_strip_spacing.model"
stacked.label_3   : string = "spacing_28_29"

stacked.model_4   : string = "source_internal_full_strip.model"
stacked.label_4   : string = "strip_29"

stacked.model_5   : string = "source_strip_spacing.model"
stacked.label_5   : string = "spacing_29_30"

stacked.model_6   : string = "source_internal_full_strip.model"
stacked.label_6   : string = "strip_30"

stacked.model_7   : string = "source_strip_spacing.model"
stacked.label_7   : string = "spacing_30_31"

stacked.model_8   : string = "source_internal_full_strip.model"
stacked.label_8   : string = "strip_31"

stacked.model_9   : string = "source_strip_spacing.model"
stacked.label_9   : string = "spacing_31_32"

stacked.model_10  : string = "source_internal_full_strip.model"
stacked.label_10  : string = "strip_32"

visibility.hidden          : boolean = false
visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

mapping.daughter_id.strip_27 : string = "[source_strip_path: strip=27]"
mapping.daughter_id.strip_28 : string = "[source_strip_path: strip=28]"
mapping.daughter_id.strip_29 : string = "[source_strip_path: strip=29]"
mapping.daughter_id.strip_30 : string = "[source_strip_path: strip=30]"
mapping.daughter_id.strip_31 : string = "[source_strip_path: strip=31]"
mapping.daughter_id.strip_32 : string = "[source_strip_path: strip=32]"


###################################################################
[name="source_strip_block_6.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 5

stacked.model_0   : string = "source_internal_full_strip.model"
stacked.label_0   : string = "strip_33"

stacked.model_1   : string = "source_strip_spacing.model"
stacked.label_1   : string = "spacing_33_34"

stacked.model_2   : string = "source_internal_full_strip.model"
stacked.label_2   : string = "strip_34"

stacked.model_3   : string = "source_strip_spacing.model"
stacked.label_3   : string = "spacing_34_35"

stacked.model_4   : string = "source_external_full_strip.model"
stacked.label_4   : string = "strip_35"

visibility.hidden          : boolean = false
visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

mapping.daughter_id.strip_33 : string = "[source_strip_path: strip=33]"
mapping.daughter_id.strip_34 : string = "[source_strip_path: strip=34]"
mapping.daughter_id.strip_35 : string = "[source_strip_path: strip=35]"

#################################################
#  The assembly of all blocks of source strips  #
#################################################


########################################################################################################
#
#                                                 ^ z
#                 calib       calib       calib   :   calib       calib       calib
#     left          0           1           2     :     3           4           5          right
#     gap                                         :                                         gap
#     +--+ +-----+ +-+ +-----+ +-+ +-----+ +-+ +-----+ +-+ +-----+ +-+ +-----+ +-+ +-----+ +--+
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
# ----|--|-|-----|-|-|-|-----|-|-|-|-----|-|-|-|--+--|-|-|-|-----|-|-|-|-----|-|-|-|-----|-|--|-----> y
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     |  | |     | | | |     | | | |     | | | |  :  | | | |     | | | |     | | | |     | |  |
#     +--+ +-----+ +-+ +-----+ +-+ +-----+ +-+ +-----+ +-+ +-----+ +-+ +-----+ +-+ +-----+ +--+
#           block       block       block       block       block       block       block
#             0           1           2           3           4           5           6
#                                                 :
#                                                 :
#
##########################################################################
[name="source_strip_block_assembly.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 15

stacked.model_0   : string = "source_strip_ender_spacing.model"
stacked.label_0   : string = "source_gap_left"

stacked.model_1   : string = "source_strip_block_0.model"
stacked.label_1   : string = "source_block_0"

#stacked.model_2   : string = "source_calibration_track_active.model"
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

stacked.model_14  : string = "source_strip_ender_spacing.model"
stacked.label_14  : string = "source_gap_right"

visibility.hidden          : boolean = false
visibility.hidden_envelope : boolean = true
visibility.color           : string  = "grey"

mapping.daughter_id.calibration_0 : string = "[calibration_path: path=0]"
mapping.daughter_id.calibration_1 : string = "[calibration_path: path=1]"
mapping.daughter_id.calibration_2 : string = "[calibration_path: path=2]"
mapping.daughter_id.calibration_3 : string = "[calibration_path: path=3]"
mapping.daughter_id.calibration_4 : string = "[calibration_path: path=4]"
mapping.daughter_id.calibration_5 : string = "[calibration_path: path=5]"

#####################
#  The core volume  #
#####################

#################################################################
[name="source_core_volume.model" type="geomtools::stacked_model"]

#@config Source core volume

x : real as length =   58.0 mm
y : real as length = 5000.0 mm
z : real as length = 3410.0 mm

material.ref : string = "tracking_gas"

stacked.axis            : string  = "z"
stacked.number_of_items : integer = 5

stacked.model_4 : string = "source_foil_top_gas_gap.model"
stacked.label_4 : string = "top_gas_gap"

stacked.model_3 : string = "source_foil_top_support.model"
stacked.label_3 : string = "top_foil_support"

stacked.model_2 : string = "source_strip_block_assembly.model"
stacked.label_2 : string = "sources"

stacked.model_1 : string = "source_foil_bottom_support.model"
stacked.label_1 : string = "bottom_foil_support"

stacked.model_0 : string = "source_foil_bottom_gas_gap.model"
stacked.label_0 : string = "bottom_gas_gap"

visibility.hidden          : boolean = false
visibility.hidden_envelope : boolean = true
visibility.color           : string  = "cyan"


#############################################################################
[name="source_side_beam_gas_gap.model" type="geomtools::simple_shaped_model"]

#@config Side beam gas gap

shape_type : string  = "box"
x : real as length =   58 mm
y : real as length =  155 mm
z : real as length = 3410 mm

material.ref : string  = "tracking_gas"

visibility.hidden : boolean = false
visibility.color  : string  = "cyan"


########################################################################
[name="source_left_beam_with_gap.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2

stacked.model_1 : string = "source_side_beam_gas_gap.model"
stacked.label_1 : string = "gas_gap"

stacked.model_0 : string = "source_left_beam.model"
stacked.label_0 : string = "beam"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string  = "grey"


#########################################################################
[name="source_right_beam_with_gap.model" type="geomtools::stacked_model"]

material.ref : string = "tracking_gas"

stacked.axis            : string = "y"
stacked.number_of_items : integer = 2

stacked.model_0 : string = "source_side_beam_gas_gap.model"
stacked.label_0 : string = "gas_gap"

stacked.model_1 : string = "source_right_beam.model"
stacked.label_1 : string = "beam"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string  = "grey"


##########################
#  The source submodule  #
##########################

########################################################################
#
#                              ^ z
#                              :
#                              :
#            +------------------------------------+
#            | top beam        :                  |
#            +------------------------------------+
#          +---+ +----------------------------+ +---+
#    left  |   | | core        :              | |   | right
#    beam  |   | | volume      :              | |   | beam
#          |   | |             :              | |   |
#          |   | |             :              | |   |
#  --------|---|-|-------------x--------------|-|---|---------> y
#          |   | |             :              | |   |
#          |   | |             :              | |   |
#          |   | |             :              | |   |
#          |   | |             :              | |   |
#          +---+ +----------------------------+ +---+
#            +------------------------------------+
#            | bottom beam     :                  |
#            +------------------------------------+
#                              :
#                              :
#
########################################################################
[name="source_submodule.model" type="geomtools::surrounded_boxed_model"]

#@config Source submodule

material.ref : string = "lab_air"

surrounded.centered_x : boolean = false
surrounded.centered_y : boolean = false
surrounded.centered_z : boolean = false

surrounded.model : string = "source_core_volume.model"
surrounded.label : string = "source_core_volume"

surrounded.top_model    : string = "source_top_beam.model"
surrounded.bottom_model : string = "source_bottom_beam.model"
surrounded.left_model   : string = "source_left_beam_with_gap.model"
surrounded.right_model  : string = "source_right_beam_with_gap.model"

surrounded.top_label    : string = "top_beam"
surrounded.bottom_label : string = "bottom_beam"
surrounded.left_label   : string = "left_beam"
surrounded.right_label  : string = "right_beam"

visibility.hidden         : boolean = false
visibility.hidden_envelop : boolean = true
visibility.color          : string  = "grey"


# end of source_module.gom
