# -*- mode: conf-unix; -*-

######################################################################
# Muon trigger plate                                                 #
######################################################################
[name="muon_trigger_plate.model" type="geomtools::simple_boxed_model"]

#@config The geometric description of a muon trigger plate

#######################
# Geometry parameters #
#######################

length_unit : string = "mm"

x : real as length = 1000.0 mm

y : real as length = 1000.0 mm

z : real as length =   20.0 mm

#######################
# Material parameters #
#######################

#@description Material of the muon trigger plate
material.ref : string = "PVT_scintillator"

#########################
# Visibility parameters #
#########################

visibility.hidden           : boolean = 0
#visibility.hidden_envelope  : boolean = 0
#visibility.daughters.hidden : boolean = 0
visibility.color            : string  = "blue"

#################################
# Sensitive detector parameters #
#################################

#@description Sensitive detector category associated to any muon trigger plate
sensitive.category  : string = "muon_trigger"

#sensitive.record_alpha_quenching  : boolean = 1
#sensitive.record_track_id         : boolean = 1
#sensitive.record_primary_particle : boolean = 1

# end of muon_trigger.geom