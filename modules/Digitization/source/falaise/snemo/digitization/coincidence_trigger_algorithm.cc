// snemo/digitization/coincidence_trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/coincidence_trigger_algorithm.h>
#include <snemo/digitization/clock_utils.h>

namespace snemo {
  
  namespace digitization {

    const uint32_t coincidence_trigger_algorithm::SHIFT_COMPUTING_CLOCKTICK_1600NS;
    const uint32_t coincidence_trigger_algorithm::SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS;

    coincidence_trigger_algorithm::coincidence_calo_record::coincidence_calo_record()
    {
      clocktick_1600ns = -1;
      return;
    }

    void coincidence_trigger_algorithm::coincidence_calo_record::active_next_zone()
    {
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int izone = calo_trigger_algorithm::ZONING_PER_SIDE_BITSET_SIZE - 1; izone >= 0 ; izone --)
	    {
	      if (zoning_word[iside].test(izone) == true)
		{
		  if (izone < 9) zoning_word[iside].set(izone + 1, true);
		}
	    }
	}
      return;
    }

    coincidence_trigger_algorithm::coincidence_output::coincidence_output()
    {
      clocktick_1600ns = -1;
      zoning_word[0].reset();
      zoning_word[1].reset();
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
	{
	  for (int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer ++)
	    {
	      for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow ++)
		{
		  geiger_matrix[iside][jlayer][krow] = 0;
		} // end of krow
	    } // end of jlayer
	} // end of iside
	       
      coincidence_finale_decision = false;
    }

    void coincidence_trigger_algorithm::coincidence_output::display()
    {
      std::clog << "*************************** Clocktick 1600 = " << clocktick_1600ns << " ***************************" << std::endl << std::endl;
      std::clog << "Coincidence zoning word : " << std::endl;
      std::clog << "ZW [0] : " << zoning_word[0] << std::endl;
      std::clog << "ZW [1] : " << zoning_word[1] << std::endl;
     
      std::clog << "   |-Zone-0-|---Zone-1--|---Zone-2--|---Zone-3--|---Zone-4--|--Zone-5--|---Zone-6--|---Zone-7--|--Zone-8---|--Zone-9-|" << std::endl << std::endl;;
      for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
      	{
      	  if (iside == 1)
	    {  
	      for (int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer++)
		{
		  std::clog << jlayer << ' ' ;
		  for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
		    {
		      if( krow == 0 )        std::clog<<"|";
		  
		      if (geiger_matrix[iside][jlayer][krow] ) std::clog << "*";
		  
		      if(!geiger_matrix[iside][jlayer][krow])  std::clog << ".";	  

		      if( krow == 112)     std::clog<<"|";

		    } // end of krow
		  if (jlayer == 5) std::clog << "  Side [" << iside << "]";
		  std::clog<<std::endl;	    
  
		} // end of jlayer

	    } // end of if iside==1

      	  for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
      	    {
	      if (izone == 0) std::clog << "   ";
	      if (izone == 0 || izone == 9) 
      		{
      		  if (zoning_word[iside][izone] == true) std::clog << "[*******]";
      		  else std::clog  << "[       ]";
      		}
      	      else if (izone == 5) 
      		{
      		  if (zoning_word[iside][izone] == true) std::clog  << "[*********]";
      		  else std::clog  << "[         ]";
      		}
      	      else 
      		{
      		  if (zoning_word[iside][izone] == true) std::clog  << "[**********]";
      		  else std::clog << "[          ]";
      		}
	      if (iside == 0 && izone == 9) std::clog << std::endl;
	    }

	  if (iside == 0)
	    {
	      for (int jlayer = mapping::GEIGER_LAYERS_SIZE - 1; jlayer >= 0; jlayer--) // Value GEIGER_LAYER_SIZE = 9
		{
		  std::clog << jlayer << ' ';
		  for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow++)
		    {
		      if( krow == 0 )        std::clog<<"|";
		  
		      if (geiger_matrix[iside][jlayer][krow] ) std::clog << "*";
		  
		      if(!geiger_matrix[iside][jlayer][krow])  std::clog << ".";	  

		      if( krow == 112)     std::clog<<"|";

		    } // end of row loop
		  if (jlayer == 5) std::clog << "  Side [" << iside << "]";
		  std::clog<<std::endl;	

		  if (jlayer == 0)
		    {
		      std::clog << "  |_________________________________________________________________________________________________________________|" << std::endl;
		    }

		} // end of layer loop

	    } // end of if == 0

	} // end of iside
      std::clog << std::endl << std::endl;;
      std::clog << "   |-Zone-0-|---Zone-1--|---Zone-2--|---Zone-3--|---Zone-4--|--Zone-5--|---Zone-6--|---Zone-7--|--Zone-8---|--Zone-9-|" << std::endl;
      std::clog << std::endl << std::endl;

      return;
    }

    coincidence_trigger_algorithm::coincidence_trigger_algorithm()
    {
      _initialized_ = false;
      _electronic_mapping_ = 0;
      _calorimeter_gate_size_ = 0;
      _coincidence_calo_records_.reserve(SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS);
      _coincidence_decision_ = false;
      return;
    }

    coincidence_trigger_algorithm::~coincidence_trigger_algorithm()
    {   
      if (is_initialized())
	{
	  reset();
	}
      return;
    }

    void coincidence_trigger_algorithm::set_electronic_mapping(const electronic_mapping & my_electronic_mapping_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, electronic mapping can't be set ! ");
      _electronic_mapping_ = & my_electronic_mapping_;
      return;
    }

    bool coincidence_trigger_algorithm::has_calorimeter_gate_size() const
    {
      return _calorimeter_gate_size_ != 0;
    }

    void coincidence_trigger_algorithm::set_calorimeter_gate_size(unsigned int calorimeter_gate_size_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Calo trigger algorithm is already initialized, calo circular buffer depth can't be set ! ");
      _calorimeter_gate_size_ = calorimeter_gate_size_;
      return;
    }

    void coincidence_trigger_algorithm::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void coincidence_trigger_algorithm::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Coincidence trigger algorithm is already initialized ! ");
      if (! has_calorimeter_gate_size()) {
	if (config_.has_key("calorimeter_gate_size")) {
	  int calorimeter_gate_size = config_.fetch_integer("calorimeter_gate_size");
	  DT_THROW_IF(calorimeter_gate_size <= 0, std::domain_error, "Invalid value calorimeter_gate_size !");
	  set_calorimeter_gate_size((unsigned int) calorimeter_gate_size);
	}
      }
      _initialized_ = true;
      return;
    }

    bool coincidence_trigger_algorithm::is_initialized() const
    {
      return _initialized_;
    }

    void coincidence_trigger_algorithm::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't be reset ! ");
      _electronic_mapping_ = 0;
      reset_data();
      _initialized_ = false;
      return;
    }
    
    void coincidence_trigger_algorithm::reset_data()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't be reset ! ");
      _coincidence_calo_records_.clear();
      _coincidence_decision_ = false;
      return;
    }

    const bool coincidence_trigger_algorithm::get_coincidence_decision() const
    {
      return _coincidence_decision_;
    }

    void coincidence_trigger_algorithm::_preparing_calo_coincidence(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_)
    {     
      std::vector<calo_trigger_algorithm::calo_summary_record>::const_iterator it = calo_records_.begin();
      bool decision_already_true = false;
      coincidence_trigger_algorithm::coincidence_calo_record coincidence_calo_record;
      unsigned int clocktick_25ns_decision;
      unsigned int coincidence_calo_record_position = 0;
      for (it; it != calo_records_.end(); it++)
      	{
	  const calo_trigger_algorithm::calo_summary_record a_ctrec= *it;
	  unsigned int a_ctrec_clocktick_1600ns;
	  _compute_clocktick_1600ns(a_ctrec.clocktick_25ns, a_ctrec_clocktick_1600ns);
	  a_ctrec_clocktick_1600ns += SHIFT_COMPUTING_CLOCKTICK_1600NS;
	  
	  if (a_ctrec.calo_finale_decision == true && decision_already_true == false)
	    {
	      clocktick_25ns_decision = a_ctrec.clocktick_25ns;
	      coincidence_calo_record.clocktick_1600ns = a_ctrec_clocktick_1600ns;
	      coincidence_calo_record.zoning_word[0] = a_ctrec.zoning_word[0];
	      coincidence_calo_record.zoning_word[1] = a_ctrec.zoning_word[1];
	      coincidence_calo_record.total_multiplicity_side_0 = a_ctrec.total_multiplicity_side_0;
	      coincidence_calo_record.total_multiplicity_side_1 = a_ctrec.total_multiplicity_side_1;
	      coincidence_calo_record.LTO_side_0 = a_ctrec.LTO_side_0;
	      coincidence_calo_record.LTO_side_1 = a_ctrec.LTO_side_1;
	      coincidence_calo_record.total_multiplicity_gveto = a_ctrec.total_multiplicity_gveto;
	      coincidence_calo_record.LTO_gveto = a_ctrec.LTO_gveto;
	      coincidence_calo_record.xt_info_bitset = a_ctrec.xt_info_bitset;
	      coincidence_calo_record.single_side_coinc = a_ctrec.single_side_coinc;
	      coincidence_calo_record.total_multiplicity_threshold = a_ctrec.total_multiplicity_threshold;
	      coincidence_calo_record.calo_finale_decision = a_ctrec.calo_finale_decision;
	      _coincidence_calo_records_.push_back(coincidence_calo_record);
	      decision_already_true = true;
	      _coincidence_decision_ = true;
	    }
	  
	  if (decision_already_true == true && 
	      a_ctrec_clocktick_1600ns == _coincidence_calo_records_[coincidence_calo_record_position].clocktick_1600ns &&
	      a_ctrec.clocktick_25ns <= clocktick_25ns_decision + 3) // 3 == calo circular buffer depth
	    {
	      _coincidence_calo_records_[coincidence_calo_record_position].zoning_word[0] = a_ctrec.zoning_word[0];
	      _coincidence_calo_records_[coincidence_calo_record_position].zoning_word[1] = a_ctrec.zoning_word[1];
	      _coincidence_calo_records_[coincidence_calo_record_position].active_next_zone(); 
	    }
	  
	  else if (decision_already_true == true && 
		   a_ctrec_clocktick_1600ns != _coincidence_calo_records_[coincidence_calo_record_position].clocktick_1600ns &&
		   a_ctrec.clocktick_25ns <= clocktick_25ns_decision + 3) // 3 == calo circular buffer depth
	    {
	      coincidence_trigger_algorithm::coincidence_calo_record coincidence_calo_record_next_clocktick;
	      coincidence_calo_record_next_clocktick = coincidence_calo_record;
	      coincidence_calo_record_next_clocktick.clocktick_1600ns = coincidence_calo_record.clocktick_1600ns + 1;
	      _coincidence_calo_records_.push_back(coincidence_calo_record_next_clocktick);
	      coincidence_calo_record_position++;
	    }
	}
      
      if (decision_already_true == true)
       	{
	  for (int iclocktick = _coincidence_calo_records_[coincidence_calo_record_position].clocktick_1600ns + 1; iclocktick <= coincidence_calo_record.clocktick_1600ns + _calorimeter_gate_size_; iclocktick++)
	    {
      	      coincidence_trigger_algorithm::coincidence_calo_record a_coinc_calo_record;
      	      a_coinc_calo_record = _coincidence_calo_records_[coincidence_calo_record_position];	      
      	      a_coinc_calo_record.clocktick_1600ns = iclocktick;
      	      _coincidence_calo_records_.push_back(a_coinc_calo_record);
	    }
      	}
      
      return;
    }

    void coincidence_trigger_algorithm::_compute_clocktick_1600ns(const uint32_t clocktick_25ns_, uint32_t & clocktick_1600ns_)
    {
      clocktick_1600ns_ = (clocktick_25ns_ * clock_utils::MAIN_CLOCKTICK) / clock_utils::TRIGGER_CLOCKTICK;
      return;
    }
         
    void coincidence_trigger_algorithm::process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
						const std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_,
						std::vector<coincidence_trigger_algorithm::coincidence_output> & coincidence_records_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Coincidence trigger algorithm is not initialized, it can't process ! ");
      _process(calo_records_,
	       tracker_records_,
	       coincidence_records_);
      return;
    }

    void coincidence_trigger_algorithm::_process(const std::vector<calo_trigger_algorithm::calo_summary_record> & calo_records_,
						 const std::vector<tracker_trigger_algorithm::tracker_record> & tracker_records_,
						 std::vector<coincidence_trigger_algorithm::coincidence_output> & coincidence_records_)
    {
      reset_data();
      _preparing_calo_coincidence(calo_records_);
     
      std::vector<coincidence_calo_record>::const_iterator it_calo = _coincidence_calo_records_.begin();
      std::clog << "Size of coincidence calo records : " << _coincidence_calo_records_.size() << std::endl;
      for (it_calo; it_calo != _coincidence_calo_records_.end(); it_calo++)
	{
	  coincidence_calo_record a_calo_record = *it_calo;
	  std::vector<tracker_trigger_algorithm::tracker_record>::const_iterator it_tracker = tracker_records_.begin();
	  for (it_tracker; it_tracker != tracker_records_.end(); it_tracker++)
	    {
	      tracker_trigger_algorithm::tracker_record a_tracker_record = *it_tracker;

	      if (a_calo_record.clocktick_1600ns == a_tracker_record.clocktick_1600ns)
		{
		  coincidence_output a_coincidence_output;
		  a_coincidence_output.clocktick_1600ns = a_calo_record.clocktick_1600ns;
		  for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
		    {
		      for (int jlayer = 0; jlayer < mapping::GEIGER_LAYERS_SIZE; jlayer ++)
			{
			  for (int krow = 0; krow < mapping::GEIGER_ROWS_SIZE; krow ++)
			    {
			      a_coincidence_output.geiger_matrix[iside][jlayer][krow] = a_tracker_record.geiger_matrix[iside][jlayer][krow];
			    } // end of krow
			} // end of jlayer
		    } // end of iside

		  for (int iside = 0; iside < mapping::NUMBER_OF_SIDES; iside++)
		    {
		      for (int izone = 0; izone < mapping::NUMBER_OF_TRIGGER_ZONES; izone++)
			{
			  if (a_tracker_record.final_tracker_trigger_info[iside][izone] != 0)
			    {
			      a_coincidence_output.coincidence_finale_decision = true;

			      if (a_calo_record.zoning_word[iside].test(izone) != 0) 
				{
				  a_coincidence_output.zoning_word[iside].set(izone, true);
				}
			      if (izone < mapping::NUMBER_OF_TRIGGER_ZONES - 1 && a_calo_record.zoning_word[iside].test(izone+1) != 0)
				{
				  a_coincidence_output.zoning_word[iside].set(izone + 1, true);
				}
			      
				
			    } //end of tracker_record[side][zone] != 0
			} // end of izone
		    } // end of iside


		  // a_coincidence_output.display();
		  coincidence_records_.push_back(a_coincidence_output);

		  		  
		} // end of clocktick egality
	    } // end of it_tracker
	} // end of it_calo
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
