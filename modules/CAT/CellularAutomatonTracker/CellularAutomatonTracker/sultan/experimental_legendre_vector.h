/* -*- mode: c++ -*- */
#ifndef __sultan__EXPERIMENTALLEGENDRE_VECTOR
#define __sultan__EXPERIMENTALLEGENDRE_VECTOR
#include <iostream>
#include <cmath>
#include <sultan/Clock.h>
#include <mybhep/utilities.h>
#include <sultan/experimental_helix.h>
#include <sultan/cluster_of_experimental_helices.h>

namespace SULTAN {
namespace topology {

class experimental_legendre_vector : public tracking_object {
 private:
  std::string appname_;

  std::vector<experimental_helix> helices_;
  std::vector<cluster_of_experimental_helices> clusters_;
  int index_of_largest_cluster_;

  double nsigmas_;

  double x0dist_;
  double y0dist_;
  double z0dist_;
  double Rdist_;
  double Hdist_;

 protected:
  Clock clock;

 public:
  //! Default constructor
  experimental_legendre_vector(mybhep::prlevel level = mybhep::NORMAL, double probmin = 1.e-200) {
    appname_ = "experimental_legendre_vector: ";
    helices_.clear();
    clusters_.clear();
    set_print_level(level);
    set_probmin(probmin);
    nsigmas_ = 1.;
    index_of_largest_cluster_ = -1;
    x0dist_ = mybhep::default_min;
    y0dist_ = mybhep::default_min;
    z0dist_ = mybhep::default_min;
    Rdist_ = mybhep::default_min;
    Hdist_ = mybhep::default_min;
  }

  //! Default destructor
  virtual ~experimental_legendre_vector(){};

  /*** dump ***/
  virtual void dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                    const std::string& a_indent = "", bool /*a_inherit*/ = false) {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;
    if (!a_title.empty()) {
      a_out << indent << a_title << std::endl;
    }
    a_out << indent << appname_ << " helices " << helices_.size() << std::endl;
    for (std::vector<experimental_helix>::const_iterator ip = helices_.begin();
         ip != helices_.end(); ++ip) {
      ip->dump();
    }
    a_out << " clusters " << clusters_.size() << std::endl;
    for (std::vector<cluster_of_experimental_helices>::const_iterator ip = clusters_.begin();
         ip != clusters_.end(); ++ip) {
      ip->dump();
    }

    return;
  }

  void set_helices(std::vector<experimental_helix> a);

  void set_clusters(std::vector<cluster_of_experimental_helices> a);

  void set_nsigmas(double a);

  void set_index_of_largest_cluster(int a);

  std::vector<experimental_helix> helices();

  std::vector<cluster_of_experimental_helices> clusters();

  double get_nsigmas();

  int get_index_of_largest_cluster();

  void add_helix(experimental_helix a);

  void add_helix_to_clusters(experimental_helix a);

  void add_a_helix_to_clusters(experimental_helix a);

  void reset();

  void get_neighbours(experimental_helix a, std::vector<experimental_helix>* neighbours);

  void get_neighbours_ids(experimental_helix a, size_t* nids);

  void get_neighbour_ids(experimental_helix a, size_t* nids);

  double gauss(double mean, double sigma);

  experimental_helix gaussian_max(size_t n_iterations, experimental_helix seed);

  experimental_helix max(std::vector<experimental_helix>* neighbours);

  experimental_helix max(std::vector<size_t>* neighbouring_cells);

  experimental_helix max_with_metric();

  experimental_helix max_with_ids();

  cluster_of_experimental_helices max_cluster(experimental_helix* helix, bool* found);

  void merge_cluster_of_index(size_t i);

  void merge_the_cluster_of_index(size_t i);

  void merge_clusters();

  void merge_the_clusters();

  void create_cluster(experimental_helix a);

  void print_clocks();

  bool assign_cell(size_t cell_id);

  void reset_helices_errors();

  void calculate_metric();
};

}  // namespace topology

}  // namespace SULTAN

#endif
