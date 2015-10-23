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
      _crate_number_ = -1;
      _mode_ = MODE_UNDEFINED;
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
      DT_THROW_IF(_crate_number_ == -1, std::logic_error, "Crate number is not defined ! ");
      if (_crate_number_ == 0 || _crate_number_ == 1) _set_mode(MODE_MAIN_WALL);
      if (_crate_number_ == 2) _set_mode(MODE_XWALL_GVETO);
      _initialized_ = true;
      return;
    }

    void calo_tp_to_ctw_algo::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo tp to ctw algo is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _crate_number_ = -1;
      _mode_ = MODE_UNDEFINED;
      return;
    }
    
    void calo_tp_to_ctw_algo::set_crate_number(unsigned int crate_number_)
    {
      DT_THROW_IF(((crate_number_ < 0) || (crate_number_ >= mapping::NUMBER_OF_CRATES)), std::logic_error, "Crate number can't be set because crate number [" << _crate_number_ << "] is not valid ! ");

      _crate_number_ = crate_number_;

      if (_crate_number_ == mapping::MAIN_CALO_SIDE_0_CRATE || _crate_number_ == mapping::MAIN_CALO_SIDE_1_CRATE) _set_mode(MODE_MAIN_WALL);
      if (_crate_number_ == mapping::XWALL_GVETO_CALO_CRATE) _set_mode(MODE_XWALL_GVETO);
      return;
    }

    int calo_tp_to_ctw_algo::get_crate_number() const
    {
      return _crate_number_;
    }
		
    snemo::digitization::calo_tp_to_ctw_algo::mode_type calo_tp_to_ctw_algo::get_mode() const
    {
      return _mode_;
    }

    bool calo_tp_to_ctw_algo::is_main_wall() const
    {
      if (_mode_ == MODE_MAIN_WALL) return true;
      else return false;      
    }
    
    bool calo_tp_to_ctw_algo::is_xwall_gveto() const
    {
      if (_mode_ == MODE_XWALL_GVETO) return true;
      else return false;
    }

    void calo_tp_to_ctw_algo::set_ctw_htm(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_)
    {
      if (my_ctw_.is_main_wall()) // Mode == Main Wall
	{
	  if (my_ctw_.is_htm_main_wall())
	    {
	      unsigned int ctw_multiplicity = my_ctw_.get_htm_main_wall_info();
	      my_ctw_.set_htm_main_wall(ctw_multiplicity + my_calo_tp_.get_htm());
	    }
	  else
	    {
	      my_ctw_.set_htm_main_wall(my_calo_tp_.get_htm());
	    }	  
	}
      
      if (!my_ctw_.is_main_wall()) // Mode == XWall Gveto
	{
	  unsigned int board_id = my_calo_tp_.get_geom_id().get(mapping::BOARD_INDEX);
	  if (board_id == 4 || board_id == 5 || board_id == 15 || board_id == 16) 
	    { // Filling gamma veto multiplicity
	      if (my_ctw_.is_htm_gveto())
		{
		  unsigned int ctw_multiplicity = my_ctw_.get_htm_gveto_info();
		  my_ctw_.set_htm_gveto(ctw_multiplicity + my_calo_tp_.get_htm());
		}
	      else
		{
		  my_ctw_.set_htm_gveto(my_calo_tp_.get_htm());
		}
	    } // end of gamma veto board
	  
	  if (board_id == 6 || board_id == 7 || board_id == 8 || board_id == 9)
	    { // Filling xwall side 0 multiplicity
	      if (my_ctw_.is_htm_xwall_side_0())
		{
		  unsigned int ctw_multiplicity = my_ctw_.get_htm_xwall_side_0_info();
		  my_ctw_.set_htm_xwall_side_0(ctw_multiplicity + my_calo_tp_.get_htm());
		}
	      else
		{
		  my_ctw_.set_htm_xwall_side_0(my_calo_tp_.get_htm());
		}
	    } // end of wall side 0 board
	      
	  if (board_id == 11 || board_id == 12 || board_id == 13 || board_id == 14)
	    { // Filling xwall side 1 multiplicity
	      if (my_ctw_.is_htm_xwall_side_1())
		{
		  unsigned int ctw_multiplicity = my_ctw_.get_htm_xwall_side_1_info();
		  my_ctw_.set_htm_xwall_side_1(ctw_multiplicity + my_calo_tp_.get_htm());
		}
	      else
		{
		  my_ctw_.set_htm_xwall_side_1(my_calo_tp_.get_htm());
		}
	    }
	}
      return;
    }
    
    void calo_tp_to_ctw_algo::set_ctw_clocktick(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_)
    {
      my_ctw_.set_clocktick_25ns(my_calo_tp_.get_clocktick_25ns());
      return;
    }
    void calo_tp_to_ctw_algo::set_ctw_zone_bit_htm(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_)
    {
      unsigned int activated_zone_id = 0;

      if (my_ctw_.is_main_wall()) // Mode == Main Wall
	{
	  if (my_calo_tp_.is_htm())
	    {
	      if (my_calo_tp_.get_geom_id().get(mapping::BOARD_INDEX) > 9)
		{
		  activated_zone_id = (my_calo_tp_.get_geom_id().get(mapping::BOARD_INDEX) + BOARD_SHIFT_INDEX) / 2;
		}
	      else
		{
		  activated_zone_id = my_calo_tp_.get_geom_id().get(mapping::BOARD_INDEX) / 2;
		}
	      my_ctw_.set_zoning_bit(calo::ctw::W_ZW_BIT0 + activated_zone_id, true);
	    }
	}

      if (!my_ctw_.is_main_wall()) // Mode == XWall Gveto
	{
	  if (my_calo_tp_.is_htm())
	    {
	      unsigned int board_id = my_calo_tp_.get_geom_id().get(mapping::BOARD_INDEX);
	  
	      if (board_id == 6 || board_id == 7)
		{
		  activated_zone_id = 0;
		}	 
	      if (board_id == 8 || board_id == 9)
		{
		  activated_zone_id = 1;
		}
	  	  
	      if (board_id == 11 || board_id == 12)
		{
		  activated_zone_id = 3;
		}
	  	 
	      if (board_id == 13 || board_id == 14)
		{
		  activated_zone_id = 2;
		}

	      my_ctw_.set_zoning_bit(calo::ctw::ZONING_XWALL_BIT0 + activated_zone_id, true);
	    }
	}
          
      return ;
     }

    void calo_tp_to_ctw_algo::set_ctw_lto(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_)
    {    
      if (my_ctw_.is_main_wall()) // Mode == Main Wall
	{
	  if (my_calo_tp_.is_lto()) my_ctw_.set_lto_main_wall_bit(true);
	}

      if (!my_ctw_.is_main_wall()) // Mode == XWall Gveto
	{
	  unsigned int board_id = my_calo_tp_.get_geom_id().get(mapping::BOARD_INDEX);
	  if (board_id == 4 || board_id == 5 || board_id == 15 || board_id == 16) 
	    { // Filling gamma veto multiplicity
	      if (my_calo_tp_.is_lto()) my_ctw_.set_lto_gveto_bit(true);
	    }
		  
	  if (board_id == 6 || board_id == 7 || board_id == 8 || board_id == 9)
	    { // Filling xwall side 0 multiplicity
	      if (my_calo_tp_.is_lto()) my_ctw_.set_lto_xwall_side_0_bit(true);
	    }
	  
	  if (board_id == 11 || board_id == 12 || board_id == 13 || board_id == 14)
	    { // Filling xwall side 1 multiplicity
	      if (my_calo_tp_.is_lto()) my_ctw_.set_lto_xwall_side_1_bit(true);
	    }
	}
      
      return;
    }

    void calo_tp_to_ctw_algo::_set_mode(mode_type mode_)
    {
      _mode_ = mode_;
      return;
    }
           
    void calo_tp_to_ctw_algo::_fill_a_main_wall_ctw(const calo_tp & my_calo_tp_, calo_ctw & a_calo_ctw_)
    { 
      geomtools::geom_id temporary_feb_id;
      temporary_feb_id.set_type(my_calo_tp_.get_geom_id().get_type());
      temporary_feb_id.set_depth(mapping::BOARD_DEPTH);
      my_calo_tp_.get_geom_id().extract_to(temporary_feb_id);
      temporary_feb_id.set(mapping::BOARD_INDEX, mapping::CONTROL_BOARD_ID);

      a_calo_ctw_.set_header(my_calo_tp_.get_hit_id(),
			     temporary_feb_id,
			     my_calo_tp_.get_clocktick_25ns());

      set_ctw_htm(my_calo_tp_, a_calo_ctw_);
      set_ctw_zone_bit_htm(my_calo_tp_, a_calo_ctw_);
      set_ctw_lto(my_calo_tp_, a_calo_ctw_);
      return;    
    }


    void calo_tp_to_ctw_algo::_fill_a_xwall_gveto_ctw(const calo_tp & my_calo_tp_, calo_ctw & a_calo_ctw_)
    {
      geomtools::geom_id temporary_feb_id;
      temporary_feb_id.set_type(my_calo_tp_.get_geom_id().get_type());
      temporary_feb_id.set_depth(mapping::BOARD_DEPTH);
      my_calo_tp_.get_geom_id().extract_to(temporary_feb_id);
      temporary_feb_id.set(mapping::BOARD_INDEX, mapping::CONTROL_BOARD_ID);

      a_calo_ctw_.set_header(my_calo_tp_.get_hit_id(),
			     temporary_feb_id,
			     my_calo_tp_.get_clocktick_25ns());
      set_ctw_htm(my_calo_tp_, a_calo_ctw_);
      set_ctw_zone_bit_htm(my_calo_tp_, a_calo_ctw_);
      set_ctw_lto(my_calo_tp_, a_calo_ctw_);
      return;   
    }
    
    void calo_tp_to_ctw_algo::_process_for_a_ctw_for_a_clocktick_for_main_wall(const std::vector<datatools::handle<calo_tp> > & my_list_of_calo_tp_,  calo_ctw & a_calo_ctw_)
    {
    DT_THROW_IF(_mode_ != MODE_MAIN_WALL, std::logic_error, "Process for main wall can't process because crate number [" << _crate_number_ << "] is not valid ! ");
    DT_THROW_IF(!is_main_wall(), std::logic_error, "Process for main wall can't process because mode number is not valid ! ");
    for(int i = 0; i < my_list_of_calo_tp_.size(); i++)
      {
	const calo_tp & my_calo_tp = my_list_of_calo_tp_[i].get();
	_fill_a_main_wall_ctw(my_calo_tp, a_calo_ctw_);	  
      }
    return;
  }   

    void calo_tp_to_ctw_algo::_process_for_a_ctw_for_a_clocktick_for_xwall_gveto(const std::vector<datatools::handle<calo_tp> > & my_list_of_calo_tp_,  calo_ctw & a_calo_ctw_)
    {
    DT_THROW_IF(_mode_ != MODE_XWALL_GVETO, std::logic_error, "Process for main wall can't process because crate number [" << _crate_number_ << "] is not valid ! ");
    DT_THROW_IF(!is_xwall_gveto(), std::logic_error, "Process for main wall can't process because mode number is not valid ! ");
    for(int i = 0; i < my_list_of_calo_tp_.size(); i++)
      {
	const calo_tp & my_calo_tp = my_list_of_calo_tp_[i].get();
	_fill_a_xwall_gveto_ctw(my_calo_tp, a_calo_ctw_);	  
      }
    return;
  }  

    void calo_tp_to_ctw_algo::process(const calo_tp_data & calo_tp_data_,  calo_ctw_data & calo_ctw_data_)
    { 
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo tp to ctw algo is not initialized, it can't process ! ");
      DT_THROW_IF(_mode_ == MODE_UNDEFINED, std::logic_error, "Mode type is not defined, check your crate number ! ");

      std::clog << "DEBUG : BEGINING OF CALO TP TO CTW PROCESS " << std::endl;
      std::clog << "**************************************************************" << std::endl;
      for(int32_t i = calo_tp_data_.get_clocktick_min(); i <= calo_tp_data_.get_clocktick_max(); i++)
	{
	  if (_mode_ == MODE_MAIN_WALL)
	    {
	      std::vector<datatools::handle<calo_tp> > tp_list_per_clocktick_per_crate;
	      calo_tp_data_.get_list_of_tp_per_clocktick_per_crate(i, _crate_number_, tp_list_per_clocktick_per_crate);
	      
	      if (!tp_list_per_clocktick_per_crate.empty())
		{
		  calo_ctw & a_ctw_ = calo_ctw_data_.add();
		  _process_for_a_ctw_for_a_clocktick_for_main_wall(tp_list_per_clocktick_per_crate, a_ctw_);
		  a_ctw_.tree_dump(std::clog, "a_calo_ctw : ", "INFO : ");
		}
	    }
	  
	  if (_mode_ == MODE_XWALL_GVETO)
	    {
	      std::vector<datatools::handle<calo_tp> > tp_list_per_clocktick_per_crate;
	      calo_tp_data_.get_list_of_tp_per_clocktick_per_crate(i, _crate_number_, tp_list_per_clocktick_per_crate);
	      
	      if (!tp_list_per_clocktick_per_crate.empty())
		{
		  calo_ctw & a_ctw_ = calo_ctw_data_.add();
		  _process_for_a_ctw_for_a_clocktick_for_xwall_gveto(tp_list_per_clocktick_per_crate, a_ctw_);
		  a_ctw_.tree_dump(std::clog, "a_calo_ctw : ", "INFO : ");
		}
	    }
	}
      return;
    }
    
  } // end of namespace digitization

} // end of namespace snemo
