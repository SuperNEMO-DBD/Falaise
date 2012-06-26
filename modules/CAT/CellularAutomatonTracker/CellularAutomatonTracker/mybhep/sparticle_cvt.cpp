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

#include <mybhep/sparticle_cvt.h>
#include <mybhep/particle_cvt.h>
#include <mybhep/mybhep_svc.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>

namespace mybhep{

  using namespace std;

  // FIXME label consistency!
  sparticle_cvt::sparticle_cvt( const sparticle& trk, int index )
    : ibconverter()
  {
    set_index( index );
    set_name( "__sparticle__" );
    set_owner(false);
    sparticle_ = &trk;
    nsparticle_ = NULL;
    particle_label_ = "__particle__";

  }

  sparticle_cvt::sparticle_cvt( std::string def ) : ibconverter()
  {
    particle_label_ = "__particle__";
    restore( def );

  }

  sparticle_cvt::~sparticle_cvt()
  {
    if (owner())
      delete nsparticle_;
  }

  // serialize object
  std::string sparticle_cvt::store()
  {

    std::string tmp;

    tmp =name();
    to_string(tmp,index());

    const std::vector<double> qual = sparticle_->quality() ;

    to_string(tmp,qual.size());
    for(size_t i =0; i < qual.size(); i++)
      {
        double  q= qual[i];

        std::string tmp2 = to_string(q);
        to_string(tmp,tmp2);

      }

    // indexes of objects in class

    to_string(tmp,(long int) &sparticle_->seed_particle());

    const std::vector<const particle*>&  subp = sparticle_->subparticles();
    to_string(tmp,subp.size());

    for(size_t idau = 0; idau < subp.size(); idau++)
      {
        const particle& pd = *subp[idau];
        to_string(tmp,(long int) &pd);
      }

    // write inside particle info
    const particle& ip =  sparticle_->seed_particle();
    int it = (long int) (&ip);
    particle_cvt pcv(ip,it);
    std::string st = pcv.store();
    to_string(tmp,st);


    //    clog << "this is the std::string stored in sparticle ",
    // << tmp << std::endl ;

    return tmp;
  }


void  sparticle_cvt::restore( std::string def )
{
#ifndef HAVE_SSTREAM
    istrstream istr (def.c_str());
#else
    istringstream istr(def.c_str());
#endif


    // get converter service

    converter_svc& csvc = mybhep_svc::instance().
      converter_service();

    std::string name;
    istr >> name ;

    int indx;
    istr >> indx ;

    set_name( name );
    set_index( indx );

    //create new sparticle
    nsparticle_ = new sparticle();
    set_owner(true);

    size_t qual_size;
    istr >> qual_size ;

    for(size_t iqual =0; iqual < qual_size; iqual++)
      {

        // read quality and set to sparticle
        double q;
        istr >> q;
        nsparticle_->add_quality(q);
      }


    // read index of seed particle

    istr >> iseed_;


    // and subparticles
    size_t ndau;
    istr >> ndau;

    for(size_t idau = 0; idau < ndau; idau++)
      {
        int jdau;
        istr >> jdau;
        isubp_.push_back(jdau);
      }


    // restore seed particle

    strip(def,def.find(particle_label_));

    // create a particle converter
    // invoke restore method
    // register to converter service

    particle_cvt*  pcv = new particle_cvt(def);
    csvc.add_converter(pcv);
    strip(def,particle_label_.size());

}

  void sparticle_cvt::complete_restore()
  {
    // get converter service

    converter_svc& csvc = mybhep_svc::instance().
      converter_service();

    // restore pointer to seed particle

    try{
      particle_cvt& pcvt = *(dynamic_cast<particle_cvt*>
                             (&csvc.converter(iseed_)));

      particle& pp = pcvt.reference();
      nsparticle_->set_seed_particle(pp);
    }
    catch(bad_index&)
      {
        std::cerr <<"could not resolver pointer to sparticle seed particle"
             << endl;
        std::cerr <<"event is likely corrupted" << endl;
        throw;
      }


    for(size_t ipar =0; ipar < isubp_.size(); ipar++)
      {
        int index = isubp_[ipar];
        try{
          particle_cvt& pcvt = *(dynamic_cast<particle_cvt*>
                                 (&csvc.converter(index)));


          particle& pp = pcvt.reference();
          nsparticle_->add_particle(pp);
        }
        catch (bad_index&)
          {
            //      std::cerr << " could not resolve pointer to subparticle" << endl;
          }
      }
  }

  const sparticle&  sparticle_cvt::retrieve() const
  {

    return *sparticle_;
  }

  sparticle&  sparticle_cvt::reference()
  {
    return *nsparticle_;
  }


  sparticle* sparticle_cvt::create()
  {

    Assert(nsparticle_, __FILE__,__LINE__,
           internal_logic("  null pointer!!!"));

    set_owner(false);
    return nsparticle_;

  }

}


