
#include "RVersion.h"
#include "TDirectory.h"
#include "TProcessID.h"
#include "TRandom.h"
#include "TEllipse.h"

#include <ctime>
#include <time.h>
#include <iostream>

#include <CATAlgorithm/Circle.h>


namespace CAT{
  namespace topology{

    using namespace std;

    double Circle::phi(double x, double y){
      return atan2(y - center().y().value(), x - center().x().value());
    }

    void Circle::draw(Color_t color){

      TEllipse *t = new TEllipse(this->center().x().value(),
				 this->center().y().value(),
				 this->radius().value(),
				 this->radius().value());
      t->SetLineColor(color);
      t->SetFillColor(4000);
      t->Draw("same");

  
    }

    void Circle::build_random(double x0, double y0, double Lx, double Ly){

      double diagonal = sqrt(pow(Lx,2) + pow(Ly,2));

      //sleep(1);
      double seed = (double)clock();

      TRandom *random = new TRandom((UInt_t)seed);

      double circle_x0 = random->Gaus(x0, Lx);
      double circle_y0 = random->Gaus(y0, Ly);
      double circle_r = fabs(random->Gaus(diagonal,diagonal));;
      this->set_center(experimental_point(experimental_double(circle_x0,0.), experimental_double(circle_y0,0.), experimental_double(0.,0.)));
      this->set_radius(experimental_double(circle_r, 0.));

    }
  }
}
