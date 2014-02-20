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
// #ifndef HAVE_SSTREAM
// //#include <strstream.h>
// #include <strstream>
// #else
#include <sstream>
//#endif

#include <mybhep/mparticle_cvt.h>
#include <mybhep/particle_cvt.h>
#include <mybhep/mybhep_svc.h>

#include <mybhep/error.h>
#include <mybhep/utilities.h>

namespace mybhep{

  // using namespace std;

  // FIXME label consistency!
  mparticle_cvt::mparticle_cvt( const mparticle& par, int index )
    : ibconverter()
  {
    set_index( index );
    set_name( "__mirror__" );
    set_owner(false);
    particle_ = &par;
    nparticle_ = NULL;

  }

  mparticle_cvt::mparticle_cvt( std::string def ) : ibconverter()
  {
    restore( def );

  }

  mparticle_cvt::~mparticle_cvt()
  {
    if (owner())
      delete nparticle_;
  }

    // serialize object
  std::string mparticle_cvt::store()
  {

    std::string tmp;

    tmp =name();
    to_string(tmp,index());
    to_string(tmp,particle_->view());
    to_string(tmp,particle_->xz());
    to_string(tmp,particle_->yz());
    to_string(tmp,particle_->quality());

    const std::map<std::string,std::string>& dmap = particle_->properties_map();

    to_string(tmp,dmap.size());

    typedef std::map<std::string, std::string>::const_iterator I;
    for(I i=dmap.begin(); i !=dmap.end(); ++i)
      {

        std::string tmp2 = to_string(i->first)+ " " +
          to_string(i->second);
        to_string(tmp,tmp2);
      }


    // indexes of objects in class
    if (&particle_->mirror() != NULL)
      to_string(tmp,(long int) &particle_->mirror());
    else
      to_string(tmp,-1);


    return tmp;
  }


  void  mparticle_cvt::restore( std::string def )
  {

// #ifndef HAVE_SSTREAM
//     istrstream istr (def.c_str());
// #else
    std::istringstream istr(def.c_str());
// #endif


    std::string name;
    istr >> name ;

    int indx;
    istr >> indx ;

    set_index( indx );
    set_name( name );

    //create new particle
    nparticle_ = new mparticle();
    set_owner(true);

    // read view
    std::string view;

    istr >> view;
    nparticle_->set_view(view);

    // read xz,yz,quality and set to mparticle
    size_t xz,yz;
    double qual;

    istr >> xz >> yz >> qual;

    nparticle_->set_xz(xz);
    nparticle_->set_yz(yz);
    nparticle_->set_quality(qual);

    size_t ndata;
    istr >> ndata;

    for(size_t i=0; i < ndata; ++i)
      {
        std::string name;
        std::string value;
        istr >> name >> value;
        nparticle_->add_property(name,value);
      }


    // indexes of objects in class
    istr >> imirror_;

  }

  void mparticle_cvt::complete_restore()
  {
    // get converter service

    converter_svc& csvc = mybhep_svc::instance().
      converter_service();


    try
      {
        particle_cvt& pcvt = *(dynamic_cast<particle_cvt*>
                               (&csvc.converter(imirror_)));

        particle& pp = pcvt.reference();
        nparticle_->set_mirror(pp);
      }
    catch (bad_index&)
      {
        std::cerr << " could not resolve pointer to mirror particle " << std::endl;
        std::cerr << "event is likely corrupted" << std::endl;
        throw;
      }
  }

  const mparticle&  mparticle_cvt::retrieve() const
  {
    return *particle_;
  }

  mparticle&  mparticle_cvt::reference()
  {
    return *nparticle_;
  }

  mparticle* mparticle_cvt::create()
  {
    Assert(nparticle_, __FILE__,__LINE__,
           internal_logic("  null pointer!!!"));

    set_owner(false);
    return nparticle_;
  }
}
