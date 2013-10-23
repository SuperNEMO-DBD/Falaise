# -*- mode: conf-unix; -*-

##############################################################################
# Drift cell anode bus bar:
[name="anode_bus_bar.model" type="geomtools::simple_shaped_model"]
shape_type         : string = "cylinder"
r                  : real   =    1.32 # mm
z                  : real   =   82.0  # mm
length_unit        : string = "mm"
material.ref       : string = "std::copper"
visibility.hidden  : boolean = 0
visibility.color   : string  = "red"


##############################################################################
# Drift cell anode wire:
[name="anode_wire.model" type="geomtools::simple_shaped_model"]
shape_type                : string = "cylinder"
diameter                  : real   =    0.040 # mm
z                         : real   = 2866.0   # mm
length_unit               : string = "mm"
material.ref              : string = "wire_metal"
visibility.hidden         : boolean = 0
visibility.wired_cylinder : boolean = 1
visibility.color          : string  = "blue"


##############################################################################
# Drift cell base:
[name="drift_cell_base.model" type="geomtools::simple_shaped_model"]
shape_type                : string = "tube"
inner_r                   : real   =  10.0  # mm
outer_r                   : real   =  14.5 # mm
z                         : real   =  74.0 # mm
length_unit               : string = "mm"
material.ref              : string = "std::delrin"
visibility.hidden         : boolean = 0
visibility.wired_cylinder : boolean = 0
visibility.color          : string  = "orange"


##############################################################################
# Drift cell cathode bar:
[name="cathode_bus_bar.model" type="geomtools::simple_shaped_model"]
shape_type                : string = "tube"
outer_r                   : real   = 21.0 # mm
inner_r                   : real   = 19.5 # mm
z                         : real   = 10.0 # mm
length_unit               : string = "mm"
material.ref              : string = "std::copper"
visibility.hidden         : boolean = 0
visibility.wired_cylinder : boolean = 0
visibility.color          : string  = "red"


##############################################################################
# Drift cell cathode ring:
[name="drift_cell_ring.model" type="geomtools::simple_shaped_model"]
shape_type          : string  = "tube"
length_unit         : string  = "mm"
outer_r             : real    = 21.0 # mm
inner_r             : real    = 20.0 # mm
z                   : real    = 53.5 # mm
material.ref        : string  = "std::copper"
visibility.hidden   : boolean = 0
visibility.color    : string  = "red"


##############################################################################
# The core of a drift cell:
[name="drift_cell_core.model" type="geomtools::stacked_model"]
length_unit             : string  = "mm"
x                       : real    = 43.950
y                       : real    = 43.950
z                       : real    = 3030
stacked.axis            : string  = "z"
stacked.number_of_items : integer = 9
material.ref            : string  = "tracking_gas"

stacked.model_8     : string  = "drift_cell_base.model"
stacked.label_8     : string  = "top_base"
stacked.limit_min_8 : real    = +37.00 # allowed Z-shift for stacking

stacked.model_7     : string  = "anode_bus_bar.model"
stacked.label_7     : string  = "top_anode_bus_bar"
stacked.limit_min_7 : real    = -9.0 # allowed Z-shift for stacking

stacked.model_6     : string  = "cathode_bus_bar.model"
stacked.label_6     : string  = "top_cathode_bus_bar"
stacked.limit_min_6 : real    =  -6.5 # allowed Z-shift for stacking

stacked.model_5     : string  = "drift_cell_ring.model"
stacked.label_5     : string  = "top_ring"
stacked.limit_min_5 : real    =  +6.25 # allowed Z-shift for stacking

stacked.model_4     : string  = "anode_wire.model"
stacked.label_4     : string  = "anode_wire"

stacked.model_3     : string  = "drift_cell_ring.model"
stacked.label_3     : string  = "bottom_ring"
stacked.limit_max_3 : real    = -6.25 # allowed Z-shift for stacking

stacked.model_2     : string  = "cathode_bus_bar.model"
stacked.label_2     : string  = "bottom_cathode_bus_bar"
stacked.limit_max_2 : real    = +6.5 # allowed Z-shift for stacking

stacked.model_1     : string  = "anode_bus_bar.model"
stacked.label_1     : string  = "bottom_anode_bus_bar"
stacked.limit_max_1 : real    = +9.0 # allowed Z-shift for stacking

stacked.model_0     : string  = "drift_cell_base.model"
stacked.label_0     : string  = "bottom_base"
stacked.limit_max_0 : real    = -37.00 # allowed Z-shift for stacking

# --> Visibility:
visibility.hidden           : boolean = 0
visibility.hidden_envelop   : boolean = 0
visibility.daughters.hidden : boolean = 1
visibility.color            : string  = "cyan" #"grey"

# --> Sensitive detector category:
sensitive.category                : string = "tracker.SD"
sensitive.record_alpha_quenching  : boolean = 1
sensitive.record_track_id         : boolean = 1
sensitive.record_primary_particle : boolean = 1

# --> Daughters mapping informations:
mapping.daughter_id.anode_wire  : string = "[drift_cell_anodic_wire]"
mapping.daughter_id.bottom_base : string = "[drift_cell_base:base=0]"
mapping.daughter_id.top_base    : string = "[drift_cell_base:base=1]"
mapping.daughter_id.bottom_ring : string = "[drift_cell_cathodic_ring:ring=0]"
mapping.daughter_id.top_ring    : string = "[drift_cell_cathodic_ring:ring=1]"

##############################################################################
# Drift cell field wire:
[name="field_wire.model" type="geomtools::simple_shaped_model"]
shape_type                : string = "cylinder"
diameter                  : real   =    0.050 # mm
z                         : real   = 2920.0  # mm
length_unit               : string = "mm"
material.ref              : string = "wire_metal"
visibility.hidden         : boolean = 0
visibility.wired_cylinder : boolean = 0
visibility.color          : string  = "blue"


##############################################################################
# Drift cell field wire:
# A set of 3 field wires to enclose the core cell in a node:
#
#           +-------+
#   wire #0 |o  o  o| wire #2
#           +-------+
#            wire #1
#
#
[name="field_wires_set.model" type="snemo::geometry::field_wires_set_model"]
field_wire_model             : string  = "field_wire.model"
drift_cell_core_model        : string  = "drift_cell_core.model"
number_of_field_wires        : integer = 3
distance_between_wires       : real as length = 9.11 mm
visibility.hidden            : boolean = 0
visibility.hidden_envelop    : boolean = 0
visibility.daughters.hidden  : boolean = 1
visibility.color             : string  = "cyan"

# --> Daughters mapping informations:
mapping.daughter_id.field_wires  : string  = "[drift_cell_field_wire:wire+0]"

# end of file
