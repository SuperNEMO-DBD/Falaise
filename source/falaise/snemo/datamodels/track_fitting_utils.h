/// \file falaise/snemo/datamodels/track_fitting_utils.h
/* Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-12-13
 * Last modified: 2022-12-13
 *
 * Description: Track fitting utilities
 */

#ifndef FALAISE_SNEMO_DATAMODELS_TRACK_FITTING_UTILS_H
#define FALAISE_SNEMO_DATAMODELS_TRACK_FITTING_UTILS_H 1

// Third party:
// - Boost:
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>

namespace snemo {

  namespace datamodel {

    enum track_fit_algo_type
      {
        TRACK_FIT_ALGO_UNDEF = 0,
        TRACK_FIT_ALGO_LINE  = 1,
        TRACK_FIT_ALGO_HELIX = 2,
        TRACK_FIT_ALGO_OTHER = 100
      };

    std::string to_string(const track_fit_algo_type algo_);

    class track_fit_infos
      : public datatools::i_serializable
      , public datatools::i_tree_dumpable
    {
    public:

      track_fit_infos() = default;

      void reset();
  
      bool has_algo() const;
  
      void set_algo(const track_fit_algo_type algo_);

      track_fit_algo_type get_algo() const;
   
      bool has_guess() const;
  
      void set_guess(const std::string & guess_);

      const std::string & get_guess() const;
      
      bool has_t0() const;
      
      double get_t0() const;
  
      void set_t0(const double t0_);
  
      bool has_chi2() const;
    
      double get_chi2() const;
  
      void set_chi2(const double chi2_);
  
      bool has_ndof() const;
    
      uint32_t get_ndof() const;

      void set_ndof(const uint32_t ndof_);
 
      bool has_pvalue() const;
      
      double get_pvalue() const;
  
      void set_pvalue(const double pvalue_);
      
      bool is_best() const;
  
      void set_best(const bool best_);
    
      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;
 
    private:

      track_fit_algo_type _algo_ = TRACK_FIT_ALGO_UNDEF; ///< Fitting algorithm identifier
      std::string _guess_; ///< Fit initial guess (algo dependent)
      double _t0_ = datatools::invalid_real(); ///< Fitted shift drift time (for delayed tracks)
      double _chi2_ = datatools::invalid_real(); ///< Fit chi square
      uint32_t _ndof_ = 0; ///< Fit number of degrees of freedom
      double _pvalue_ = datatools::invalid_real(); ///< Fit P-value
      bool _best_ = false; ///< Best fit flag

      DATATOOLS_SERIALIZATION_DECLARATION()
   
    };
    
  } // end of namespace datamodel

} // end of namespace snemo

// // Class version:
// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(snemo::datamodel::track_fit_infos, 1)

#endif // FALAISE_SNEMO_DATAMODELS_TRACK_FITTING_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
