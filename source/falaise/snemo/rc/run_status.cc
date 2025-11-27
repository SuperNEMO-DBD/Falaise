/// \file falaise/snemo/rc/run_status.cc
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2025-05-06
/// Last modified: 2025-05-06
///

// Ourselves:
#include <falaise/snemo/rc/run_status.h>

// Standard library:
#include <bitset>
#include <sstream>

namespace snemo {

  namespace rc {

    // static
    run_status_type run_status::make_good()
    {
      run_status_type rs = run_status::good;
      return rs;
    }
			
    // static
    run_status_type run_status::make_from_bitset(const std::uint32_t bits_)
    {
      run_status_type rs = run_status::good;
      rs |= bits_ & calo_issues;
      rs |= bits_ & tracker_issues;
      rs |= bits_ & gas_issues;
      rs |= bits_ & anti_radon_factory_issues;
      rs |= bits_ & sds_issues;
      rs |= bits_ & truncated_issues;
      rs |= bits_ & lis_issues;
      rs |= bits_ & other_issues;
      return rs;
    }
		
    // static
    bool run_status::is_good(const run_status_type s_)
    {
      return s_ == 0x0;
    }
		
    // static
    bool run_status::has_issues(const run_status_type s_)
    {
      return not is_good(s_);
    }
		
    // static
    bool run_status::has_calo_issues(const run_status_type s_)
    {
      return s_ & calo_issues;
    }
		
    // static
    bool run_status::has_tracker_issues(const run_status_type s_)
    {
      return s_ & tracker_issues;
    }
		
    // static
    bool run_status::has_gas_issues(const run_status_type s_)
    {
      return s_ & gas_issues;
    }
		
    // static
    bool run_status::has_anti_radon_factory_issues(const run_status_type s_)
    {
      return s_ & anti_radon_factory_issues;
    }
		
    // static
    bool run_status::has_sds_issues(const run_status_type s_)
    {
      return s_ & sds_issues;
    }
		
    // static
    bool run_status::has_lis_issues(const run_status_type s_)
    {
      return s_ & lis_issues;
    }
		
    // static
    bool run_status::has_truncated_issues(const run_status_type s_)
    {
      return s_ & truncated_issues;
    }
		
    // static
    bool run_status::has_other_issues(const run_status_type s_)
    {
      return s_ & other_issues;
    }

    // static 
    std::string run_status::to_string(const run_status_type s_)
    {
      std::ostringstream oss;
      oss << std::bitset<32>(s_);
      return oss.str();
    }

    // static
    void run_status::print_tree(const run_status_type s_,
				std::ostream & out_,
				const boost::property_tree::ptree & options_)
    {
      datatools::i_tree_dumpable::base_print_options popts;
      popts.configure_from(options_);
      if (! popts.title.empty ()) {
        out_ << popts.indent << popts.title << std::endl;
      }
      bool binFormat = options_.get<bool>("bin", false);
      if (binFormat) {
	out_ << popts.indent << datatools::i_tree_dumpable::inherit_tag(popts.inherit)
	     << "Status bits : " << std::bitset<32>(s_)
	     << std::endl;
      } else {
	// std::bitset<32> statusBits(s_);
	// auto count = statusBits.count();
	// if (count) {
	if (s_ == 0) {
	  out_ << popts.indent << datatools::i_tree_dumpable::inherit_tag(popts.inherit)
	       << "No issue"
	       << std::endl;
	} else {
	  std::ostringstream sout;
	  if (has_calo_issues(s_)) {
	    sout << "calo ";
	  }
	  if (has_tracker_issues(s_)) {
	    sout << "tracker ";
	  }
	  if (has_gas_issues(s_)) {
	    sout << "gas ";
	  }
	  if (has_anti_radon_factory_issues(s_)) {
	    sout << "radon ";
	  }
	  if (has_sds_issues(s_)) {
	    sout << "sds ";
	  }
	  if (has_truncated_issues(s_)) {
	    sout << "truncated ";
	  }
	  if (has_other_issues(s_)) {
	    sout << "other ";
	  }
	  out_ << popts.indent << datatools::i_tree_dumpable::inherit_tag(popts.inherit)
	       << "Issues : "
	       << sout.str()
	       << std::endl;
	}
      }
      
      return;
    }
    
  } // end of namespace rc

} // end of namespace snemo
