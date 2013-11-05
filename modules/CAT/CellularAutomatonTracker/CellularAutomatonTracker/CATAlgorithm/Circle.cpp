
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
      // angle of point (x, y) wrt center of circle

      double dx = x - center().x().value();
      double dy = y - center().y().value();
      return atan2(dy, dx);
    }

  }
}
