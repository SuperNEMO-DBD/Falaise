/* -*- mode: c++ -*- */
#ifndef __sultan__ITRACKED_DATA
#define __sultan__ITRACKED_DATA
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <mybhep/utilities.h>
#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>
#include <sultan/cell.h>
#include <sultan/node.h>
#include <sultan/scenario.h>


namespace SULTAN{
  namespace topology{


    class tracked_data : public tracking_object{

      // a tracked_data is composed of a list of cells
      // a list of clusters
      // and a list of scenarios

    protected:
      std::string appname_;

    public:

      // list of cells
      std::vector<cell> cells_;

      // list of clusters
      std::vector<cluster> clusters_;

      // list of scenarios
      std::vector<scenario> scenarios_;

      //!Default constructor
      tracked_data()
      {
        appname_= "tracked_data: ";
      }

      //!Default destructor
      virtual ~tracked_data(){};

      //! constructor
      tracked_data(const std::vector<cell> &cells,
                   const std::vector<cluster> &clusters,
                   const std::vector<scenario> &scenarios,
                   mybhep::prlevel level=mybhep::NORMAL,
                   double probmin=1.e-200){
        set_print_level(level);
        set_probmin(probmin);
        appname_= "tracked_data: ";
        cells_ = cells;
        clusters_ = clusters;
        scenarios_ = scenarios;
      }

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const {
        std::string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << std::endl;
          }

        a_out << indent << appname_ << " ------------------- " << std::endl;
        a_out << indent << " number of cells : " << cells_.size() << std::endl;
        for(std::vector<cell>::const_iterator icell=cells_.begin(); icell!=cells_.end();++icell)
          icell->dump(a_out, "",indent + "     ");
        a_out << indent << " number of clusters : " << clusters_.size() << std::endl;
        for(std::vector<cluster>::const_iterator icluster=clusters_.begin(); icluster!=clusters_.end();++icluster)
          icluster->dump(a_out, "",indent + "     ");
        a_out << indent << " number of scenarios : " << scenarios_.size() << std::endl;
        for(std::vector<scenario>::const_iterator iscenario=scenarios_.begin(); iscenario!=scenarios_.end();++iscenario)
          iscenario->dump(a_out, "",indent + "     ");
        a_out << indent << " ------------------- " << std::endl;

        return;
      }

      //! set cells
      void set_cells(const std::vector<cell> & cells)
      {
        cells_ = cells;
      }

      //! set clusters
      void set_clusters(const std::vector<cluster> & clusters)
      {
        clusters_ = clusters;
      }

      //! set scenarios
      void set_scenarios(const std::vector<scenario> & scenarios)
      {
        scenarios_ = scenarios;
      }

      //! get cells
      std::vector<cell>& get_cells()
      {
        return cells_;
      }

      const std::vector<cell>& get_cells()const
      {
        return cells_;
      }

      //! get clusters
      std::vector<cluster>& get_clusters()
      {
        return clusters_;
      }

      const std::vector<cluster>& get_clusters()const
      {
        return clusters_;
      }

      //! get scenarios
      std::vector<scenario>& get_scenarios()
      {
        return scenarios_;
      }

      const std::vector<scenario>& get_scenarios()const
      {
        return scenarios_;
      }

      void reset(){
	cells_.clear();
	clusters_.clear();
	scenarios_.clear();
      }


    };
  }
}

#endif
