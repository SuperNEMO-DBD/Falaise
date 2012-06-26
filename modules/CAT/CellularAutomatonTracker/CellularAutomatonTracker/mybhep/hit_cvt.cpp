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

#include <mybhep/hit_cvt.h>
#include <mybhep/particle_cvt.h>
#include <mybhep/mybhep_svc.h>

namespace mybhep{

  using namespace std;

  // FIXME label consistency!
  hit_cvt::hit_cvt( const hit& ht, int index )
    : ibconverter()
  {
    set_index( index );
    set_name( "__hit__" );
    set_owner(false);
    hit_ = &ht;
    nhit_ = NULL;
  }

  hit_cvt::hit_cvt( std::string def ) : ibconverter()
  {
    restore( def );
  }

  hit_cvt::~hit_cvt()
  {
    if (owner())
      delete nhit_;
  }

  // serialize object
  std::string hit_cvt::store()
  {

    string tmp;


    tmp =name();
    to_string(tmp,index());
    to_string(tmp,hit_->detector());

    string tmp2 = to_string(hit_->x()[0])+ " " +
      to_string(hit_->x()[1])+ " " + to_string(hit_->x()[2]);
    to_string(tmp,tmp2);


    const map<string,string>& dmap = hit_->data_map();

    to_string(tmp,dmap.size());

    typedef map<string, string>::const_iterator I;
    for(I i=dmap.begin(); i !=dmap.end(); ++i)
      {
        tmp2 = to_string(i->first)+ " " +
          to_string(i->second);
        to_string(tmp,tmp2);

      }


    to_string(tmp,(long int) &hit_->mother_particle());
    //    clog << "this is the string stored in hit= " << tmp << endl;
    return tmp;
  }


void  hit_cvt::restore( string def )
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

    set_name( name );
    set_index( indx );

    //name

    string det_name;
    istr >> det_name ;

    //create new hit
    nhit_ = new hit(det_name);
    set_owner(true);

    double x,y,z;
    istr >> x >> y >> z;
    nhit_->set_point(mybhep::Point3D(x,y,z));

    size_t ndata;
    istr >> ndata;

    for(size_t i=0; i < ndata; ++i)
      {

        string name;
        string value;

        istr >> name >> value;
        nhit_->add_data(name,value);
      }

    istr >> imother_;
}

  void hit_cvt::complete_restore()
  {
    // get converter service

    converter_svc& csvc = mybhep_svc::instance().
      converter_service();

    // restore pointer to mother particle
    try{
      particle_cvt& pcvt = *(dynamic_cast<particle_cvt*>
                             (&csvc.converter(imother_)));

      particle& pp = pcvt.reference();
      nhit_->set_mother_particle(pp);
    }
    catch(bad_index&)
      {
        cerr << "could not resolver pointer to hit mother particle"
             << endl;
        cerr << "event is likely corrupted"
             << endl;

      }

  }

  const hit&  hit_cvt::retrieve() const
  {

    return *hit_;
  }

  hit&  hit_cvt::reference()
  {
    return *nhit_;
  }


  hit* hit_cvt::create()
  {
    Assert(nhit_, __FILE__,__LINE__,
           internal_logic("  null pointer!!!"));

    set_owner(false);
    return nhit_;

  }

}


