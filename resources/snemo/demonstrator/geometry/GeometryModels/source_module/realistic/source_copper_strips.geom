# -*- mode: conf-unix; -*-
# Author : Y.Lemière, F.Mauger
# Date   : June 2018, October 2021
# Geometry based on Andrea source foil review 
# NemoDocDB-doc-4457-v5
#@description Geometry models for flat copper strips
#@key_label   "name"
#@meta_label  "type"


###################################
# Copper strip Mylar support film #
###################################

#######################################################################################
[name="snemo_strip_0_short_film.realistic.model" type="geomtools::simple_shaped_model"]
#@variant_block_only !geometry:layout/if_basic/source_layout/if_basic|false
shape_type         : string  = "box"
x                  : real as length =   12.0 um 
y                  : real as length =  125.0 mm
z                  : real as length =  430.0 mm
material.ref       : string  = "basic::mylar"
visibility.hidden  : boolean = false
visibility.color   : string = "cyan"


##################
# Copper strip 0 #
##################

##################################################################################
[name="snemo_strip_0_pad_0.realistic.model" type="geomtools::simple_shaped_model"]
#@variant_block_only !geometry:layout/if_basic/source_layout/if_basic|false
shape_type         : string  = "box"
x                  : real as length =  74.0 um
y                  : real as length = 122.0 mm
z                  : real as length = 249.0 cm
material.ref       : string  = "basic::copper"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


############################################################################
[name="snemo_strip_0.realistic.model" type="geomtools::simple_shaped_model"]
#@variant_block_only !geometry:layout/if_basic/source_layout/if_basic|false
shape_type         : string  = "box"
x                  : real    =    1.0 cm
y                  : real    =  125.0 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[3] = "bottom_film" "pad_0" "top_film"
internal_item.model.bottom_film       : string  = "snemo_strip_0_short_film.realistic.model"
internal_item.placement.bottom_film   : string  = "0 0 -1460 (mm) "
internal_item.model.pad_0             : string  = "snemo_strip_0_pad_0.realistic.model"
internal_item.placement.pad_0         : string  = "0 0 0 (mm) "
internal_item.model.top_film          : string  = "snemo_strip_0_short_film.realistic.model"
internal_item.placement.top_film      : string  = "0 0 +1460 (mm) "
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.source_0_0 : string = "[source_pad:pad=0]"


#################################################################################
[name="snemo_strip_path_0.realistic.model" type="geomtools::simple_shaped_model"]
#@variant_block_only !geometry:layout/if_basic/source_layout/if_basic|false
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  125.0 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_0.realistic.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


###################
# Copper strip 35 #
###################

###################################################################################
[name="snemo_strip_35_pad_0.realistic.model" type="geomtools::simple_shaped_model"]
#@variant_block_only !geometry:layout/if_basic/source_layout/if_basic|false
shape_type         : string  = "box"
x                  : real as length =  74.0 um
y                  : real as length = 122.0 mm
z                  : real as length = 249.0 cm
material.ref       : string  = "basic::copper"
visibility.hidden  : boolean = false
visibility.color   : string  = "magenta"


#############################################################################
[name="snemo_strip_35.realistic.model" type="geomtools::simple_shaped_model"]
#@variant_block_only !geometry:layout/if_basic/source_layout/if_basic|false
shape_type         : string  = "box"
x                  : real    =    1.0 cm
y                  : real    =  125.0 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[3] = "bottom_film" "pad_0" "top_film"
internal_item.model.bottom_film       : string  = "snemo_strip_0_short_film.realistic.model"
internal_item.placement.bottom_film   : string  = "0 0 -1460 (mm) "
internal_item.model.pad_0             : string  = "snemo_strip_35_pad_0.realistic.model"
internal_item.placement.pad_0         : string  = "0 0 0 (mm) "
internal_item.model.top_film          : string  = "snemo_strip_0_short_film.realistic.model"
internal_item.placement.top_film      : string  = "0 0 +1460 (mm) "
visibility.color  : string = "green"
# Mapping the source foil from its parent category :
mapping.daughter_id.source_0_0 : string = "[source_pad:pad=0]"


##################################################################################
[name="snemo_strip_path_35.realistic.model" type="geomtools::simple_shaped_model"]
#@variant_block_only !geometry:layout/if_basic/source_layout/if_basic|false
shape_type         : string  = "box"
x                  : real    =   58.0 mm
y                  : real    =  125.0 mm
z                  : real    = 3350.0 mm
material.ref       : string  = "tracking_gas"
internal_item.labels : string[1] =  "strip"
internal_item.model.strip      : string = "snemo_strip_35.realistic.model"
internal_item.placement.strip  : string = "0 0 0 (mm) "
mapping.daughter_id.strip      : string = "[source_strip]"


# end
