/* -*- mode: c++ -*- */
#ifndef __sultan__CLUSTEROFEXPERIMENTALHELICES
#define __sultan__CLUSTEROFEXPERIMENTALHELICES
#include <iostream>
#include <cmath>
#include <mybhep/utilities.h>
#include <sultan/experimental_helix.h>


namespace SULTAN {
  namespace topology{


    class cluster_of_experimental_helices : public tracking_object {

      // a cluster_of_experimental_helices is identified by:
      // min values (collected in helix_min)
      // max values (collected in helix_max)
      //  __ min__ ........ __central value__ ........ __max__
      // a vector of the helices in the clusetr

    private:
      std::string appname_;

      experimental_helix helix_min_;
      experimental_helix helix_max_;
      std::vector<experimental_helix> helices_;
      std::vector<size_t> ids_;
      std::vector<size_t> assigned_ids_;

    public:

      //!Default constructor
      cluster_of_experimental_helices(mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200)
      {
        appname_= "cluster_of_experimental_helices: ";
	helices_.clear();
	helix_min_ = experimental_helix();
	helix_max_ = experimental_helix();
	ids_.clear();
	assigned_ids_.clear();
        set_print_level(level);
        set_probmin(probmin);
      }

      //!Default destructor
      virtual ~cluster_of_experimental_helices(){};

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false)const{
	std::string indent;
	if (! a_indent.empty ()) indent = a_indent;
	if (! a_title.empty ())
	  {
	    a_out << indent << a_title << std::endl;
	  }
	
	a_out << indent << appname_ << " helix min: "; helix_min_.dump();
	a_out << " helix max: "; helix_max_.dump();
	a_out << helices_.size() << " helices: ";
	for(std::vector<experimental_helix>::const_iterator ih=helices_.begin(); ih!=helices_.end(); ++ih)
	  ih->dump();
	a_out << " ids: ( ";
	for(std::vector<size_t>::const_iterator id= ids_.begin(); id!=ids_.end(); ++id)
	  a_out << *id << " ";
	a_out << ") " << std::endl;	

	return;
      }
    

      void set_helix_min(experimental_helix a){helix_min_ = a;}
      void set_helix_max(experimental_helix a){helix_max_ = a;}

      experimental_helix helix_min()const{return helix_min_;}
      experimental_helix helix_max()const{return helix_max_;}
      std::vector<experimental_helix> helices()const{return helices_;}
      std::vector<size_t> ids()const{return ids_;}
      void reset_ids(){
	ids_.clear();
      }
      std::vector<size_t> assigned_ids()const{return assigned_ids_;}
      void reset_assigned_ids(){
	assigned_ids_.clear();
      }

      void add_id(size_t id);
      void add_assigned_id(size_t id);
      void print_ids()const{
	std::clog << " ( ";
	for(std::vector<size_t>::const_iterator id= ids_.begin(); id!=ids_.end(); ++id)
	  std::clog << *id << " ";
	std::clog << ") ";
      }

      bool can_store__optimist(experimental_helix a, double nsigmas);
      bool can_merge__optimist(cluster_of_experimental_helices cluster, double nsigmas)const;
      cluster_of_experimental_helices merge(cluster_of_experimental_helices b)const;
      bool add_helix(experimental_helix a);
      void add_helices(std::vector<experimental_helix> as);
      experimental_helix average_helix()const;


    };

  }

}

#endif
