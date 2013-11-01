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

#include <CATAlgorithm/Cell.h>
#include <CATAlgorithm/Circle.h>

namespace CAT {
  namespace topology{

    //class Detector : public TObject {
    class Detector {

    private:

      std::map<size_t, Cell> cells_;
      std::vector<Cell> leftover_cells_;
      double x0_;
      double y0_;
      double Lx_;
      double Ly_;
      double cell_size_;

    public:
      Detector();
      virtual ~Detector();

      void set_cells(std::vector<Cell> a);
      void set_leftover_cells(std::vector<Cell> a){leftover_cells_ = a;}
      void set_x0(double a){x0_ = a;}
      void set_y0(double a){y0_ = a;}
      void set_Lx(double a){Lx_ = a;}
      void set_Ly(double a){Ly_ = a;}
      void set_cell_size(double a){cell_size_ = a;}

      std::vector<Cell> cells();

      std::vector<Cell> leftover_cells(){return leftover_cells_;}
      double x0(){return x0_;}
      double y0(){return y0_;}
      double Lx(){return Lx_;}
      double Ly(){return Ly_;}
      double cell_size(){return cell_size_;}

      void build_cells();
      void draw();
      bool detect(Circle h, double sigma);
      std::vector<Cell> cells(int itrack);
      double average_x();
      double average_y();
      void draw_surfaces_rough(Circle *h, bool draw, std::vector<Cell> cs, int ievent);
      void draw_surfaces_precise(Circle rough, Circle *h, bool draw, std::vector<Cell> cs, int ievent);
      void assign_reco_points_based_on_circle(std::vector<Cell> cluster, Circle h, size_t itrack);

      void reset();

      //   ClassDef(Detector,1)  //Detector structure
    };
  }
}

#endif
