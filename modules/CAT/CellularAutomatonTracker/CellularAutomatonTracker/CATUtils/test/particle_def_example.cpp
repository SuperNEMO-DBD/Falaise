/* -*- mode: c++ -*- */
/*
 */

#include <mybhep/particle_definition.h>
#include <mybhep/ray.h>
#include <mybhep/material.h>
#include <mybhep/particle.h>
#include <mybhep/track.h>
#include <mybhep/system_of_units.h>

using namespace std;
using namespace mybhep;

/** Example of use of particle definition */

int main(int, char* []) {
  particle_definition electron("e-");

  cout << "Electron definition " << electron << endl;

  cout << "++++++++++++++++++++++++++" << endl;
  cout << "name  = " << electron.name() << endl;
  cout << "mass (MeV)  = " << electron.mass() / MeV << endl;
  cout << "charge  = " << electron.charge() / MeV << endl;
  cout << "pdg code  = " << electron.pdg() / MeV << endl;
  cout << "lieftime (infinite if negative)  = " << electron.life_time() / second << endl;

  int eplus_g3 = 2;
  particle_definition positron(eplus_g3);

  cout << "Positron definition " << positron << endl;

  cout << "++++++++++++++++++++++++++" << endl;
  cout << "name  = " << positron.name() << endl;
  cout << "mass (MeV)  = " << positron.mass() / MeV << endl;
  cout << "charge  = " << positron.charge() / MeV << endl;
  cout << "pdg code  = " << positron.pdg() / MeV << endl;
  cout << "lieftime (infinite if negative)  = " << positron.life_time() / second << endl;

  Point3D x(0.1, 1, 10);
  Vector3D p(-5, 5, 100);

  ray* r = new ray(x, p);
  cout << "ray definition " << *r << endl;

  particle* eminus = new particle(TRUTH, "e-", *r);
  cout << " e- = " << *eminus << endl;

  material* iron = new material("Fe", 7.87 * g / cm3, 1.76 * cm, 131.9 * g / cm2);
  cout << "properties of iron" << iron << endl;

  particle* gamma = new particle(TRUTH, "gamma", *r);
  gamma->set_mother(*eminus);

  eminus->add_daughter(*gamma);

  cout << " gamma = " << *gamma << endl;
  cout << " eminus = " << *eminus << endl;

  track t_eminus(*eminus, iron);

  cout << " track t_eminus = " << t_eminus << endl;

  t_eminus.add_ray(r);

  cout << " track t_eminus = " << t_eminus << endl;

  Point3D x2(0.2, 2, 20);
  Vector3D p2(-4.5, 4.5, 90);

  ray* r2 = new ray(x2, p2);
  cout << "ray definition " << *r2 << endl;

  t_eminus.add_ray(r2);

  Point3D x3(0.3, 3, 30);
  Vector3D p3(-4.0, 4.0, 80);

  ray* r3 = new ray(x3, p3);
  cout << "ray definition " << *r3 << endl;

  t_eminus.add_ray(r3);

  cout << " track t_eminus = " << t_eminus << endl;

  eminus->add_track(&t_eminus);

  cout << " eminus = " << *eminus << endl;
  return 0;
}
