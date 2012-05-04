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


namespace CAT {
  namespace topology{

    using namespace std;
    using namespace mybhep;

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

      // list of cell triplets
      std::vector<cell_triplet>  ccc_;

      // list of linkable cells
      std::vector<cell>  links_;

      // status of node
      bool free_;

      // fitted point
      experimental_point ep_;

      // chi2 of connection in a sequence
      double chi2_;

    public:   

      //!Default constructor     
      node();

      //!Default destructor
      virtual ~node();

      //! constructor
      node(const cell & c, const std::vector<cell_couplet> & cc, const std::vector<cell_triplet> & ccc);

      //! constructor
      node(const cell &c, prlevel level=mybhep::NORMAL, double nsigma=10.);

      //! constructor from bhep true hit
      node(const mybhep::hit &truehit,
           size_t id, 
           bool SuperNemo, 
           prlevel level=mybhep::NORMAL,
           double nsigma=10.);

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

      //! set fitted experimental_point
      void set_ep( const experimental_point &ep );

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

      //! get fitted experimental_point
      const experimental_point& ep()const;

    public:

      void calculate_triplets(double Ratio, double separation_limit=90., double phi_limit=25., double theta_limit=180.);

      node invert();

      std::string topological_type() const;

      bool has_couplet(const cell & a, cell_couplet* ct)const;

      bool has_couplet(const cell& a, size_t* index)const;

      bool has_couplet(size_t idd, size_t* index)const;

      bool has_triplet(const cell &a, const cell &c)const;

      bool has_triplet(const cell &a)const;
 
      friend bool operator==(const node& left,
                             const node& right);

  };
}
}


#endif // __CATAlgorithm__node_h
