# -*- mode: conf-unix; -*-
#@description Geometry models for all flat ITEP strips (but 2)
#@key_label   "name"
#@meta_label  "type"

#####################################
# ITEP strip 3 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_3_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 3 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 240.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.592"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_3.flat.model" type="geomtools::stacked_model"]
#@config Strip 3 geometry model (LAPP-ITEP-3, legacy ID=7)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_3_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 3 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_3.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 8 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_8_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 8 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 220.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.960"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_8.flat.model" type="geomtools::stacked_model"]
#@config Strip 8 geometry model (LAPP-ITEP-2, legacy ID=6)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_8_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_8.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 8 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_8.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 9 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_9_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 9 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 240.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched98_pva0.0820_density2.160"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_9.flat.model" type="geomtools::stacked_model"]
#@config Strip 9 geometry model (ITEP-11, legacy ID=18)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_9_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_9.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 9 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_9.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 14 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_14_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 14 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 250.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched97.5_pva0.0810_density2.300"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_14.flat.model" type="geomtools::stacked_model"]
#@config Strip 14 geometry model (ITEP-10, legacy ID=17)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_14_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_14.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 14 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_14.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 15 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_15_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 15 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 280.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched97_pva0.0801_density2.276"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_15.flat.model" type="geomtools::stacked_model"]
#@config Strip 15 geometry model (ITEP-9, legacy ID=16)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_15_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_15.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 15 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_15.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 20 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_20_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 20 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 280.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched99.6_pva0.0784_density2.388"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_20.flat.model" type="geomtools::stacked_model"]
#@config Strip 20 geometry model (ITEP-8, legacy ID=15)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_20_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_20.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 20 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_20.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 21 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_21_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 21 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 300.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched99.92_pva0.0771_density2.217"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_21.flat.model" type="geomtools::stacked_model"]
#@config Strip 21 geometry model (ITEP-7, legacy ID=14)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_21_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_21.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 21 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_21.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 22 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_22_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 22 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 280.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched98.31_pva0.0820_density2.104"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_22.flat.model" type="geomtools::stacked_model"]
#@config Strip 22 geometry model (ITEP-6, legacy ID=13)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_22_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_22.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 22 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_22.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 23 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_23_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 23 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 300.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched98.31_pva0.0786_density2.198"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_23.flat.model" type="geomtools::stacked_model"]
#@config Strip 23 geometry model (ITEP-5, legacy ID=12)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_23_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_23.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 23 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_23.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 24 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_24_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 24 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 200.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2695.0 mm
material.ref       : string  = "snemo::se82_enriched96.92_pva0.0783_density1.949"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_24.flat.model" type="geomtools::stacked_model"]
#@config Strip 24 geometry model (ITEP-4, legacy ID=11)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_24_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_24.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 24 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_24.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 25 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_25_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 25 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 200.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched96.92_pva0.0790_density1.971"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_25.flat.model" type="geomtools::stacked_model"]
#@config Strip 25 geometry model (ITEP-3, legacy ID=10)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_25_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_25.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 25 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_25.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 26 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_26_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 26 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 190.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched96.92_pva0.0780_density2.081"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_26.flat.model" type="geomtools::stacked_model"]
#@config Strip 26 geometry model (ITEP-2, legacy ID=9)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_26_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_26.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 26 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_26.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 27 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_27_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 27 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 200.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2705.0 mm
material.ref       : string  = "snemo::se82_enriched96.92_pva0.0787_density2.122"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_27.flat.model" type="geomtools::stacked_model"]
#@config Strip 27 geometry model (ITEP-1, legacy ID=8)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_27_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_27.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 27 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_27.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 28 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_28_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 28 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 280.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched97.0_pva0.1000_density2.137"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_28.flat.model" type="geomtools::stacked_model"]
#@config Strip 28 geometry model (LAPP-ITEP-21, legacy ID=34)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_28_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_28.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 28 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_28.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 31 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_31_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 31 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 200.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.823"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_31.flat.model" type="geomtools::stacked_model"]
#@config Strip 31 geometry model (LAPP-ITEP-1, legacy ID=5)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_31_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_31.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 31 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_31.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 32 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_32_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 32 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 210.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.951"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_32.flat.model" type="geomtools::stacked_model"]
#@config Strip 32 geometry model (LAPP-ITEP-5, legacy ID=2)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_32_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_32.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 32 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_32.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 33 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_33_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 33 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 220.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.800"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_33.flat.model" type="geomtools::stacked_model"]
#@config Strip 33 geometry model (LAPP-ITEP-6, legacy ID=3)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_33_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_33.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 33 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_33.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 34 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_34_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 34 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 230.0 um
y                  : real as length = 135.0 mm
z                  : real as length = 2700.0 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.811"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


######################################################################################################
[name="snemo_strip_34.flat.model" type="geomtools::stacked_model"]
#@config Strip 34 geometry model (LAPP-ITEP-4, legacy ID=1)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_34_pad_0.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.source     : string = "[source_pad:pad=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_34.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 34 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_34.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


