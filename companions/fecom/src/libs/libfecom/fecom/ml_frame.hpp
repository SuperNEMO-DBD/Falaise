//! \file fecom/calo_hit_parser.hpp
//! \brief Parser for a calo hit
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_ML_FRAME_HPP
#define FECOM_ML_FRAME_HPP

namespace fecom {

  typedef struct ml_frame {

  unsigned int frame_id; // Frame index 32 bits
  int frame_num;    //?
  int data_size;    // user data buffer size
  char sub_address; // 0==SAMINDEX, 1==SAMINDEX, other possible but not used
  int nlayer;       // number of layers for this frame
  int path[MAXLAYER];
// path[0, // Layer 0: Control board index == Control FPGA
//      1, // Layer 1: FE FPGA [0..3]
//      1, // Layer 2: FE board == FE FPGA [0..4] (5 chips)
//      3, // Layer 3: FE FPGA == [0..1] (5 FEBs)
//      -1 // end of path
// ,]
  unsigned char * user_data;


} ML_Frame;

} // namespace fecom

#endif // FECOM_ML_FRAME_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
