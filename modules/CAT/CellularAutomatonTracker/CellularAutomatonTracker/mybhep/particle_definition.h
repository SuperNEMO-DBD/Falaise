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




#ifndef particle_definition_h
#define particle_definition_h 1

#include <vector>
#include <string>
#include <mybhep/bproperties.h>
#include <mybhep/system_of_units.h>

using namespace std;


namespace mybhep{

  //! Particle numbering schemes
  //! PDG = pdg id code, GEANT3 = Geant3 id code
  enum idcode{PDG=0, GEANT3=1};

    
  //! Contains static data of a particle.
  class particle_definition : public bproperties {

  public:
  

    //! constructor
    particle_definition(string name);
    particle_definition(int code, idcode scheme=GEANT3);
    particle_definition(string name, int pdg,
			double m, 
			double q, 
			double l=-1);

    //! virtual destructor, safe to inherit
    virtual ~particle_definition()
    {
    }
      
    
  public: 
    //! name of the particle
    /*!
      Particles defined: e+,e-,mu+,mu-,tau+,tau-,nu_e,nu_mu
      nu_tau, anti_nu_e,anti_nu_mu,anti_nu_tau,gamma,pi+,pi-,pi0,
      kaon+,kaon-,kaon0L,kaon0,proton,anti_proton,neutron
     */
    const string& name() const { return name_; }
    //! mass of particle
    double mass() const;
    //! charge of particle
    double charge() const { return charge_; }
    //! pdg code
    int pdg() const { return pdg_; }
    //! geant3 code
    int  geant3() const;
    //! liftime
    double life_time() const { return lifetime_; }

    //! sets the parameters of the particle
    // a stable particle has a negative lifetime
    void set_parameters(double m, double q, double l);

    //! sets only the charge of the particle
    void set_charge(double q);
    
    //! set name
    void set_name(string name);
    
  private:
    
    //  !!!  can not use "copy constructor" 
       particle_definition(const particle_definition &right);
    //  !!!  Assignment operation is forbidden !!!
    const particle_definition & operator=(const particle_definition &right);
    
    void set_particle_name(string name) ;

    void geant3(int code);
    void pdg(int code);

  protected:

    string name_;
    double mass_;
    double charge_;
    int pdg_;
    double lifetime_;
  };

  ostream& operator << (ostream& s, const particle_definition& p); 
}
#endif
