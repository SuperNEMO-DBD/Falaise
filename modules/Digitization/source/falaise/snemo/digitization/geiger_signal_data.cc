// snemo/digitization/geiger_signal_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/geiger_signal_data.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    // DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(geiger_signal_data, "snemo::digitalization::geiger_signal_data")

    geiger_signal_data::geiger_signal_data()
    {
      _locked_ = false;
      return;
    }

    geiger_signal_data::~geiger_signal_data()
    {   
      reset();
      return;
    } 		       

    bool geiger_signal_data::is_locked() const
    {
      return _locked_;
    }
    
    void geiger_signal_data::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Geiger signal collection is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void geiger_signal_data::unlock()
    { 
      DT_THROW_IF(!is_locked(), std::logic_error, " Geiger signal collection is already unlocked ! ");
      _locked_ = false;
      return;
    }

    void geiger_signal_data::reset_signals()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Operation prohibited, object is locked ! ");
      _geiger_signals_.clear();
      return ;
    }
		
    geiger_signal & geiger_signal_data::add()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Operation prohibited, object is locked ! ");
      {
	geiger_signal_handle_type dummy;
	_geiger_signals_.push_back(dummy);
      }
      geiger_signal_handle_type & last = _geiger_signals_.back();
      last.reset(new geiger_signal);
      return last.grab();
    }

    const geiger_signal_data::geiger_signal_collection_type & geiger_signal_data::get_geiger_signals() const
    {
      return _geiger_signals_;
    }

		geiger_signal_data::geiger_signal_collection_type & geiger_signal_data::grab_geiger_signals()
    {
      return _geiger_signals_;
    }

    void geiger_signal_data::reset()
    {
      if (is_locked())
	{
	  unlock();
	}
      reset_signals();
      return;
    }

    void geiger_signal_data::tree_dump (std::ostream & out_,
				  const std::string & title_,
				  const std::string & indent_,
				  bool inherit_) const
    {
      out_ << indent_ << title_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Is locked SIGNALs  : " << _locked_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
	   << "Geiger SIGNALs : " << _geiger_signals_.size() << std::endl;
      
      return;
    }

    void geiger_signal_data::_check()
    {
      for (int i = 0; i < _geiger_signals_.size() - 1; i++)
	{
	  const geiger_signal & geiger_signal_a = _geiger_signals_[i].get();

	  for (int j = i+1; j < _geiger_signals_.size(); j++)
	    {
	      const geiger_signal & geiger_signal_b = _geiger_signals_[j].get();

	      DT_THROW_IF(geiger_signal_a.get_geom_id() == geiger_signal_b.get_geom_id(),
			  std::logic_error,
			  "GID=" << geiger_signal_b.get_geom_id());
	    }
	}
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
