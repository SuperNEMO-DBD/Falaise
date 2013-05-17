/* -*- mode: c++ -*- */

#ifndef __CATAlgorithm__node_h
#define __CATAlgorithm__node_h 1

#include <iostream>
#include <vector>
#include <string>
#include <mybhep/utilities.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <algorithm>
#include <map>

namespace CAT {
  namespace topology{


    class node : public tracking_object {

      // a node is composed of a main cell,
      // a list of cell_couplet
      // and a list of cell_triplet

    protected:
      std::string appname_;

    public:

      // main cell
      cell c_;

      // list of cell couplets
      std::vector<cell_couplet> cc_;
      std::map<size_t,size_t> cc_index_;

      // list of cell triplets
      std::vector<cell_triplet>  ccc_;
      std::map<size_t,size_t> ccc_ca_index_;
      std::map<size_t,size_t> ccc_cc_index_;

      // list of linkable cells
      std::vector<cell>  links_;

      // status of node
      bool free_;

      // fitted point
      experimental_point ep_;

      // chi2 of connection in a sequence
      double chi2_;
      int32_t ndof_;

      bool is_kink_;

    public:

      //!Default constructor
      node();

      //!Default destructor
      virtual ~node();

      //! constructor
      node(const cell & c, const std::vector<cell_couplet> & cc, const std::vector<cell_triplet> & ccc);

      //! constructor
      node(const cell &c, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200);

      //! constructor from bhep true hit
      node(const mybhep::hit &truehit,
           size_t id,
           bool SuperNemo,
           mybhep::prlevel level=mybhep::NORMAL,
           double probmin=1.e-200);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const;

      //! set cells
      void set(const cell &c,
               const std::vector<cell_couplet> &cc,
               const std::vector<cell_triplet> & ccc);

      //! set main cell
      void set_c(const cell& c);

      //! set cell couplets
      void set_cc(const std::vector<cell_couplet> &cc);

      //! set cell triplets
      void set_ccc(const std::vector<cell_triplet>  &ccc);

      //! set links
      void set_links(const std::vector<cell>  &links);

      //! set free level
      void set_free(bool free);

      //! set chi2
      void set_chi2(double chi2);

      //! set ndof
      void set_ndof(int32_t ndof);

      //! set fitted experimental_point
      void set_ep( const experimental_point &ep );

      //! set is_kink
      void set_is_kink(bool is_kink);

      //! get main cell
      const cell& c()const;

      //! get cell couplets
      const std::vector<cell_couplet> &cc()const;

      //! get cell triplets
      const std::vector<cell_triplet> &ccc()const;

      //! get links
      const std::vector<cell> &links()const;

      //! get free level
      bool free()const;

      //! get chi2
      double chi2()const;

      //! get ndof
      int32_t ndof()const;

      //! get Prob
      double Prob()const;

      //! get is_kink
      bool is_kink()const;

      //! get fitted experimental_point
      const experimental_point& ep()const;

      //! grab cc index map
      std::map<size_t,size_t> cc_index()const;

      //! grab ccc index map
      std::map<size_t,size_t> ccc_ca_index()const;
      std::map<size_t,size_t> ccc_cc_index()const;

    private:
      void setup_cc_maps();
      void setup_ccc_maps();

    public:

      void add_triplet(const cell_triplet &ccc);

      void remove_couplet(size_t index);

      void remove_triplet(size_t index);

      void remove_link(size_t index);

      void calculate_triplets(double Ratio, double separation_limit=90., double phi_limit=25., double theta_limit=180.);

      node invert();

      std::string topological_type() const;

      bool has_couplet(const cell & a, cell_couplet* ct)const;

      bool has_couplet(const cell& a, size_t* index)const;

      bool has_couplet(size_t idd, size_t* index)const;

      bool has_triplet(const cell &a, const cell &c, size_t *index)const;

      bool has_triplet(const cell &a, const cell &c)const;

      bool has_triplet(const cell &a)const;

      friend bool operator==(const node& left,
                             const node& right);

  };
}
}


#endif // __CATAlgorithm__node_h
