#/////////////////////////////Modified Iron Shielding Model/////////////////////////////////






#///////////////Floor Sections////////////////


  [name="shielding_floor_iron_outer_block.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 1149 mm
    z : real as length =  200 mm
  material.ref : string = "basic::polyethylene"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"


  [name="shielding_floor_iron_inner_block.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 2583 mm
    z : real as length =  200 mm
  material.ref : string = "basic::polyethylene"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"

  [name="shielding_floor_iron_beam_gap.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3100 mm 
    y : real as length = 30 mm
    z : real as length =  340 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "green"


  
  [name="shielding_floor_iron.model" type="geomtools::simple_shaped_model"]
   #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 3100 mm
    y : real as length = 7554 mm
    z : real as length = 340 mm
  material.ref : string = "lab_air"

        internal_item.labels : string[7] = \
        "outer_1" \
        "iron_1" \
        "inner_1" \
        "iron_2" \
        "inner_2" \
        "iron_3" \
        "outer_2" 

        internal_item.model.outer_1     : string = "shielding_floor_iron_outer_block.model"
        internal_item.placement.outer_1 : string = "0 3202.5 -70 (mm) "
        internal_item.model.iron_1     : string = "shielding_floor_iron_beam_gap.model"
        internal_item.placement.iron_1 : string = "0 2613 0 (mm) "
        internal_item.model.inner_1     : string = "shielding_floor_iron_inner_block.model"
        internal_item.placement.inner_1 : string = "0 1306.5 -70 (mm) "
        internal_item.model.iron_2     : string = "shielding_floor_iron_beam_gap.model"
        internal_item.placement.iron_2 : string = "0 0 0 (mm) "
        internal_item.model.outer_2     : string = "shielding_floor_iron_outer_block.model"
        internal_item.placement.outer_2 : string = "0 -3202.5 -70 (mm) "
        internal_item.model.iron_3     : string = "shielding_floor_iron_beam_gap.model"
        internal_item.placement.iron_3 : string = "0 -2613 0 (mm) "
        internal_item.model.inner_2     : string = "shielding_floor_iron_inner_block.model"
        internal_item.placement.inner_2 : string = "0 -1306.5 -70 (mm) "









  [name="iron_floor_gap.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm 
    y : real as length = 950 mm
    z : real as length =  400 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "orange"





#///////////////Fr - It Wall Componenets////////////////
 [name="fr_it_block_a.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm
    y : real as length = 1668 mm
    z : real as length =  6138 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "orange"

 [name="fr_it_block_b.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm
    y : real as length = 1000 mm
    z : real as length =  6138 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "orange"

 [name="fr_it_block_c.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm
    y : real as length = 1197 mm
    z : real as length =  6138 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "orange"

  [name="jerry_can_iron_wall_fr_it_block_a.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm 
    y : real as length = 1608 mm
    z : real as length = 500 mm
  material.ref : string = "basic::water"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"

  [name="jerry_can_iron_wall_fr_it_block_c.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 180 mm 
    y : real as length = 1125 mm
    z : real as length = 500 mm

  material.ref : string = "basic::water"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"



# 
#    France and Italy Iron Sheilding Wall Basic Structure
#
#                      ^ y
#                      :
#                      :
#     |       |  |    |  |    |  |       |   
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     |       |  |    |  |    |  |       |
#     +-------+  +----+  +----+  +-------+
#         c   +--+ a  +--+ a  +--+   c
#               b       b       b
#
#



[name="shielding_wall_fr_it.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 360 mm
    y : real as length = 8550 mm
    z : real as length = 6138.0 mm
  material.ref : string = "lab_air"

        internal_item.labels : string[13] = \
        "shield_fr_block_c1" \
        "shield_fr_block_b1" \
        "shield_fr_block_a1" \
        "shield_fr_block_b2" \
        "shield_fr_block_a2" \
        "shield_fr_block_b3" \
        "shield_fr_block_c2" \
        "shield_floor_gap_1" \
        "shield_floor_gap_2" \
        "shield_floor_gap_3" \
        "support_top_beam_1" \
        "support_top_beam_2" \
        "support_top_beam_3" 
      

        internal_item.model.shield_fr_block_c1     : string = "fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c1 : string = "-90 -3676.5 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b1     : string = "fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b1 : string = "90 -2608 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_1     : string = "iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_1 : string = "-90 -2608 -2859.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a1     : string = "fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a1 : string = "-90 -1304 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b2     : string = "fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b2 : string = "90 0 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_2     : string = "iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_2 : string = "-90 0 -2859.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a2     : string = "fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a2 : string = "-90 1304 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b3     : string = "fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b3 : string = "90 2608 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_3     : string = "iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_3 : string = "-90 2608 -2859.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_c2     : string = "fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c2 : string = "-90 3676.5 0 (mm) / y 180 (degree)"

        internal_item.model.support_top_beam_1     : string = "support_frame_top_long_in_iron_wall_beam.model"
        internal_item.placement.support_top_beam_1 : string = "-90 -2608 2769 (mm) / z 90 (degree)"
        internal_item.model.support_top_beam_2     : string = "support_frame_top_long_in_iron_wall_beam.model"
        internal_item.placement.support_top_beam_2 : string = "-90 0 2769 (mm) / z 90 (degree)"
        internal_item.model.support_top_beam_3     : string = "support_frame_top_long_in_iron_wall_beam.model"
        internal_item.placement.support_top_beam_3 : string = "-90 2608 2769 (mm) / z 90 (degree)"









[name="shielding_wall_fr_it_with_jerry_can.model" type="geomtools::simple_shaped_model"]
  #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 360 mm
    y : real as length = 8550 mm
    z : real as length = 6138.0 mm
  material.ref : string = "lab_air"

        internal_item.labels : string[14] = \
        "shield_fr_block_c1" \
        "shield_fr_block_c1_jerry_can" \
        "shield_fr_block_b1" \
        "shield_fr_block_a1" \
        "shield_fr_block_a1_jerry_can" \
        "shield_fr_block_b2" \
        "shield_fr_block_a2" \
        "shield_fr_block_a2_jerry_can" \
        "shield_fr_block_b3" \
        "shield_fr_block_c2" \
        "shield_fr_block_c2_jerry_can" \
        "shield_floor_gap_1" \
        "shield_floor_gap_2" \
        "shield_floor_gap_3" 

        internal_item.model.shield_fr_block_c1     : string = "fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c1 : string = "-90 -3676.5 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_c1_jerry_can     : string = "jerry_can_iron_wall_fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c1_jerry_can : string = "90 -3676.5 -2819 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b1     : string = "fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b1 : string = "90 -2608 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a1     : string = "fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a1 : string = "-90 -1304 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a1_jerry_can     : string = "jerry_can_iron_wall_fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a1_jerry_can : string = "90 -1304 -2819 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_1     : string = "iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_1 : string = "-90 -2608 -2859.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b2     : string = "fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b2 : string = "90 0 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_2     : string = "iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_2 : string = "-90 0 -2859.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a2     : string = "fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a2 : string = "-90 1304 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_a2_jerry_can     : string = "jerry_can_iron_wall_fr_it_block_a.model"
        internal_item.placement.shield_fr_block_a2_jerry_can : string = "90 1304 -2819 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_b3     : string = "fr_it_block_b.model"
        internal_item.placement.shield_fr_block_b3 : string = "90 2608 0 (mm) / y 180 (degree)"
        internal_item.model.shield_floor_gap_3     : string = "iron_floor_gap.model"
        internal_item.placement.shield_floor_gap_3 : string = "-90 2608 -2859.5 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_c2     : string = "fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c2 : string = "-90 3676.5 0 (mm) / y 180 (degree)"
        internal_item.model.shield_fr_block_c2_jerry_can     : string = "jerry_can_iron_wall_fr_it_block_c.model"
        internal_item.placement.shield_fr_block_c2_jerry_can : string = "90 3676.5 -2819 (mm) / y 180 (degree)"


    
 









#///////////////Tu - Mo Wall Componenets////////////////


  [name="tu_mo_block_main_piece.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 2800 mm 
    y : real as length = 180 mm
    z : real as length =  6138 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "orange"


  [name="tu_mo_block_edge_piece.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 510 mm 
    y : real as length = 15 mm
    z : real as length =  6138 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "orange"



  [name="jerry_can_iron_wall_mo_tu.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3320 mm 
    y : real as length = 500 mm
    z : real as length = 500 mm
  material.ref : string = "basic::water"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "blue"




  [name="tu_mo_block.model" type="geomtools::simple_shaped_model"]
   #@variant_section_only geometry:layout/if_basic|true
  #@config Simplified demonstrator module
   
   shape_type : string = "box"
    x : real as length = 3820 mm
    y : real as length = 180 mm
    z : real as length = 6138.0 mm
  material.ref : string = "lab_air"

        internal_item.labels : string[3] = \
        "main" \
        "side1" \
        "side2" 

        internal_item.model.main     : string = "tu_mo_block_main_piece.model"
        internal_item.placement.main : string = "0 0 0 (mm) "
        internal_item.model.side1     : string = "tu_mo_block_edge_piece.model"
        internal_item.placement.side1 : string = "1655 82.5 0 (mm) "
        internal_item.model.side2     : string = "tu_mo_block_edge_piece.model"
        internal_item.placement.side2 : string = "-1655 82.5 0 (mm) "








#///////////////Ceiling////////////////


  [name="shielding_ceiling_iron.model" type="geomtools::simple_boxed_model"]
  #@config New shielding wall
  shape_type : string = "box"
    x : real as length = 3650 mm 
    y : real as length = 8800 mm
    z : real as length = 180 mm
  material.ref : string = "basic::iron"
  visibility.hidden           : boolean = false
  visibility.hidden_envelope  : boolean = false
  visibility.daughters.hidden : boolean = false
  visibility.color            : string  = "orange"










