// snemo/digitization/calo_trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/calo_trigger_algorithm.h>
#include <snemo/digitization/calo_ctw.h>
#include <snemo/digitization/clock_utils.h>


namespace snemo {
  
  namespace digitization {
     
    const int32_t calo_trigger_algorithm::LEVEL_ONE_MULT_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::ZONING_PER_SIDE_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::ZONING_GVETO_BITSET_SIZE;
    const int32_t calo_trigger_algorithm::XT_INFO_BITSET_SIZE;

    
    calo_trigger_algorithm::calo_record::calo_record()
    {
      calo_record::reset();
      return;
    }
        
    void calo_trigger_algorithm::calo_record::reset()
    {
      clocktick_25ns = clock_utils::INVALID_CLOCKTICK;
      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
	{
	  zoning_word[iside].reset();
	  zoning_word[iside].reset();
	}
      total_multiplicity_side_0.reset();
      total_multiplicity_side_1.reset();
      LTO_side_0 = false;
      LTO_side_1 = false;
      total_multiplicity_gveto.reset();
      LTO_gveto = false;
      xt_info_bitset.reset();
      return;
    }

    void calo_trigger_algorithm::calo_record::display() const
    {      
      std::clog << "Calo Trigger info record : " << std::endl; 
      std::clog << "CT |XTS|L|HG|L|L|H1|H0| ZONING S1| ZONING S0 " << std::endl; 
      std::clog << clocktick_25ns << ' ';
      std::clog << xt_info_bitset << ' ';
      std::clog << LTO_gveto << ' ';
      std::clog << total_multiplicity_gveto << ' ';
      std::clog << LTO_side_1 << ' ';
      std::clog << LTO_side_0 << ' ';
      std::clog << total_multiplicity_side_1 << ' ';
      std::clog << total_multiplicity_side_0 << ' ';

      for (unsigned int iside = trigger_info::NSIDES-1; iside > 0; iside--)
      	{
      	  for (unsigned int izone = trigger_info::NZONES-1; izone > 0 ; izone--)
      	    {
      	      std::clog << zoning_word[iside][izone];
      	    }
      	  std::clog << ' ';
      	}
      std::clog << std::endl << std::endl;
      return;
    }
    
    calo_trigger_algorithm::calo_summary_record::calo_summary_record()
    {
      calo_summary_record::reset();
      return; 
    }

   void calo_trigger_algorithm::calo_summary_record::reset()
   {
     calo_record::reset();
     single_side_coinc = false;
     total_multiplicity_threshold = false;
     calo_finale_decision = false;
     return;
   }
    
    void calo_trigger_algorithm::calo_summary_record::reset_summary_boolean_only()
    {
      single_side_coinc = false;
      total_multiplicity_threshold = false;
      calo_finale_decision = false;
      return;
    }

    void calo_trigger_algorithm::calo_summary_record::display() const
    {
      calo_record::display();
      std::clog << "Single Side coinc : "      << single_side_coinc << std::endl;
      std::clog << "Threshold total mult : "   << total_multiplicity_threshold << std::endl;
      std::clog << "Trigger final decision : " << calo_finale_decision  << std::endl;
      std::clog << std::endl;
      return;
    }
    
    bool calo_trigger_algorithm::calo_summary_record::is_empty() const
    {
      if (zoning_word[0].any() || zoning_word[1].any() 
	  || total_multiplicity_side_0.any() || total_multiplicity_side_1.any() || total_multiplicity_gveto.any()
	  || LTO_side_0 || LTO_side_1 || LTO_gveto || xt_info_bitset.any())
	{
	  return false;
	}

      else return true;
    }

    calo_trigger_algorithm::calo_trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _circular_buffer_depth_ = 0;
      _activated_threshold_ = false;
      _total_multiplicity_threshold_.reset();
      _inhibit_both_side_coinc_ = false;
      _inhibit_single_side_coinc_ = false;
      _calo_finale_decision_ = false;
      return;
    }

