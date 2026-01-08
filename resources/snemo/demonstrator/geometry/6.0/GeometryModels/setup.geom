#####################
# Experimental hall #
#####################


# [name="nothing.model" type="geomtools::simple_shaped_model"]
#   #@config A tiny model replacing anything
#   shape_type : string = "box"
#   x : real as length = 1.0 um
#   y : real as length = 1.0 um
#   z : real as length = 1.0 um
#   material.ref : string = "lab_air"
#   visibility.color : string  = "cyan"


[name="hall_ground.model" type="geomtools::simple_shaped_model"]
  #@config The experimental hall ground
  shape_type : string = "box"
  x : real as length = 10000.0 mm
  y : real as length = 10000.0 mm
  z : real as length =  2300.0 mm
  material.ref : string = "basic::concrete"
  visibility.color : string  = "brown"


[name="experimental_hall.model" type="geomtools::simple_shaped_model"]
  #@config The experimental hall
  # #@verbose_parsing
  # #@variant_devel
  shape_type : string = "box"
  x : real as length = 10000.0 mm
  y : real as length = 10000.0 mm
  z : real as length = 10000.0 mm
  material.ref : string = "lab_air"

  #@allow_key_override

  visibility.hidden           : boolean = false
  visibility.color            : string  = "transparent"
  visibility.envelop_hidden   : boolean = false
  visibility.daughters.hidden : boolean = false
  
  #@variant_if geometry:layout/if_basic|true

    # SN module:
    internal_item.labels             : string[1] = "module_0" 
    internal_item.model.module_0     : string = "module_basic.model"
    internal_item.placement.module_0 : string = "0 0 0 (mm)"
    mapping.daughter_id.module_0     : string = "[module:module=0]"
    visibility.daughters.module_0.shown : boolean = true
 
    # Ground:
    internal_item.labels             : string[1] += "ground"
    internal_item.model.ground       : string = "hall_ground.model"
    internal_item.placement.ground   : string = "0 0 -3870 (mm)"
    mapping.daughter_id.ground       : string = "[ground]"
    visibility.daughters.ground.shown : boolean = true
 
    #@variant_if geometry:layout/if_basic/shielding/is_present|false
 
      #@variant_if geometry:layout/if_basic/shielding/is_present/layout/if_simplified_iron|false

      ########################################
      # Simplified iron shielding (obsolete) #
      ########################################

      internal_item.labels : string[6] += \
        "bottom_shield" \
        "top_shield"    \
        "back_shield"   \
        "front_shield"  \
        "left_shield"   \
        "right_shield"

      visibility.envelop_hidden   : boolean = false
      visibility.daughters.hidden : boolean = false
  	
      internal_item.model.bottom_shield     : string = "bottom_iron_shield_block.model"
      internal_item.placement.bottom_shield : string = "0 0 -2200 (mm) / y 180 (degree)"
      
      internal_item.model.top_shield        : string = "top_iron_shield_block.model"
      internal_item.placement.top_shield    : string = "0 0 +3860 (mm)"
      
      internal_item.model.back_shield       : string = "bf_iron_shield_block.model"
      internal_item.placement.back_shield   : string = "-1635 0 530 (mm) / y 90 (degree)"
      
      internal_item.model.front_shield      : string = "bf_iron_shield_block.model"
      internal_item.placement.front_shield  : string = "+1635 0 530 (mm) / y 270 (degree)"
      
      internal_item.model.left_shield       : string = "lr_iron_shield_block.model"
      internal_item.placement.left_shield   : string = "0 -4335 530 (mm) / x 90 (degree)"
      
      internal_item.model.right_shield      : string = "lr_iron_shield_block.model"
      internal_item.placement.right_shield  : string = "0 +4335 530 (mm) / x 270 (degree)"

      mapping.daughter_id.back_shield   : string = "[external_shield:side=0]"
      mapping.daughter_id.front_shield  : string = "[external_shield:side=1]"
      mapping.daughter_id.left_shield   : string = "[external_shield:side=2]"
      mapping.daughter_id.right_shield  : string = "[external_shield:side=3]"
      mapping.daughter_id.bottom_shield : string = "[external_shield:side=4]"
      mapping.daughter_id.top_shield    : string = "[external_shield:side=5]"

      #@variant_endif geometry:layout/if_basic/shielding/is_present/layout/if_simplified_iron
      
      #@variant_if !geometry:layout/if_basic/shielding/is_present/layout/if_simplified_iron|true

      ###############################################################
      # More realistic versions of the shielding (NOT "simplified") #
      ###############################################################

      visibility.envelop_hidden   : boolean = false
      visibility.daughters.hidden : boolean = false

      #########################
      # ART (Anti Radon Tent) #
      #########################
      internal_item.labels : string[2] += \
      "art_panel_fr_wall" \
      "art_panel_it_wall" 
 
      internal_item.labels : string[1] += \
       "art_panel_floor_wall" 

      internal_item.labels : string[2] += \
      "art_panel_mo_wall" \
      "art_panel_tu_wall" 

      internal_item.labels : string[1] += \
      "art_panel_ce_wall" 
     
      internal_item.model.art_panel_floor_wall      : string = "art_panel_ce_panel_full_build.model"
      internal_item.placement.art_panel_floor_wall  : string = "0 0 -2100 (mm) "

      internal_item.model.art_panel_fr_wall         : string = "art_panel_fr_it_wall_full_build.model"
      internal_item.placement.art_panel_fr_wall     : string = "1370 0 168 (mm) "
      
      internal_item.model.art_panel_it_wall         : string = "art_panel_fr_it_wall_full_build.model"
      internal_item.placement.art_panel_it_wall     : string = "-1370 0 168 (mm) "

      internal_item.model.art_panel_mo_wall         : string = "art_panel_mo_tu_wall_full_build.model"
      internal_item.placement.art_panel_mo_wall     : string = "0 -3660 168 (mm) "
      
      internal_item.model.art_panel_tu_wall         : string = "art_panel_mo_tu_wall_full_build.model"
      internal_item.placement.art_panel_tu_wall     : string = "0 +3660 168 (mm) "
 
      internal_item.model.art_panel_ce_wall         : string = "art_panel_ce_panel_full_build.model"
      internal_item.placement.art_panel_ce_wall     : string = "0 0 2436 (mm) "

      mapping.daughter_id.art_panel_it_wall         : string = "[art_wall:wall=0]"
      mapping.daughter_id.art_panel_fr_wall         : string = "[art_wall:wall=1]"
      mapping.daughter_id.art_panel_mo_wall         : string = "[art_wall:wall=2]"
      mapping.daughter_id.art_panel_tu_wall         : string = "[art_wall:wall=3]"
      mapping.daughter_id.art_panel_floor_wall      : string = "[art_wall:wall=4]"
      mapping.daughter_id.art_panel_ce_wall         : string = "[art_wall:wall=5]"

      ##### Support frames:
      internal_item.labels : string[2] += \
      "support_frame_it" \
      "support_frame_fr" 

      internal_item.model.support_frame_it      : string = "support_frame_fr_it.model"
      internal_item.placement.support_frame_it  : string = "-3105 0 +369 (mm)"

      internal_item.model.support_frame_fr      : string = "support_frame_fr_it.model"
      internal_item.placement.support_frame_fr  : string = "+2195 0 +369 (mm) / z 180 (degree) "

      mapping.daughter_id.support_frame_it      : string = "[support_frame:frame=0]"
      mapping.daughter_id.support_frame_fr      : string = "[support_frame:frame=1]"
    
      #@variant_if geometry:layout/if_basic/shielding/is_present/layout/if_realistic_lab_no_shield|false

      ##### Support frames:
      internal_item.labels : string[1] += "support_frame_floor" 

      internal_item.labels : string[1] += "support_frame_ceiling" 

      internal_item.model.support_frame_ceiling     : string = "support_frame_ceiling.model"
      internal_item.placement.support_frame_ceiling : string = "-455 0 3197.5 (mm) / z 90 (degree)"

      internal_item.model.support_frame_floor       : string = "support_frame_floor.model"
      internal_item.placement.support_frame_floor   : string = "-455 0 -2409.5 (mm) / z 90 (degree)"

      mapping.daughter_id.support_frame_floor       : string = "[support_frame:frame=4]"
      mapping.daughter_id.support_frame_ceiling     : string = "[support_frame:frame=5]"

      # visibility.daughters.support_frame_it.shown      : boolean = true
      # visibility.daughters.support_frame_fr.shown      : boolean = true
      # visibility.daughters.support_frame_floor.shown   : boolean = true
      # visibility.daughters.support_frame_ceiling.shown : boolean = true

      #@variant_endif geometry:layout/if_basic/shielding/is_present/layout/if_realistic_lab_no_shield

      #@variant_if geometry:layout/if_basic/shielding/is_present/layout/if_realistic_lab_shield1|true

      visibility.hidden           : boolean = true
      visibility.color   <        : string  = "cyan"
      visibility.envelop_hidden   : boolean = true
      visibility.daughters.hidden : boolean = false
      # visibility.daughters.support_frame_it.shown : boolean = true

      ##### Support frames:

      internal_item.labels : string[1] += "support_frame_ceiling" 
      internal_item.model.support_frame_ceiling     : string = "support_frame_ceiling_inside_shield.model"
      internal_item.placement.support_frame_ceiling : string = "0 0 3197.5 (mm) "
      mapping.daughter_id.support_frame_ceiling  : string = "[support_frame:frame=5]"

      internal_item.labels : string[1] += "support_frame_it_extra_top" 
      internal_item.model.support_frame_it_extra_top     : string = "support_frame_it_out_iron_shield_top.model"
      ### internal_item.placement.support_frame_it_extra_top : string = "2382.5 0 +3197.5 (mm) " 1927.5
      internal_item.placement.support_frame_it_extra_top : string = "-2382.5 0 +3197.5 (mm) "
      mapping.daughter_id.support_frame_it_extra_top : string = "[support_frame:frame=101]"

      internal_item.labels : string[1] += "support_frame_it_extra_bot" 
      internal_item.model.support_frame_it_extra_bot     : string = "support_frame_it_out_iron_shield_bot.model"
      internal_item.placement.support_frame_it_extra_bot : string = "-2382.5 0 -2409.5 (mm) "
      mapping.daughter_id.support_frame_it_extra_bot : string = "[support_frame:frame=100]"

      internal_item.labels : string[1] += "support_frame_fr_extra_top" 
      internal_item.model.support_frame_fr_extra_top     : string = "support_frame_fr_out_iron_shield_top.model"
      internal_item.placement.support_frame_fr_extra_top : string = "1927.5 0 +3197.5 (mm) "
      mapping.daughter_id.support_frame_fr_extra_top : string = "[support_frame:frame=111]"

      internal_item.labels : string[1] += "support_frame_fr_extra_bot" 
      internal_item.model.support_frame_fr_extra_bot     : string = "support_frame_fr_out_iron_shield_bot.model"
      internal_item.placement.support_frame_fr_extra_bot : string = "1927.5 0 -2409.5 (mm) "
      mapping.daughter_id.support_frame_fr_extra_bot : string = "[support_frame:frame=110]"


      # Iron shielding walls:
      internal_item.labels : string[1] += "iron_floor" 
      internal_item.model.iron_floor       : string = "shielding_floor_iron.model"
      internal_item.placement.iron_floor   : string = "0 0 -2409.5 (mm) "
      mapping.daughter_id.iron_floor       : string = "[external_shield:side=4]" 

      internal_item.labels : string[1] += "iron_ceiling" 
      internal_item.model.iron_ceiling       : string = "shielding_ceiling_iron.model"
      internal_item.placement.iron_ceiling   : string = "0 0 3540 (mm) "
      mapping.daughter_id.iron_ceiling     : string = "[external_shield:side=5]"  

      internal_item.labels : string[1] += "iron_wall_fr" 
      internal_item.model.iron_wall_fr     : string = "shielding_wall_fr_it.model"
      internal_item.placement.iron_wall_fr : string = "+1730 0 +369 (mm) "
      mapping.daughter_id.iron_wall_fr     : string = "[external_shield:side=1]"

      internal_item.labels : string[1] += "iron_wall_it" 
      internal_item.model.iron_wall_it     : string = "shielding_wall_fr_it.model"
      internal_item.placement.iron_wall_it : string = "-1730 0 +369 (mm) / z 180 (degree) "
      mapping.daughter_id.iron_wall_it     : string = "[external_shield:side=0]"

      internal_item.labels : string[1] += "iron_wall_mo"
      internal_item.model.iron_wall_mo     : string = "shielding_wall_tu_mo_block.model"
      internal_item.placement.iron_wall_mo : string = "0 -4366 +369 (mm)"
      mapping.daughter_id.iron_wall_mo     : string = "[external_shield:side=2]" 

      internal_item.labels : string[1] +=  "iron_wall_tu"
      internal_item.model.iron_wall_tu     : string = "shielding_wall_tu_mo_block.model"
      internal_item.placement.iron_wall_tu : string = "0 +4366 +369 (mm) / x 180 (degree) "
      mapping.daughter_id.iron_wall_tu     : string = "[external_shield:side=3]" 

      ##### PE shielding:

      internal_item.labels : string[1] += "pe_ceiling"
      internal_item.model.pe_ceiling     : string = "shielding_ceiling_polyethelene.model"
      internal_item.placement.pe_ceiling : string = "330 0 3762 (mm) "
      mapping.daughter_id.pe_ceiling     : string = "[pe_shield:side=5]"  
 
      ##### Water shielding:

      internal_item.labels : string[2] += "pe_wall_mo" "pe_wall_tu"
      
      internal_item.model.pe_wall_mo            : string = "shielding_tu_mo_polyethelene.model"
      internal_item.placement.pe_wall_mo        : string = "330 -4630 +600 (mm) "
      mapping.daughter_id.pe_wall_mo    : string = "[pe_shield:side=2]" 

      internal_item.model.pe_wall_tu            : string = "shielding_tu_mo_polyethelene.model"
      internal_item.placement.pe_wall_tu        : string = "330 +4630 +600 (mm) "
      mapping.daughter_id.pe_wall_tu    : string = "[pe_shield:side=3]"  

      #@variant_endif geometry:layout/if_basic/shielding/is_present/layout/if_realistic_lab_shield1
      
      #@variant_endif !geometry:layout/if_basic/shielding/is_present/layout/if_simplified_iron

    #@variant_endif geometry:layout/if_basic/shielding/is_present

  #@variant_endif geometry:layout/if_basic

 #@forbid_key_override


[name="world" type="geomtools::simple_world_model"]
  #@config The top-level world volume
  material.ref : string = "vacuum"
  world.x : real as length = 11000.0 mm
  world.y : real as length = 11000.0 mm
  world.z : real as length = 11000.0 mm
  setup.model : string = "experimental_hall.model"
  setup.x     : real as length = 0.0 mm
  setup.y     : real as length = 0.0 mm
  setup.z     : real as length = 0.0 mm
  setup.phi   : real as angle  = 0.0 degree
  setup.theta : real as angle  = 0.0 degree
  visibility.color : string = "grey"
  mapping.daughter_id.setup : string = "[hall:hall=0]"


# end
