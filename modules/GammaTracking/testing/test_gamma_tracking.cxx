// Standard libraries
#include <algorithm>
#include <random>

// This project
#include <GammaTracking/event.h>
#include <GammaTracking/tof_computing.h>
#include <GammaTracking/gamma_tracking.h>

std::random_device rd;
std::default_random_engine generator(rd());

void generate_calorimeters(gt::event & event_)
{
  const size_t total_nbr_calos = 12;
  std::uniform_int_distribution<int> distribution(0,total_nbr_calos-1);

  const size_t nbr_calos = 5;
  const size_t nbr_gammas = 1;

  // Sanity check
  if (nbr_calos * nbr_gammas > total_nbr_calos) {
    std::cerr << "Too much gammas for the given number of calorimeters ("
              << total_nbr_calos << ")" << std::endl;
    return;
  }

  for (size_t ig = 0; ig < nbr_gammas; ++ig) {
    // Initial start time
    double time = 0.0;
    for (size_t ic = 0; ic < nbr_calos; ++ic) {
      gt::event::calorimeter_collection_type & the_calos = event_.grab_calorimeters();

      while(true) {
        const size_t gid = distribution(generator);
        if (the_calos.count(gid)) {
          continue;
        }
        {
          gt::event::calorimeter_hit dummy;
          the_calos.insert(std::make_pair(gid,dummy));
        }
        auto icalo = the_calos[gid];

        const double angle = 2*M_PI/double(total_nbr_calos);
        const double radius = 100;
        icalo.position.set(radius*cos(gid*angle), radius*sin(gid*angle), 0.0);

        if (ic == 0) {
          std::uniform_real_distribution<double> urdt(0.0, 100.0);
          time += urdt(generator);
        } else {
          time += gt::tof_computing::get_track_length(icalo, *std::prev(&icalo))/30.;
        }

        icalo.energy = 1.0;
        // Smearing in energy
        const double fwhm2sig = 1.0/(2*sqrt(2*log(2.0)));
        icalo.sigma_energy = 0.08 * fwhm2sig * sqrt(icalo.energy) ;
        // std::normal_distribution<double> nde(icalo->energy, icalo->sigma_energy);
        // icalo->energy = nde(generator);
        // Smearing in time
        icalo.sigma_time = 0.250/sqrt(icalo.energy); // ns
        std::normal_distribution<double> ndt(time, icalo.sigma_time);
        icalo.time = ndt(generator);
        break;
      }
    }
  }

  return;
}

int main()
{
  const size_t nbr_events = 1;

  for (size_t i_evt = 0; i_evt < nbr_events; i_evt++) {
    gt::event a_event;
    generate_calorimeters(a_event);

    std::cout << "Event #" << i_evt << std::endl;
    std::cout << a_event << std::endl;

    // Gamma tracking
    gt::gamma_tracking gt;
    gt.prepare_process(a_event);
    // gt.set_absolute(true);
    gt.process();
    gt::gamma_tracking::solution_type gamma_tracked_coll;
    gt.get_reflects(gamma_tracked_coll);
    gt.dump();
    // gt.count();
    std::cout << "Number of gammas found = " << gamma_tracked_coll.size() << std::endl;
    for (auto icol : gamma_tracked_coll) {
      std::cout << "Size of collection : " << icol.size() <<std::endl;
      for (auto ilis : icol) {
        std::cout<< ilis << "->";
      }
      std::cout << std::endl;
    }
  }
  return 0;
}
