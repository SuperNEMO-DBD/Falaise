# -*- mode: conf-unix; -*-
# calo_pmt_8inch.geom

#################################################
### PMT HAMAMATSU R5912 (8")                  ###
#################################################


###############################################################################
[name="calo.PMT_HAMAMATSU_R5912.dynodes" type="geomtools::simple_shaped_model"]
#@config Simplified dynodes

shape_type  : string = "tube"
outer_r     : real as length = 20.0 mm
inner_r     : real as length = 19.5 mm
z           : real as length = 100 mm

material.ref      : string = "std::copper"

visibility.hidden : boolean = 0
visibility.color  : string  = "orange"


############################################################################
[name="calo.PMT_HAMAMATSU_R5912.base" type="geomtools::simple_shaped_model"]
#@config Simplified base

shape_type  : string = "cylinder"
r           : real as length = 27.5 mm
z           : real as length = 60.0 mm

material.ref      : string = "std::delrin"

visibility.hidden : boolean = 0
visibility.color  : string  = "orange"


#######################################################################
[name="calo.PMT_HAMAMATSU_R5912" type="geomtools::simple_shaped_model"]
#@config The configuration parameters for the PMT's bulb and its contents

#@description The default implicit length unit
length_unit : string = "mm"

#@description The shape of the PMT's bulb
shape_type  : string = "polycone"

#@description The polycone build mode
build_mode  : string = "datafile"

#@description The polycone coordinates filename
datafile    : string = "@falaise:config/common/geometry/pmt/1.0/hamamatsu_R5912MOD/hamamatsu_R5912MOD_shape.data"

#@description The 'filled' mode to build the model
filled_mode : string = "by_envelope"

#@description The label of the PMT's bulb volume as daughter volume of the model's envelope
filled_label : string = "bulb"

#######################
# Material parameters #
#######################

#@description The material name
material.ref        : string = "glass"

#@description The inner material name
material.filled.ref : string = "vacuum"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden           : boolean = 1

#@description The visibility hidden flag for the envelope
visibility.hidden_envelop   : boolean = 0

#@description The recommended color for the display
visibility.color            : string  = "cyan"

#@description The visibility hidden flag for the daughters volumes
visibility.daughters.hidden : boolean = 1

###########################
# Internal/daughter items #
###########################

#@description The list of daughter volumes by labels
internal_item.filled.labels            : string[1] = "dynodes"

#@description The placement of the "dynodes" daughter volume
internal_item.filled.placement.dynodes : string  = "0 0 -50 (mm)"

#@description The model of the "dynodes" daughter volume
internal_item.filled.model.dynodes     : string  = "calo.PMT_HAMAMATSU_R5912.dynodes"

#################
# Mapping rules #
#################

mapping.filled.daughter_id.bulb : string = "[calorimeter_pmt_glass]"

# end of calo_pmt_8inch.geom
