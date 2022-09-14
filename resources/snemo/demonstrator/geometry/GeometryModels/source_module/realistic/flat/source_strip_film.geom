# -*- mode: conf-unix; -*-
# Author : Y.Lemière, F.Mauger
# Date   : June 2018, October 2021
# Geometry based on Andrea source foil review 
# NemoDocDB-doc-4457-v5
#@description Geometry models for flat Mylar film
#@key_label   "name"
#@meta_label  "type"


#######################################
# Flat LAPP strip Mylar wrapping film #
#######################################

############################################################################################
[name="snemo_strip_wrapper_film.realistic.flat.model" type="geomtools::simple_shaped_model"]
#@variant_block_only !geometry:layout/if_basic/source_layout/if_basic|false
shape_type         : string  = "box"
x                  : real as length =   12.0 um 
y                  : real as length =  135.5 mm
z                  : real as length = 3350.0 mm
material.ref       : string  = "basic::mylar"
visibility.hidden  : boolean = false
visibility.color   : string = "cyan"


# end
