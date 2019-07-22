/* -*- mode: c++ -*- */

#include <algorithm>
#include <sultan/experimental_legendre_vector.h>

namespace SULTAN {
namespace topology {

using namespace std;
using namespace mybhep;

void experimental_legendre_vector::set_helices(std::vector<experimental_helix> a) { helices_ = a; }

std::vector<experimental_helix> experimental_legendre_vector::helices() { return helices_; }

void experimental_legendre_vector::set_clusters(std::vector<cluster_of_experimental_helices> a) {
  clusters_ = a;
}

std::vector<cluster_of_experimental_helices> experimental_legendre_vector::clusters() {
  return clusters_;
}

void experimental_legendre_vector::set_nsigmas(double a) { nsigmas_ = a; }

void experimental_legendre_vector::set_index_of_largest_cluster(int a) {
  index_of_largest_cluster_ = a;
}

double experimental_legendre_vector::get_nsigmas() { return nsigmas_; }

int experimental_legendre_vector::get_index_of_largest_cluster() {
  return index_of_largest_cluster_;
}

void experimental_legendre_vector::add_helix(experimental_helix a) { helices_.push_back(a); }

void experimental_legendre_vector::reset() {
  helices_.clear();
  clusters_.clear();
  index_of_largest_cluster_ = -1;
}

void experimental_legendre_vector::get_neighbours(experimental_helix a,
                                                  std::vector<experimental_helix>* neighbours) {
  experimental_double dx, dy, dz, dR, dH;
  neighbours->clear();
  for (std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end();
       ++ip) {
    if (!a.different_cells(*ip)) continue;
    dx = a.x0() - ip->x0();
    if (std::abs(dx.value()) > get_nsigmas() * dx.error()) continue;
    dy = a.y0() - ip->y0();
    if (std::abs(dy.value()) > get_nsigmas() * dy.error()) continue;
    dz = a.z0() - ip->z0();
    if (std::abs(dz.value()) > get_nsigmas() * dz.error()) continue;
    dR = a.R() - ip->R();
    if (std::abs(dR.value()) > get_nsigmas() * dR.error()) continue;
    dH = a.H() - ip->H();
    if (std::abs(dH.value()) > get_nsigmas() * dH.error()) continue;
    neighbours->push_back(*ip);
  }
  return;
}

void experimental_legendre_vector::get_neighbours_ids(experimental_helix a, size_t* nids) {
  experimental_double dx, dy, dz, dR, dH;
  *nids = 0;
  for (std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end();
       ++ip) {
    if (!a.different_cells(*ip)) continue;
    dx = a.x0() - ip->x0();
    if (std::abs(dx.value()) > get_nsigmas() * dx.error()) continue;
    dy = a.y0() - ip->y0();
    if (std::abs(dy.value()) > get_nsigmas() * dy.error()) continue;
    dz = a.z0() - ip->z0();
    if (std::abs(dz.value()) > get_nsigmas() * dz.error()) continue;
    dR = a.R() - ip->R();
    if (std::abs(dR.value()) > get_nsigmas() * dR.error()) continue;
    dH = a.H() - ip->H();
    if (std::abs(dH.value()) > get_nsigmas() * dH.error()) continue;
    a.add_ids(ip->ids());
  }
  *nids = a.ids().size();
  return;
}

void experimental_legendre_vector::get_neighbour_ids(experimental_helix a, size_t* nids) {
  double dx, dy, dz, dR, dH;
  *nids = 0;
  for (std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end();
       ++ip) {
    if (!a.different_cells(*ip)) continue;

    dx = std::abs(a.x0().value() - ip->x0().value());
    if (dx > get_nsigmas() * x0dist_) continue;

    dy = std::abs(a.y0().value() - ip->y0().value());
    if (dy > get_nsigmas() * y0dist_) continue;

    dz = std::abs(a.z0().value() - ip->z0().value());
    if (dz > get_nsigmas() * z0dist_) continue;

    dR = std::abs(a.R().value() - ip->R().value());
    if (dR > get_nsigmas() * Rdist_) continue;

    dH = std::abs(a.H().value() - ip->H().value());
    if (dH > get_nsigmas() * Hdist_) continue;

    a.add_ids(ip->ids());
  }

  *nids = a.ids().size();
  return;
}

double experimental_legendre_vector::gauss(double x, double sigma) {
  double exponent = pow(x / sigma, 2.) / 2.;
  return exp(-exponent) / sigma;
}

experimental_helix experimental_legendre_vector::gaussian_max(size_t n_iterations,
                                                              experimental_helix seed) {
  double bestx0 = seed.x0().value();
  double besty0 = seed.x0().value();
  double bestz0 = seed.x0().value();
  double bestR = seed.x0().value();
  double bestH = seed.x0().value();
  double numx0, numy0, numz0, numR, numH;
  double denx0, deny0, denz0, denR, denH;
  double weight;

  seed.dump();

  std::vector<experimental_helix> neis;
  for (size_t iter = 0; iter < n_iterations; iter++) {
    numx0 = 0.;
    numy0 = 0.;
    numz0 = 0.;
    numR = 0.;
    numH = 0.;
    denx0 = 0.;
    deny0 = 0.;
    denz0 = 0.;
    denR = 0.;
    denH = 0.;

    for (std::vector<experimental_helix>::const_iterator ip = helices_.begin();
         ip != helices_.end(); ++ip) {
      if (std::isnan(ip->x0().error()) || std::isnan(ip->y0().error()) ||
          std::isnan(ip->z0().error()) || std::isnan(ip->R().error()) ||
          std::isnan(ip->H().error()))
        continue;

      weight = gauss(bestx0 - ip->x0().value(), ip->x0().error()) *
               gauss(besty0 - ip->y0().value(), ip->y0().error()) *
               gauss(bestz0 - ip->z0().value(), ip->z0().error()) *
               gauss(bestR - ip->R().value(), ip->R().error()) *
               gauss(bestH - ip->H().value(), ip->H().error());

      numx0 += ip->x0().value() / pow(ip->x0().error(), 2) * weight;
      numy0 += ip->y0().value() / pow(ip->y0().error(), 2) * weight;
      numz0 += ip->z0().value() / pow(ip->z0().error(), 2) * weight;
      numR += ip->R().value() / pow(ip->R().error(), 2) * weight;
      numH += ip->H().value() / pow(ip->H().error(), 2) * weight;

      denx0 += 1. / pow(ip->x0().error(), 2) * weight;
      deny0 += 1. / pow(ip->y0().error(), 2) * weight;
      denz0 += 1. / pow(ip->z0().error(), 2) * weight;
      denR += 1. / pow(ip->R().error(), 2) * weight;
      denH += 1. / pow(ip->H().error(), 2) * weight;
    }

    bestx0 = numx0 / denx0;
    besty0 = numy0 / deny0;
    bestz0 = numz0 / denz0;
    bestR = numR / denR;
    bestH = numH / denH;
  }

  experimental_helix r(experimental_double(bestx0, 0.), experimental_double(besty0, 0.),
                       experimental_double(bestz0, 0.), experimental_double(bestR, 0.),
                       experimental_double(bestH, 0.));

  return r;
}

experimental_helix experimental_legendre_vector::max(std::vector<experimental_helix>* neighbours) {
  experimental_helix r;
  size_t nmax = 0;
  std::vector<experimental_helix> neis;
  for (std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end();
       ++ip) {
    if (ip->isnan() || ip->isinf()) {
      if (print_level() >= mybhep::NORMAL) {
        std::clog << " problem: helix is nan or inf ";
        ip->dump();
        std::clog << " " << std::endl;
      }
      continue;
    }

    get_neighbours(*ip, &neis);

    if (neis.size() > nmax) {
      nmax = neis.size();
      *neighbours = neis;
      r = *ip;
    }
  }

  if (nmax == 0) r = helices_.front();

  if (r.isnan() || r.isinf()) {
    if (print_level() >= mybhep::NORMAL) {
      std::clog << " problem: helix is nan or inf ";
      r.dump();
      std::clog << " helices " << helices_.size() << " nmax " << nmax << std::endl;
    }
  }

  return r;
}

experimental_helix experimental_legendre_vector::max_with_ids() {
  experimental_helix r;
  size_t nmax = 0;
  size_t nids = 0;
  for (std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end();
       ++ip) {
    if (ip->isnan() || ip->isinf()) {
      if (print_level() >= mybhep::NORMAL) {
        std::clog << " problem: helix is nan or inf ";
        ip->dump();
        std::clog << " " << std::endl;
      }
      continue;
    }

    get_neighbours_ids(*ip, &nids);

    if (nids > nmax) {
      nmax = nids;
      r = *ip;
    }
  }

  if (nmax == 0) r = helices_.front();

  if (r.isnan() || r.isinf()) {
    if (print_level() >= mybhep::NORMAL) {
      std::clog << " problem: helix is nan or inf ";
      r.dump();
      std::clog << " helices " << helices_.size() << " nmax " << nmax << std::endl;
    }
  }

  return r;
}

experimental_helix experimental_legendre_vector::max(std::vector<size_t>* neighbouring_cells) {
  experimental_helix r;
  // size_t n = 0;
  size_t nmax = 0;
  std::vector<experimental_helix> neis, neis_best;
  for (std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end();
       ++ip) {
    get_neighbours(*ip, &neis);

    if (neis.size() > nmax) {
      nmax = neis.size();
      neis_best = neis;
      r = *ip;
    }
  }

  neighbouring_cells->clear();
  std::vector<size_t> ids;
  for (std::vector<experimental_helix>::const_iterator ip = neis_best.begin();
       ip != neis_best.end(); ++ip) {
    ids = ip->ids();
    for (std::vector<size_t>::const_iterator id = ids.begin(); id != ids.end(); ++id) {
      if (std::find(neighbouring_cells->begin(), neighbouring_cells->end(), *id) ==
          neighbouring_cells->end())
        neighbouring_cells->push_back(*id);
    }
  }

  return r;
}

experimental_helix experimental_legendre_vector::max_with_metric() {
  experimental_helix r;

  size_t nmax = 0;
  size_t nids;
  for (std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end();
       ++ip) {
    if (ip->isnan() || ip->isinf()) {
      if (print_level() >= mybhep::NORMAL) {
        std::clog << " problem: helix is nan or inf ";
        ip->dump();
        std::clog << " " << std::endl;
      }
      continue;
    }

    nids = 0;
    get_neighbour_ids(*ip, &nids);

    if (nids > nmax) {
      nmax = nids;
      r = *ip;
    }

    if (print_level() >= mybhep::VERBOSE) {
      std::clog << " helix " << ip - helices_.begin() << " has " << nids << " ids, max " << nmax
                << std::endl;
    }
  }

  if (r.isnan() || r.isinf()) {
    if (print_level() >= mybhep::NORMAL) {
      std::clog << " problem: helix is nan or inf ";
      r.dump();
      std::clog << " helices " << helices_.size() << " nmax " << nmax << std::endl;
    }
  }

  return r;
}

cluster_of_experimental_helices experimental_legendre_vector::max_cluster(experimental_helix* helix,
                                                                          bool* found) {
  cluster_of_experimental_helices best_cluster;

  if (index_of_largest_cluster_ < 0 || index_of_largest_cluster_ > (int)clusters_.size()) {
    if (print_level() >= mybhep::NORMAL)
      std::clog << " problem: index_of_largest_cluster " << index_of_largest_cluster_
                << " clusters size " << clusters_.size() << std::endl;
    *found = false;
    return best_cluster;
  }

  *found = true;
  best_cluster = clusters_[index_of_largest_cluster_];
  *helix = best_cluster.average_helix();

  return best_cluster;
}

void experimental_legendre_vector::add_helix_to_clusters(experimental_helix a) {
  clock.start(" experimental_legendre_vector: add_helix_to_clusters ", "cumulative");

  bool a_maximum_is_already_defined = (index_of_largest_cluster_ >= 0);

  if (print_level() >= mybhep::VVERBOSE) {
    std::clog << " add helix ";
    a.print_ids();
    std::clog << " when there are " << clusters_.size() << " clusters (";
    for (std::vector<cluster_of_experimental_helices>::iterator ic = clusters_.begin();
         ic != clusters_.end(); ++ic)
      std::clog << ic->ids().size() << ", ";
    std::clog << ") largest " << index_of_largest_cluster_ << std::endl;
  }

  if (!a_maximum_is_already_defined) {
    create_cluster(a);
    index_of_largest_cluster_ = 0;
    clock.stop(" experimental_legendre_vector: add_helix_to_clusters ");
    return;
  }

  bool added = false;
  bool changed = false;
  for (std::vector<cluster_of_experimental_helices>::iterator ic = clusters_.begin();
       ic != clusters_.end(); ++ic) {
    if (print_level() >= mybhep::VVERBOSE) {
      std::clog << " compare helix with cluster " << ic - clusters_.begin() << " of "
                << ic->ids().size() << " cells " << std::endl;
    }

    if (ic->can_store__optimist(a, get_nsigmas())) {
      changed = ic->add_helix(a);
      added = true;

      if (ic->ids().size() > clusters_[index_of_largest_cluster_].ids().size())
        index_of_largest_cluster_ = ic - clusters_.begin();

      if (print_level() >= mybhep::VVERBOSE) {
        std::clog << " can store, changed: " << changed << std::endl;
      }

      if (changed) merge_cluster_of_index(ic - clusters_.begin());

      break;
    }
  }

  if (!added) {
    create_cluster(a);
  }

  clock.stop(" experimental_legendre_vector: add_helix_to_clusters ");
  return;
}

void experimental_legendre_vector::add_a_helix_to_clusters(experimental_helix a) {
  clock.start(" experimental_legendre_vector: add_a_helix_to_clusters ", "cumulative");

  if (print_level() >= mybhep::VVERBOSE) {
    std::clog << " add helix ";
    a.print_ids();
    std::clog << " when there are " << clusters_.size() << " clusters (";
    for (std::vector<cluster_of_experimental_helices>::iterator ic = clusters_.begin();
         ic != clusters_.end(); ++ic)
      std::clog << ic->ids().size() << ", ";
    std::clog << " " << std::endl;
  }

  if (!clusters_.size()) {
    create_cluster(a);
    clock.stop(" experimental_legendre_vector: add_a_helix_to_clusters ");
    return;
  }

  bool added = false;
  bool changed = false;
  for (std::vector<cluster_of_experimental_helices>::iterator ic = clusters_.begin();
       ic != clusters_.end(); ++ic) {
    if (print_level() >= mybhep::VVERBOSE) {
      std::clog << " compare helix with cluster " << ic - clusters_.begin() << " of "
                << ic->ids().size() << " cells " << std::endl;
    }

    if (ic->can_store__optimist(a, get_nsigmas())) {
      changed = ic->add_helix(a);
      added = true;

      if (print_level() >= mybhep::VVERBOSE) {
        std::clog << " can store, changed: " << changed << std::endl;
      }

      if (changed) merge_the_cluster_of_index(ic - clusters_.begin());

      break;
    }
  }

  if (!added) {
    create_cluster(a);
  }

  clock.stop(" experimental_legendre_vector: add_helix_to_clusters ");
  return;
}

void experimental_legendre_vector::merge_cluster_of_index(size_t i) {
  clock.start(" experimental_legendre_vector: merge_cluster_of_index ", "cumulative");

  cluster_of_experimental_helices _ic = clusters_[i];
  for (std::vector<cluster_of_experimental_helices>::const_iterator jc = clusters_.begin();
       jc != clusters_.end(); ++jc) {
    if (jc - clusters_.begin() == (int)i) continue;
    if (jc->can_merge__optimist(_ic, nsigmas_)) {
      if (print_level() >= mybhep::VVERBOSE) {
        std::clog << " merge " << i << "[" << clusters_[i].ids().size() << "] and "
                  << jc - clusters_.begin() << " [" << jc->ids().size() << "]" << std::endl;
      }

      _ic = _ic.merge(*jc);
      clusters_[i] = _ic;
      clusters_.erase(clusters_.begin() + (jc - clusters_.begin()));
      break;
    }
  }

  size_t max_n_ids = 0;
  for (std::vector<cluster_of_experimental_helices>::const_iterator jc = clusters_.begin();
       jc != clusters_.end(); ++jc) {
    if (jc->ids().size() > max_n_ids) {
      max_n_ids = jc->ids().size();
      index_of_largest_cluster_ = jc - clusters_.begin();
    }
  }

  if (print_level() >= mybhep::VVERBOSE) {
    std::clog << " --> (";
    for (std::vector<cluster_of_experimental_helices>::iterator qc = clusters_.begin();
         qc != clusters_.end(); ++qc)
      std::clog << qc->ids().size() << ", ";
    std::clog << ") largest " << index_of_largest_cluster_ << std::endl;
  }

  return;

  clock.stop(" experimental_legendre_vector: merge_cluster_of_index ");
}

void experimental_legendre_vector::merge_the_cluster_of_index(size_t i) {
  clock.start(" experimental_legendre_vector: merge_the_cluster_of_index ", "cumulative");

  if (i >= clusters_.size()) {
    if (print_level() >= mybhep::NORMAL) {
      std::clog << " problem: trying to merge cluster " << i
                << " which is not there, cluster size = " << clusters_.size() << std::endl;
    }
    clock.stop(" experimental_legendre_vector: merge_the_cluster_of_index ");
    return;
  }

  cluster_of_experimental_helices _ic = clusters_[i];
  for (std::vector<cluster_of_experimental_helices>::const_iterator jc = clusters_.begin();
       jc != clusters_.end(); ++jc) {
    if (jc - clusters_.begin() == (int)i) continue;
    if (jc->can_merge__optimist(_ic, nsigmas_)) {
      if (print_level() >= mybhep::VVERBOSE) {
        std::clog << " merge " << i << "[" << clusters_[i].ids().size() << "] and "
                  << jc - clusters_.begin() << " [" << jc->ids().size() << "]" << std::endl;
      }

      _ic = _ic.merge(*jc);
      clusters_[i] = _ic;
      clusters_.erase(clusters_.begin() + (jc - clusters_.begin()));
      break;
    }
  }

  if (print_level() >= mybhep::VVERBOSE) {
    std::clog << " --> (";
    for (std::vector<cluster_of_experimental_helices>::iterator qc = clusters_.begin();
         qc != clusters_.end(); ++qc)
      std::clog << qc->ids().size() << ", ";
    std::clog << " " << std::endl;
  }

  clock.stop(" experimental_legendre_vector: merge_the_cluster_of_index ");
  return;
}

void experimental_legendre_vector::merge_clusters() {
  for (std::vector<cluster_of_experimental_helices>::const_iterator ic = clusters_.begin();
       ic != clusters_.end() - 1; ++ic)
    merge_cluster_of_index(ic - clusters_.begin());
}

void experimental_legendre_vector::merge_the_clusters() {
  for (std::vector<cluster_of_experimental_helices>::const_iterator ic = clusters_.begin();
       ic != clusters_.end() - 1; ++ic)
    merge_the_cluster_of_index(ic - clusters_.begin());
}

void experimental_legendre_vector::create_cluster(experimental_helix a) {
  cluster_of_experimental_helices newc;
  newc.add_helix(a);
  newc.set_helix_min(a);
  newc.set_helix_max(a);
  clusters_.push_back(newc);
}

void experimental_legendre_vector::print_clocks() { clock.dump(); }

bool experimental_legendre_vector::assign_cell(size_t cell_id) {
  // size_t maxn = 0;
  std::vector<size_t> ids;
  // size_t cluster_index;

  if (print_level() >= mybhep::VVERBOSE) {
    std::clog << " assign cell " << cell_id << " when there are " << clusters_.size()
              << " clusters " << std::endl;
  }

#if 0
      // method 1
      // assign the cell to that cluster of triplets/helices
      // which contains the largest number of different cells

      // loop on clusters of triplets/cells
      for(std::vector<cluster_of_experimental_helices>::const_iterator ic = clusters_.begin(); ic!=clusters_.end(); ++ic){
        ids = ic->ids();
        if( print_level() >= mybhep::VVERBOSE ){
          std::clog << " cluster " << ic - clusters_.begin() << " has " << ids.size() << " cells "; ic->print_ids(); std::clog << " " << std::endl;
        }

        // if the cluster contains cell_id
        if( std::find(ids.begin(), ids.end(), cell_id) != ids.end() ){
          // look for the cluster with largest number of different cells
          if( ids.size() > maxn ){
            maxn = ids.size();
            cluster_index = ic - clusters_.begin();
          }
          if( print_level() >= mybhep::VVERBOSE ){
            std::clog << " contained in cluster with " << ids.size() << " different cells, maximum now is " << maxn << std::endl;
          }
        }
      }

      // pick the containing cluster with the largest n of different cells
      if( maxn > 0 ){
        if( print_level() >= mybhep::VVERBOSE ){
          std::clog << " finally assigned to cluster " << cluster_index << " containing " << maxn << " different cells " << std::endl;
        }
        clusters_[cluster_index].add_assigned_id(cell_id);
        return true;
      }

      if( print_level() >= mybhep::VVERBOSE ){
        std::clog << " finally not assigned " << std::endl;
      }

      return false;
#else
  // method 2
  // assign the cell to any cluster of triplets/helices
  // which contains >= 4 different cells

  size_t min_n_different_cells = 4;
  bool assigned = false;

  // loop on clusters of triplets/cells
  for (std::vector<cluster_of_experimental_helices>::iterator ic = clusters_.begin();
       ic != clusters_.end(); ++ic) {
    ids = ic->ids();
    if (print_level() >= mybhep::VVERBOSE) {
      std::clog << " cluster " << ic - clusters_.begin() << " has " << ids.size() << " cells ";
      ic->print_ids();
      std::clog << " " << std::endl;
    }

    // if the cluster contains cell_id
    if (std::find(ids.begin(), ids.end(), cell_id) != ids.end()) {
      if (print_level() >= mybhep::VVERBOSE) {
        std::clog << " contained in cluster with " << ids.size() << " different cells, minimum is "
                  << min_n_different_cells << std::endl;
      }
      // if the cluster contains enough different cells
      if (ids.size() >= min_n_different_cells) {
        ic->add_assigned_id(cell_id);
        assigned = true;
      }
    }
  }

  return assigned;
#endif
}

void experimental_legendre_vector::reset_helices_errors() {
  double nhelices = (double)(helices_.size());

  double x0min = mybhep::default_min;
  double y0min = mybhep::default_min;
  double z0min = mybhep::default_min;
  double Rmin = mybhep::default_min;
  double Hmin = mybhep::default_min;

  double x0max = mybhep::default_max;
  double y0max = mybhep::default_max;
  double z0max = mybhep::default_max;
  double Rmax = mybhep::default_max;
  double Hmax = mybhep::default_max;

  for (std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end();
       ++ip) {
    if (ip->x0().value() < x0min) x0min = ip->x0().value();
    if (ip->y0().value() < y0min) y0min = ip->y0().value();
    if (ip->z0().value() < z0min) z0min = ip->z0().value();
    if (ip->R().value() < Rmin) Rmin = ip->R().value();
    if (ip->H().value() < Hmin) Hmin = ip->H().value();

    if (ip->x0().value() > x0max) x0max = ip->x0().value();
    if (ip->y0().value() > y0max) y0max = ip->y0().value();
    if (ip->z0().value() > z0max) z0max = ip->z0().value();
    if (ip->R().value() > Rmax) Rmax = ip->R().value();
    if (ip->H().value() > Hmax) Hmax = ip->H().value();
  }

  double x0error = (x0max - x0min) / nhelices;
  double y0error = (y0max - y0min) / nhelices;
  double z0error = (z0max - z0min) / nhelices;
  double Rerror = (Rmax - Rmin) / nhelices;
  double Herror = (Hmax - Hmin) / nhelices;

  for (std::vector<experimental_helix>::iterator ip = helices_.begin(); ip != helices_.end();
       ++ip) {
    ip->set_x0(experimental_double(ip->x0().value(), x0error));
    ip->set_y0(experimental_double(ip->y0().value(), y0error));
    ip->set_z0(experimental_double(ip->z0().value(), z0error));
    ip->set_R(experimental_double(ip->R().value(), Rerror));
    ip->set_H(experimental_double(ip->H().value(), Herror));
  }

  return;
}

void experimental_legendre_vector::calculate_metric() {
  double dx0min = mybhep::default_min;
  double dy0min = mybhep::default_min;
  double dz0min = mybhep::default_min;
  double dRmin = mybhep::default_min;
  double dHmin = mybhep::default_min;

  double dx0max = mybhep::default_max;
  double dy0max = mybhep::default_max;
  double dz0max = mybhep::default_max;
  double dRmax = mybhep::default_max;
  double dHmax = mybhep::default_max;

  for (std::vector<experimental_helix>::const_iterator ip = helices_.begin();
       ip != helices_.end() - 1; ++ip) {
    if (print_level() >= mybhep::VERBOSE) {
      std::clog << " helix A [" << ip - helices_.begin() << "] ";
      ip->dump();
    }
    for (std::vector<experimental_helix>::const_iterator jp = ip; jp != helices_.end(); ++jp) {
      if (jp - helices_.begin() == ip - helices_.begin()) continue;

      if (print_level() >= mybhep::VERBOSE) {
        std::clog << " helix B [" << jp - helices_.begin() << "] ";
        jp->dump();
      }

      if (!ip->different_cells(*jp)) continue;

      double dx0 = std::abs(ip->x0().value() - jp->x0().value());
      double dy0 = std::abs(ip->y0().value() - jp->y0().value());
      double dz0 = std::abs(ip->z0().value() - jp->z0().value());
      double dR = std::abs(ip->R().value() - jp->R().value());
      double dH = std::abs(ip->H().value() - jp->H().value());

      if (print_level() >= mybhep::VERBOSE) {
        std::clog << " dx0 " << dx0 << " dy0 " << dy0 << " dz0 " << dz0 << " dR " << dR << " dH "
                  << dH << std::endl;
      }

      if (dx0 < dx0min) dx0min = dx0;
      if (dx0 > dx0max) dx0max = dx0;
      if (dy0 < dy0min) dy0min = dy0;
      if (dy0 > dy0max) dy0max = dy0;
      if (dz0 < dz0min) dz0min = dz0;
      if (dz0 > dz0max) dz0max = dz0;
      if (dR < dRmin) dRmin = dR;
      if (dR > dRmax) dRmax = dR;
      if (dH < dHmin) dHmin = dH;
      if (dH > dHmax) dHmax = dH;
    }
  }

  double weight_min = 0.99;

  x0dist_ = weight_min * dx0min + (1. - weight_min) * dx0max;
  y0dist_ = weight_min * dy0min + (1. - weight_min) * dy0max;
  z0dist_ = weight_min * dz0min + (1. - weight_min) * dz0max;
  Rdist_ = weight_min * dRmin + (1. - weight_min) * dRmax;
  Hdist_ = weight_min * dHmin + (1. - weight_min) * dHmax;

  if (print_level() >= mybhep::VERBOSE) {
    std::clog << " metric for " << helices_.size() << " helices: x0dist " << x0dist_ << " y0dist "
              << y0dist_ << " z0dist " << z0dist_ << " Rdist " << Rdist_ << " Hdist " << Hdist_
              << std::endl;
  }

  return;
}

}  // namespace topology

}  // namespace SULTAN
