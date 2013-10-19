# -*- mode: conf-unix; -*-
# setup.geom

#@description The geometry models that compose the top level volumes of the setup
#@key_label  "name"
#@meta_label "type"


######################################################################
# Experimental hall volume                                           #
######################################################################
[name="experimental_hall.model" type="geomtools::simple_shaped_model"]

#@config The experimental hall volume description

#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit : string = "mm"

#@description Shape of the volume
shape_type : string = "box"

#@description X dimension fo the hall
x            : real as length = 5500.0 mm

#@description Y dimension fo the hall
y            : real as length = 5500.0 mm

#@description Z dimension fo the hall
z            : real as length = 2000.0 mm

#######################
# Material parameters #
#######################

#@description Material of the laboratory atmosphere
material.ref : string = "lab_air"

#########################
# Visibility parameters #
#########################

#@description Hidden visibility flag
visibility.hidden           : boolean = 0

#@description Hidden envelop visibility flag
visibility.hidden_envelope  : boolean = 0

#@description Hidden daughter volumes visibility flag
visibility.daughters.hidden : boolean = 0

#@description Display color
visibility.color            : string  = "cyan"

####################
# Daughter volumes #
####################

#@properties List of objects inside the experimental hall
internal_item.labels : string[3] = \
  "top_muon_trigger" \
  "bottom_muon_trigger" \
  "C0_module"

  ####################
  ### Muon trigger ###
  ####################

#@description the name of the geometry model used to describe the muon trigger scin. top plate
internal_item.model.top_muon_trigger        : string = "muon_trigger_plate.model"

#@description the placement of the muon trigger scin. top plate
internal_item.placement.top_muon_trigger    : string = "0 0 50 (cm)"

#@description the name of the geometry model used to describe a muon trigger scin. bottom plate
internal_item.model.bottom_muon_trigger     : string = "muon_trigger_plate.model"

#@description the placement of the muon trigger scin. bottom plate
internal_item.placement.bottom_muon_trigger : string = "0 0 -50 (cm)"

  #######################
  ### C0-shape module ###
  #######################

#@description the name of the geometry model used to describe the C0-shape half-tracker submodule
internal_item.model.C0_module     : string = "C0_module.model"

#@description the placement of the C0-shape half-tracker submodule
internal_item.placement.C0_module : string = "0 0 0 (cm) / y +90 (degree)"

######################
# Mapping parameters #
######################

#@description Daughter bottom muon trigger plate mapping rule
mapping.daughter_id.bottom_muon_trigger : string = "[mu_trigger:position=0]"

#@description Daughter top muon trigger plate mapping rule
mapping.daughter_id.top_muon_trigger    : string = "[mu_trigger:position=1]"

#@description Daughter C0-module mapping rule
mapping.daughter_id.C0_module           : string = "[module:module=0]"


####################################################
# World volume                                     #
####################################################
[name="world" type="geomtools::simple_shaped_model"]

#@config The world volume description

#######################
# Geometry parameters #
#######################

#@description Default length unit
length_unit : string = "mm"

#@description Shape of the world volume
shape_type  : string = "box"

#@description X dimension of the world volume
x : real = 6000 mm

#@description Y dimension of the world volume
y : real = 6000 mm

#@description Z dimension of the world volume
z : real = 6000 mm

#######################
# Material parameters #
#######################

#@description Material of the laboratory atmosphere
material.ref : string = "vacuum"

#########################
# Visibility parameters #
#########################

#@description Hidden visibility flag
visibility.hidden           : boolean = 0

#@description Hidden envelop visibility flag
visibility.hidden_envelope   : boolean = 0

#@description Hidden daughter volumes visibility flag
visibility.daughters.hidden : boolean = 0

#@description Display color
visibility.color            : string  = "grey"

####################
# Daughter volumes #
####################

#@properties List of objects inside the experimental hall
internal_item.labels : string[1] = "experimental_hall"

#@description Name of the geometry model of the setup
setup.model  : string = "experimental_hall.model"

#@description The name of the geometry model used to describe the experimental hall
internal_item.model.experimental_hall     : string = "experimental_hall.model"

#@description The placement of the experimental hall
internal_item.placement.experimental_hall : string = "0 0 0 (cm)"

######################
# Mapping parameters #
######################

#@description Daughter setup volume mapping rule
mapping.daughter_id.experimental_hall : string = "[hall:hall=0]"


# end of setup.geom
