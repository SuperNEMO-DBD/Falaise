// TK headers
#include "tkrec/TKtrhit.h"

#include "tkrec/TKOMhit.h"
#include "tkrec/TKtrack.h"

#include <datatools/exception.h>

//ClassImp(tkrec::TKtrhit);

namespace tkrec {

  // dimensions in mm
  // origin in the center of detector
  static double foil_spacex = 62.0; // falaise value. (originally set to 58)
  const double tc_radius = 22.0; // in mm
  const double tc_sizez = 2770.0; // in mm

  const double TKtrhit::default_sigma_R = 2.0; // in mm
  const double TKtrhit::default_sigma_Z = 17.0; // in mm
  const double TKtrhit::default_h = 0.0; // in mm
  const double TKtrhit::default_r = 22.0; // in mm

  void TKtrhit::set_SRL_xy()
  {
    SRL[0] =  cell_num / 1017; 	// compute side
    SRL[1] = (cell_num % 1017)/ 9; // compute row
    SRL[2] =  cell_num % 9; 	// compute layer
    xy[0] = (2.0*SRL[0]-1.0) * (SRL[2]*2.0*tc_radius + tc_radius + foil_spacex/2.0);
    xy[1] = (SRL[1]-56.0) * 2.0 * tc_radius;
    return;
  }

  TKtrhit::TKtrhit(int _cell_num)
    : TKtrhit()
  {
    DT_THROW_IF(_cell_num > 2033 || _cell_num < 0, std::logic_error,
		"Not valid tracker cell number " << _cell_num); 
    cell_num = _cell_num;
    set_SRL_xy();	
    return;
  }

  TKtrhit::TKtrhit(int _SRL[3])
    : TKtrhit()
  {
    cell_num = 113*9*_SRL[0] + 9*_SRL[1] + _SRL[2];
    DT_THROW_IF(cell_num > 2033 || cell_num < 0, std::logic_error,
		"Not valid tracker cell SRL combination (" << _SRL[0] << ", " << _SRL[1] << ", " << _SRL[2] << ")"); 
    // MIRO: PREHODNOTIT ROZDELENIE SRL A XY
    set_SRL_xy();
    return;
  }

  TKtrhit::TKtrhit(int _cell_num, int64_t _tsp[7])
    : TKtrhit(_cell_num)
  {
    set_tsp(_tsp);
    return;
  }

  TKtrhit::TKtrhit(int _SRL[3], int64_t _tsp[7])
    : TKtrhit(_SRL)
  {
    // MIRO: PREHODNOTIT ROZDELENIE SRL A XY
    set_tsp(_tsp);
   return;
  }

  void TKtrhit::set_cell_num(int _cell_num)
  {
    cell_num = _cell_num;
    return;
  }

  void TKtrhit::set_tsp(int64_t _tsp[7]) // sets timestamps
  {
    for (int i = 0; i < 7; i++)
      {
	tsp[i] = _tsp[i];
      }
    return;
  }

  void TKtrhit::set_r(double _r)
  {
    r = _r;
    return;
  }

  void TKtrhit::set_sigma_R(double _sigma_R)
  {
    sigma_R = _sigma_R;
    return;
  }

  void TKtrhit::set_sigma_R()
  {
    if(r != -1)
      {
	// TODO: possible to implement some uncertainty model
	set_sigma_R(2.0);
      }	
    return;
  }

  void TKtrhit::set_h(double _h)
  {
    h = _h;
    return;
   }

  void TKtrhit::set_sigma_Z(double _sigma_Z)
  {
    sigma_Z = _sigma_Z;
    return;
   }

  void TKtrhit::set_sigma_Z()
  {
    // TODO: possible to implement some uncertainty model
    set_sigma_Z(17.0);
    return;
   }


