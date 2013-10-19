# -*- mode: conf-unix; -*-

#################################################
#@description Calibration Source for BiPo3 module
#################################################


#######################################################################
[name="calibration_source.model" type="geomtools::simple_shaped_model"]
#@config Calibration Source for BiPo3 module

#######################
# Geometry parameters #
#######################

length_unit  : string = "mm"
shape_type   : string = "cylinder"
r            : real   = 10.0  mm
z            : real   =  1.0  mm

#######################
# Material parameters #
#######################

#material.ref : string = "bipo::sodium"
material.ref : string = "bipo::manganese"

#########################
# Visibility parameters #
#########################

visibility.hidden      : boolean = 0
visibility.color       : string = "red"

############################
# Radioactivity parameters #
############################

#radioactivity.nucleide      : string = "Na-22"
radioactivity.nucleide      : string = "Mn-54"

radioactivity.activity      : real   = 37.0
radioactivity.activity.unit : string = "kBq"

# end