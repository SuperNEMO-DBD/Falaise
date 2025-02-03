#ifndef FALAISE_TKRECONSTRUCT_TKTRHIT_H
#define FALAISE_TKRECONSTRUCT_TKTRHIT_H

// Standard headers
#include <iostream>
#include <limits>
#include <memory>

// ROOT headers
//#include "TObject.h"

namespace tkrec {

  class TKtrack;
  class TKOMhit;
  
  class TKtrhit
  //  : public TObject
  {
  public:
    
    static const double default_sigma_R; // in mm
    static const double default_sigma_Z; // in mm
    static const double default_h; // in mm
    static const double default_r; // in mm
    
  private:
    // MIRO: Pridať is_corner, is_side, is_inside, is_broken - určí drift model
		
    int    cell_num = -1; ///< Invalid number
    int    SRL[3];	  ///< 0 = Side, 1 = Row, 2 = Layer
    double xy [2];	  ///< (x,y) coordinate of the tracker cell: xy[0] = x, xy[1] = y

    int64_t tsp[7]; // 0-4 = timestamp 0-4, 5 = cathode bottom, 6 = cathode top
    
    typedef std::shared_ptr<const TKOMhit> ConstTKOMhitHdl;
    ConstTKOMhitHdl associated_OMhit;
    
    // typedef std::shared_ptr<TKtrack> TKtrackHdl;
    typedef std::shared_ptr<const TKtrack> ConstTKtrackHdl;
    ConstTKtrackHdl associated_track;

    // vertical position of the hit
    double h = default_h; // std::numeric_limits<double>::quiet_NaN();
    double sigma_Z = default_sigma_Z; // std::numeric_limits<double>::quiet_NaN(); // default value: 17.0 mm (Gaussian model)

    // r = drift radius 		
    double r = default_r; // std::numeric_limits<double>::quiet_NaN();
    double sigma_R = default_sigma_R; //std::numeric_limits<double>::quiet_NaN(); // default value: 2.0 mm (Gaussian model)
		
    void set_SRL_xy();

  public:
		
    TKtrhit() = default;
    TKtrhit(int _cell_num);
    TKtrhit(int _SRL[3]);
    TKtrhit(int _cell_num, int64_t _tsp[7]);
    TKtrhit(int _SRL[3],   int64_t _tsp[7]);
    virtual ~TKtrhit() = default;

    void set_cell_num(int _cell_num);
    void set_tsp(int64_t _tsp[7]); // sets timestamps

    void set_h(double _h);
    void set_h();	// space for better model implementation
    void set_sigma_Z(double _sigma_Z);

    // Remove this
    void set_sigma_Z(); // space for better model implementation 
		
    void set_r(double _r);
    void set_sigma_R(double _sigma_R);

    // Remove this
    void set_sigma_R(); // space for better model implementation
		
    void set_associated_OMhit(const ConstTKOMhitHdl & _associated_OMhit);
    void set_associated_track(const ConstTKtrackHdl & _associated_track);

    int     get_cell_num  () const;
    int     get_SRL       (char _SRL_n) const;
    double  get_xy        (char _xy_n) const;
    int64_t get_tsp       (char _tsp_n) const; // returns timestamp
    double  get_r         () const;
    double  get_sigma_R   () const;
    double  get_h         () const;
    double  get_sigma_Z   () const;
    const ConstTKOMhitHdl & get_associated_OMhit() const;
    const ConstTKtrackHdl & get_associated_track() const;
    bool has_associated_track() const;
    bool has_associated_OMhit() const;
    // TKtrack & get_associated_track();
		
    void print(std::ostream & out_ = std::cout) const;
		
    // ClassDef(TKtrhit,1);
    
  };
  
  typedef std::shared_ptr<TKtrhit> TKtrhitHdl;
  typedef std::shared_ptr<const TKtrhit> ConstTKtrhitHdl;

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKTRHIT_H
