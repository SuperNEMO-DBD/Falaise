// snemo/digitization/calo_ctw_data.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_ctw_data.h>

// Third party:
// - Bayeux/datatools:
//#include <datatools/utils.h>
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    // Serial tag for datatools::serialization::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(calo_ctw_data, "snemo::digitalization::calo_ctw_data")

    calo_ctw_data::calo_ctw_data()
    {
      _locked_ = false;
      return;
    }

    calo_ctw_data::~calo_ctw_data()
    {   
      reset();
      return;
    }
    
    int calo_ctw_data::get_clocktick_min_index() const
    {
      DT_THROW_IF(_calo_ctws_.size() == 0, std::logic_error, " Calorimeter CTW collection is empty ! ");

      unsigned int index_with_min = 0;

      int32_t clocktick_min = _calo_ctws_[0].get().get_clocktick_25ns();
      
      for (unsigned int i = 1; i < _calo_ctws_.size(); i++)
				{
					if (_calo_ctws_[i].get().get_clocktick_25ns() < clocktick_min)
						{
							clocktick_min = _calo_ctws_[i].get().get_clocktick_25ns();
							index_with_min = i;
						}
				}
      return index_with_min;
    }
			
    int calo_ctw_data::get_clocktick_max_index() const
    {
      DT_THROW_IF(_calo_ctws_.size() == 0, std::logic_error, " Calorimeter CTW collection is empty ! ");

      unsigned int index_with_max = 0;

      int32_t clocktick_max = _calo_ctws_[0].get().get_clocktick_25ns();
      
      for (unsigned int i = 1; i < _calo_ctws_.size(); i++)
				{
					if (_calo_ctws_[i].get().get_clocktick_25ns() > clocktick_max)
						{
							clocktick_max = _calo_ctws_[i].get().get_clocktick_25ns();
							index_with_max = i;
						}
				}
      return index_with_max;
    }

    int calo_ctw_data::get_clocktick_min() const
    {
      DT_THROW_IF(_calo_ctws_.size() == 0, std::logic_error, " Calorimeter CTW collection is empty ! ");
      return _calo_ctws_[get_clocktick_min_index()].get().get_clocktick_25ns();
    }

    int calo_ctw_data::get_clocktick_max() const
    {
      DT_THROW_IF(_calo_ctws_.size() == 0, std::logic_error, " Calorimeter CTW collection is empty ! ");
      return _calo_ctws_[get_clocktick_max_index()].get().get_clocktick_25ns();
    }


    int calo_ctw_data::get_clocktick_range() const
    {
      DT_THROW_IF(_calo_ctws_.size() == 0, std::logic_error, " Calorimeter CTW collection is empty ! ");
      return get_clocktick_max() - get_clocktick_min();
    }
			
    void calo_ctw_data::get_list_of_calo_ctw_per_clocktick(int32_t clocktick_25ns_, calo_ctw_collection_type & ctws_) const
    {
      DT_THROW_IF(_calo_ctws_.size() == 0, std::logic_error, " Calorimeter CTW collection is empty ! ");
      // DT_THROW_IF(!is_locked(), std::logic_error, " Calorimeter CTW collection is not locked ! ");
      for (unsigned int i = 0; i < _calo_ctws_.size(); i++)
      	{
      	  if(_calo_ctws_[i].get().get_clocktick_25ns() == clocktick_25ns_)
      	    {
      	      ctws_.push_back(_calo_ctws_[i]);
      	    }
      	}
      return;
    }

    bool calo_ctw_data::is_locked() const
    {
      return _locked_;
    }
    
    void calo_ctw_data::lock()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Calorimeter CTW collection is already locked ! ");
      _check();
      _locked_ = true;
      return;
    }
    
    void calo_ctw_data::unlock()
    { 
      DT_THROW_IF(!is_locked(), std::logic_error, " Calorimeter CTW collection is already unlocked ! ");
      _locked_ = false;
      return;
    }

    void calo_ctw_data::reset_ctws()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Operation prohibited, object is locked ! ");
      _calo_ctws_.clear();
      return ;
    }
		
    calo_ctw & calo_ctw_data::add()
    {
      DT_THROW_IF(is_locked(), std::logic_error, " Operation prohibited, object is locked ! ");
      {
				calo_ctw_handle_type dummy;
				_calo_ctws_.push_back(dummy);
      }
      calo_ctw_handle_type & last = _calo_ctws_.back();
      last.reset(new calo_ctw);
      return last.grab();
    }

    const calo_ctw_data::calo_ctw_collection_type & calo_ctw_data::get_calo_ctws() const
    {
      return _calo_ctws_;
    }
	 
		bool calo_ctw_data::has_calo_ctw() const
		{
			if (_calo_ctws_.empty() == true) return false;
			else return true;			
		}
		
    void calo_ctw_data::reset()
    {
      if (is_locked())
				{
					unlock();
				}
      reset_ctws();
      return;
    }

    void calo_ctw_data::tree_dump (std::ostream & out_,
																	 const std::string & title_,
																	 const std::string & indent_,
																	 bool inherit_) const
    {

			out_ << indent_ << title_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Lock CTWs  : " << _locked_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
					 << "Calorimeter CTWs : " << _calo_ctws_.size() << std::endl;

      return;
    }

    void calo_ctw_data::_check()
    {
      DT_THROW_IF(_calo_ctws_.size() == 0, std::logic_error, " Calorimeter CTW collection is empty ! ");
      for (unsigned int i = 0; i < _calo_ctws_.size() - 1; i++)
				{
					const calo_ctw & ctw_a = _calo_ctws_[i].get();

					for (unsigned int j = i+1; j < _calo_ctws_.size(); j++)
						{
							const calo_ctw & ctw_b = _calo_ctws_[j].get();

							DT_THROW_IF(ctw_a.get_clocktick_25ns() == ctw_b.get_clocktick_25ns() 
													&& ctw_a.get_geom_id() == ctw_b.get_geom_id(),
													std::logic_error,
													"Duplicate clocktick=" << ctw_a.get_clocktick_25ns() 
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
