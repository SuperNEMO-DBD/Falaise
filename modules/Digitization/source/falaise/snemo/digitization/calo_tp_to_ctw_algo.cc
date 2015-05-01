// snemo/digitization/calo_tp_to_ctw_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_tp_to_ctw_algo.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    calo_tp_to_ctw_algo::calo_tp_to_ctw_algo()
    {
      _initialized_ = false;
      return;
    }

    calo_tp_to_ctw_algo::~calo_tp_to_ctw_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    bool calo_tp_to_ctw_algo::is_initialized() const
    {
      return _initialized_;
    }

    void calo_tp_to_ctw_algo::initialize()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo tp to ctw algo is already initialized ! ");
      _initialized_ = true;
      return;
    }

    void calo_tp_to_ctw_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo tp to ctw algo is not initialized, it can't be reset ! ");
      _initialized_ = false;
      return;
    }

    void calo_tp_to_ctw_algo::set_ctw_clocktick(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_)
    {
      my_ctw_.set_clocktick_25ns(my_calo_tp_.get_clocktick_25ns());
      return;
    }

    void calo_tp_to_ctw_algo::set_ctw_htm(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_)
    {
      if(my_ctw_.get_htm_pc_info() != 0)
	{
	  unsigned int ctw_multiplicity = my_ctw_.get_htm_pc_info();
	  my_ctw_.set_htm_pc(ctw_multiplicity + my_calo_tp_.get_htm());
	}
      else
	{
	  my_ctw_.set_htm_pc(my_calo_tp_.get_htm());
	}
      return;
    }

    void calo_tp_to_ctw_algo::set_ctw_zone_bit(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_)
    {     
      if (my_calo_tp_.get_htm() != 0)
	{
	  unsigned int activated_zone_id = 0;	      
	  if(my_calo_tp_.get_geom_id().get(mapping::BOARD_INDEX) > 9)
	    {
	      activated_zone_id = (my_calo_tp_.get_geom_id().get(mapping::BOARD_INDEX) + BOARD_SHIFT_INDEX) / 2;
	    }
	  else
	    {
	      activated_zone_id = my_calo_tp_.get_geom_id().get(mapping::BOARD_INDEX) / 2;
	    }	      
	  my_ctw_.set_zoning_bit(calo_ctw::ZONING_BIT0 + activated_zone_id, 1);	      
	}          
      return ;
    }

    void calo_tp_to_ctw_algo::set_ctw_lto(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_)
    {    
      if (my_calo_tp_.is_lto() == true)
	{
	  my_ctw_.set_lto_pc_bit(1);
	}
      return;
    }
           
    void calo_tp_to_ctw_algo::_fill_a_calo_ctw(const calo_tp & my_calo_tp_, calo_ctw & a_calo_ctw_)
    {  
      a_calo_ctw_.set_header(my_calo_tp_.get_hit_id(),
			     my_calo_tp_.get_geom_id(),
			     my_calo_tp_.get_clocktick_25ns(),
			     calo_ctw::MAIN_WALL); //1 == wall type undefined for the moment
      set_ctw_htm(my_calo_tp_, a_calo_ctw_);
      set_ctw_zone_bit(my_calo_tp_, a_calo_ctw_);
      set_ctw_lto(my_calo_tp_, a_calo_ctw_);
      return;    
    }
    
    void calo_tp_to_ctw_algo::_process_for_a_ctw_for_a_clocktick(const std::vector<datatools::handle<calo_tp> > & my_list_of_calo_tp_,  calo_ctw & a_calo_ctw_) 
    {       
      for(int i = 0; i < my_list_of_calo_tp_.size(); i++)
	{
	  const calo_tp & my_calo_tp = my_list_of_calo_tp_[i].get();
	  _fill_a_calo_ctw(my_calo_tp, a_calo_ctw_);	  
	}
      return;
    }

    void calo_tp_to_ctw_algo::process(const calo_tp_data & calo_tp_data_,  calo_ctw_data & calo_ctw_data_)
    { 
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo tp to ctw algo is not initialized, it can't process ! ");

      for(int32_t i = calo_tp_data_.get_clocktick_min(); i <= calo_tp_data_.get_clocktick_max(); i++)
	{
	  for(int32_t j = 0 ; j <= mapping::NUMBER_OF_CRATES ; j++) 
	    {	  
	      std::vector<datatools::handle<calo_tp> > tp_list_per_clocktick_per_crate;
	      calo_tp_data_.get_list_of_tp_per_clocktick_per_crate(i,j,tp_list_per_clocktick_per_crate);
	      if(!tp_list_per_clocktick_per_crate.empty())
		{
		  calo_ctw & a_ctw_ = calo_ctw_data_.add();
		  _process_for_a_ctw_for_a_clocktick(tp_list_per_clocktick_per_crate, a_ctw_);
		  // a_ctw_.tree_dump(std::clog, "a_calo_ctw : ", "INFO : ");
		}
	    }
	}
      return;
    }
    
  } // end of namespace digitization

} // end of namespace snemo
