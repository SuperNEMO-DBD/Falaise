// snemo/digitization/geiger_tp_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/geiger_tp_data.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(geiger_tp_data, "snemo::digitalization::geiger_tp_data")

    geiger_tp_data::geiger_tp_data()
    {
      _locked_ = false;
      return;
    }

    geiger_tp_data::~geiger_tp_data()
    {   
      reset();
      return;
    }
    
    int geiger_tp_data::get_clocktick_min_index() const
    {
      DT_THROW_IF(_geiger_tps_.size() == 0, std::logic_error, " Geiger TP collection is empty ! ");

      int index_with_min = 0;

      int32_t clocktick_min = _geiger_tps_[0].get().get_clocktick_800ns();
      
      for (int i = 1; i < _geiger_tps_.size(); i++)
	{
	  if (_geiger_tps_[i].get().get_clocktick_800ns() < clocktick_min)
	    {
	      clocktick_min = _geiger_tps_[i].get().get_clocktick_800ns();
	      index_with_min = i;
	    }
	}
      return index_with_min;
    }
			
    int geiger_tp_data::get_clocktick_max_index() const
    {
      DT_THROW_IF(_geiger_tps_.size() == 0, std::logic_error, " Geiger TP collection is empty ! ");

      int index_with_max = 0;

      int32_t clocktick_max = _geiger_tps_[0].get().get_clocktick_800ns();
      
      for (int i = 1; i < _geiger_tps_.size(); i++)
	{
	  if (_geiger_tps_[i].get().get_clocktick_800ns() > clocktick_max)
	    {
	      clocktick_max = _geiger_tps_[i].get().get_clocktick_800ns();
	      index_with_max = i;
	    }
	}
      return index_with_max;
    }

    int geiger_tp_data::get_clocktick_min() const
    {
      DT_THROW_IF(_geiger_tps_.size() == 0, std::logic_error, " Geiger TP collection is empty ! ");
      return _geiger_tps_[get_clocktick_min_index()].get().get_clocktick_800ns();
    }

    int geiger_tp_data::get_clocktick_max() const
    {
      DT_THROW_IF(_geiger_tps_.size() == 0, std::logic_error, " Geiger TP collection is empty ! ");
      return _geiger_tps_[get_clocktick_max_index()].get().get_clocktick_800ns();
    }

    int geiger_tp_data::get_clocktick_range() const
    {
      DT_THROW_IF(_geiger_tps_.size() == 0, std::logic_error, " Geiger TP collection is empty ! ");
      return get_clocktick_max() - get_clocktick_min();
    }
			
    void geiger_tp_data::get_list_of_gg_tp_per_clocktick(int32_t clocktick_800ns_, geiger_tp_collection_type & my_list_of_gg_tps_per_clocktick_) const
    {
      for (int i = 0; i < _geiger_tps_.size(); i++)
      	{
      	  if(_geiger_tps_[i].get().get_clocktick_800ns() == clocktick_800ns_)
      	    {
      	      my_list_of_gg_tps_per_clocktick_.push_back(_geiger_tps_[i]);
      	    }
      	}
      return;
    }

    void geiger_tp_data::get_list_of_gg_tp_per_clocktick_per_crate(int32_t clocktick_800ns_, unsigned int crate_number_, geiger_tp_collection_type & my_list_of_gg_tps_per_clocktick_per_crate_) const
    {
      for (int i = 0; i < _geiger_tps_.size(); i++)
				{
					if(_geiger_tps_[i].get().get_clocktick_800ns() == clocktick_800ns_ && _geiger_tps_[i].get().get_geom_id().get(mapping::CRATE_INDEX) == crate_number_)
						{
							my_list_of_gg_tps_per_clocktick_per_crate_.push_back(_geiger_tps_[i]);
						}
				}
      return;
    }

		void geiger_tp_data::get_list_of_gg_tp_per_eid(const geomtools::geom_id & electronic_id_, geiger_tp_collection_type & my_list_of_gg_tps_per_eid_) const
		{
			geomtools::geom_id temporary_feb_id;
      temporary_feb_id.set_type(electronic_id_.get_type());
      temporary_feb_id.set_depth(mapping::BOARD_DEPTH);
      electronic_id_.extract_to(temporary_feb_id);

      for (int i = 0; i < _geiger_tps_.size(); i++)
				{
					if(_geiger_tps_[i].get().get_geom_id() == temporary_feb_id)
						{
							my_list_of_gg_tps_per_eid_.push_back(_geiger_tps_[i]);
						}

				}
			return;
		}

    bool geiger_tp_data::is_locked() const
    {
      return _locked_;
    }
    
    void geiger_tp_data::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Geiger TP collection is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void geiger_tp_data::unlock()
    { 
      DT_THROW_IF(!is_locked(), std::logic_error, " Geiger TP collection is already unlocked ! ");
      _locked_ = false;
      return;
    }

    void geiger_tp_data::reset_tps()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Operation prohibited, object is locked ! ");
      _geiger_tps_.clear();
      return ;
    }
		
    geiger_tp & geiger_tp_data::add()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Operation prohibited, object is locked ! ");
      {
				geiger_tp_handle_type dummy;
				_geiger_tps_.push_back(dummy);
      }
      geiger_tp_handle_type & last = _geiger_tps_.back();
      last.reset(new geiger_tp);
      return last.grab();
    }

    const geiger_tp_data::geiger_tp_collection_type & geiger_tp_data::get_geiger_tps() const
    {
      return _geiger_tps_;
    }

		geiger_tp_data::geiger_tp_collection_type & geiger_tp_data::grab_geiger_tps()
    {
      return _geiger_tps_;
    }

    void geiger_tp_data::reset()
    {
      if (is_locked())
				{
					unlock();
				}
      reset_tps();
      return;
    }

    void geiger_tp_data::tree_dump (std::ostream & out_,
				  const std::string & title_,
				  const std::string & indent_,
				  bool inherit_) const
    {
      out_ << indent_ << title_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Is locked TPs  : " << _locked_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
	   << "Geiger TPs : " << _geiger_tps_.size() << std::endl;
      
      return;
    }

    void geiger_tp_data::_check()
    {
      for (int i = 0; i < _geiger_tps_.size() - 1; i++)
				{
					const geiger_tp & tp_a = _geiger_tps_[i].get();

					for (int j = i+1; j < _geiger_tps_.size(); j++)
						{
							const geiger_tp & tp_b = _geiger_tps_[j].get();

							DT_THROW_IF(tp_a.get_clocktick_800ns() == tp_b.get_clocktick_800ns() 
													&& tp_a.get_geom_id() == tp_b.get_geom_id(),
													std::logic_error,
													"Duplicate clocktick=" << tp_a.get_clocktick_800ns() 
													<< " * " 
													<< "GID=" << tp_b.get_geom_id());
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
