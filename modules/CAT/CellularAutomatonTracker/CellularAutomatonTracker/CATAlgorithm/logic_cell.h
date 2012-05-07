/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ILOGICCELL
#define __CATAlgorithm__ILOGICCELL
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/hit.h>
#include <mybhep/system_of_units.h>
#include <CATAlgorithm/tracking_object.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>

namespace CAT {
namespace topology{

using namespace std;
using namespace mybhep;

  class logic_cell : public tracking_object{

    // a logic_cell is composed of an id

  private:
    // id
    size_t id_;

  public:   

    //!Default constructor 
    logic_cell()
    {
      id_ = default_integer;
   }

    //!Default destructor
    virtual ~logic_cell(){};

    //! constructor
    logic_cell(size_t id){
      id_ = id;
    }

    //! set id
    void set_id(size_t id)
      {
        id_ = id;
      }

    //!get id
    const size_t& id() const {return id_;} 


  };
}
}

#endif
