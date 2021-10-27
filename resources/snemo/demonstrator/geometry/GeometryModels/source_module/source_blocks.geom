# -*- mode: conf-unix; -*-
# Source module:
# Description of source blocks geometry for various source layouts

[name="source_strip_gap.model" type="geomtools::simple_shaped_model"]
  shape_type : string  = "box"
    x : real as length =   58.0 mm
    y : real as length =    1.0 mm
    z : real as length = 3350.0 mm
  material.ref : string  = "tracking_gas"
  visibility.color : string  = "cyan"


###########
# Block 0 #
###########

[name="source_strip_block_0.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 5

  #@variant_if geometry:layout/if_basic/source_layout/if_basic|true
    stacked.model_0 : string = "source_external_strip.basic.model"
    stacked.model_2 : string = "source_internal_strip.basic.model"
    stacked.model_4 : string = "source_internal_strip.basic.model"
  #@variant_endif

  #@variant_if !geometry:layout/if_basic/source_layout/if_basic|false
    stacked.model_0   : string = "snemo_strip_path_0.realistic.model" # Position  0 (Copper)
    stacked.model_2   : string = "snemo_strip_path_1.flat.model"      # Position  1 
    stacked.model_4   : string = "snemo_strip_path_2.flat.model"      # Position  2
  #@variant_endif
  
  stacked.model_1 : string = "source_strip_gap.model"
  stacked.model_3 : string = "source_strip_gap.model"
  stacked.label_0 : string = "strip_0"
  stacked.label_1 : string = "strip_gap_0-1"
  stacked.label_2 : string = "strip_1"
  stacked.label_3 : string = "strip_gap_1-2"
  stacked.label_4 : string = "strip_2"
  material.ref : string = "tracking_gas"
  visibility.hidden_envelope : boolean = true
  visibility.color           : string  = "grey"
  mapping.daughter_id.strip_0 : string = "[source_strip_path:strip=0]"
  mapping.daughter_id.strip_1 : string = "[source_strip_path:strip=1]"
  mapping.daughter_id.strip_2 : string = "[source_strip_path:strip=2]"


###########
# Block 1 #
###########

[name="source_strip_block_1.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 11

  #@variant_if geometry:layout/if_basic/source_layout/if_basic|true
    stacked.model_0  : string = "source_internal_strip.basic.model"
    stacked.model_2  : string = "source_internal_strip.basic.model"
    stacked.model_4  : string = "source_internal_strip.basic.model"
    stacked.model_6  : string = "source_internal_strip.basic.model"
    stacked.model_8  : string = "source_internal_strip.basic.model"
    stacked.model_10 : string = "source_internal_strip.basic.model"
  #@variant_endif
 
  #@variant_if !geometry:layout/if_basic/source_layout/if_basic|false
    stacked.model_2  : string = "snemo_strip_path_4.flat.model"   # Position  4
    stacked.model_4  : string = "snemo_strip_path_5.flat.model"   # Position  5
    stacked.model_6  : string = "snemo_strip_path_6.flat.model"   # Position  6
    stacked.model_8  : string = "snemo_strip_path_7.flat.model"   # Position  7

    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_flat|false
      stacked.model_0  : string = "snemo_strip_path_3.flat.model"   # Position  3
      stacked.model_10 : string = "snemo_strip_path_8.flat.model"   # Position  8
    #@variant_endif

    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
      stacked.model_0  : string = "snemo_strip_path_3.snrs1.model"  # Position  3
      stacked.model_10 : string = "snemo_strip_path_8.snrs1.model"  # Position  8
    #@variant_endif

  #@variant_endif

  stacked.model_1  : string = "source_strip_gap.model"
  stacked.model_3  : string = "source_strip_gap.model"
  stacked.model_5  : string = "source_strip_gap.model"
  stacked.model_7  : string = "source_strip_gap.model"
  stacked.model_9  : string = "source_strip_gap.model"
  stacked.label_0  : string = "strip_3"
  stacked.label_1  : string = "strip_gap_3-4"
  stacked.label_2  : string = "strip_4"
  stacked.label_3  : string = "strip_gap_4-5"
  stacked.label_4  : string = "strip_5"
  stacked.label_5  : string = "strip_gap_5-6"
  stacked.label_6  : string = "strip_6"
  stacked.label_7  : string = "strip_gap_6-7"
  stacked.label_8  : string = "strip_7"
  stacked.label_9  : string = "strip_gap_7-8"
  stacked.label_10 : string = "strip_8"
  material.ref : string = "tracking_gas"
  visibility.hidden_envelope : boolean = true
  visibility.color           : string  = "grey"
  mapping.daughter_id.strip_3 : string = "[source_strip_path:strip=3]"
  mapping.daughter_id.strip_4 : string = "[source_strip_path:strip=4]"
  mapping.daughter_id.strip_5 : string = "[source_strip_path:strip=5]"
  mapping.daughter_id.strip_6 : string = "[source_strip_path:strip=6]"
  mapping.daughter_id.strip_7 : string = "[source_strip_path:strip=7]"
  mapping.daughter_id.strip_8 : string = "[source_strip_path:strip=8]"


###########
# Block 2 #
###########

[name="source_strip_block_2.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 11

  #@variant_if geometry:layout/if_basic/source_layout/if_basic|true
    stacked.model_0  : string = "source_internal_strip.basic.model"
    stacked.model_2  : string = "source_internal_strip.basic.model"
    stacked.model_4  : string = "source_internal_strip.basic.model"
    stacked.model_6  : string = "source_internal_strip.basic.model"
    stacked.model_8  : string = "source_internal_strip.basic.model"
    stacked.model_10 : string = "source_internal_strip.basic.model"
  #@variant_endif

  #@variant_if !geometry:layout/if_basic/source_layout/if_basic|false
  
    stacked.model_2  : string = "snemo_strip_path_10.flat.model"  # Position 10
    stacked.model_4  : string = "snemo_strip_path_11.flat.model"  # Position 11
    stacked.model_6  : string = "snemo_strip_path_12.flat.model"  # Position 12
    stacked.model_8  : string = "snemo_strip_path_13.flat.model"  # Position 13
  
    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_flat|false
      stacked.model_0  : string = "snemo_strip_path_9.flat.model"   # Position 9
      stacked.model_10 : string = "snemo_strip_path_14.flat.model"  # Position 14
    #@variant_endif

    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
      stacked.model_0  : string = "snemo_strip_path_9.snrs1.model"  # Position 9
      stacked.model_10 : string = "snemo_strip_path_14.snrs1.model" # Position 14
    #@variant_endif

  #@variant_endif

  stacked.model_1  : string = "source_strip_gap.model"
  stacked.model_3  : string = "source_strip_gap.model"
  stacked.model_5  : string = "source_strip_gap.model"
  stacked.model_7  : string = "source_strip_gap.model"
  stacked.model_9  : string = "source_strip_gap.model"
  stacked.label_0  : string = "strip_9"
  stacked.label_1  : string = "strip_gap_9-10"
  stacked.label_2  : string = "strip_10"
  stacked.label_3  : string = "strip_gap_10-11"
  stacked.label_4  : string = "strip_11"
  stacked.label_5  : string = "strip_gap_11-12"
  stacked.label_6  : string = "strip_12"
  stacked.label_7  : string = "strip_gap_12-13"
  stacked.label_8  : string = "strip_13"
  stacked.label_9  : string = "strip_gap_13-14"
  stacked.label_10 : string = "strip_14"
  material.ref : string = "tracking_gas"
  visibility.hidden_envelope : boolean = true
  visibility.color           : string  = "grey"
  mapping.daughter_id.strip_9  : string = "[source_strip_path:strip=9]"
  mapping.daughter_id.strip_10 : string = "[source_strip_path:strip=10]"
  mapping.daughter_id.strip_11 : string = "[source_strip_path:strip=11]"
  mapping.daughter_id.strip_12 : string = "[source_strip_path:strip=12]"
  mapping.daughter_id.strip_13 : string = "[source_strip_path:strip=13]"
  mapping.daughter_id.strip_14 : string = "[source_strip_path:strip=14]"


###########
# Block 3 #
###########

[name="source_strip_block_3.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 11

  #@variant_if geometry:layout/if_basic/source_layout/if_basic|true
    stacked.model_0  : string = "source_internal_strip.basic.model"
    stacked.model_2  : string = "source_internal_strip.basic.model"
    stacked.model_4  : string = "source_internal_strip.basic.model"
    stacked.model_6  : string = "source_internal_strip.basic.model"
    stacked.model_8  : string = "source_internal_strip.basic.model"
    stacked.model_10 : string = "source_internal_strip.basic.model"
  #@variant_endif

  #@variant_if !geometry:layout/if_basic/source_layout/if_basic|false
  
    stacked.model_2  : string = "snemo_strip_path_16.flat.model"       # Position 16
    stacked.model_4  : string = "snemo_strip_path_17.flat.model"       # Position 17
    stacked.model_6  : string = "snemo_strip_path_18.flat.model"       # Position 18
    stacked.model_8  : string = "snemo_strip_path_19.flat.model"       # Position 19

    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_flat|false
      stacked.model_0  : string = "snemo_strip_path_15.flat.model"  # Position 15
      stacked.model_10 : string = "snemo_strip_path_20.flat.model"  # Position 20
    #@variant_endif
 
    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
      stacked.model_0  : string = "snemo_strip_path_15.snrs1.model" # Position 15
      stacked.model_10 : string = "snemo_strip_path_20.snrs1.model" # Position 20
    #@variant_endif
   
  #@variant_endif

  stacked.model_1  : string = "source_strip_gap.model"
  stacked.model_3  : string = "source_strip_gap.model"
  stacked.model_5  : string = "source_strip_gap.model"
  stacked.model_7  : string = "source_strip_gap.model"
  stacked.model_9  : string = "source_strip_gap.model"
  stacked.label_0  : string = "strip_15"
  stacked.label_1  : string = "strip_gap_15-16"
  stacked.label_2  : string = "strip_16"
  stacked.label_3  : string = "strip_gap_16-17"
  stacked.label_4  : string = "strip_17"
  stacked.label_5  : string = "strip_gap_17-18"
  stacked.label_6  : string = "strip_18"
  stacked.label_7  : string = "strip_gap_18-19"
  stacked.label_8  : string = "strip_19"
  stacked.label_9  : string = "strip_gap_19-20"
  stacked.label_10 : string = "strip_20"
  material.ref : string = "tracking_gas"
  visibility.hidden_envelope : boolean = true
  visibility.color           : string  = "grey"
  mapping.daughter_id.strip_15 : string = "[source_strip_path:strip=15]"
  mapping.daughter_id.strip_16 : string = "[source_strip_path:strip=16]"
  mapping.daughter_id.strip_17 : string = "[source_strip_path:strip=17]"
  mapping.daughter_id.strip_18 : string = "[source_strip_path:strip=18]"
  mapping.daughter_id.strip_19 : string = "[source_strip_path:strip=19]"
  mapping.daughter_id.strip_20 : string = "[source_strip_path:strip=20]"


###########
# Block 4 #
###########

[name="source_strip_block_4.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 11
  
  #@variant_if geometry:layout/if_basic/source_layout/if_basic|true
    stacked.model_0  : string = "source_internal_strip.basic.model"
    stacked.model_2  : string = "source_internal_strip.basic.model"
    stacked.model_4  : string = "source_internal_strip.basic.model"
    stacked.model_6  : string = "source_internal_strip.basic.model"
    stacked.model_8  : string = "source_internal_strip.basic.model"
    stacked.model_10 : string = "source_internal_strip.basic.model"
  #@variant_endif
  
  #@variant_if !geometry:layout/if_basic/source_layout/if_basic|true
    
    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_flat|false
    stacked.model_0  : string = "snemo_strip_path_21.flat.model" # Position 21
    stacked.model_2  : string = "snemo_strip_path_22.flat.model" # Position 22
    stacked.model_4  : string = "snemo_strip_path_23.flat.model" # Position 23
    stacked.model_6  : string = "snemo_strip_path_24.flat.model" # Position 24
    stacked.model_8  : string = "snemo_strip_path_25.flat.model" # Position 25
    stacked.model_10 : string = "snemo_strip_path_26.flat.model" # Position 26
    #@variant_endif

    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
    stacked.model_0  : string = "snemo_strip_path_21.snrs1.model" # Position 21
    stacked.model_2  : string = "snemo_strip_path_22.snrs1.model" # Position 22
    stacked.model_4  : string = "snemo_strip_path_23.snrs1.model" # Position 23
    stacked.model_6  : string = "snemo_strip_path_24.snrs1.model" # Position 24
    stacked.model_8  : string = "snemo_strip_path_25.snrs1.model" # Position 25
    stacked.model_10 : string = "snemo_strip_path_26.snrs1.model" # Position 26
    #@variant_endif

  #@variant_endif
  
  stacked.model_1  : string = "source_strip_gap.model"
  stacked.model_3  : string = "source_strip_gap.model"
  stacked.model_5  : string = "source_strip_gap.model"
  stacked.model_7  : string = "source_strip_gap.model"
  stacked.model_9  : string = "source_strip_gap.model"
  stacked.label_0  : string = "strip_21"
  stacked.label_1  : string = "strip_gap_21-22"
  stacked.label_2  : string = "strip_22"
  stacked.label_3  : string = "strip_gap_22-23"
  stacked.label_4  : string = "strip_23"
  stacked.label_5  : string = "strip_gap_23-24"
  stacked.label_6  : string = "strip_24"
  stacked.label_7  : string = "strip_gap_24-25"
  stacked.label_8  : string = "strip_25"
  stacked.label_9  : string = "strip_gap_25-26"
  stacked.label_10 : string = "strip_26"
  material.ref : string = "tracking_gas"
  visibility.hidden_envelope : boolean = true
  visibility.color           : string  = "grey"
  mapping.daughter_id.strip_21 : string = "[source_strip_path:strip=21]"
  mapping.daughter_id.strip_22 : string = "[source_strip_path:strip=22]"
  mapping.daughter_id.strip_23 : string = "[source_strip_path:strip=23]"
  mapping.daughter_id.strip_24 : string = "[source_strip_path:strip=24]"
  mapping.daughter_id.strip_25 : string = "[source_strip_path:strip=25]"
  mapping.daughter_id.strip_26 : string = "[source_strip_path:strip=26]"


###########
# Block 5 #
###########

[name="source_strip_block_5.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 11

  #@variant_if geometry:layout/if_basic/source_layout/if_basic|true
    stacked.model_0  : string = "source_internal_strip.basic.model"
    stacked.model_2  : string = "source_internal_strip.basic.model"
    stacked.model_4  : string = "source_internal_strip.basic.model"
    stacked.model_6  : string = "source_internal_strip.basic.model"
    stacked.model_8  : string = "source_internal_strip.basic.model"
    stacked.model_10 : string = "source_internal_strip.basic.model"
  #@variant_endif

  #@variant_if !geometry:layout/if_basic/source_layout/if_basic|false
  
    stacked.model_4  : string = "snemo_strip_path_29.flat.model"  # Position 29
    stacked.model_6  : string = "snemo_strip_path_30.flat.model"  # Position 30
    
    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_flat|false
      stacked.model_0  : string = "snemo_strip_path_27.flat.model"  # Position 27
      stacked.model_2  : string = "snemo_strip_path_28.flat.model"  # Position 28
      stacked.model_8  : string = "snemo_strip_path_31.flat.model"  # Position 31
      stacked.model_10 : string = "snemo_strip_path_32.flat.model"  # Position 32
    #@variant_endif
     
    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
      stacked.model_0  : string = "snemo_strip_path_27.snrs1.model" # Position 27
      stacked.model_2  : string = "snemo_strip_path_28.snrs1.model" # Position 28
      stacked.model_8  : string = "snemo_strip_path_31.snrs1.model" # Position 31
      stacked.model_10 : string = "snemo_strip_path_32.snrs1.model" # Position 32
    #@variant_endif

  #@variant_endif

  stacked.model_1  : string = "source_strip_gap.model"
  stacked.model_3  : string = "source_strip_gap.model"
  stacked.model_5  : string = "source_strip_gap.model"
  stacked.model_7  : string = "source_strip_gap.model"
  stacked.model_9  : string = "source_strip_gap.model"
  stacked.label_0  : string = "strip_27"
  stacked.label_1  : string = "strip_gap_27-28"
  stacked.label_2  : string = "strip_28"
  stacked.label_3  : string = "strip_gap_28-29"
  stacked.label_4  : string = "strip_29"
  stacked.label_5  : string = "strip_gap_29-30"
  stacked.label_6  : string = "strip_30"
  stacked.label_7  : string = "strip_gap_30-31"
  stacked.label_8  : string = "strip_31"
  stacked.label_9  : string = "strip_gap_31-32"
  stacked.label_10 : string = "strip_32"
  material.ref : string = "tracking_gas"
  visibility.hidden_envelope : boolean = true
  visibility.color           : string  = "grey"
  mapping.daughter_id.strip_27 : string = "[source_strip_path:strip=27]"
  mapping.daughter_id.strip_28 : string = "[source_strip_path:strip=28]"
  mapping.daughter_id.strip_29 : string = "[source_strip_path:strip=29]"
  mapping.daughter_id.strip_30 : string = "[source_strip_path:strip=30]"
  mapping.daughter_id.strip_31 : string = "[source_strip_path:strip=31]"
  mapping.daughter_id.strip_32 : string = "[source_strip_path:strip=32]"


###########
# Block 6 #
###########

[name="source_strip_block_6.model" type="geomtools::stacked_model"]
  stacked.axis            : string = "y"
  stacked.number_of_items : integer = 5

  #@variant_if geometry:layout/if_basic/source_layout/if_basic|true
    stacked.model_0 : string = "source_internal_strip.basic.model"
    stacked.model_2 : string = "source_internal_strip.basic.model"
    stacked.model_4 : string = "source_external_strip.basic.model"
  #@variant_endif
  
  #@variant_if !geometry:layout/if_basic/source_layout/if_basic|true

    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_flat|false
      stacked.model_0 : string = "snemo_strip_path_33.flat.model"  # Position 33
      stacked.model_2 : string = "snemo_strip_path_34.flat.model"  # Position 34
    #@variant_endif

    #@variant_if geometry:layout/if_basic/source_layout/if_realistic_snrs1|false
      stacked.model_0 : string = "snemo_strip_path_33.snrs1.model" # Position 33
      stacked.model_2 : string = "snemo_strip_path_34.snrs1.model" # Position 34
    #@variant_endif

    stacked.model_4 : string = "snemo_strip_path_35.realistic.model" # Position 35 (Copper)

  #@variant_endif

  stacked.model_1 : string = "source_strip_gap.model"
  stacked.model_3 : string = "source_strip_gap.model"
  stacked.label_0 : string = "strip_33"
  stacked.label_1 : string = "strip_gap_33-34"
  stacked.label_2 : string = "strip_34"
  stacked.label_3 : string = "strip_gap_34-35"
  stacked.label_4 : string = "strip_35"
  material.ref : string = "tracking_gas"
  visibility.hidden_envelope : boolean = true
  visibility.color           : string  = "grey"
  mapping.daughter_id.strip_33 : string = "[source_strip_path:strip=33]"
  mapping.daughter_id.strip_34 : string = "[source_strip_path:strip=34]"
  mapping.daughter_id.strip_35 : string = "[source_strip_path:strip=35]"

# end
