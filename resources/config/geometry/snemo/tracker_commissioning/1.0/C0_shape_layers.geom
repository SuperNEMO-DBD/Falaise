# -*- mode: conf-unix; -*-

#@description The geometry models that compose the C-shape layers of drift cells

#@key_label   "name"
#@meta_label  "type"


##########################################################################
[name="C_shape_layer_base.model" type="geomtools::replicated_boxed_model"]

#@config Packed neighbouring cells (but the first one)

# Within a standard tracker layer (not the first one in a block of layers):
#
#   o o o
#   +---+o
#   | o |o  cell #N-1 (type=13)
#   +---+o
#     .
#     .
#     .
#   o o o
#   +---+o
#   | o |o  cell #3 (type=13)
#   +---+o
#   o o o
#   +---+o
#   | o |o  cell #2 (type=13)
#   +---+o
#   o o o
#   +---+o
#   | o |o  cell #1 (type=13)
#   +---+o
#   . . .
#   + - +.
#   : . :.  cell #0 does not belong to this model
#   + - +.
#   . . .
#

#######################
# Geometry parameters #
#######################

replicated.number_of_items  : integer = 55
replicated.model            : string  = "drift_cell_node_13.model"
replicated.axis             : string  = "y"
replicated.label            : string = "cells"

#######################
# Material parameters #
#######################

material.ref     : string  = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = 0
visibility.hidden_envelope  : boolean = 1
visibility.daughters.hidden : boolean = 0
visibility.color            : string = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.cells : string = "[drift_cell:row+1]"


############################################################
[name="C_shape_layer.model" type="geomtools::stacked_model"]

#@config A standard tracker layer stacks the first cell and a pack of many successive cells
#   . . .
#   + - +.
#   : . :.  cell #N-1 (type=13)
#   + - +.
#     .
#     .
#     .
#   . . .
#   + - +.
#   : . :.  cell #2 (type=13)
#   + - +.
#   . . .
#   + - +.
#   : . :.  cell #1 (type=13)
#   + - +.
#   o o o
#   +---+o
#   | o |o  cell #0 (type=123)
#   +---+o
#   o o o
#

#######################
# Geometry parameters #
#######################

stacked.axis                : string  = "y"
stacked.number_of_items     : integer = 2
stacked.model_0             : string  = "drift_cell_node_123.model"
stacked.label_0             : string  = "first_drift_cell"
stacked.model_1             : string  = "C_shape_layer_base.model"
stacked.label_1             : string  = "other_drift_cells"

#######################
# Material parameters #
#######################

material.ref     : string  = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = 0
visibility.hidden_envelope  : boolean = 1
visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.first_drift_cell : string = "[drift_cell:row=0]"


##################################################################################
[name="C_shape_layer_base_closing.model" type="geomtools::replicated_boxed_model"]

#@config Packed neighbouring cells (but the first one) within the first tracker layer in a block of layers
#
#   o o o
#  o+---+o
#  o| o |o  cell #N-1 (type=013)
#  o+---+o
#     .
#     .
#     .
#   o o o
#  o+---+o
#  o| o |o  cell #3 (type=013)
#  o+---+o
#   o o o
#  o+---+o
#  o| o |o  cell #2 (type=013)
#  o+---+o
#   o o o
#  o+---+o
#  o| o |o  cell #1 (type=013)
#  o+---+o
#   . . .
#  .+ - +.
#  .: . :.  cell #0 does not belong to this model
#  .+ - +.
#   . . .
#

#######################
# Geometry parameters #
#######################

replicated.number_of_items  : integer = 55
replicated.model            : string  = "drift_cell_node_013.model"
replicated.axis             : string  = "y"
replicated.label            : string = "cells"

#######################
# Material parameters #
#######################

material.ref     : string  = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = 0
visibility.hidden_envelope  : boolean = 1
visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.cells : string = "[drift_cell:row+1]"

####################################################################
[name="C_shape_layer_closing.model" type="geomtools::stacked_model"]

#@config The first tracker layer stacks the first cell and a pack of many successive cells
#
#   . . .
#  .+ - +.
#  .: . :.  cell #N-1 (type=013)
#  .+ - +.
#     .
#     .
#     .
#   . . .
#  .+ - +.
#  .: . :.  cell #2 (type=013)
#  .+ - +.
#   . . .
#  .+ - +.
#  .: . :.  cell #1 (type=013)
#  .+ - +.
#   o o o
#  o+---+o
#  o| o |o  cell #0 (type=O123)
#  o+---+o
#   o o o
#

#######################
# Geometry parameters #
#######################

stacked.axis                : string  = "y"
stacked.number_of_items     : integer = 2
stacked.model_0             : string  = "drift_cell_node_0123.model"
stacked.label_0             : string  = "first_drift_cell"
stacked.model_1             : string  = "C_shape_layer_base_closing.model"
stacked.label_1             : string  = "other_drift_cells"

#######################
# Material parameters #
#######################

material.ref     : string  = "tracking_gas"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = 0
visibility.hidden_envelope  : boolean = 1
visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "cyan"

######################
# Mapping parameters #
######################

mapping.daughter_id.first_drift_cell : string = "[drift_cell:row=0]"

# end