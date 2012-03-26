/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ITRACKED_DATA
#define __CATAlgorithm__ITRACKED_DATA
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/clhep.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/node.h>
#include <CATAlgorithm/calorimeter_hit.h>
#include <CATAlgorithm/scenario.h>
#include <CATAlgorithm/lt_utils.h>


namespace CAT{
  namespace topology{

    using namespace std;
    using namespace mybhep;

    class tracked_data : public tracking_object{

      // a tracked_data is composed of a list of cells
      // a list of clusters
      // and a list of scenarios

    protected:
      string appname_;

    public:   

      // list of cells
      std::vector<cell> cells_;

      // list of cells
      std::vector<calorimeter_hit> calos_;

      // list of clusters
      std::vector<cluster> clusters_;

      // list of scenarios
      std::vector<scenario> scenarios_;

      // list of true sequences
      std::vector<sequence> true_sequences_;

      // list of nemo3 sequences
      std::vector<sequence> nemo_sequences_;

      // is event selcted?
      bool selected_;

      // is event skipped?
      bool skipped_;

      //!Default constructor     
      tracked_data()
      {
        appname_= "tracked_data: ";
        cells_.clear();
        calos_.clear();
        clusters_.clear();
        scenarios_.clear();
        true_sequences_.clear();
        nemo_sequences_.clear();
        selected_ = true;
        skipped_ = false;
      }

      //!Default destructor
      virtual ~tracked_data(){};

      //! constructor
      tracked_data(std::vector<cell> cells, std::vector<calorimeter_hit> calos, std::vector<cluster> clusters, std::vector<scenario> scenarios, std::vector<sequence> true_sequences, std::vector<sequence> nemo_sequences, prlevel level=mybhep::NORMAL, double nsigma=10.){
        set_print_level(level);
        set_nsigma(nsigma);
        appname_= "tracked_data: ";
        cells_ = cells;
        calos_ = calos;
        clusters_ = clusters;
        scenarios_ = scenarios;
        true_sequences_ = true_sequences;
        nemo_sequences_ = nemo_sequences;
        selected_ = true;
        skipped_ = false;
      }

      /*** dump ***/
      virtual void dump (ostream & a_out         = clog,
                         const string & a_title  = "",
                         const string & a_indent = "",
                         bool a_inherit          = false) const {
        string indent;
        if (! a_indent.empty ()) indent = a_indent;
        if (! a_title.empty ())
          {
            a_out << indent << a_title << endl;
          }

        a_out << indent << appname_ << " ------------------- " << endl;
        a_out << indent << " number of cells : " << cells_.size() << endl;
        for(std::vector<cell>::const_iterator icell=cells_.begin(); icell!=cells_.end();++icell)
          icell->dump(a_out, "",indent + "     ");
        a_out << indent << " number of calos : " << calos_.size() << endl;
        for(std::vector<calorimeter_hit>::const_iterator icalo=calos_.begin(); icalo!=calos_.end();++icalo)
          icalo->dump(a_out, "",indent + "     ");
        a_out << indent << " number of clusters : " << clusters_.size() << endl;
        for(std::vector<cluster>::const_iterator icluster=clusters_.begin(); icluster!=clusters_.end();++icluster)
          icluster->dump(a_out, "",indent + "     ");
        a_out << indent << " number of scenarios : " << scenarios_.size() << endl;
        for(std::vector<scenario>::const_iterator iscenario=scenarios_.begin(); iscenario!=scenarios_.end();++iscenario)
          iscenario->dump(a_out, "",indent + "     ");
        a_out << indent << " ------------------- " << endl;
 
        return;
      }
    
      //! set cells
      void set_cells(std::vector<cell> cells)
      {
        cells_ = cells;
      }

      //! set calos
      void set_calos(std::vector<calorimeter_hit> calos)
      {
        calos_ = calos;
      }

      //! set clusters
      void set_clusters(std::vector<cluster> clusters)
      {
        clusters_ = clusters;
      }

      //! set scenarios
      void set_scenarios(std::vector<scenario> scenarios)
      {
        scenarios_ = scenarios;
      }

      //! set true sequences
      void set_true_sequences(std::vector<sequence> sequences)
      {
        true_sequences_ = sequences;
      }

      //! set nemo sequences
      void set_nemo_sequences(std::vector<sequence> sequences)
      {
        nemo_sequences_ = sequences;
      }

      //! set selected
      void set_selected(bool selected)
      {
        selected_ = selected;
      }

      //! set skipped
      void set_skipped(bool skipped)
      {
        skipped_ = skipped;
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

      //! get calos 
      std::vector<calorimeter_hit>& get_calos()
      {
        return calos_;
      }

      const std::vector<calorimeter_hit>& get_calos()const
      {
        return calos_;
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

      //! get true sequences 
      std::vector<sequence>& get_true_sequences()
      {
        return true_sequences_;
      }
      
      const std::vector<sequence>& get_true_sequences()const
      {
        return true_sequences_;
      }

      //! get nemo sequences 
      std::vector<sequence>& get_nemo_sequences()
      {
        return nemo_sequences_;
      }

      const std::vector<sequence>& get_nemo_sequences()const
      {
        return nemo_sequences_;
      }
      //! get selected

      bool selected()const
      {
        return selected_;
      }

      //! get skipped
      bool skipped()const
      {
        return skipped_;
      }

  
    };
  }
}

#endif