  void TKtrhit::set_h()
  {
    // TODO: possible to implement better model
    if (tsp[0] != -1 && tsp[5] != -1 && tsp[6] != -1)
      {
	h = tc_sizez * ( double(tsp[5] - tsp[0]) / double(tsp[5] + tsp[6] - 2*tsp[0]) ) - tc_sizez/2.0;
      }
    return;
   }

  void TKtrhit::set_associated_OMhit(const ConstTKOMhitHdl & _associated_OMhit)
  {
    associated_OMhit = _associated_OMhit;
    return;
  }

  const ConstTKOMhitHdl & TKtrhit::get_associated_OMhit() const
  {
    return associated_OMhit;
  }

  void TKtrhit::set_associated_track(const ConstTKtrackHdl &_associated_track)
  {
    associated_track = _associated_track;
    return;
   }

  const ConstTKtrackHdl & TKtrhit::get_associated_track() const
  {
    return associated_track;
  }

  bool TKtrhit::has_associated_track() const
  {
    return associated_track.get() != nullptr;
  }

  bool TKtrhit::has_associated_OMhit() const
  {
    return associated_OMhit.get() != nullptr;
  }

  int TKtrhit::get_cell_num() const
  {
    return cell_num;
  }

  int TKtrhit::get_SRL(char _SRL_n) const
  {	
    switch (_SRL_n)
      {
      case 's':	return SRL[0];
	break;
      case 'S':	return SRL[0];
	break;
      case 'r':	return SRL[1];
	break;
      case 'R':	return SRL[1];
	break;
      case 'l':	return SRL[2];
	break;
      case 'L':	return SRL[2];
	break;
      default:	std::cout << "ERROR in int TKtrhit::get_SRL(char _SRL_n): " << _SRL_n << " is not a valid argument value! " << std::endl;  
	return -1;
      }
  }

  double TKtrhit::get_xy(char _xy_n) const
  {	
    switch (_xy_n)
      {
      case 'x':	return xy[0];
	break;
      case 'X':	return xy[0];
	break;
      case 'y':	return xy[1];
	break;
      case 'Y':	return xy[1];
	break;
      default:	std::cout << "ERROR in double TKtrhit::get_xy(char): " << _xy_n << " is not a valid argument value! " << std::endl;  
	return -1;
      }
  }

  int64_t TKtrhit::get_tsp (char _tsp_n) const // returns timestamp
  {
    switch (_tsp_n)
      {
      case '0':	return tsp[0];
	break;
      case '1':	return tsp[1];
	break;
      case '2':	return tsp[2];
	break;
      case '3':	return tsp[3];
	break;
      case '4':	return tsp[4];
	break;
      case 'b':	return tsp[5];
	break;
      case 'B':	return tsp[5];
	break;
      case 't':	return tsp[6];
	break;
      case 'T':	return tsp[6];
	break;
      default:	std::cout << "ERROR in int64_t TKtrhit::get_tsp (char): " << _tsp_n << " is not a valid argument value! " << std::endl;  
	return -1;
      }
  }

  double TKtrhit::get_r() const
  {
    return r;
  }

  double TKtrhit::get_sigma_R() const
  {
    return sigma_R;
  }

  double TKtrhit::get_h() const
  {
    return h;
  }

  double TKtrhit::get_sigma_Z() const
  {
    return sigma_Z;
  }

  void TKtrhit::print(std::ostream & out_) const
  {
    out_ << "	SRL: " << SRL[0] << "." << SRL[1] << "." << SRL[2] << std::endl
	 << "	h = " << h << " mm"
	 << ", r = " << r << " mm" << std::endl;
    if( associated_OMhit != nullptr )
      {
	out_ << "	associated OM hit: " << associated_OMhit->get_SWCR('s') << "." << associated_OMhit->get_SWCR('w') << "." << associated_OMhit->get_SWCR('c') << "." << associated_OMhit->get_SWCR('r') << std::endl << std::endl;
      }
    return;
  }

} //  end of namespace tkrec
