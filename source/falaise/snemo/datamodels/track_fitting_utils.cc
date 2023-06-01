// falaise/snemo/datamodels/track_fitting_utils.cc

// Ourselves:
#include <falaise/snemo/datamodels/track_fitting_utils.h>

namespace snemo {

  namespace datamodel {

    std::string to_string(const track_fit_algo_type algo_)
    {
      if (algo_ == TRACK_FIT_ALGO_LINE) return std::string("line");
      if (algo_ == TRACK_FIT_ALGO_HELIX) return std::string("helix");
      if (algo_ == TRACK_FIT_ALGO_OTHER) return std::string("other");
      return std::string("");
    }

    // serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(track_fit_infos,
                                                      "snemo::datamodel::track_fit_infos")

    void track_fit_infos::reset()
    {
      _algo_ = TRACK_FIT_ALGO_UNDEF;
      _guess_.clear();
      _t0_ = datatools::invalid_real();   
      _chi2_ = datatools::invalid_real();
      _ndof_ = 0; 
      _pvalue_ = datatools::invalid_real(); 
      _best_ = false;
      return;     
    }
  
    bool track_fit_infos::has_algo() const
    {
      return _algo_ != TRACK_FIT_ALGO_UNDEF;
    }
    
    void track_fit_infos::set_algo(const track_fit_algo_type algo_)
    {
      _algo_ = algo_;
      return;
    }

    track_fit_algo_type track_fit_infos::get_algo() const
    {
      return _algo_;
    }
   
    bool track_fit_infos::has_guess() const
    {
      return not _guess_.empty();
    }
  
    void track_fit_infos::set_guess(const std::string & guess_)
    {
      _guess_ = guess_;
      return;
    }

    const std::string & track_fit_infos::get_guess() const
    {
      return _guess_;
    }
                
    bool track_fit_infos::has_t0() const
    {
      return datatools::is_valid(_t0_);
    }
       
    double track_fit_infos::get_t0() const
    {
      return _t0_;
    }
  
    void track_fit_infos::set_t0(const double t0_)
    {
      _t0_ = t0_;
    }
                
    bool track_fit_infos::has_chi2() const
    {
      return datatools::is_valid(_chi2_);
    }
     
    double track_fit_infos::get_chi2() const
    {
      return _chi2_;
    }
  
    void track_fit_infos::set_chi2(const double chi2_)
    {
      _chi2_ = chi2_;
    }
              
    bool track_fit_infos::has_ndof() const
    {
      return _ndof_ > 0;
    }
     
    uint32_t track_fit_infos::get_ndof() const
    {
      return _ndof_;
    }

    void track_fit_infos::set_ndof(const uint32_t ndof_)
    {
      _ndof_ = ndof_;
    }
               
    bool track_fit_infos::has_pvalue() const
    {
      return datatools::is_valid(_pvalue_);
    }
     
  
    double track_fit_infos::get_pvalue() const
    {
      return _pvalue_;
    }

    void track_fit_infos::set_pvalue(const double pvalue_)
    {
      _pvalue_ = pvalue_;
    }
     
    bool track_fit_infos::is_best() const
    {
      return _best_;
    }
  
    void track_fit_infos::set_best(const bool best_)
    {
      _best_ = best_;
    }

    void track_fit_infos::tree_dump(std::ostream & out,
                                    const std::string & title,
                                    const std::string & indent,
                                    bool is_last) const
    {
      if (not title.empty()) {
        out << indent << title << std::endl;
      }
      out << indent << datatools::i_tree_dumpable::tag << "Algorithm : '" << to_string(_algo_) << "'" << std::endl;
      out << indent << datatools::i_tree_dumpable::tag << "Guess : '" << _guess_ << "'" << std::endl;
      out << indent << datatools::i_tree_dumpable::tag << "Chi2 : " << _chi2_ << std::endl;
      out << indent << datatools::i_tree_dumpable::tag << "Ndof : " << _ndof_ << std::endl;
      out << indent << datatools::i_tree_dumpable::tag << "P-value : " << _pvalue_ << std::endl;
      out << indent << datatools::i_tree_dumpable::inherit_tag(is_last)
          << "Best  : " << std::boolalpha << _best_ << std::endl;
      return;
    }

  } // end of namespace datamodel

} // end of namespace snemo
