#/////////////////////////////Modified Polyeth Model/////////////////////////////////

#///////////////Floor////////////////

  [name="shielding_floor_polyethene_outer_block_extended.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 1647 mm
    z : real as length =  260 mm
  material.ref : string = "basic::polyethylene"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"

  [name="shielding_floor_polyethene_outer_block.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 1149 mm
    z : real as length =  260 mm
  material.ref : string = "basic::polyethylene"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"


  [name="shielding_floor_polyethene_inner_block.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 2583 mm
    z : real as length =  260 mm
  material.ref : string = "basic::polyethylene"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"


  [name="shielding_floor_polyethene_iron_gap.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 30 mm
    z : real as length =  260 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "green"
  
  
  [name="shielding_floor_polyethene.model" type="geomtools::simple_shaped_model"]
   #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 3100 mm
    y : real as length = 7554 mm
    z : real as length = 260.0 mm
  material.ref : string = "lab_air"
  visibility.color            : string  = "blue"

        internal_item.labels : string[7] = \
        "outer_1" \
        "iron_1" \
        "inner_1" \
        "iron_2" \
        "inner_2" \
        "iron_3" \
        "outer_2" 

        internal_item.model.outer_1     : string = "shielding_floor_polyethene_outer_block.model"
        internal_item.placement.outer_1 : string = "0 3202.5 0 (mm) "
        internal_item.model.iron_1     : string = "shielding_floor_polyethene_iron_gap.model"
        internal_item.placement.iron_1 : string = "0 2613 0 (mm) "
        internal_item.model.inner_1     : string = "shielding_floor_polyethene_inner_block.model"
        internal_item.placement.inner_1 : string = "0 1306.5 0 (mm) "
        internal_item.model.iron_2     : string = "shielding_floor_polyethene_iron_gap.model"
        internal_item.placement.iron_2 : string = "0 0 0 (mm) "
        internal_item.model.outer_2     : string = "shielding_floor_polyethene_outer_block.model"
        internal_item.placement.outer_2 : string = "0 -3202.5 0 (mm) "
        internal_item.model.iron_3     : string = "shielding_floor_polyethene_iron_gap.model"
        internal_item.placement.iron_3 : string = "0 -2613 0 (mm) "
        internal_item.model.inner_2     : string = "shielding_floor_polyethene_inner_block.model"
        internal_item.placement.inner_2 : string = "0 -1306.5 0 (mm) "



  [name="shielding_floor_polyethene_extended.model" type="geomtools::simple_shaped_model"]
   #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 3100 mm
    y : real as length = 8550 mm
    z : real as length = 260.0 mm
  material.ref : string = "lab_air"

        internal_item.labels : string[7] = \
        "outer_1" \
        "iron_1" \
        "inner_1" \
        "iron_2" \
        "inner_2" \
        "iron_3" \
        "outer_2" 

        internal_item.model.outer_1     : string = "shielding_floor_polyethene_outer_block_extended.model"
        internal_item.placement.outer_1 : string = "0 3451.5 0 (mm) "
        internal_item.model.iron_1     : string = "shielding_floor_polyethene_iron_gap.model"
        internal_item.placement.iron_1 : string = "0 2613 0 (mm) "
        internal_item.model.inner_1     : string = "shielding_floor_polyethene_inner_block.model"
        internal_item.placement.inner_1 : string = "0 1306.5 0 (mm) "
        internal_item.model.iron_2     : string = "shielding_floor_polyethene_iron_gap.model"
        internal_item.placement.iron_2 : string = "0 0 0 (mm) "
        internal_item.model.outer_2     : string = "shielding_floor_polyethene_outer_block_extended.model"
        internal_item.placement.outer_2 : string = "0 -3451.5 0 (mm) "
        internal_item.model.iron_3     : string = "shielding_floor_polyethene_iron_gap.model"
        internal_item.placement.iron_3 : string = "0 -2613 0 (mm) "
        internal_item.model.inner_2     : string = "shielding_floor_polyethene_inner_block.model"
        internal_item.placement.inner_2 : string = "0 -1306.5 0 (mm) "



#///////////////Ceiling////////////////

  [name="shielding_ceiling_polyethelene.model" type="geomtools::simple_boxed_model"]
  #@config Ceiling polyethelene shielding
  shape_type : string = "box"
    x : real as length = 5220 mm 
    y : real as length = 9000 mm
    z : real as length = 260 mm
  material.ref : string = "basic::polyethylene"
  visibility.hidden           : boolean = false
  visibility.color            : string  = "orange"


#///////////////Mo-Tu////////////////

  [name="shielding_tu_mo_polyethelene.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 5220 mm 
    y : real as length = 240 mm
    z : real as length = 6600 mm
  material.ref : string = "basic::polyethylene"
  visibility.hidden : boolean = false
  visibility.color  : string  = "orange"



# end

 





