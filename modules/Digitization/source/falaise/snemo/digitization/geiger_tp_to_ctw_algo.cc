// geiger_tp_to_ctw_algo.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/geiger_tp_to_ctw_algo.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {
  
  namespace digitization {

    geiger_tp_to_ctw_algo::geiger_tp_to_ctw_algo()
    {
      _initialized_ = false;
      return;
    }

    geiger_tp_to_ctw_algo::~geiger_tp_to_ctw_algo()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    bool geiger_tp_to_ctw_algo::is_initialized() const
    {
      return _initialized_;
    }

    void geiger_tp_to_ctw_algo::initialize()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Geiger tp to ctw algo is already initialized ! ")

      _initialized_ = true;
      return;
    }

    void geiger_tp_to_ctw_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Geiger tp to ctw algo is not initialized, it can't be reset ! ");
      _initialized_ = false;
      return;
    }
    
    unsigned int geiger_tp_to_ctw_algo::board_to_index_mapping(uint32_t board_id_)
    {
      return board_id_;   
    }    

    // void geiger_tp_to_ctw_algo::set_ctw_clocktick(int32_t a_clocktick_, geiger_ctw & a_geiger_ctw_)
    // {
    //   if (!a_geiger_ctw_.has_clocktick_800ns())
    // 	{
    // 	  a_geiger_ctw_.set_clocktick_800ns(a_clocktick_);
    // 	}
      
    //   DT_THROW_IF (a_geiger_ctw_.get_clocktick_800ns() !=  a_clocktick_, std::logic_error, "Unmatching Geiger TP clocktick 800 ns ! ");
    //   return;
    // }

    void geiger_tp_to_ctw_algo::_fill_a_geiger_ctw(const geiger_tp & my_geiger_tp_, geiger_ctw & a_geiger_ctw_)
    {  
      unsigned int block_index = my_geiger_tp_.get_geom_id().get(mapping::BOARD_INDEX);
      a_geiger_ctw_.set_100_bits_in_ctw_word(block_index, my_geiger_tp_.get_gg_bitset());
      return;    
    }
    
    void geiger_tp_to_ctw_algo::_process_for_a_ctw_for_a_clocktick(const std::vector<datatools::handle<geiger_tp> > & my_list_of_geiger_tp_,  geiger_ctw & a_geiger_ctw_) 
    {       
      for(int i = 0; i < my_list_of_geiger_tp_.size(); i++)
	{
	  const geiger_tp & my_geiger_tp = my_list_of_geiger_tp_[i].get();
	  _fill_a_geiger_ctw(my_geiger_tp, a_geiger_ctw_);	  
	}
      return;
    }

    void geiger_tp_to_ctw_algo::process(const geiger_tp_data & geiger_tp_data_,  geiger_ctw_data & geiger_ctw_data_)
    { 
      DT_THROW_IF(!is_initialized(), std::logic_error, "Geiger tp to ctw algo is not initialized, it can't process ! ");

      for(int32_t i = geiger_tp_data_.get_clocktick_min(); i <= geiger_tp_data_.get_clocktick_max(); i++)
      	{
      	  for(int j = 0 ; j < 3 ; j++) //crate number
      	    {	  
      	      std::vector<datatools::handle<geiger_tp> > geiger_tp_list_per_clocktick_per_crate;
      	      geiger_tp_data_.get_list_of_gg_tp_per_clocktick_per_crate(i, j, geiger_tp_list_per_clocktick_per_crate);
      	      if(! geiger_tp_list_per_clocktick_per_crate.empty())
      		{
      		  geiger_ctw & a_geiger_ctw_ = geiger_ctw_data_.add();
		  a_geiger_ctw_.set_header(geiger_ctw_data_.get_geiger_ctws().size(), j, i); //Change hit id 
      		  _process_for_a_ctw_for_a_clocktick(geiger_tp_list_per_clocktick_per_crate, a_geiger_ctw_);
      		  a_geiger_ctw_.tree_dump(std::clog, "a_geiger_ctw : ", "INFO : ");
      		}
      	    }
      	}
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
