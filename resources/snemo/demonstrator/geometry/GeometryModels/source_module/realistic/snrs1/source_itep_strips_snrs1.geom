# -*- mode: conf-unix; -*-
#@description Geometry models for all deformed ITEP strips (model SNRS1)
#@key_label   "name"
#@meta_label  "type"

#####################################
# ITEP strip 3 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_3_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 3 pad 0 geometry model (SNRS1 model)
strip_id : integer = 3
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_3.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 3 geometry model (LAPP-ITEP-3, legacy ID=7)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_3_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_3.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_3.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 8 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_8_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 8 pad 0 geometry model (SNRS1 model)
strip_id : integer = 8
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_8.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 8 geometry model (LAPP-ITEP-2, legacy ID=6)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_8_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_8.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_8.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 9 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_9_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 9 pad 0 geometry model (SNRS1 model)
strip_id : integer = 9
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_9.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 9 geometry model (ITEP-11, legacy ID=18)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_9_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_9.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_9.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 14 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_14_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 14 pad 0 geometry model (SNRS1 model)
strip_id : integer = 14
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_14.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 14 geometry model (ITEP-10, legacy ID=17)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_14_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_14.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_14.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 15 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_15_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 15 pad 0 geometry model (SNRS1 model)
strip_id : integer = 15
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_15.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 15 geometry model (ITEP-9, legacy ID=16)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_15_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_15.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_15.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 20 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_20_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 20 pad 0 geometry model (SNRS1 model)
strip_id : integer = 20
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_20.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 20 geometry model (ITEP-8, legacy ID=15)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_20_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_20.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_20.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 21 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_21_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 21 pad 0 geometry model (SNRS1 model)
strip_id : integer = 21
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_21.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 21 geometry model (ITEP-7, legacy ID=14)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_21_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_21.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_21.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 22 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_22_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 22 pad 0 geometry model (SNRS1 model)
strip_id : integer = 22
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_22.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 22 geometry model (ITEP-6, legacy ID=13)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_22_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_22.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_22.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 23 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_23_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 23 pad 0 geometry model (SNRS1 model)
strip_id : integer = 23
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_23.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 23 geometry model (ITEP-5, legacy ID=12)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_23_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_23.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_23.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 24 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_24_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 24 pad 0 geometry model (SNRS1 model)
strip_id : integer = 24
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_24.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 24 geometry model (ITEP-4, legacy ID=11)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_24_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_24.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_24.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 25 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_25_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 25 pad 0 geometry model (SNRS1 model)
strip_id : integer = 25
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_25.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 25 geometry model (ITEP-3, legacy ID=10)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_25_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_25.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_25.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 26 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_26_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 26 pad 0 geometry model (SNRS1 model)
strip_id : integer = 26
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_26.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 26 geometry model (ITEP-2, legacy ID=9)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_26_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_26.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_26.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 27 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_27_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 27 pad 0 geometry model (SNRS1 model)
strip_id : integer = 27
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_27.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 27 geometry model (ITEP-1, legacy ID=8)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_27_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_27.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_27.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 28 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_28_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 28 pad 0 geometry model (SNRS1 model)
strip_id : integer = 28
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_28.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 28 geometry model (LAPP-ITEP-21, legacy ID=34)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_28_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_28.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_28.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 31 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_31_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 31 pad 0 geometry model (SNRS1 model)
strip_id : integer = 31
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_31.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 31 geometry model (LAPP-ITEP-1, legacy ID=5)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_31_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_31.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_31.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 32 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_32_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 32 pad 0 geometry model (SNRS1 model)
strip_id : integer = 32
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_32.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 32 geometry model (LAPP-ITEP-5, legacy ID=2)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_32_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_32.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_32.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 33 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_33_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 33 pad 0 geometry model (SNRS1 model)
strip_id : integer = 33
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_33.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 33 geometry model (LAPP-ITEP-6, legacy ID=3)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_33_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_33.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_33.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


#####################################
# ITEP strip 34 (SNRS1)      
#####################################

######################################################################################################
[name="snemo_strip_34_pad_0.snrs1.model" type="snrs::mesh_pad_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 34 pad 0 geometry model (SNRS1 model)
strip_id : integer = 34
visibility.hidden            : boolean = false
visibility.color             : string = "green"
source.visibility.hidden     : boolean = false
source.visibility.color      : string = "magenta"
back_film.visibility.hidden  : boolean = true
back_film.visibility.color   : string = "cyan"
front_film.visibility.hidden : boolean = true
front_film.visibility.color  : string = "cyan"
mapping.daughter_id.back_film   : string = "[source_pad_film:film=0]"
mapping.daughter_id.source      : string = "[source_pad_bulk]"
mapping.daughter_id.front_film  : string = "[source_pad_film:film=1]"


######################################################################################################
[name="snemo_strip_34.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
#@config Strip 34 geometry model (LAPP-ITEP-4, legacy ID=1)
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 2700.0 mm
material.ref : string = "tracking_gas"
internal_item.labels          : string[1] =  "pad0"
internal_item.model.pad0      : string = "snemo_strip_34_pad_0.snrs1.model"
internal_item.placement.pad0  : string = "0 0 0 (mm) "
mapping.daughter_id.pad0      : string = "[source_pad:pad=0]"


######################################################################################################
[name="snemo_strip_path_34.snrs1.model" type="geomtools::simple_shaped_model"]
#@variant_block_only geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
shape_type   : string = "box"
x            : real as length = 58.0 mm
y            : real as length = 135.5 mm
z            : real as length = 3350.0 mm
material.ref : string = "tracking_gas"
internal_item.labels           : string[1] = "strip"
internal_item.model.strip      : string = "snemo_strip_34.snrs1.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


