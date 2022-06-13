# -*- mode: conf-unix; -*-
#@description Geometry models for all flat LAPP strips
#@key_label   "name"
#@meta_label  "type"

#####################################
# LAPP strip 1 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_1_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 1 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 310.75 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.767"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_1_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 1 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 298.5 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.762"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_1_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 1 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 322.5 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.785"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_1_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 1 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 331.25 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.789"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_1_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 1 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 377.5 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.723"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_1_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 1 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 286.67 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.914"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_1_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 1 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 276.67 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density2.024"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_1_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 1 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 302.5 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.865"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_1_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 1 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_1_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_1_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_1_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_1_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_1_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_1_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_1_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_1_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_1.flat.model" type="geomtools::stacked_model"]
#@config Strip 1 geometry model (LAPP-8, legacy ID=31)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_1_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 1 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_1.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 4 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_4_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 4 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 288.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.890"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_4_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 4 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 288.3 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.911"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_4_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 4 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 296.25 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.828"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_4_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 4 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 271.67 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.974"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_4_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 4 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 323.33 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.861"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_4_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 4 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 308.89 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.762"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_4_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 4 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 275.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.930"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_4_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 4 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 286.67 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.850"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_4_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 4 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_4_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_4_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_4_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_4_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_4_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_4_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_4_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_4_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_4.flat.model" type="geomtools::stacked_model"]
#@config Strip 4 geometry model (LAPP-9, legacy ID=32)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_4_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 4 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_4.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 5 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_5_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 5 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 309.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.975"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_5_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 5 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 310.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.990"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_5_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 5 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 304.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.964"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_5_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 5 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 293.33 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.947"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_5_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 5 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 309.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.991"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_5_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 5 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 305.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density1.955"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_5_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 5 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 285.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched97.9_pva0.1000_density2.098"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_5_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 5 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 452.5 um
y                  : real as length = 132.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.367"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_5_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 5 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_5_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_5_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_5_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_5_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_5_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_5_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_5_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_5_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_5.flat.model" type="geomtools::stacked_model"]
#@config Strip 5 geometry model (LAPP-20, legacy ID=33)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_5_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 5 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_5.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 6 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_6_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 6 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 370.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.543"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_6_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 6 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 372.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.530"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_6_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 6 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 360.83 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.521"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_6_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 6 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 372.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.537"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_6_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 6 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 386.67 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.525"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_6_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 6 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 360.83 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.566"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_6_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 6 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 337.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.599"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_6_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 6 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 364.17 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.577"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_6_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 6 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_6_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.9 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_6_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.9 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_6_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.9 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_6_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.9 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_6_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.9 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_6_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.9 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_6_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.9 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_6_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.9 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_6.flat.model" type="geomtools::stacked_model"]
#@config Strip 6 geometry model (LAPP-14, legacy ID=26)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_6_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 6 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_6.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 7 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_7_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 7 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 366.67 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.584"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_7_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 7 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 355.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.526"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_7_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 7 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 353.33 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.545"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_7_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 7 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 362.5 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.414"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_7_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 7 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 360.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.448"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_7_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 7 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 379.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.390"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_7_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 7 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 383.33 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.300"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_7_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 7 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 350.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.535"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_7_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 7 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_7_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_7_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_7_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_7_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_7_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_7_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_7_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_7_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.9 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_7.flat.model" type="geomtools::stacked_model"]
#@config Strip 7 geometry model (LAPP-13, legacy ID=25)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_7_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 7 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_7.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 10 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_10_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 10 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 347.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.597"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_10_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 10 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 390.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.444"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_10_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 10 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 351.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.531"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_10_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 10 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 352.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.578"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_10_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 10 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 402.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.317"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_10_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 10 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 411.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.257"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_10_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 10 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 400.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.306"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_10_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 10 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 471.25 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.0 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.408"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_10_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 10 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_10_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.9 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_10_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.95 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_10_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.95 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_10_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.95 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_10_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.95 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_10_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.95 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_10_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.95 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_10_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.95 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_10.flat.model" type="geomtools::stacked_model"]
#@config Strip 10 geometry model (LAPP-16, legacy ID=28)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_10_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_10.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 10 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_10.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 11 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_11_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 11 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 316.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.749"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_11_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 11 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 334.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.664"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_11_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 11 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 343.33 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.654"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_11_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 11 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 326.67 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.708"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_11_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 11 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 330.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.721"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_11_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 11 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 340.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.730"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_11_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 11 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 342.5 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.758"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_11_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 11 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 345.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.591"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_11_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 11 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_11_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_11_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_11_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_11_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_11_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_11_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_11_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_11_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_11.flat.model" type="geomtools::stacked_model"]
#@config Strip 11 geometry model (LAPP-17, legacy ID=23)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_11_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_11.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 11 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_11.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 12 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_12_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 12 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 360.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.547"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_12_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 12 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 372.53 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.552"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_12_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 12 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 365.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.517"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_12_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 12 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 363.33 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.524"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_12_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 12 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 365.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.580"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_12_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 12 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 328.33 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.714"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_12_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 12 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 370.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.480"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_12_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 12 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 472.5 um
y                  : real as length = 132.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.363"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_12_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 12 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_12_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_12_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_12_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_12_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_12_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_12_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_12_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_12_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_12.flat.model" type="geomtools::stacked_model"]
#@config Strip 12 geometry model (LAPP-19, legacy ID=24)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_12_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_12.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 12 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_12.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 13 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_13_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 13 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 495.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.060"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_13_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 13 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 472.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.030"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_13_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 13 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 424.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.088"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_13_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 13 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 427.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.333"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_13_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 13 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 420.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.300"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_13_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 13 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 420.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.296"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_13_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 13 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 322.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.648"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_13_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 13 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 320.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.708"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_13_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 13 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_13_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_13_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_13_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_13_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_13_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_13_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_13_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_13_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_13.flat.model" type="geomtools::stacked_model"]
#@config Strip 13 geometry model (LAPP-11, legacy ID=21)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_13_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_13.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 13 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_13.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 16 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_16_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 16 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 266.67 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density2.068"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_16_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 16 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 279.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density2.093"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_16_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 16 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 263.33 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density2.096"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_16_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 16 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 289.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.972"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_16_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 16 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 365.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.288"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_16_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 16 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 377.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.268"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_16_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 16 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 410.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.140"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_16_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 16 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 460.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.236"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_16_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 16 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_16_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_16_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_16_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_16_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_16_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_16_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_16_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_16_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_16.flat.model" type="geomtools::stacked_model"]
#@config Strip 16 geometry model (LAPP-10, legacy ID=20)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_16_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_16.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 16 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_16.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 17 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_17_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 17 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 282.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 333.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.596"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_17_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 17 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 290.83 um
y                  : real as length = 133.0 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.449"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_17_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 17 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 304.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.444"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_17_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 17 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 287.5 um
y                  : real as length = 133.0 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.509"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_17_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 17 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 311.67 um
y                  : real as length = 133.0 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.486"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_17_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 17 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 302.5 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.570"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_17_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 17 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 275.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.646"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_17_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 17 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 259.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.593"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_17_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 17 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_17_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.85 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_17_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.9 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_17_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.9 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_17_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.9 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_17_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.9 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_17_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.875 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_17_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.9 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_17_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.9 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_17.flat.model" type="geomtools::stacked_model"]
#@config Strip 17 geometry model (LAPP-18, legacy ID=29)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_17_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_17.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 17 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_17.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 18 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_18_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 18 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 330.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.678"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_18_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 18 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 320.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.717"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_18_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 18 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 342.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.576"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_18_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 18 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 315.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.670"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_18_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 18 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 312.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.586"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_18_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 18 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 480.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.347"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_18_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 18 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 467.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.362"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_18_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 18 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 465.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.372"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_18_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 18 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_18_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_18_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_18_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_18_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_18_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_18_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_18_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_18_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_18.flat.model" type="geomtools::stacked_model"]
#@config Strip 18 geometry model (LAPP-12, legacy ID=22)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_18_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_18.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 18 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_18.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 19 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_19_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 19 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 285.0 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.919"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_19_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 19 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 273.3 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.959"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_19_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 19 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 292.5 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.899"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_19_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 19 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 304.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.795"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_19_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 19 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 304.17 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.886"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_19_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 19 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 283.3 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.952"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_19_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 19 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 397.5 um
y                  : real as length = 132.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.299"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_19_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 19 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 390.0 um
y                  : real as length = 132.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched99.88_pva0.1000_density1.227"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_19_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 19 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_19_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_19_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_19_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_19_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_19_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_19_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_19_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_19_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_19.flat.model" type="geomtools::stacked_model"]
#@config Strip 19 geometry model (LAPP-7, legacy ID=19)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_19_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_19.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 19 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_19.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 29 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_29_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 29 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 357.5 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.590"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_29_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 29 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 365.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.562"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_29_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 29 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 349.17 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.596"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_29_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 29 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 355.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.597"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_29_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 29 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 368.18 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.542"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_29_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 29 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 339.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.626"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_29_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 29 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 336.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.617"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_29_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 29 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 361.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 334.0 mm
material.ref       : string  = "snemo::se82_enriched96.65_pva0.1000_density1.487"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_29_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 29 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_29_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.9 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_29_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.9 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_29_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.9 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_29_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.9 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_29_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.9 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_29_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.9 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_29_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.9 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_29_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.9 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_29.flat.model" type="geomtools::stacked_model"]
#@config Strip 29 geometry model (LAPP-15, legacy ID=27)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_29_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_29.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 29 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_29.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# LAPP strip 30 (flat)      
#####################################

