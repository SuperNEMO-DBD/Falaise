#ifndef ROOT_Detector
#define ROOT_Detector

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Detector                                                                //
//                                                                      //
// Description of the detector and track parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <vector>

#include "TObject.h"
#include "TH1F.h"
#include "TH3D.h"

#include <mybhep/messenger.h>
#include <CATAlgorithm/Clock.h>
#include <CATAlgorithm/Cell.h>
#include <CATAlgorithm/Circle.h>

namespace CAT {
namespace topology {

// class Detector : public TObject {
class Detector {
 private:
  // active cells in an event (mapped by id)
  std::map<size_t, Cell> cells_;

  // cells not yet assigned to a track
  std::vector<Cell> leftover_cells_;

  // geometry parameters
  double x0_;  // center of detector
  double y0_;
  double Lx_;  // size of detector
  double Ly_;
  double cell_size_;                               // distance between side-by-side cells
  double InnerRadius_, OuterRadius_, FoilRadius_;  // for Nemo3 geometry

  bool SuperNemo_;
  bool SuperNemoChannel_;

  // how many layers can be skipped
  size_t nofflayers_;

  // 3D histograms
  TH3D* surfaces_rough;
  TH3D* surfaces_precise;

 protected:
  mybhep::prlevel level;
  mybhep::messenger m;
  Clock clock;

 public:
  Detector();
  virtual ~Detector();

  void set_cells(std::vector<Cell> a);
  void set_leftover_cells(std::vector<Cell> a) { leftover_cells_ = a; }
  void set_x0(double a) { x0_ = a; }
  void set_y0(double a) { y0_ = a; }
  void set_Lx(double a) { Lx_ = a; }
  void set_Ly(double a) { Ly_ = a; }
  void set_cell_size(double a) { cell_size_ = a; }
  void set_SuperNemo(bool a) { SuperNemo_ = a; }
  void set_SuperNemoChannel(bool a) { SuperNemoChannel_ = a; }
  void set_nofflayers(size_t a) { nofflayers_ = a; }
  void set_FoilRadius(double a) { FoilRadius_ = a; }
  void set_InnerRadius(double a) { InnerRadius_ = a; }
  void set_OuterRadius(double a) { OuterRadius_ = a; }
  void set_messenger(mybhep::prlevel l);

  double x0() { return x0_; }
  double y0() { return y0_; }
  double Lx() { return Lx_; }
  double Ly() { return Ly_; }
  double cell_size() { return cell_size_; }
  bool SuperNemo() { return SuperNemo_; }
  bool SuperNemoChannel() { return SuperNemoChannel_; }
  size_t nofflayers() { return nofflayers_; }
  double FoilRadius() { return FoilRadius_; }
  double InnerRadius() { return InnerRadius_; }
  double OuterRadius() { return OuterRadius_; }

  std::vector<Cell> leftover_cells() { return leftover_cells_; }
  std::vector<Cell> cells();
  std::vector<Cell> cells(int itrack);
  void fill_surfaces_rough(std::vector<Cell>* cs);
  void fill_surfaces_precise(Circle* h, std::vector<Cell>* cs);
  void draw_surfaces(Circle* h, std::vector<Cell>* cs, size_t itrack);
  void assign_reco_points_based_on_circle(std::vector<Cell>* cluster, Circle* h,
                                          std::vector<Cell>* cs, std::vector<Cell>* ls);

  void reset();
  size_t near_level(const topology::cell& c1, const topology::cell& c2);
  bool continous(std::vector<Cell>* cs);
  void setup_histograms();
  void finalize();
};
}  // namespace topology
}  // namespace CAT

#endif
