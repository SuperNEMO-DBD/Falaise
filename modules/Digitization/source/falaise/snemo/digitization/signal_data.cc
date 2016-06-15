// snemo/digitization/signal_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/signal_data.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project :
#include <snemo/digitization/mapping.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    // DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(signal_data, "snemo::digitalization::signal_data")

    signal_data::signal_data()
    {
      _locked_ = false;
      return;
    }

    signal_data::~signal_data()
    {   
      reset();
      return;
    } 		       

    bool signal_data::is_locked() const
    {
      return _locked_;
    }
    
    void signal_data::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Geiger signal collection is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void signal_data::unlock()
    { 
      DT_THROW_IF(!is_locked(), std::logic_error, " Geiger signal collection is already unlocked ! ");
      _locked_ = false;
      return;
    }

    void signal_data::reset_geiger_signals()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Operation prohibited, object is locked ! ");
      _geiger_signals_.clear();
      return ;
    }

    void signal_data::reset_calo_signals()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Operation prohibited, object is locked ! ");
      _calo_signals_.clear();
      return ;
    }

		
		geiger_signal & signal_data::add_geiger_signal()
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

    calo_signal & signal_data::add_calo_signal()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Operation prohibited, object is locked ! ");
      {
				calo_signal_handle_type dummy;
				_calo_signals_.push_back(dummy);
      }
      calo_signal_handle_type & last = _calo_signals_.back();
      last.reset(new calo_signal);
      return last.grab();
    }

    const signal_data::geiger_signal_collection_type & signal_data::get_geiger_signals() const
    {
      return _geiger_signals_;
    }

    signal_data::geiger_signal_collection_type & signal_data::grab_geiger_signals()
    {
      return _geiger_signals_;
    }
		
		std::size_t signal_data::get_number_of_geiger_signals() const
		{
			return _geiger_signals_.size();
		}
		
		std::size_t signal_data::get_number_of_prompt_geiger_signals(double time_limit_) const
		{
			std::size_t number_of_prompt_gg_signal = 0;
			std::vector<datatools::handle<geiger_signal> >::const_iterator it_signal = _geiger_signals_.begin();
			for (; it_signal != _geiger_signals_.end(); it_signal++)
				{
					geiger_signal_handle_type a_gg_signal_handle = *it_signal;
					geiger_signal a_gg_signal = a_gg_signal_handle.get();
					
					if (a_gg_signal.get_anode_avalanche_time() < time_limit_)
						{
							number_of_prompt_gg_signal++;
						}
				}
			
			return number_of_prompt_gg_signal;
		}
		
		std::size_t signal_data::get_number_of_delayed_geiger_signals(double time_limit_) const
		{
			std::size_t number_of_delayed_gg_signal = 0;
			std::vector<datatools::handle<geiger_signal> >::const_iterator it_signal = _geiger_signals_.begin();
			for (; it_signal != _geiger_signals_.end(); it_signal++)
				{
					geiger_signal_handle_type a_gg_signal_handle = *it_signal;
					geiger_signal a_gg_signal = a_gg_signal_handle.get();
					
					if (a_gg_signal.get_anode_avalanche_time() > time_limit_)
						{
							number_of_delayed_gg_signal++;
						}
				}
			return number_of_delayed_gg_signal;
		}

    const signal_data::calo_signal_collection_type & signal_data::get_calo_signals() const
    {
      return _calo_signals_;
    }

    signal_data::calo_signal_collection_type & signal_data::grab_calo_signals()
    {
      return _calo_signals_;
    }

		std::size_t signal_data::get_number_of_calo_signals() const
		{
			return _calo_signals_.size();
		}
		
		std::size_t signal_data::get_number_of_main_calo_signals() const
		{
			std::size_t number_of_main_calo_signal = 0;
			std::vector<datatools::handle<calo_signal> >::const_iterator it_signal = _calo_signals_.begin();
			for (; it_signal != _calo_signals_.end(); it_signal++)
				{
					calo_signal_handle_type a_calo_signal_handle = *it_signal;
					calo_signal a_calo_signal = a_calo_signal_handle.get();
					
					const geomtools::geom_id a_calo_gid = a_calo_signal.get_geom_id();
					if(a_calo_gid.get_type() == mapping::CALO_MAIN_WALL_CATEGORY_TYPE)
						{
							number_of_main_calo_signal++;
						}
				}
			
			return number_of_main_calo_signal;
		}

		std::size_t signal_data::get_number_of_xcalo_signals() const
		{
			std::size_t number_of_xcalo_signal = 0;
			std::vector<datatools::handle<calo_signal> >::const_iterator it_signal = _calo_signals_.begin();
			for (; it_signal != _calo_signals_.end(); it_signal++)
				{
					calo_signal_handle_type a_calo_signal_handle = *it_signal;
					calo_signal a_calo_signal = a_calo_signal_handle.get();
					
					const geomtools::geom_id a_calo_gid = a_calo_signal.get_geom_id();
					if(a_calo_gid.get_type() == mapping::CALORIMETER_X_WALL_CATEGORY_TYPE)
						{
							number_of_xcalo_signal++;
						}
				}
			
			return number_of_xcalo_signal;
		}


		std::size_t signal_data::get_number_of_gveto_signals() const
		{
			std::size_t number_of_gveto_signal = 0;
			std::vector<datatools::handle<calo_signal> >::const_iterator it_signal = _calo_signals_.begin();
			for (; it_signal != _calo_signals_.end(); it_signal++)
				{
					calo_signal_handle_type a_calo_signal_handle = *it_signal;
					calo_signal a_calo_signal = a_calo_signal_handle.get();
					
					const geomtools::geom_id a_calo_gid = a_calo_signal.get_geom_id();
					if(a_calo_gid.get_type() == mapping::CALORIMETER_GVETO_CATEGORY_TYPE)
						{
							number_of_gveto_signal++;
						}
				}
			
			return number_of_gveto_signal;
		}
		
		bool signal_data::has_geiger_signals()
		{
			if (_geiger_signals_.size() != 0)
				{
					return 1;
				}
			return 0;
		}
		
		bool signal_data::has_calo_signals()
		{
			if (_calo_signals_.size() != 0)
				{
					return 1;
				}
			return 0;
		}

    void signal_data::reset()
    {
      if (is_locked())
				{
					unlock();
				}
      reset_geiger_signals();
      reset_calo_signals();
      return;
    }
    void signal_data::tree_dump (std::ostream & out_,
																 const std::string & title_,
																 const std::string & indent_,
																 bool inherit_) const
    {
      out_ << indent_ << title_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Is locked signals  : " << _locked_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
					 << "Geiger signals : " << _geiger_signals_.size() << std::endl;

			out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
					 << "Calo signals : " << _calo_signals_.size() << std::endl;

      
      return;
    }

    void signal_data::_check()
    {
      for (unsigned int i = 0; i < _geiger_signals_.size() - 1; i++)
				{
					const geiger_signal & geiger_signal_a = _geiger_signals_[i].get();

					for (unsigned int j = i+1; j < _geiger_signals_.size(); j++)
						{
							const geiger_signal & geiger_signal_b = _geiger_signals_[j].get();

							DT_THROW_IF(geiger_signal_a.get_geom_id() == geiger_signal_b.get_geom_id(),
													std::logic_error,
													"GID=" << geiger_signal_b.get_geom_id());
						}
				}

      for (unsigned int i = 0; i < _calo_signals_.size() - 1; i++)
				{
					const calo_signal & calo_signal_a = _calo_signals_[i].get();

					for (unsigned int j = i+1; j < _calo_signals_.size(); j++)
						{
							const calo_signal & calo_signal_b = _calo_signals_[j].get();

							DT_THROW_IF(calo_signal_a.get_geom_id() == calo_signal_b.get_geom_id(),
													std::logic_error,
													"GID=" << calo_signal_b.get_geom_id());
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
