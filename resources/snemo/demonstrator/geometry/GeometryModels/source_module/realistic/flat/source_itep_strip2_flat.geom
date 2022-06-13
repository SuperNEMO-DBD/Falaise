# -*- mode: conf-unix; -*-
#@description Geometry models for flat ITEP strip 2
#@key_label   "name"
#@meta_label  "type"

#####################################
# ITEP strip 2 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_2_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 2 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 250.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.258"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_2.flat.model" type="geomtools::stacked_model"]
#@config Strip 2 geometry model (LAPP-ITEP-7, legacy ID=4)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_2_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 2 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_2.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


