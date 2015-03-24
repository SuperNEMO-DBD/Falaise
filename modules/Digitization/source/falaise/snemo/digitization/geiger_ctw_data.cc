// geiger_ctw_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/geiger_ctw_data.h>

// Third party:
// - Bayeux/datatools:
//#include <datatools/utils.h>
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    // DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(geiger_ctw_data, "snemo::digitalization::geiger_ctw_data")

    geiger_ctw_data::geiger_ctw_data()
    {
      _locked_ctws_ = false;
      return;
    }

    geiger_ctw_data::~geiger_ctw_data()
    {   
      reset();
      return;
    }
    
    int geiger_ctw_data::get_clocktick_min_index() const
    {
      DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");

      int index_with_min = 0;

      int32_t clocktick_min = _geiger_ctws_[0].get().get_clocktick_800ns();
      
      for (int i = 1; i < _geiger_ctws_.size(); i++)
	{
	  if (_geiger_ctws_[i].get().get_clocktick_800ns() < clocktick_min)
	    {
	      clocktick_min = _geiger_ctws_[i].get().get_clocktick_800ns();
	      index_with_min = i;
	    }
	}
      return index_with_min;
    }
			
    int geiger_ctw_data::get_clocktick_max_index() const
    {
      DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");

      int index_with_max = 0;

      int32_t clocktick_max = _geiger_ctws_[0].get().get_clocktick_800ns();
      
      for (int i = 1; i < _geiger_ctws_.size(); i++)
	{
	  if (_geiger_ctws_[i].get().get_clocktick_800ns() > clocktick_max)
	    {
	      clocktick_max = _geiger_ctws_[i].get().get_clocktick_800ns();
	      index_with_max = i;
	    }
	}
      return index_with_max;
    }

    int geiger_ctw_data::get_clocktick_min() const
    {
      DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
      return _geiger_ctws_[get_clocktick_min_index()].get().get_clocktick_800ns();
    }

    int geiger_ctw_data::get_clocktick_max() const
    {
      DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
      return _geiger_ctws_[get_clocktick_max_index()].get().get_clocktick_800ns();
    }


    int geiger_ctw_data::get_clocktick_range() const
    {
      DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
      return get_clocktick_max() - get_clocktick_min();
    }
			
    void geiger_ctw_data::get_list_of_geiger_ctw_per_clocktick(int32_t clocktick_800ns_, geiger_ctw_collection_type & ctws_)
    {
      DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
      DT_THROW_IF(!is_locked_ctws(), std::logic_error, " Geiger CTW collection is not locked ! ");
      for (int i = 0; i < _geiger_ctws_.size(); i++)
      	{
      	  if(_geiger_ctws_[i].get().get_clocktick_800ns() == clocktick_800ns_)
      	    {
      	      ctws_.push_back(_geiger_ctws_[i]);
      	    }
      	}
      return;
    }

    bool geiger_ctw_data::is_locked_ctws() const
    {
      return _locked_ctws_;
    }
    
    void geiger_ctw_data::lock_ctws()
    {
      DT_THROW_IF(is_locked_ctws(), std::logic_error, " Geiger CTW collection is already locked ! ");
      _check();
      _locked_ctws_ = true;
      return;
    }
    
    void geiger_ctw_data::unlock_ctws()
    { 
      DT_THROW_IF(!is_locked_ctws(), std::logic_error, " Geiger CTW collection is already unlocked ! ");
      _locked_ctws_ = false;
      return;
    }

    void geiger_ctw_data::reset_ctws()
    {
      DT_THROW_IF(is_locked_ctws(), std::logic_error, " Operation prohibited, object is locked ! ");
      _geiger_ctws_.clear();
      return ;
    }
		
    geiger_ctw & geiger_ctw_data::add()
    {
      DT_THROW_IF(is_locked_ctws(), std::logic_error, " Operation prohibited, object is locked ! ");
      {
	geiger_ctw_handle_type dummy;
	_geiger_ctws_.push_back(dummy);
      }
      geiger_ctw_handle_type & last = _geiger_ctws_.back();
      last.reset(new geiger_ctw);
      return last.grab();
    }

    const geiger_ctw_data::geiger_ctw_collection_type & geiger_ctw_data::get_geiger_ctws() const
    {
      return _geiger_ctws_;
    }

    void geiger_ctw_data::reset()
    {
      if (is_locked_ctws())
	{
	  unlock_ctws();
	}
      reset_ctws();
      return;
    }

    void geiger_ctw_data::tree_dump (std::ostream & out_,
				   const std::string & title_,
				   const std::string & indent_,
				   bool inherit_) const
    {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Lock CTWs  : " << _locked_ctws_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
	   << "Geiger CTWs : " << _geiger_ctws_.size() << std::endl;

      return;
    }

    void geiger_ctw_data::_check()
    {
      DT_THROW_IF(_geiger_ctws_.size() == 0, std::logic_error, " Geiger CTW collection is empty ! ");
      for (int i = 0; i < _geiger_ctws_.size() - 1; i++)
	{
	  const geiger_ctw & ctw_a = _geiger_ctws_[i].get();

	  for (int j = i+1; j < _geiger_ctws_.size(); j++)
	    {
	      const geiger_ctw & ctw_b = _geiger_ctws_[j].get();

	      DT_THROW_IF(ctw_a.get_clocktick_800ns() == ctw_b.get_clocktick_800ns() 
			  && ctw_a.get_geom_id() == ctw_b.get_geom_id(),
			  std::logic_error,
			  "Duplicate clocktick=" << ctw_a.get_clocktick_800ns() 
			  << " * " 
			  << "GID=" << ctw_b.get_geom_id());
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
