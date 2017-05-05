# -*- mode: conf-unix; -*-
# @falaise:config/snemo/demonstrator/geometry/4.0/models/tracker_module/tracker_layers_missing_wires.geom





# Pawel Guzowski 05 May 2017: Set up the missing wire geometry for the following known missing wires:
#
# SNCM 050 - field (between cell 0 and 9, closest to calo)
# SNCM 123 - anode cell 5 cut out at manchester
# SNCM C0C1 - field (cell 0, middle, closest to calo wall) had fallen out on inspection at LSM gas seal plate removal
# SNCM C0C1 - 3 fields between cell 3 and 4 never constructed by design
# SNCM C2C3 - 3 fields between cell 3 and 4 never constructed by design
#
#
#
# The following cases were not diligently noted during cell insertion, and will need to be determined later:
# SNCM 070 - field (cell 15 somewhere?)
# SNCM 073 - field (somewhere?)
# SNCM 075 - field (somewhere?)
# SNCM 099 - field (cell 10 somewhere?)
# SNCM 101 - field (cell 16 somewhere?)
# SNCM 114 - field (cell 13 'external bottom'?)











#
# Last 108 cells (beginning at #5) of layer 3 are normal
#
##########################################################################
[name="tracker_layer_back_l3_108.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 108
replicated.model           : string = "drift_cell_node_13.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+5]"


#
# Cells 1-4 of layer 3 are normal
#
##########################################################################
[name="tracker_layer_back_l3_003.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 3
replicated.model           : string = "drift_cell_node_13.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+1]"

#
# Set up complete layer 3 (edge normal; 3 normal replicated; missing 1; 108 normal replicated)
#
##########################################################################
[name="tracker_layer_back_l3.model" type="geomtools::stacked_model"]

stacked.axis             : string = "y"
stacked.number_of_items  : integer = 4
stacked.model_0          : string = "drift_cell_node_123.model"
stacked.label_0          : string = "first_drift_cell"
stacked.model_1          : string = "tracker_layer_back_l3_003.model"
stacked.label_1          : string = "other_drift_cells_below"
stacked.model_2          : string = "drift_cell_node_13_missing_anode.model"
stacked.label_2          : string = "missing_anode_cell"
stacked.model_3          : string = "tracker_layer_back_l3_108.model"
stacked.label_3          : string = "other_drift_cells_above"

material.ref : string = "tracking_gas"

# Daughters mapping informations:
mapping.daughter_id.first_drift_cell : string = "[drift_cell:row=0]"
mapping.daughter_id.missing_anode_cell : string = "[drift_cell:row=4]"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"


######################################################
#                                                    #
# Layers 4(front) and 5(back) look identical because #
# these have the missing row of cathode wires in the #
# special cassette in the middle                     #
#                                                    #
######################################################


#
# first 55 replicated cells in layer 4 are normal
#
##########################################################################
[name="tracker_layer_front_l4_055.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 55
replicated.model           : string = "drift_cell_node_13.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+1]"

#
# last 56 replicated cells in layer 4 are normal
#
##########################################################################
[name="tracker_layer_front_l4_056.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 56
replicated.model           : string = "drift_cell_node_13.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+57]"

#
# Set up layer 4 (edge normal, 55 normal replicated, 1 missing, 56 normal replicated)
#
##########################################################################
[name="tracker_layer_front_l4.model" type="geomtools::stacked_model"]

stacked.axis             : string = "y"
stacked.number_of_items  : integer = 4
stacked.model_0          : string = "drift_cell_node_123.model"
stacked.label_0          : string = "first_drift_cell"
stacked.model_1          : string = "tracker_layer_front_l4_055.model"
stacked.label_1          : string = "other_drift_cells_below"
stacked.model_2          : string = "drift_cell_node_13_miss1.model"
stacked.label_2          : string = "missing_cathode_cell"
stacked.model_3          : string = "tracker_layer_front_l4_056.model"
stacked.label_3          : string = "other_drift_cells_above"


material.ref : string = "tracking_gas"

# Daughters mapping informations:
mapping.daughter_id.first_drift_cell : string = "[drift_cell:row=0]"
mapping.daughter_id.missing_cathode_cell : string = "[drift_cell:row=56]"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"


#
# first 55 replicated cells in layer 5 are normal
#
##########################################################################
[name="tracker_layer_back_l5_055.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 55
replicated.model           : string = "drift_cell_node_13.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+1]"

#
# last 56 replicated cells in layer 5 are normal
#
##########################################################################
[name="tracker_layer_back_l5_056.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 56
replicated.model           : string = "drift_cell_node_13.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+57]"

#
# Set up layer 5 (edge normal, 55 normal replicated, 1 missing, 56 normal replicated
#
##########################################################################
[name="tracker_layer_back_l5.model" type="geomtools::stacked_model"]

stacked.axis             : string = "y"
stacked.number_of_items  : integer = 4
stacked.model_0          : string = "drift_cell_node_123.model"
stacked.label_0          : string = "first_drift_cell"
stacked.model_1          : string = "tracker_layer_back_l5_055.model"
stacked.label_1          : string = "other_drift_cells_below"
stacked.model_2          : string = "drift_cell_node_13_miss1.model"
stacked.label_2          : string = "missing_cathode_cell"
stacked.model_3          : string = "tracker_layer_back_l5_056.model"
stacked.label_3          : string = "other_drift_cells_above"


material.ref : string = "tracking_gas"

# Daughters mapping informations:
mapping.daughter_id.first_drift_cell : string = "[drift_cell:row=0]"
mapping.daughter_id.missing_cathode_cell : string = "[drift_cell:row=56]"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"


#
# First 31 replicaed cells are normal
#
##########################################################################
[name="tracker_layer_front_l8_031.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 31
replicated.model           : string = "drift_cell_node_13.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+1]"

#
# Last 80 replicated cells are normal
#
##########################################################################
[name="tracker_layer_front_l8_080.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 80
replicated.model           : string = "drift_cell_node_13.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+33]"

#
# Set up layer 8 (edge normal, 31 normal replicated, 1 missing, 80 normal)
#
############################################################
[name="tracker_layer_front_l8.model" type="geomtools::stacked_model"]

stacked.axis             : string = "y"
stacked.number_of_items  : integer = 4
stacked.model_0          : string = "drift_cell_node_123.model"
stacked.label_0          : string = "first_drift_cell"
stacked.model_1          : string = "tracker_layer_front_l8_031.model"
stacked.label_1          : string = "other_drift_cells_below"
stacked.model_2          : string = "drift_cell_node_13_miss32.model"
stacked.label_2          : string = "missing_cathode_cell"
stacked.model_3          : string = "tracker_layer_front_l8_080.model"
stacked.label_3          : string = "other_drift_cells_above"


material.ref : string = "tracking_gas"

# Daughters mapping informations:
mapping.daughter_id.first_drift_cell : string = "[drift_cell:row=0]"
mapping.daughter_id.missing_cathode_cell : string = "[drift_cell:row=32]"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"



#
# First 55 replicated are normal
#
##################################################################################
[name="tracker_layer_front_l0_055.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 55
replicated.model           : string = "drift_cell_node_013.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+1]"

#
# Last 56 replicated are normal
#
##################################################################################
[name="tracker_layer_front_l0_056.model" type="geomtools::replicated_boxed_model"]

replicated.number_of_items : integer = 56
replicated.model           : string = "drift_cell_node_013.model"
replicated.axis            : string = "y"
replicated.label           : string = "cells"

material.ref               : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.cells : string = "[drift_cell:row+57]"

#
# Set up layer 0 (edge normal, 55 normal replicated, 1 missing, 56 replicated normal
#
####################################################################
[name="tracker_layer_front_l0.model" type="geomtools::stacked_model"]

stacked.axis             : string = "y"
stacked.number_of_items  : integer = 4
stacked.model_0          : string = "drift_cell_node_0123.model"
stacked.label_0          : string = "first_drift_cell"
stacked.model_1          : string = "tracker_layer_front_l0_055.model"
stacked.label_1          : string = "other_drift_cells_below"
stacked.model_2          : string = "drift_cell_node_013_miss01.model"
stacked.label_2          : string = "missing_cathode_cell"
stacked.model_3          : string = "tracker_layer_front_l0_056.model"
stacked.label_3          : string = "other_drift_cells_above"

material.ref             : string = "tracking_gas"

visibility.hidden_envelope  : boolean = true
# visibility.daughters.hidden : boolean = false
visibility.color            : string = "grey"

# Daughters mapping informations:
mapping.daughter_id.first_drift_cell : string = "[drift_cell:row=0]"
mapping.daughter_id.missing_cathode_cell : string = "[drift_cell:row=56]"




# end of @falaise:config/snemo/demonstrator/geometry/4.0/models/tracker_module/tracker_layers_missing_wires.geom
