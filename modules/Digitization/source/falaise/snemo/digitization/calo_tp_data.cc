// snemo/digitization/calo_tp_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_tp_data.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calo_tp_data, "snemo::digitalization::calo_tp_data")

    calo_tp_data::calo_tp_data()
    {
      return;
    }

    calo_tp_data::~calo_tp_data()
    {   
      reset();
      return;
    }
    
    unsigned int calo_tp_data::get_clocktick_min_index() const
    {
			if(_calo_tps_.size() == 0)
				{
					return 0;
				}
      unsigned int index_with_min = 0;

      uint32_t clocktick_min = _calo_tps_[0].get().get_clocktick_25ns();
      
      for (unsigned int i = 1; i < _calo_tps_.size(); i++)
				{
					if (_calo_tps_[i].get().get_clocktick_25ns() < clocktick_min)
						{
							clocktick_min = _calo_tps_[i].get().get_clocktick_25ns();
							index_with_min = i;
						}
				}
      return index_with_min;
    }
			
    unsigned int calo_tp_data::get_clocktick_max_index() const
    {
			if(_calo_tps_.size() == 0)
				{
					return 0;
				}
      unsigned int index_with_max = 0;

      uint32_t clocktick_max = _calo_tps_[0].get().get_clocktick_25ns();
      
      for (unsigned int i = 1; i < _calo_tps_.size(); i++)
				{
					if (_calo_tps_[i].get().get_clocktick_25ns() > clocktick_max)
						{
							clocktick_max = _calo_tps_[i].get().get_clocktick_25ns();
							index_with_max = i;
						}
				}
      return index_with_max;
    }

		uint32_t calo_tp_data::get_clocktick_min() const
    {
			if(_calo_tps_.size() == 0)
				{
					return 0;
				}
      return _calo_tps_[get_clocktick_min_index()].get().get_clocktick_25ns();
    }

		uint32_t calo_tp_data::get_clocktick_max() const
    {
			if(_calo_tps_.size() == 0)
				{
					return 0;
				}
      return _calo_tps_[get_clocktick_max_index()].get().get_clocktick_25ns();
    }

		uint32_t calo_tp_data::get_clocktick_range() const
    {
			if(_calo_tps_.size() == 0)
				{
					return 0;
				}
      return get_clocktick_max() - get_clocktick_min();
    }
			
    void calo_tp_data::get_list_of_tp_per_clocktick(uint32_t clocktick_25ns_, calo_tp_collection_type & my_list_of_tps_per_clocktick_) const
    {
			if(_calo_tps_.size() == 0)
				{
					return;
				}
      for (unsigned int i = 0; i < _calo_tps_.size(); i++)
      	{
      	  if(_calo_tps_[i].get().get_clocktick_25ns() == clocktick_25ns_)
      	    {
      	      my_list_of_tps_per_clocktick_.push_back(_calo_tps_[i]);
      	    }
      	}
      return;
    }

    void calo_tp_data::get_list_of_tp_per_clocktick_per_crate(uint32_t clocktick_25ns_, unsigned int crate_number_, calo_tp_collection_type & my_list_of_tps_per_clocktick_per_crate_ ) const
    {
			if(_calo_tps_.size() == 0)
				{
					return;
				}
      for (unsigned int i = 0; i < _calo_tps_.size(); i++)
				{
					if(_calo_tps_[i].get().get_clocktick_25ns() == clocktick_25ns_ && _calo_tps_[i].get().get_geom_id().get(mapping::CRATE_INDEX) == crate_number_)
						{
							my_list_of_tps_per_clocktick_per_crate_.push_back(_calo_tps_[i]);
						}
				}
      return;
    }

    void calo_tp_data::reset_tps()
    {
      _calo_tps_.clear();
      return ;
    }
		
    calo_tp & calo_tp_data::add()
    {
      {
				calo_tp_handle_type dummy;
				_calo_tps_.push_back(dummy);
      }
      calo_tp_handle_type & last = _calo_tps_.back();
      last.reset(new calo_tp);
      return last.grab();
    }

    const calo_tp_data::calo_tp_collection_type & calo_tp_data::get_calo_tps() const
    {
      return _calo_tps_;
    }

    calo_tp_data::calo_tp_collection_type & calo_tp_data::grab_calo_tps() 
    {
      return _calo_tps_;
    }

    void calo_tp_data::reset()
    {
      reset_tps();
      return;
    }

    void calo_tp_data::tree_dump (std::ostream & out_,
																	const std::string & title_,
																	const std::string & indent_,
																	bool inherit_) const
    {
			
			out_ << indent_ << title_ << std::endl;
			
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
					 << "Calorimeter TP(s) : " << _calo_tps_.size() << std::endl;
      
      return;
    }

    void calo_tp_data::_check()
    {
      for (unsigned int i = 0; i < _calo_tps_.size() - 1; i++)
				{
					const calo_tp & tp_a = _calo_tps_[i].get();

					for (unsigned int j = i+1; j < _calo_tps_.size(); j++)
						{
							const calo_tp & tp_b = _calo_tps_[j].get();

							DT_THROW_IF(tp_a.get_clocktick_25ns() == tp_b.get_clocktick_25ns() 
													&& tp_a.get_geom_id() == tp_b.get_geom_id(),
													std::logic_error,
													"Duplicate clocktick=" << tp_a.get_clocktick_25ns() 
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
