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

namespace mybhep {

particle_definition::particle_definition(std::string name_arg, int pdg_arg, double mass_arg,
                                         double charge_arg, double lifetime) {
  name_ = name_arg;
  pdg_ = pdg_arg;
  mass_ = mass_arg;
  charge_ = charge_arg;
  lifetime_ = lifetime;
}

particle_definition::particle_definition(int code, idcode scheme) {
  if (scheme == GEANT3)
    geant3(code);
  else
    pdg(code);
}

void particle_definition::geant3(int code) {
  std::string name_par = "unknown";

  if (code == 1)
    name_par = "gamma";
  else if (code == 2)
    name_par = "e+";
  else if (code == 3)
    name_par = "e-";
  else if (code == 5)
    name_par = "mu+";
  else if (code == 6)
    name_par = "mu-";
  else if (code == 4)
    name_par = "nu_mu";
  else if (code == 8)
    name_par = "pi+";
  else if (code == 9)
    name_par = "pi-";
  else if (code == 7)
    name_par = "pi0";
  else if (code == 11)
    name_par = "kaon+";
  else if (code == 12)
    name_par = "kaon-";
  else if (code == 10)
    name_par = "kaon0L";
  else if (code == 16)
    name_par = "kaon0";
  else if (code == 14)
    name_par = "proton";
  else if (code == 15)
    name_par = "anti_proton";
  else if (code == 13)
    name_par = "neutron";
  else if (code == 25)
    name_par = "anti_neutron";
  else if (code == 45)
    name_par = "deuteron";
  else if (code == 46)
    name_par = "tritium";
  else if (code == 47)
    name_par = "alpha";

  set_particle_name(name_par);
}

void particle_definition::pdg(int code) {
  std::string name_par = "unknown";

  if (code == 22)
    name_par = "gamma";
  else if (code == -11)
    name_par = "e+";
  else if (code == 11)
    name_par = "e-";
  else if (code == -13)
    name_par = "mu+";
  else if (code == 13)
    name_par = "mu-";
  else if (code == 14)
    name_par = "nu_mu";
  else if (code == 211)
    name_par = "pi+";
  else if (code == -211)
    name_par = "pi-";
  else if (code == 111)
    name_par = "pi0";
  else if (code == 321)
    name_par = "kaon+";
  else if (code == -321)
    name_par = "kaon-";
  else if (code == 130)
    name_par = "kaon0L";
  else if (code == 311)
    name_par = "kaon0";
  else if (code == 2212)
    name_par = "proton";
  else if (code == -2212)
    name_par = "anti_proton";
  else if (code == 2112)
    name_par = "neutron";
  else if (code == -2112)
    name_par = "anti_neutron";

  set_particle_name(name_par);
}

particle_definition::particle_definition(std::string name_par) { set_particle_name(name_par); }

void particle_definition::set_particle_name(std::string name_par) {
  name_ = name_par;

  if (name_par == "gamma") {
    pdg_ = 22;
    set_parameters(0, 0, -1);
  } else if (name_par == "e+") {
    set_parameters(5.10998902E-04 * GeV, 1, -1);
    pdg_ = -11;
  } else if (name_par == "e-") {
    set_parameters(5.10998902E-04 * GeV, -1, -1);
    pdg_ = 11;
  } else if (name_par == "mu-") {
    set_parameters(1.05658357E-01 * GeV, -1, 2.197e-6 * second);
    pdg_ = 13;
  } else if (name_par == "mu+") {
    set_parameters(1.05658357E-01 * GeV, 1, 2.197e-6 * second);
    pdg_ = -13;
  } else if (name_par == "tau+") {
    set_parameters(1.77699 * GeV, 1, 291e-15 * second);
    pdg_ = -15;
  } else if (name_par == "tau-") {
    set_parameters(1.77699 * GeV, -1, 2.291e-15 * second);
    pdg_ = 15;
  } else if (name_par == "nu_e") {
    set_parameters(0, 0, -1);
    pdg_ = 12;
  } else if (name_par == "anti_nu_e") {
    set_parameters(0, 0, -1);
    pdg_ = -12;
  } else if (name_par == "nu_mu") {
    set_parameters(0, 0, -1);
    pdg_ = 14;
  } else if (name_par == "anti_nu_mu") {
    set_parameters(0, 0, -1);
    pdg_ = -14;
  } else if (name_par == "nu_tau") {
    set_parameters(0, 0, -1);
    pdg_ = 16;
  } else if (name_par == "anti_nu_tau") {
    set_parameters(0, 0, -1);
    pdg_ = -16;
  } else if (name_par == "pi+") {
    set_parameters(1.3957018E-01 * GeV, 1, 2.603e-8 * second);
    pdg_ = 211;
  } else if (name_par == "pi-") {
    set_parameters(1.3957018E-01 * GeV, -1, 2.603e-8 * second);
    pdg_ = -211;
  } else if (name_par == "pi0") {
    set_parameters(1.349766E-01 * GeV, 0, 8.4e-17 * second);
    pdg_ = 111;
  } else if (name_par == "kaon+") {
    set_parameters(4.93677E-01 * GeV, 1, 1.2386e-8 * second);
    pdg_ = 321;
  } else if (name_par == "kaon-") {
    set_parameters(4.93677E-01 * GeV, -1, 1.2386e-8 * second);
    pdg_ = -321;
  } else if (name_par == "kaon0L") {
    set_parameters(4.97672E-01 * GeV, 0, 5.17e-8 * second);
    pdg_ = 130;
  } else if (name_par == "kaon0") {
    set_parameters(4.97672E-01 * GeV, 0, 0.89e-10 * second);
    pdg_ = 310;
  } else if (name_par == "proton") {
    set_parameters(9.3827200E-01 * GeV, 1, -1);
    pdg_ = 2212;
  } else if (name_par == "anti_proton") {
    set_parameters(9.3827200E-01 * GeV, -1, -1);
    pdg_ = -2212;
  } else if (name_par == "neutron") {
    set_parameters(9.3956533E-01 * GeV, 0, -1);
    pdg_ = 2112;
  } else if (name_par == "anti_neutron") {
    set_parameters(9.3956533E-01 * GeV, 0, -1);
    pdg_ = -2112;
  } else if (name_par == "deuteron") {
    set_parameters(1.875613 * GeV, 0, -1);
    pdg_ = 0;
  } else if (name_par == "tritium") {
    set_parameters(2.80925 * GeV, 0, -1);
    pdg_ = 0;
  } else if (name_par == "alpha") {
    set_parameters(3.727 * GeV, 0, -1);
    pdg_ = 0;
  } else if (name_par == "unknown") {  // assume pion mass
    set_parameters(1.3957018E-01 * GeV, -1, 2.603e-8 * second);
    pdg_ = 0;
  } else if (name_par == "void") {
    set_parameters(0, 0, 0);
    pdg_ = 0;
  } else {
    // Assume a pion
    set_parameters(1.3957018E-01 * GeV, 1, 2.603e-8 * second);
    pdg_ = 211;
  }
}

double particle_definition::mass() const { return mass_; }

void particle_definition::set_parameters(double mass_arg, double charge_arg, double lifetime) {
  mass_ = mass_arg;
  charge_ = charge_arg;
  lifetime_ = lifetime;
}

void particle_definition::set_charge(double q) { charge_ = q; }

void particle_definition::set_name(std::string name_arg) { set_particle_name(name_arg); }

int particle_definition::geant3() const {
  int geant3_par = 0;

  if (name_ == "gamma") geant3_par = 1;
  if (name_ == "e+") geant3_par = 2;
  if (name_ == "e-") geant3_par = 3;
  if (name_ == "mu+") geant3_par = 5;
  if (name_ == "mu-") geant3_par = 6;
  if (name_ == "nu_mu") geant3_par = 4;
  if (name_ == "pi+") geant3_par = 8;
  if (name_ == "pi-") geant3_par = 9;
  if (name_ == "pi0") geant3_par = 7;
  if (name_ == "kaon+") geant3_par = 11;
  if (name_ == "kaon-") geant3_par = 12;
  if (name_ == "kaon0L") geant3_par = 10;
  if (name_ == "kaon0") geant3_par = 16;
  if (name_ == "proton") geant3_par = 14;
  if (name_ == "anti_proton") geant3_par = 15;
  if (name_ == "neutron") geant3_par = 13;
  if (name_ == "anti_neutron") geant3_par = 25;
  if (name_ == "deuteron") geant3_par = 45;
  if (name_ == "tritium") geant3_par = 46;
  if (name_ == "alpha") geant3_par = 47;

  return geant3_par;
}

std::ostream& operator<<(std::ostream& out, const particle_definition& p) {
  out << std::endl;

  out << " particle name= " << p.name() << " "
      << " particle mass= (MeV)" << p.mass() / MeV << " "
      << " particle charge= " << p.charge() << " "
      << " particle lifetime (mus) = " << p.life_time() / microsecond << " "
      << " particle pdg= " << p.pdg() << " particle geant3= " << p.geant3() << std::endl;

  return out;
}
}  // namespace mybhep
