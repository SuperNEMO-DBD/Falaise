// calo_algo_tp_to_ctw.cc
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
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo tp to ctw algo is already initialized ! ")


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
	  my_ctw_.set_htm_pc(ctw_multiplicity + my_calo_tp_.get_htm_multiplicity());
	}
      else
	{
	  my_ctw_.set_htm_pc(my_calo_tp_.get_htm_multiplicity());
	}
      return;
    }

    void calo_tp_to_ctw_algo::set_ctw_zone_bit(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_)
    {     
      if (my_calo_tp_.get_htm_multiplicity() != 0)
	{
	  unsigned int activated_zone_id = 0;	      
	  if(my_calo_tp_.get_geom_id().get(calo_tp::BOARD_INDEX) > 9)
	    {
	      activated_zone_id = (my_calo_tp_.get_geom_id().get(calo_tp::BOARD_INDEX) + BOARD_SHIFT_INDEX) / 2;
	    }
	  else
	    {
	      activated_zone_id = my_calo_tp_.get_geom_id().get(calo_tp::BOARD_INDEX) / 2;
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
      set_ctw_clocktick(my_calo_tp_, a_calo_ctw_);
      set_ctw_htm(my_calo_tp_, a_calo_ctw_);
      set_ctw_zone_bit(my_calo_tp_, a_calo_ctw_);
      set_ctw_lto(my_calo_tp_, a_calo_ctw_);
      return;    
    }
    
    void calo_tp_to_ctw_algo::_process_for_a_ctw_for_a_clocktick(const std::vector<datatools::handle<calo_tp> > & my_list_of_calo_tp_,  calo_ctw & a_calo_ctw_) 
    {       
      for(int i = 0; i < my_list_of_calo_tp_.size(); i++)
	{
	  a_calo_ctw_.grab_geom_id().set_type(1000);
	  a_calo_ctw_.grab_geom_id().set_address(0, my_list_of_calo_tp_[0].get().get_geom_id().get(calo_tp::CRATE_INDEX), 10);
	  const calo_tp & my_calo_tp = my_list_of_calo_tp_[i].get();
	  _fill_a_calo_ctw(my_calo_tp, a_calo_ctw_);	  
	}
      std::clog << "DEBUG : process_for_a_ctw/clocktick number of TP : " << my_list_of_calo_tp_.size() << std::endl;
      return;
    }

    void calo_tp_to_ctw_algo::process(const calo_tp_data & tp_data_,  calo_ctw_data & ctw_data_)
    { 
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo tp to ctw algo is not initialized, it can't process ! ");

      for(int32_t i = tp_data_.get_clocktick_min(); i <= tp_data_.get_clocktick_max(); i++)
	{
	  for(int j = 0 ; j < 3 ; j++) //crate number
	    {	  
	      std::vector<datatools::handle<calo_tp> > tp_list_per_clocktick_per_crate;
	      tp_data_.get_list_of_tp_per_clocktick_per_crate(i,j,tp_list_per_clocktick_per_crate);
	      if(!tp_list_per_clocktick_per_crate.empty())
		{
		  calo_ctw & a_ctw_ = ctw_data_.add();
		  _process_for_a_ctw_for_a_clocktick(tp_list_per_clocktick_per_crate,a_ctw_);
		  a_ctw_.tree_dump(std::clog, "a_calo_ctw : ", "INFO : ");
		}
	    }
	}
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
