# -*- mode: conf-unix; -*-


# Packed neighbouring cells (but the first one) within
# a standard tracker layer (not the first one in a block of layers):
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
##########################################################################
[name="tracker_layer_base.model" type="geomtools::replicated_boxed_model"]
material.ref               : string  = "tracking_gas"

replicated.number_of_items : integer = 112
replicated.model           : string  = "drift_cell_node_13.model"
replicated.axis            : string  = "y"
replicated.label           : string = "cells"

visibility.hidden           : boolean = 0
visibility.hidden_envelop   : boolean = 1 #1
visibility.daughters.hidden : boolean = 0 #1
visibility.color            : string = "cyan"

# --> Daughters mapping informations:
mapping.daughter_id.cells : string  = "[drift_cell:row+1]"


# A standard tracker layer stacks the first cell and a pack of many successive cells :
#
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
############################################################
[name="tracker_layer.model" type="geomtools::stacked_model"]
material.ref             : string  = "tracking_gas"

stacked.axis             : string  = "y"
stacked.number_of_items  : integer = 2
stacked.model_0          : string  = "drift_cell_node_123.model"
stacked.label_0          : string  = "first_drift_cell"
stacked.model_1          : string  = "tracker_layer_base.model"
stacked.label_1          : string  = "other_drift_cells"

visibility.hidden           : boolean = 0
visibility.hidden_envelop   : boolean = 1 #1
visibility.daughters.hidden : boolean = 0 #1
visibility.color            : string  = "cyan"

# --> Daughters mapping informations:
mapping.daughter_id.first_drift_cell : string = "[drift_cell:row=0]"


# Packed neighbouring cells (but the first one) within
# the first tracker layer in a block of layers:
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
##################################################################################
[name="tracker_layer_base_closing.model" type="geomtools::replicated_boxed_model"]
material.ref               : string  = "tracking_gas"

replicated.number_of_items : integer = 112
replicated.model           : string  = "drift_cell_node_013.model"
replicated.axis            : string  = "y"
replicated.label           : string = "cells"

visibility.hidden           : boolean = 0
visibility.hidden_envelop   : boolean = 1
visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "cyan"

# --> Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+1]"


# The first tracker layer stacks the first cell and a pack of many successive cells :
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
####################################################################
[name="tracker_layer_closing.model" type="geomtools::stacked_model"]
material.ref             : string  = "tracking_gas"

stacked.axis             : string  = "y"
stacked.number_of_items  : integer = 2
stacked.model_0          : string  = "drift_cell_node_0123.model"
stacked.label_0          : string  = "first_drift_cell"
stacked.model_1          : string  = "tracker_layer_base_closing.model"
stacked.label_1          : string  = "other_drift_cells"

visibility.hidden           : boolean = 0
visibility.hidden_envelop   : boolean = 0
visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "cyan"

# --> Daughters mapping informations:
mapping.daughter_id.first_drift_cell : string = "[drift_cell:row=0]"


####################################################################
[name="tracker_gap_calo.model" type="geomtools::simple_boxed_model"]
#@config Tracking gas gap between a tracking layer and the calorimeter
material.ref      : string  = "tracking_gas"
x                 : real    =    6.975
y                 : real    = 4972.05
z                 : real    = 3030.0
length_unit       : string  = "mm"
visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"


######################################################################
[name="tracker_gap_source.model" type="geomtools::simple_boxed_model"]
#@config Tracking gas gap between a tracking layer and the source:
material.ref      : string  = "tracking_gas"
x                 : real    =    1.975
y                 : real    = 4972.05
z                 : real    = 3030.0
length_unit       : string  = "mm"
visibility.hidden : boolean = 0
visibility.color  : string  = "cyan"

# end
