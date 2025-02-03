#ifndef FALAISE_TKRECONSTRUCT_TKOMHIT_H
#define FALAISE_TKRECONSTRUCT_TKOMHIT_H

// Standard headers
#include <iostream>
#include <memory>

// ROOT headers
//#include "TObject.h"

namespace tkrec {

  class TKOMhit
  //  : public TObject
  {
  private:
    // MIRO: Pozrieť či sa dá nejako už pridať metóda  TKOMhit::calibrate() od filipa,  takže bude treba pridať aj double E; - energia, prípadne aj deltaE
    // MIRO: Pridať niečo ako OM_type - aby sám vedel či je main wall, XW alebo GV
    // MIRO: Pridať operator metódy aby sa dali porovnávať, napríklad operator> ktorý testuje či sú vedľa seba a operator>> či sú o dva ďalej - alebo zvoliť nejaký lepší
    
    int     OM_num;
    int     SWCR[4];	// 0 = Side, 1 = Wall, 2 = Column, 3 = Row
    double  xyz[3];	// (x,y,z) coordinate of the center of OM xyz[0] = x, xyz[1] = y, xyz[2] = z
		
    bool    HT;		// high treshold flag
    int32_t charge;
    int16_t amplitude;
    int16_t baseline;
    int64_t OM_TDC;
    int16_t OM_pcell;
		
    void 	set_SWCR_xyz();

  public:
		
    TKOMhit(int _OM_num);
    TKOMhit(int _SWCR[4]);
    TKOMhit(int _OM_num,  bool _HT, int64_t _OM_TDC, int16_t _OM_pcell);
    TKOMhit(int _SWCR[4], bool _HT, int64_t _OM_TDC, int16_t _OM_pcell);
    virtual ~TKOMhit() = default;
		
    void set_OM_num   (int     _OM_num);	
    void set_HT       (bool    _HT);
    void set_charge   (int32_t _charge);
    void set_amplitude(int16_t _amplitude);
    void set_baseline (int16_t _baseline);
    void set_OM_TDC   (int64_t _OM_TDC);
    void set_OM_pcell (int16_t _OM_pcell);

    int     get_OM_num   () const;
    int     get_SWCR     (char _SWCR_n) const;
    double  get_xyz      (char _xyz_n) const;		
    bool    is_HT        () const;
    int32_t get_charge   () const;
    int16_t get_amplitude() const;
    int16_t get_baseline () const;
    int64_t get_OM_TDC   () const;
    int64_t get_OM_pcell () const;

    void print(std::ostream & out_ = std::cout) const;
		
    //ClassDef(TKOMhit,1);
    
  };

  typedef std::shared_ptr<TKOMhit> TKOMhitHdl;
  typedef std::shared_ptr<const TKOMhit> ConstTKOMhitHdl;
  
} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKOMHIT_H
