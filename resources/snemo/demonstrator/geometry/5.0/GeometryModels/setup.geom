#####################
# Experimental hall #
#
[name="hall_ground.model" type="geomtools::simple_shaped_model"]
  #@config The experimental hall ground
  shape_type : string = "box"
  x : real as length = 10000.0 mm
  y : real as length = 10000.0 mm
  z : real as length =  2300.0 mm
  material.ref : string = "basic::concrete"


[name="experimental_hall.model" type="geomtools::simple_shaped_model"]
  #@config The experimental hall
  # #@verbose_parsing
  # #@variant_devel
  shape_type : string = "box"
  x : real as length = 10000.0 mm
  y : real as length = 10000.0 mm
  z : real as length = 10000.0 mm
  material.ref : string = "lab_air"
  visibility.color            : string  = "cyan"

  # "IF_BASIC"
  #@variant_if geometry:layout/if_basic|true
    #@variant_if geometry:layout/if_basic/shielding/is_off|true
      internal_item.labels : string[2] = \
        "module_0" \
        "ground"
    #@variant_endif geometry:layout/if_basic/shielding/is_off

    #@variant_if geometry:layout/if_basic/shielding/is_on|false
      internal_item.labels : string[8] = \
        "module_0"      \
        "ground"        \
        "bottom_shield" \
        "top_shield"    \
        "back_shield"   \
        "front_shield"  \
        "left_shield"   \
        "right_shield"
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
    #@variant_endif geometry:layout/if_basic/shielding/is_on

    internal_item.model.module_0 : string = "module_basic.model"
  #@variant_endif geometry:layout/if_basic

  # "IF_HALF_COMMISSIONING"
  #@variant_if geometry:layout/if_half_commissioning|false
    internal_item.labels : string[2] = \
      "module_0" \
      "ground"
    internal_item.model.module_0     : string = "half_module_commissioning.model"
  #@variant_endif geometry:layout/if_half_commissioning

  # "COMMON"
  internal_item.model.ground     : string = "hall_ground.model"
  internal_item.placement.ground : string = "0 0 -3850 (mm)"
  internal_item.placement.module_0 : string = "0 0 0 (mm)"
  mapping.daughter_id.module_0 : string = "[module:module=0]"
  mapping.daughter_id.ground   : string = "[ground]"


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

