// snemo/digitization/calo_ctw_constants.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_CONSTANTS_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_CONSTANTS_H

namespace snemo {

namespace digitization {

namespace calo {

namespace ctw {

enum layout { LAYOUT_UNDEFINED = 0, LAYOUT_MAIN_WALL = 1, LAYOUT_XWALL_GVETO = 2 };

/// Position of each bits in the CTW bitset word (18 bits)
enum bit_pos_main_wall {
  BEGIN_BIT = 0,
  HTM_MAIN_WALL_BIT0 = 0,
  HTM_MAIN_WALL_BIT1 = 1,
  W_ZW_BIT0 = 2,
  W_ZW_BIT1 = 3,
  W_ZW_BIT2 = 4,
  W_ZW_BIT3 = 5,
  W_ZW_BIT4 = 6,
  W_ZW_BIT5 = 7,
  W_ZW_BIT6 = 8,
  W_ZW_BIT7 = 9,
  W_ZW_BIT8 = 10,
  W_ZW_BIT9 = 11,
  LTO_MAIN_WALL_BIT = 12
};

enum ctw_bit_pos_xwall_gveto {
  HTM_GVETO_BIT0 = 0,
  HTM_GVETO_BIT1 = 1,
  X_ZW_BIT0 = 2,
  X_ZW_BIT1 = 3,
  X_ZW_BIT2 = 4,
  X_ZW_BIT3 = 5,
  HTM_XWALL_SIDE0_BIT0 = 6,
  HTM_XWALL_SIDE0_BIT1 = 7,
  HTM_XWALL_SIDE1_BIT0 = 8,
  HTM_XWALL_SIDE1_BIT1 = 9,
  LTO_XWALL_SIDE0_BIT = 10,
  LTO_XWALL_SIDE1_BIT = 11,
  LTO_GVETO_BIT = 12
};

enum xt_control_bit_pos {
  XT_PC_BIT = 13,
  CONTROL_BIT0 = 14,
  CONTROL_BIT1 = 15,
  CONTROL_BIT2 = 16,
  CONTROL_BIT3 = 17,
  END_BIT = 17
};

/// Size of each bitset in the CTW bitset word
enum bitset_size {
  FULL_BITSET_SIZE = 18,
  HTM_BITSET_SIZE = 2,
  MAIN_ZONING_BITSET_SIZE = 10,
  XWALL_ZONING_BITSET_SIZE = 4,
  LTO_BITSET_SIZE = 1,
  XT_BITSET_SIZE = 1,
  CONTROL_BITSET_SIZE = 4
};

enum xwall_bit_pos {
  ZONING_XWALL_BIT0 = 2,
  ZONING_XWALL_BIT1 = 3,
  ZONING_XWALL_BIT2 = 4,
  ZONING_XWALL_BIT3 = 5
};

}  // end of namespace ctw

}  // end of namespace calo

}  // end of namespace digitization

}  // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_TP_H */

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
