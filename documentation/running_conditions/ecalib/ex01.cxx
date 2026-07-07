#include <bitset>
#include <iostream>

/// Define positions of OM status bits in the 16-bitset
enum om_status_bitpos {
  OM_POS_DEAD = 0,
  OM_POS_OFF = 1,
  OM_POS_NOISY = 2,
  OM_POS_NO_BI_MONITORING = 3,
  OM_POS_NO_LI_MONITORING = 4,
  OM_POS_BI_UNSTABILITY_LONG = 5,
  OM_POS_BI_JUMP = 6,
  OM_POS_BI_SCREENING_SE_STRIPS = 7,
  OM_POS_LI_UNSTABILITY_LONG = 8,
  OM_POS_BI_LI_UNCONSISTENCY_SHORT = 9,
  OM_POS_BI_LI_UNCONSISTENCY_LONG = 10,
  OM_POS_BAD_ECALIB_FIT = 11,
  OM_POS_OTHER_ISSUES = 15
};


int main(void)
{
  std::bitset<16> omStatusBitset;
  omStatusBitset[OM_POS_DEAD] = false;
  omStatusBitset[OM_POS_OFF] = false;
  omStatusBitset[OM_POS_NOISY] = false;
  omStatusBitset[OM_POS_NO_BI_MONITORING] = false; // Bi_off
  omStatusBitset[OM_POS_NO_LI_MONITORING] = true; // Li_off
  omStatusBitset[OM_POS_BI_UNSTABILITY_LONG] = false; // long_term_jump_bi
  omStatusBitset[OM_POS_BI_JUMP] = false; // jump_bi
  omStatusBitset[OM_POS_BI_SCREENING_SE_STRIPS] = true; // Bi source screened by Se strip(s)
  omStatusBitset[OM_POS_LI_UNSTABILITY_LONG] = false; // ...
  omStatusBitset[OM_POS_BI_LI_UNCONSISTENCY_SHORT] = false; // ...
  omStatusBitset[OM_POS_BI_LI_UNCONSISTENCY_LONG] = false; // ...
  omStatusBitset[OM_POS_BAD_ECALIB_FIT] = false; // The quality of the energy calibration is poor/bad

  std::cout << omStatusBitset << '\n';
  return 0;
}