    calo_trigger_algorithm::~calo_trigger_algorithm()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void calo_trigger_algorithm::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      return;
    }
 
    bool calo_trigger_algorithm::has_circular_buffer_depth() const
    {
      return _circular_buffer_depth_ != 0;
    }
   
    void calo_trigger_algorithm::set_circular_buffer_depth(unsigned int circular_buffer_depth_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, calo circular buffer depth can't be set ! ");
      _circular_buffer_depth_ = circular_buffer_depth_;
      return;
    }

    unsigned int calo_trigger_algorithm::get_circular_buffer_depth() const
    {
      return _circular_buffer_depth_;
    }

    void calo_trigger_algorithm::inhibit_both_side_coinc()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, boolean back to back coinc can't be inhibited ! ");      
      _inhibit_both_side_coinc_ = true;
      return;
    }    

    bool calo_trigger_algorithm::is_inhibited_both_side_coinc() const
    {
      return _inhibit_both_side_coinc_;
    }

    void calo_trigger_algorithm::inhibit_single_side_coinc()
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, boolean single side coinc can't be inhibited ! ");
      _inhibit_single_side_coinc_ = true;
      return;
    }     

    bool calo_trigger_algorithm::is_inhibited_single_side_coinc() const
    {
      return _inhibit_single_side_coinc_;
    }

    bool calo_trigger_algorithm::has_total_multiplicity_threshold() const
    {
      return _total_multiplicity_threshold_.to_ulong() > 0;
    }

    void calo_trigger_algorithm::set_total_multiplicity_threshold(unsigned int threshold_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, calo threshold can't be set ! ");
      DT_THROW_IF(threshold_ > 3, std::range_error, "Invalid total multiplicity threshold ! ");
      _total_multiplicity_threshold_ = threshold_;
      _activated_threshold_ = (threshold_ > 0);
      return;
    }

    bool calo_trigger_algorithm::is_activated_total_multiplicity_threshold() const
    {
      return _activated_threshold_;
    }

    const std::bitset<calo::ctw::HTM_BITSET_SIZE> calo_trigger_algorithm::get_total_multiplicity_threshold_coinc() const
    {
      return _total_multiplicity_threshold_;
    }

    void calo_trigger_algorithm::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void calo_trigger_algorithm::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized ! ");

      if (! has_circular_buffer_depth()) {
	if (config_.has_key("circular_buffer_depth")) {
	  int circular_buffer_depth = config_.fetch_integer("circular_buffer_depth");
	  DT_THROW_IF(circular_buffer_depth <= 0, std::domain_error, "Invalid negative circular buffer depth!");
	  set_circular_buffer_depth((unsigned int) circular_buffer_depth);
	}
      }

      if (! has_total_multiplicity_threshold()) {
	if (config_.has_key("total_multiplicity_threshold")) {
	  int total_multiplicity_threshold = config_.fetch_integer("total_multiplicity_threshold");
	  DT_THROW_IF(total_multiplicity_threshold <= 0, std::domain_error, "Invalid negative total multiplicity threshold!");
	  set_total_multiplicity_threshold((unsigned int) total_multiplicity_threshold);
	}
      }
 
      // Checks:

      _initialized_ = true;
      return;
    }
    
    bool calo_trigger_algorithm::is_initialized() const
    {
      return _initialized_;
    }

    void calo_trigger_algorithm::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo trigger algorithm is not initialized, it can't be reset ! ");
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _activated_threshold_ = false;
      _inhibit_both_side_coinc_ = false;
      _inhibit_single_side_coinc_ = false;
      _circular_buffer_depth_ = 0;
      _calo_level_1_finale_decision_.reset();
      _calo_finale_decision_ = false;
      _calo_record_per_clocktick_.reset();
      _gate_circular_buffer_.reset();
      return;
    }

    void calo_trigger_algorithm::reset_calo_record_per_clocktick()
    {
      _calo_record_per_clocktick_.reset();
      return;
    }     

    bool calo_trigger_algorithm::get_calo_decision() const
    {
      return _calo_finale_decision_;
    }
    
    const calo_trigger_algorithm::calo_summary_record calo_trigger_algorithm::get_calo_level_1_finale_decision_structure()
    {
      return _calo_level_1_finale_decision_;
    }

    void calo_trigger_algorithm::_display_calo_info_for_a_clocktick()
    {
      _calo_record_per_clocktick_.display();

      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
      	{
      	  if (iside == 1)
      	    {
      	      std::clog << " |                                                                                                                 |" << std::endl;
      	      if (_calo_record_per_clocktick_.zoning_word[iside][0] == true) std::clog << "   |";
      	      else std::clog << " |";
      	      std::clog << "                                                                                                                 ";
      	      if (_calo_record_per_clocktick_.zoning_word[iside][9] == true) std::clog << "| Side 1" << std::endl;
      	      else std::clog << "| Side 1" << std::endl;
      	    }
	  if (iside == 0) std::clog << "    Zone0                                                                                                   Zone9 " << std::endl;
      	  std::clog << " |";
      	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
      	    {
      	      if (izone == 0 || izone == 9) 
      		{
      		  if (_calo_record_per_clocktick_.zoning_word[iside][izone] == true) std::clog << "[*******]";
      		  else std::clog  << "[       ]";
      		}
      	      else if (izone == 5) 
      		{
      		  if (_calo_record_per_clocktick_.zoning_word[iside][izone] == true) std::clog  << "[*********]";
      		  else std::clog  << "[         ]";
      		}
      	      else 
      		{
      		  if (_calo_record_per_clocktick_.zoning_word[iside][izone] == true) std::clog  << "[**********]";
      		  else std::clog << "[          ]";
      		}
      	    } // end of izone
      	  std::clog << "|" << std::endl;
	  if (iside == 1) std::clog << "    Zone0                                                                                                   Zone9 " << std::endl;
      	  if (iside == 0)
      	    {
      	      if (_calo_record_per_clocktick_.zoning_word[iside][0] == true) std::clog << " |";
      	      else std::clog << " |";
      	      std::clog << "                                                                                                                 ";
      	      if (_calo_record_per_clocktick_.zoning_word[iside][9] == true) std::clog << "| Side 0" << std::endl;
      	      else std::clog << "| Side 0" << std::endl;
      	      std::clog << " |                                                                                                                 |" << std::endl;
      	      std::clog << " |-----------------------------------------------------------------------------------------------------------------| Source foil" << std::endl;
      	    }	  
      	} // end of iside
      std::clog << std::endl;
      return;
    }

    void calo_trigger_algorithm::_display_calo_summary(calo_summary_record & my_calo_summary_record_)
    {
      my_calo_summary_record_.display(); 

      for (unsigned int iside = 0; iside < trigger_info::NSIDES; iside++)
      	{
      	  if (iside == 1)
      	    {
      	      std::clog << " |                                                                                                                 |" << std::endl;
      	      if (my_calo_summary_record_.zoning_word[iside][0] == true) std::clog << " |";
      	      else std::clog << " |";
      	      std::clog << "                                                                                                                 ";
      	      if (my_calo_summary_record_.zoning_word[iside][9] == true) std::clog << "| Side 1" << std::endl;
      	      else std::clog << "| Side 1" << std::endl;
      	    }
	  if (iside == 0) std::clog << "    Zone0                                                                                                   Zone9 " << std::endl;
      	  std::clog << " |";
      	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
      	    {
      	      if (izone == 0 || izone == 9) 
      		{
      		  if (my_calo_summary_record_.zoning_word[iside][izone] == true) std::clog << "[*******]";
      		  else std::clog  << "[       ]";
      		}
      	      else if (izone == 5) 
      		{
      		  if (my_calo_summary_record_.zoning_word[iside][izone] == true) std::clog  << "[*********]";
      		  else std::clog  << "[         ]";
      		}
      	      else 
      		{
      		  if (my_calo_summary_record_.zoning_word[iside][izone] == true) std::clog  << "[**********]";
      		  else std::clog << "[          ]";
      		}
      	    } // end of izone
      	  std::clog << "|" << std::endl;
	  if (iside == 1) std::clog << "    Zone0                                                                                                   Zone9 " << std::endl;
      	  if (iside == 0)
      	    {
      	      if (my_calo_summary_record_.zoning_word[iside][0] == true) std::clog << " |";
      	      else std::clog << " |";
      	      std::clog << "                                                                                                                 ";
      	      if (my_calo_summary_record_.zoning_word[iside][9] == true) std::clog << "| Side 0" << std::endl;
      	      else std::clog << "| Side 0" << std::endl;
      	      std::clog << " |                                                                                                                 |" << std::endl;
      	      std::clog << " |-----------------------------------------------------------------------------------------------------------------| Source foil" << std::endl;
      	    }	  
      	} // end of iside
      std::clog << std::endl;

      return;
    }
    
    void calo_trigger_algorithm::_build_calo_record_per_clocktick(const calo_ctw & my_calo_ctw_)
    {  
      uint32_t crate_index = my_calo_ctw_.get_geom_id().get(mapping::CRATE_INDEX);  
      DT_THROW_IF(crate_index > mapping::XWALL_GVETO_CALO_CRATE, std::logic_error, "Crate index '"<< crate_index << "' is not defined, check your value ! ");
      _calo_record_per_clocktick_.clocktick_25ns = my_calo_ctw_.get_clocktick_25ns();      
      
      // Fill structure if crate is number 2
      if (!my_calo_ctw_.is_main_wall())
	{
	  unsigned int multiplicity_side_0 = my_calo_ctw_.get_htm_xwall_side_0_info();
	  unsigned int multiplicity_side_1 = my_calo_ctw_.get_htm_xwall_side_1_info();
 
	  // Fill total HTM :
	  // -- Fill xwall side 0 multipliciy
	  if (multiplicity_side_0 != 0)
	    {
	      if (_calo_record_per_clocktick_.total_multiplicity_side_0 == 3) {}
	      else _calo_record_per_clocktick_.total_multiplicity_side_0 = _calo_record_per_clocktick_.total_multiplicity_side_0.to_ulong() + multiplicity_side_0;
	    }

	  // -- Fill xwall side 1 multiplicity
	  if (multiplicity_side_1 != 0)
	    {
	      if (_calo_record_per_clocktick_.total_multiplicity_side_1 == 3) {}
	      else _calo_record_per_clocktick_.total_multiplicity_side_1 = _calo_record_per_clocktick_.total_multiplicity_side_1.to_ulong() + multiplicity_side_1;
	    }
	  
	  // -- Fill gamma veto multiplicity
	  if (my_calo_ctw_.get_htm_gveto_info() != 0)
	    {
	      _calo_record_per_clocktick_.total_multiplicity_gveto = my_calo_ctw_.get_htm_gveto_info();
	    }

	  else _calo_record_per_clocktick_.total_multiplicity_gveto = 0;
	  
	  // -- Fill xwall zone corresponding of xwall zoning word :     
	  std::bitset<calo::ctw::XWALL_ZONING_BITSET_SIZE> xwall_zoning_bitset;
	  my_calo_ctw_.get_xwall_zoning_word(xwall_zoning_bitset);

	  for (unsigned int izone = calo::ctw::ZONING_XWALL_BIT0; izone < (calo::ctw::ZONING_XWALL_BIT0 + mapping::NUMBER_OF_XWALL_CALO_TRIGGER_ZONES); izone++)
	    {
	      switch (izone)
		{
		case calo::ctw::ZONING_XWALL_BIT0 :
		  if (xwall_zoning_bitset.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {		      
		      _calo_record_per_clocktick_.zoning_word[SIDE_0_INDEX].set(ZONE_0_INDEX, true);
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT1 :
		  if (xwall_zoning_bitset.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {
		      _calo_record_per_clocktick_.zoning_word[SIDE_0_INDEX].set(ZONE_9_INDEX, true);
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT2 :
		  if (xwall_zoning_bitset.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {
		      _calo_record_per_clocktick_.zoning_word[SIDE_1_INDEX].set(ZONE_0_INDEX, true);
		    }
		  break;
		case calo::ctw::ZONING_XWALL_BIT3 :
		  if (xwall_zoning_bitset.test(izone - calo::ctw::ZONING_XWALL_BIT0) == true)
		    {	
		      _calo_record_per_clocktick_.zoning_word[SIDE_1_INDEX].set(ZONE_9_INDEX, true);
		    }
		  break;
		default :
		  break;
		}
	    }

	  // Fill LTO boolean for each side of xwall and gamma veto
	  if (my_calo_ctw_.is_lto_xwall_side_0()) _calo_record_per_clocktick_.LTO_side_0 = true;
	  if (my_calo_ctw_.is_lto_xwall_side_1()) _calo_record_per_clocktick_.LTO_side_1 = true;
	  if (my_calo_ctw_.is_lto_gveto()) _calo_record_per_clocktick_.LTO_gveto = true;
	}

      // Fill structure if crate is number 0 or 1
      else
	{	  
	  // Fill total HTM :
	  unsigned int main_multiplicity = my_calo_ctw_.get_htm_main_wall_info();

	  if (crate_index == mapping::MAIN_CALO_SIDE_0_CRATE && main_multiplicity != 0)
	    {
	      if (_calo_record_per_clocktick_.total_multiplicity_side_0 == 3) {}
	      else _calo_record_per_clocktick_.total_multiplicity_side_0 = _calo_record_per_clocktick_.total_multiplicity_side_0.to_ulong() + main_multiplicity;
	    }

	  if (crate_index == mapping::MAIN_CALO_SIDE_1_CRATE && main_multiplicity != 0)
	    {
	      if (_calo_record_per_clocktick_.total_multiplicity_side_1 == 3) {}
	      else _calo_record_per_clocktick_.total_multiplicity_side_1 = _calo_record_per_clocktick_.total_multiplicity_side_1.to_ulong() + main_multiplicity;
	    }

	  // Fill zone main wall bitset :
	  std::bitset<calo::ctw::MAIN_ZONING_BITSET_SIZE> main_zoning_bitset;
	  my_calo_ctw_.get_main_zoning_word(main_zoning_bitset);

	  for (unsigned int izone = 0; izone < trigger_info::NZONES; izone++)
	    {
	      if (main_zoning_bitset.test(izone) == true) _calo_record_per_clocktick_.zoning_word[crate_index].set(izone,true);
	    }
	  	  
	  // Fill LTO boolean for each side
	  if (crate_index == mapping::MAIN_CALO_SIDE_0_CRATE && my_calo_ctw_.is_lto_main_wall()) _calo_record_per_clocktick_.LTO_side_0 = true;
	  if (crate_index == mapping::MAIN_CALO_SIDE_1_CRATE && my_calo_ctw_.is_lto_main_wall()) _calo_record_per_clocktick_.LTO_side_1 = true;
	}
      
      return;
    }    
    
    void calo_trigger_algorithm::_build_calo_record_summary_structure(calo_summary_record & my_calo_summary_record_)
    {
      unsigned int multiplicity_sum_circ_buff_side_0 = 0;
      unsigned int multiplicity_sum_circ_buff_side_1 = 0;
      unsigned int multiplicity_sum_circ_buff_gveto  = 0;
      bool         lto_sum_circ_buff_side_0 = false;     
      bool         lto_sum_circ_buff_side_1 = false;
      bool         lto_sum_circ_buff_gveto = false;
      std::bitset<ZONING_PER_SIDE_BITSET_SIZE> zoning_word_sum_side_0;
      std::bitset<ZONING_PER_SIDE_BITSET_SIZE> zoning_word_sum_side_1;
      std::bitset<XT_INFO_BITSET_SIZE> xt_info_sum;

      for (boost::circular_buffer<calo_record>::iterator it =_gate_circular_buffer_->begin() ; it != _gate_circular_buffer_->end(); it++)
	{
	  calo_record & ctrec = *it;
	  
	  // Compute the sum of multiplicity for each side : 
	  multiplicity_sum_circ_buff_side_0 += ctrec.total_multiplicity_side_0.to_ulong();
	  multiplicity_sum_circ_buff_side_1 += ctrec.total_multiplicity_side_1.to_ulong();
	  multiplicity_sum_circ_buff_gveto  += ctrec.total_multiplicity_gveto.to_ulong();
	  
	  // LTO bits :
	  if (ctrec.LTO_side_0 == true) lto_sum_circ_buff_side_0 = true;
	  if (ctrec.LTO_side_1 == true) lto_sum_circ_buff_side_1 = true;
	  if (ctrec.LTO_gveto == true)  lto_sum_circ_buff_gveto  = true;
	  
	  // Zoning word : 
	  for (unsigned int i = 0; i < trigger_info::NSIDES; i++)
      	    {
      	      for (unsigned int j = 0; j < ZONING_PER_SIDE_BITSET_SIZE; j++)
      		{
      		  if (ctrec.zoning_word[i].test(j) == true) 
		    {
		      if (i == 0) zoning_word_sum_side_0.set(j,true);
		      if (i == 1) zoning_word_sum_side_1.set(j,true);
		    }
      		  if (j < XT_INFO_BITSET_SIZE && ctrec.xt_info_bitset.test(j) == true) 
		    {
		      xt_info_sum.set(j, true);
		    }
      		}
      	    }
      	} // end of for iterator
      
      // Total mult side 0 :
      if (multiplicity_sum_circ_buff_side_0 >= 3)  my_calo_summary_record_.total_multiplicity_side_0 = 3;
      else my_calo_summary_record_.total_multiplicity_side_0 = multiplicity_sum_circ_buff_side_0;

      // Total mult side 1 :
      if (multiplicity_sum_circ_buff_side_1 >= 3) my_calo_summary_record_.total_multiplicity_side_1 = 3;
      else my_calo_summary_record_.total_multiplicity_side_1 = multiplicity_sum_circ_buff_side_1;
	  
      // Total mult gveto :
      if (multiplicity_sum_circ_buff_gveto >= 3) my_calo_summary_record_.total_multiplicity_gveto = 3;
      else my_calo_summary_record_.total_multiplicity_gveto = multiplicity_sum_circ_buff_gveto;
	  
      // Zoning words :
      my_calo_summary_record_.zoning_word[0] = zoning_word_sum_side_0;
      my_calo_summary_record_.zoning_word[1] = zoning_word_sum_side_1;
      my_calo_summary_record_.xt_info_bitset = xt_info_sum;

      // LTO : 
      my_calo_summary_record_.LTO_side_0 = lto_sum_circ_buff_side_0;
      my_calo_summary_record_.LTO_side_1 = lto_sum_circ_buff_side_1;
      my_calo_summary_record_.LTO_gveto = lto_sum_circ_buff_gveto;
      
      bool side_0_activated = my_calo_summary_record_.zoning_word[SIDE_0_INDEX].any();
      bool side_1_activated = my_calo_summary_record_.zoning_word[SIDE_1_INDEX].any();
      
      if ((side_0_activated && !side_1_activated) || (!side_0_activated && side_1_activated)) my_calo_summary_record_.single_side_coinc = true;
      else my_calo_summary_record_.single_side_coinc = false;

      if ((my_calo_summary_record_.total_multiplicity_side_0.to_ulong() + my_calo_summary_record_.total_multiplicity_side_1.to_ulong()) >= _total_multiplicity_threshold_.to_ulong())   my_calo_summary_record_.total_multiplicity_threshold = true;
      else my_calo_summary_record_.total_multiplicity_threshold = false;
      
      return;
    }

    void calo_trigger_algorithm::_compute_calo_finale_decision(calo_summary_record & my_calo_summary_record_)
    {
      if ((_activated_threshold_ && my_calo_summary_record_.total_multiplicity_threshold)
      	  && !(is_inhibited_single_side_coinc() && my_calo_summary_record_.single_side_coinc)
      	  && !(is_inhibited_both_side_coinc() && !my_calo_summary_record_.single_side_coinc))
      	{
      	  my_calo_summary_record_.calo_finale_decision = true;
      	  _calo_level_1_finale_decision_ = my_calo_summary_record_;
      	}
      else
	{
      	  _calo_level_1_finale_decision_ = my_calo_summary_record_;
	  //	  _calo_level_1_finale_decision_.reset();
	  // _calo_level_1_finale_decision_.clocktick_25ns = my_calo_summary_record_.clocktick_25ns;
	}

      return;      
    }

    void calo_trigger_algorithm::process(const calo_ctw_data & calo_ctw_data_,
					 std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Calo trigger algorithm is not initialized, it can't process ! ");
      _process(calo_ctw_data_, calo_records_);
      return;
    }

    void calo_trigger_algorithm::_process(const calo_ctw_data & calo_ctw_data_,
					  std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_)
    { 
      _calo_record_per_clocktick_.reset();
      _calo_level_1_finale_decision_.reset();
      _gate_circular_buffer_.reset(new buffer_type(_circular_buffer_depth_));
      _calo_finale_decision_ = false;
      
      for(int32_t iclocktick = calo_ctw_data_.get_clocktick_min(); iclocktick <= calo_ctw_data_.get_clocktick_max() + _circular_buffer_depth_ - 1 ; iclocktick++)
	{
	  std::vector<datatools::handle<calo_ctw> > ctw_list_per_clocktick;
	  calo_ctw_data_.get_list_of_calo_ctw_per_clocktick(iclocktick, ctw_list_per_clocktick);
	  
	  if (ctw_list_per_clocktick.size() == 0) _calo_record_per_clocktick_.clocktick_25ns = iclocktick;
	  else
	    {	  
	      for (unsigned int isize = 0; isize < ctw_list_per_clocktick.size(); isize++)
		{
		  _build_calo_record_per_clocktick(ctw_list_per_clocktick[isize].get());
		} // end of isize
	    }
	  _gate_circular_buffer_->push_back(_calo_record_per_clocktick_);

	  // Fill calo summary record for each clocktick (based on previous calo records) : 
	  calo_summary_record my_calo_summary_record;
	  my_calo_summary_record.clocktick_25ns = iclocktick;
	  _build_calo_record_summary_structure(my_calo_summary_record);
	  _compute_calo_finale_decision(my_calo_summary_record);
	  if (_calo_level_1_finale_decision_.calo_finale_decision) _calo_finale_decision_ = true;
	  
	  if(!_calo_level_1_finale_decision_.is_empty()) calo_records_.push_back(_calo_level_1_finale_decision_);
	  _calo_record_per_clocktick_.reset();
	} // end of iclocktick

      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
