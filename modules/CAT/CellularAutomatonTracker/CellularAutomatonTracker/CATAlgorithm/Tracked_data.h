/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__STRACKED_DATA
#define __CATAlgorithm__STRACKED_DATA
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
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/node.h>
#include <CATAlgorithm/calorimeter_hit.h>
#include <CATAlgorithm/scenario.h>
#include <CATAlgorithm/tracked_data_base.h>
#include <CATAlgorithm/Cell.h>
#include <CATAlgorithm/Sequence.h>


namespace CAT{
  namespace topology{


    class Tracked_data : public tracked_data {

      // a Tracked_data is composed of a list of cells
      // a list of clusters
      // and a list of scenarios

    protected:
      std::string appname_;

    public:

      // list of cells
      std::vector<Cell> cells_;

      // list of sequences
      std::vector<Sequence> sequences_;

      // list of unclustered cells
      std::vector<topology::Cell> unclustered_cells_;

      //!Default constructor
      Tracked_data()
      {
        appname_= "Tracked_data: ";
      }

      //!Default destructor
      virtual ~Tracked_data(){};

      //! set cells
      void set_cells(const std::vector<Cell> & cells)
      {
        cells_ = cells;
      }

      //! set unclustered cells
      void set_unclustered_cells(const std::vector<Cell> & cells)
      {
        unclustered_cells_ = cells;
      }

      //! set sequences
      void set_sequences(const std::vector<Sequence> & sequences)
      {
        sequences_ = sequences;
      }

      //! get cells
      std::vector<Cell>& get_cells()
      {
        return cells_;
      }

      const std::vector<Cell>& get_cells()const
      {
        return cells_;
      }

      //! get unclustered cells
      std::vector<Cell>& get_unclustered_cells()
      {
        return unclustered_cells_;
      }

      const std::vector<Cell>& get_unclustered_cells()const
      {
        return unclustered_cells_;
      }

      //! get sequences
      std::vector<Sequence>& get_sequences()
      {
        return sequences_;
      }

      const std::vector<Sequence>& get_sequences()const
      {
        return sequences_;
      }

      void reset(){
	cells_.clear();
	sequences_.clear();
	unclustered_cells_.clear();
      }


    };
  }
}

#endif