######################################################################################################
[name="snemo_strip_30_pad_0.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 30 pad 0 geometry model 
shape_type         : string  = "box"
x                  : real as length = 346.66 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.725"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_30_pad_1.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 30 pad 1 geometry model 
shape_type         : string  = "box"
x                  : real as length = 277.08 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.321"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_30_pad_2.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 30 pad 2 geometry model 
shape_type         : string  = "box"
x                  : real as length = 310.0 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.107"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_30_pad_3.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 30 pad 3 geometry model 
shape_type         : string  = "box"
x                  : real as length = 309.17 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.208"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_30_pad_4.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 30 pad 4 geometry model 
shape_type         : string  = "box"
x                  : real as length = 291.67 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.176"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_30_pad_5.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 30 pad 5 geometry model 
shape_type         : string  = "box"
x                  : real as length = 280.83 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.200"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_30_pad_6.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 30 pad 6 geometry model 
shape_type         : string  = "box"
x                  : real as length = 308.3 um
y                  : real as length = 132.5 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.9_pva0.1000_density2.148"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_30_pad_7.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip 30 pad 7 geometry model 
shape_type         : string  = "box"
x                  : real as length = 348.75 um
y                  : real as length = 133.0 mm
z                  : real as length = 335.5 mm
material.ref       : string  = "snemo::se82_enriched96.1_pva0.1000_density1.737"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"

######################################################################################################
[name="snemo_strip_30_pads.flat.model" type="geomtools::stacked_model"]
#@config Strip 30 multi-pad geometry model
material.ref            : string  = "tracking_gas"
stacked.axis            : string = "z"
stacked.number_of_items : integer = 8
stacked.model_0   : string  = "snemo_strip_30_pad_0.flat.model"
stacked.limit_max_0 : real as length = 16.975 cm
stacked.label_0   : string  = "pad_0"
stacked.model_1   : string  = "snemo_strip_30_pad_1.flat.model"
stacked.limit_max_1 : real as length = 16.975 cm
stacked.label_1   : string  = "pad_1"
stacked.model_2   : string  = "snemo_strip_30_pad_2.flat.model"
stacked.limit_max_2 : real as length = 16.975 cm
stacked.label_2   : string  = "pad_2"
stacked.model_3   : string  = "snemo_strip_30_pad_3.flat.model"
stacked.limit_max_3 : real as length = 16.975 cm
stacked.label_3   : string  = "pad_3"
stacked.model_4   : string  = "snemo_strip_30_pad_4.flat.model"
stacked.limit_max_4 : real as length = 16.975 cm
stacked.label_4   : string  = "pad_4"
stacked.model_5   : string  = "snemo_strip_30_pad_5.flat.model"
stacked.limit_max_5 : real as length = 16.975 cm
stacked.label_5   : string  = "pad_5"
stacked.model_6   : string  = "snemo_strip_30_pad_6.flat.model"
stacked.limit_max_6 : real as length = 16.975 cm
stacked.label_6   : string  = "pad_6"
stacked.model_7   : string  = "snemo_strip_30_pad_7.flat.model"
stacked.limit_max_7 : real as length = 16.975 cm
stacked.label_7   : string  = "pad_7"
visibility.hidden  : boolean = 0
# Mapping the source foil from its parent category :
mapping.daughter_id.pad_0 : string = "[source_pad:pad=0]"
mapping.daughter_id.pad_1 : string = "[source_pad:pad=1]"
mapping.daughter_id.pad_2 : string = "[source_pad:pad=2]"
mapping.daughter_id.pad_3 : string = "[source_pad:pad=3]"
mapping.daughter_id.pad_4 : string = "[source_pad:pad=4]"
mapping.daughter_id.pad_5 : string = "[source_pad:pad=5]"
mapping.daughter_id.pad_6 : string = "[source_pad:pad=6]"
mapping.daughter_id.pad_7 : string = "[source_pad:pad=7]"

######################################################################################################
[name="snemo_strip_30.flat.model" type="geomtools::stacked_model"]
#@config Strip 30 geometry model (LAPP-6, legacy ID=30)
x                  : real    =    1.0 cm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
stacked.axis            : string = "x"
stacked.number_of_items : integer = 3
stacked.model_0  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_0  : string = "back_film"
stacked.model_1  : string = "snemo_strip_30_pads.flat.model"
stacked.label_1  : string = "source"
stacked.model_2  : string = "snemo_strip_wrapper_film.realistic.flat.model"
stacked.label_2  : string = "front_film"
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.back_film  : string = "[source_strip_film:film=0]"
mapping.daughter_id.front_film : string = "[source_strip_film:film=1]"


######################################################################################################
[name="snemo_strip_path_30.flat.model" type="geomtools::simple_shaped_model"]
#@config Strip path 30 geometry model 
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  135.5 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_30.flat.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


