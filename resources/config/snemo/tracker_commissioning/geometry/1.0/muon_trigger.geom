# -*- mode: conf-unix; -*-

######################################################################
# Muon trigger plate                                                 #
######################################################################
[name="muon_trigger_plate.model" type="geomtools::simple_boxed_model"]

#@config The geometric description of a muon trigger plate

#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit : string = "mm"

#@description X dimension of the box
x : real as length = 1000.0 mm

#@description Y dimension of the box
y : real as length = 1000.0 mm

#@description Z dimension of the box
z : real as length =   20.0 mm

#######################
# Material parameters #
#######################

#@description Material of the muon trigger plate
material.ref : string = "PVT_scintillator"

#########################
# Visibility parameters #
#########################

#@description Hidden flag
visibility.hidden           : boolean = 0

#@description Default color
visibility.color            : string  = "blue"

#################################
# Sensitive detector parameters #
#################################

#@description Sensitive detector category associated to any muon trigger plate
sensitive.category : string = "trigger_SD"

# end of muon_trigger.geom
