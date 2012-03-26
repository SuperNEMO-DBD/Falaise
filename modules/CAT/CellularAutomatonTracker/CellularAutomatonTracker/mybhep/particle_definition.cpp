/* -*- mode: c++ -*- */
// 
/*   
 * 
 * Copyright (C) 2004 J.J. Gomez-Cadenas
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

#include <mybhep/particle_definition.h>
#include <iostream>

namespace mybhep{

  particle_definition::particle_definition(string name, int pdg, 
					   double m, double q, double l)
  {
    name_ = name;     
    pdg_ = pdg;     
    mass_ = m;     
    charge_ = q;   
    lifetime_ =l;
  }

  particle_definition::particle_definition(int code, idcode scheme)
  {
    if(scheme == GEANT3)
      geant3(code);
    else
      pdg(code);
  }

  void particle_definition::geant3(int code)
  {
    string name = "unknown";
    
    if (code == 1) 
      name = "gamma";
    else if (code == 2) 
      name = "e+";
    else if (code == 3) 
      name = "e-";
    else if (code == 5) 
      name = "mu+";
    else if (code == 6) 
      name = "mu-";
    else if (code == 4) 
      name = "nu_mu";
    else if (code == 8) 
      name = "pi+";
    else if (code == 9) 
      name = "pi-";
    else if (code == 7) 
      name = "pi0";
    else if (code == 11) 
      name = "kaon+";
    else if (code == 12) 
      name = "kaon-";
    else if (code == 10) 
      name = "kaon0L";
    else if (code == 16) 
      name = "kaon0";
    else if (code == 14) 
      name = "proton";
    else if (code == 15) 
      name = "anti_proton";
    else if (code == 13) 
      name = "neutron";
    else if (code == 25) 
      name = "anti_neutron";
    else if (code == 45) 
      name = "deuteron";
    else if (code == 46) 
      name = "tritium";
    else if (code == 47) 
      name = "alpha";
    
    set_particle_name(name);
    
  }
  
  void particle_definition::pdg(int code) 
  {
    string name = "unknown";
    
    if (code == 22) 
      name = "gamma";
    else if (code == -11) 
      name = "e+";
    else if (code == 11) 
      name = "e-";
    else if (code == -13) 
      name = "mu+";
    else if (code == 13) 
      name = "mu-";
    else if (code == 14) 
      name = "nu_mu";
    else if (code == 211) 
      name = "pi+";
    else if (code == -211) 
      name = "pi-";
    else if (code == 111) 
      name = "pi0";
    else if (code == 321) 
      name = "kaon+";
    else if (code == -321) 
      name = "kaon-";
    else if (code == 130) 
      name = "kaon0L";
    else if (code == 311) 
      name = "kaon0";
    else if (code == 2212) 
      name = "proton";
    else if (code == -2212) 
      name = "anti_proton";
    else if (code == 2112) 
      name = "neutron";
    else if (code == -2112) 
      name = "anti_neutron";
    
    set_particle_name(name);
  }
  
  particle_definition::particle_definition(string name) 
  {
    set_particle_name(name);
  }
  
  void particle_definition::set_particle_name(string name) 
  {
      name_ = name;

    if(name == "gamma"){
      pdg_ = 22;      
      set_parameters(0,0,-1);
    }
    else if (name == "e+"){
      set_parameters(5.10998902E-04*GeV,1,-1);
      pdg_  = -11;
    }
    else if (name == "e-"){
      set_parameters(5.10998902E-04*GeV,-1,-1);
      pdg_  = 11;
    }
    else if (name == "mu-"){
      set_parameters(1.05658357E-01*GeV,-1,2.197e-6*second);
      pdg_  = 13;
    }
    else if (name == "mu+"){
      set_parameters(1.05658357E-01*GeV,1,2.197e-6*second);
      pdg_  = -13;
    }
    else if (name == "tau+"){
      set_parameters(1.77699*GeV,1,291e-15*second);
      pdg_  = -15;
    }
    else if (name == "tau-"){
      set_parameters(1.77699*GeV,-1,2.291e-15*second);
      pdg_  = 15;
    }
    else if (name == "nu_e"){
      set_parameters(0,0,-1);
      pdg_  = 12;
    }
    else if (name == "anti_nu_e"){
      set_parameters(0,0,-1);
      pdg_  = -12;
    }
    else if (name == "nu_mu"){
      set_parameters(0,0,-1);
      pdg_  = 14;
    }
    else if (name == "anti_nu_mu"){
      set_parameters(0,0,-1);
      pdg_  = -14;
    }
    else if (name == "nu_tau"){
      set_parameters(0,0,-1);
      pdg_  = 16;
    }
    else if (name == "anti_nu_tau"){
      set_parameters(0,0,-1);
      pdg_  = -16;
    }
    else if (name == "pi+"){
      set_parameters(1.3957018E-01*GeV,1,2.603e-8*second);
      pdg_  = 211;
    }
    else if (name == "pi-"){
      set_parameters(1.3957018E-01*GeV,-1,2.603e-8*second);
      pdg_  = -211;
    }
    else if (name == "pi0"){
      set_parameters(1.349766E-01*GeV,0,8.4e-17*second);
      pdg_  = 111;
    }
    else if (name == "kaon+"){
      set_parameters(4.93677E-01*GeV,1,1.2386e-8*second);
      pdg_  = 321;
    }
    else if (name == "kaon-"){
      set_parameters(4.93677E-01*GeV,-1,1.2386e-8*second);
      pdg_  = -321;
    }
    else if (name == "kaon0L"){
      set_parameters(4.97672E-01*GeV,0,5.17e-8*second);
      pdg_  = 130;
    }
    else if (name == "kaon0"){
      set_parameters(4.97672E-01*GeV,0,0.89e-10*second);
      pdg_  = 310;
    }
    else if (name == "proton"){
      set_parameters(9.3827200E-01*GeV,1,-1);
      pdg_  = 2212;
    }
    else if (name == "anti_proton"){
      set_parameters(9.3827200E-01*GeV,-1,-1);
      pdg_  = -2212;
    }
    else if (name == "neutron"){
      set_parameters(9.3956533E-01*GeV,0,-1);
      pdg_  = 2112;
    }
    else if (name == "anti_neutron"){
      set_parameters(9.3956533E-01*GeV,0,-1);
      pdg_  = -2112;
    }
    else if (name == "deuteron"){
      set_parameters(1.875613*GeV,0,-1);
      pdg_  = 0;
    }
    else if (name == "tritium"){
      set_parameters(2.80925*GeV,0,-1);
      pdg_  = 0;
    }
    else if (name == "alpha"){
      set_parameters(3.727*GeV,0,-1);
      pdg_  = 0;
    }
    else if (name == "unknown"){ // assume pion mass
      set_parameters(1.3957018E-01*GeV,-1,2.603e-8*second);
      pdg_  = 0;
    }    
    else if (name == "void"){
      set_parameters(0,0,0);
      pdg_  = 0;
    }    
    else {
      // Assume a pion
      set_parameters(1.3957018E-01*GeV,1,2.603e-8*second);
      pdg_  = 211;
    }
    
  }

  double particle_definition::mass() const { 
    return mass_; }  

  void particle_definition::set_parameters(double m, double q, double l)
  {
    mass_ = m;     
    charge_ = q;   
    lifetime_ =l;
  }
  
  void particle_definition::set_charge(double q)
  {
    charge_ = q;
  }
    
  void particle_definition::set_name(string name)
  {
    set_particle_name(name);
  }

  int  particle_definition::geant3() const
  {
    int geant3 = 0;
    
    if(name_ == "gamma") geant3 = 1;
    if(name_ == "e+") geant3 = 2;
    if(name_ == "e-") geant3 = 3;
    if(name_ == "mu+") geant3 = 5;
    if(name_ == "mu-") geant3 = 6;
    if(name_ == "nu_mu") geant3 = 4;
    if(name_ == "pi+") geant3 = 8;
    if(name_ == "pi-") geant3 = 9;
    if(name_ == "pi0") geant3 = 7;
    if(name_ == "kaon+") geant3 = 11;
    if(name_ == "kaon-") geant3 = 12;
    if(name_ == "kaon0L") geant3 = 10;
    if(name_ == "kaon0") geant3 = 16;
    if(name_ == "proton") geant3 = 14;
    if(name_ == "anti_proton") geant3 = 15;
    if(name_ == "neutron") geant3 = 13;
    if(name_ == "anti_neutron") geant3 = 25;
    if(name_ == "deuteron") geant3 = 45;
    if(name_ == "tritium") geant3 = 46;
    if(name_ == "alpha") geant3 = 47;
    
    return geant3;
  }

 
  ostream& operator << (ostream& s, const particle_definition& p) {
    s << endl;
 
    s << " particle name= " << p.name() << " "
      << " particle mass= (MeV)" << p.mass()/MeV << " "
      << " particle charge= " << p.charge() << " "
      << " particle lifetime (mus) = " << p.life_time()/microsecond << " "
      << " particle pdg= " << p.pdg() 
      << " particle geant3= " << p.geant3() 
      << endl;

    return s;
  }
}
