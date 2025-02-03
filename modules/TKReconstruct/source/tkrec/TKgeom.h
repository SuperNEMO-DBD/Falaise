#ifndef FALAISE_TKRECONSTRUCT_TKGEOM_H
#define FALAISE_TKRECONSTRUCT_TKGEOM_H


namespace tkrec {

  struct TKgeom
  {
    
    // dimensions in mm
    // origin in the center of detector

    bool has_Bi_source = false;
    double Bi_source_x = 1.4;
    double Bi_source_y = 14.2;
    double Bi_source_z = 21.1;
    double Bi_source_dist_y = 835.0;
    double Bi_source_dist_z = 425.0;

    // dimensions in mm
    // origin in the center of detector
    double tc_radius = 22.0;

    // OM dimensions in mm
    // warning: dimensions are approximately taken from Falaise and might not be exactly right
    double mw_sizex = 194.0;
    double mw_sizey = 256.0;
    double mw_sizez = 256.0;

    double gv_sizex = 308.0;
    double gv_sizey = 310.0;
    double gv_sizez = 150.0;

    double xw_sizex = 200.0;
    double xw_sizey = 150.0;
    double xw_sizez = 208.5;

  };

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKGEOM_H
