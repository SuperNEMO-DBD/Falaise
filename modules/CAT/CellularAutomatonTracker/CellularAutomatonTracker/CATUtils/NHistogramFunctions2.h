//
//  NHistogramFunctions2.h
//  CellularAutomatonTracker
//
//  Created by Stefano Torre on 15/02/12.
//  Copyright (c) 2012 Univesity College London. All rights reserved.
//

#ifndef __cat_utils__NHistogramFunctions2_h_
#define __cat_utils__NHistogramFunctions2_h_ 1

#include "CATUtils/ROOTbase.h"

namespace CAT{
  namespace utils{

    TH3F* Divide3D( TH3F *num, TH3F *den );
    TH2F* Divide2D( TH2F *num, TH2F *den );
    TH1F* Divide1D( TH1F *num, TH1F *den );
    TH2F* Average3D_to_2D( TH3F *object, char *axis1, char *axis2 );
    TH1F* Average2D_to_1D( TH2F* object, char *axis1 );
    TH1F* Integrate2D_to_1D( TH2F* object, char *axis1, double mult, char *option);
    double Integrate2D( TH2F* object, double mult, char *option );
    TH2F* Convert_to_pth( TH3F* object );

  } // end of namespace utils
} // end of namespace CAT

#endif// __cat_utils__NHistogramFunctions2_h_
