/* -*- mode: c++ -*- */
// 
/*   
 * 
 * Copyright (C) 2004 J.J. Gomez Cadenas
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

//#include "config.h"
#ifndef HAVE_SSTREAM
//#include <strstream.h>
#include <strstream>
#else
#include <sstream>
#endif

#include <mybhep/track_cvt.h>
#include <mybhep/particle_cvt.h>
#include <mybhep/mybhep_svc.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

namespace mybhep{

  track_cvt::track_cvt( const track& trk, int index ) 
    : ibconverter()
  {
    set_index( index );
    set_name( "__track__" );
    set_owner(false);
    track_ = &trk;
    ntrack_ = NULL;
  }

  track_cvt::track_cvt( std::string def ) : ibconverter()
  {
    restore( def );
  }

  track_cvt::~track_cvt()
  {
    if (owner()) 
      delete ntrack_;
  }

  // serialize object
  std::string track_cvt::store()
  {
    
    string tmp;

    tmp =name();
    to_string(tmp,index());
    to_string(tmp,track_->view());
    to_string(tmp,track_->mate().name());

    const vector<ray*> rays = track_->rays() ;

    to_string(tmp,rays.size());
    for(size_t iray =0; iray < rays.size(); iray++)
      {
        const ray& r = *rays[iray]; 
        const mybhep::Point3D& x = r.x();
        const mybhep::Vector3D& p3 = r.p3();
        double e = r.edep();

        string tmp2 = r.view();
        to_string(tmp,tmp2);

        tmp2 = to_string(e);
        to_string(tmp,tmp2);

        tmp2 = to_string(x[0])+ " " +
          to_string(x[1])+ " " + to_string(x[2]);
        to_string(tmp,tmp2);
        
        tmp2 = to_string(p3[0])+ " " +
          to_string(p3[1])+ " " + to_string(p3[2]);
        to_string(tmp,tmp2);
        
      } 

    // indexes of objects in class
    to_string(tmp,(long int) &track_->mother_particle());

    //    clog << "this is the string stored in track= " << tmp << endl;
    return tmp;
  }


void  track_cvt::restore( string def )
{
#ifndef HAVE_SSTREAM
    istrstream istr (def.c_str());
#else
    istringstream istr(def.c_str());
#endif

    string name;
    istr >> name ;
    
    int indx;
    istr >> indx ;     
    string view;
    istr >> view ;     

    set_name( name ); 
    set_index( indx );

    //name

    string material_name;
    istr >> material_name ;

    material* mat = new material(material_name);

    //create new track
    ntrack_ = new track(mat,view);
    set_owner(true);

    size_t ray_size;
    istr >> ray_size ;  

    for(size_t iray =0; iray < ray_size; iray++)
      { 
        
        // read x,y,z of ray and set to particle
        double x,y,z,px,py,pz;
        string view;
        double ed;

        istr >> view;
        istr >> ed;
        istr >> x >> y >> z;
        istr >> px >> py >> pz;
        
        ray* r = new ray(view);
        r->set_edep(ed);
        r->set_p(px,py,pz);

        if(view == "XZ")
          r->set_point(x,z,view);
        else if(view == "YZ")
          r->set_point(y,z,view);
        else if(view == "XYZ")
          r->set_point(x,y,z);

        ntrack_->add_ray(r);
      }

    istr >> imother_;
}

  void track_cvt::complete_restore()  
  {
    // get converter service

    converter_svc& csvc = mybhep_svc::instance().
      converter_service();
    
    // restore pointer to mother particle
    try{
      particle_cvt& pcvt = *(dynamic_cast<particle_cvt*>
                             (&csvc.converter(imother_)));

      particle& pp = pcvt.reference();
      ntrack_->set_mother_particle(pp);
    }
    catch(bad_index&)
      {
//      cerr << "could not resolver pointer to track mother particle"
//           << endl;
//      cerr << "event is likely corrupted"
//           << endl;

      }
  }

  const track&  track_cvt::retrieve() const
  {

    return *track_;
  }

  track&  track_cvt::reference() 
  {
    return *ntrack_;
  }
  

  track* track_cvt::create()
  {
    Assert(ntrack_, __FILE__,__LINE__,
           internal_logic("  null pointer!!!")); 

    set_owner(false);
    return ntrack_;

  }

}


