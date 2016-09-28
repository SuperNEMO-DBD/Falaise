//test_background_model.cxx

// Standard libraries :
#include <iostream>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
#include <datatools/temporary_files.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/base_hit.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// Falaise:
#include <falaise/falaise.h>

// Third part : 
// GSL:
#include <bayeux/mygsl/rng.h>

// bool calo_spurious_comp(const calo_spurious_hit & calo_spu_hit_1_, const calo_spurious_hit & calo_spu_hit_2_)
// {
//   return calo_spu_hit_1_.timestamp < calo_spu_hit_2_.timestamp;
// }

struct calo_spurious_hit : public geomtools::base_hit
{
  bool operator<(const calo_spurious_hit & other_) const
  {
    return this-> timestamp < other_.timestamp;
  }

  void tree_dump(std::ostream & out_ = std::clog,
		 const std::string & title_ = "",
		 const std::string & indent_ = "",
		 bool inherit_ = false) const
  {
    if ( ! title_.empty() ) {
      out_ << indent_ << title_ << std::endl;
    }
    
    out_ << indent_ << i_tree_dumpable::tag
         << "GID : " << get_geom_id() << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Hit ID : " << get_hit_id () << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Timestamp : " << timestamp << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Yield : " << yield << std::endl;

    return;
  }

  double timestamp;
  double yield;
};

struct geiger_spurious_hit : public geomtools::base_hit
{
  bool operator<(const geiger_spurious_hit & other_) const
  {
    return this-> timestamp < other_.timestamp;
  }

  void tree_dump(std::ostream & out_ = std::clog,
		 const std::string & title_ = "",
		 const std::string & indent_ = "",
		 bool inherit_ = false) const
  {
    if ( ! title_.empty() ) {
      out_ << indent_ << title_ << std::endl;
    }
    
    out_ << indent_ << i_tree_dumpable::tag
         << "GID : " << get_geom_id() << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Hit ID : " << get_hit_id () << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Timestamp : " << timestamp << std::endl;

    return;
  }

  double timestamp;
};

void generate_pool_of_calo_spurious_hit(mygsl::rng * rdm_gen_,
					const std::vector<geomtools::geom_id> & gid_collection_,
					std::vector<calo_spurious_hit> & calo_spurious_hit_collection_);

void generate_pool_of_geiger_spurious_hit(mygsl::rng * rdm_gen_,
					  const std::vector<geomtools::geom_id> & gid_collection_,
					  std::vector<geiger_spurious_hit> & geiger_spurious_hit_collection_);

int main( int  argc_ , char **argv_  )
{
  falaise::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  try {

    bool draw = false;
    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      iarg++;
    }

    std::clog << "Test program for class 'snemo::digitization::trigger_algorithm_efficiency_validation' !" << std::endl;
    int32_t seed = 314159;
    mygsl::rng random_generator;
    random_generator.initialize(seed);
    
    std::string manager_config_file;
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/4.1/manager.conf";
    datatools::fetch_path_with_env(manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config (manager_config_file,
					manager_config);
    geomtools::manager my_manager;
    manager_config.update ("build_mapping", true);
    if (manager_config.has_key ("mapping.excluded_categories"))
      {
	manager_config.erase ("mapping.excluded_categories");
      }
    my_manager.initialize(manager_config);
    
    unsigned int module_number = 0;
    
    // Select calo main wall GID :
    geomtools::geom_id main_wall_gid_pattern(1302, 
					     module_number, 
					     geomtools::geom_id::ANY_ADDRESS, // Side
					     geomtools::geom_id::ANY_ADDRESS, // Column 
					     geomtools::geom_id::ANY_ADDRESS, // Row
					     0); // part only 0, to convert into any (to not have part 0 and 1 in the vector double count)
    std::vector<geomtools::geom_id> collection_of_main_wall_gid;
    my_manager.get_mapping().compute_matching_geom_id(main_wall_gid_pattern,
						      collection_of_main_wall_gid);
    for (unsigned int i = 0; i < collection_of_main_wall_gid.size(); i++)
      {
        collection_of_main_wall_gid[i].set_any(5);
      }
    
    // Select calo xwall GID :
    geomtools::geom_id xwall_gid_pattern(1232, 
					 module_number, 
					 geomtools::geom_id::ANY_ADDRESS,  // Side
					 geomtools::geom_id::ANY_ADDRESS,  // Wall
					 geomtools::geom_id::ANY_ADDRESS,  // Column
					 geomtools::geom_id::ANY_ADDRESS); // Row

    std::vector<geomtools::geom_id> collection_of_xwall_gid;
    my_manager.get_mapping().compute_matching_geom_id(xwall_gid_pattern,
						      collection_of_xwall_gid);

    // Select calo gveto GID : 
    geomtools::geom_id gveto_gid_pattern(1252, 
					 module_number, 
					 geomtools::geom_id::ANY_ADDRESS,  // Side
					 geomtools::geom_id::ANY_ADDRESS,  // Wall
					 geomtools::geom_id::ANY_ADDRESS); // Column

    std::vector<geomtools::geom_id> collection_of_gveto_gid;
    my_manager.get_mapping().compute_matching_geom_id(gveto_gid_pattern,
						      collection_of_gveto_gid);

    std::vector<calo_spurious_hit> collection_of_calo_spurious_hits;
    generate_pool_of_calo_spurious_hit(&random_generator,
				       collection_of_main_wall_gid,
				       collection_of_calo_spurious_hits);
    generate_pool_of_calo_spurious_hit(&random_generator,
				       collection_of_xwall_gid,
				       collection_of_calo_spurious_hits);
    generate_pool_of_calo_spurious_hit(&random_generator,
				       collection_of_gveto_gid,
				       collection_of_calo_spurious_hits);
    
    std::clog << "Main wall GID vector : " << collection_of_main_wall_gid.size() << std::endl;
    std::clog << "Xwall GID vector : " << collection_of_xwall_gid.size() << std::endl;
    std::clog << "Gveto GID vector : " << collection_of_gveto_gid.size() << std::endl;
    
    bool temp_file_delete = true;
    datatools::temp_file ftmp;
    ftmp.set_remove_at_destroy(temp_file_delete);

    std::clog << "Create a temporary file: " << std::endl;
    ftmp.create ("/tmp", "temp_");
    std::clog << "Temporary filename is '"
	      << ftmp.get_filename () << "' !" << std::endl;

    std::clog << "Write in the temporary file: " << std::endl;

    std::sort(collection_of_calo_spurious_hits.begin(), collection_of_calo_spurious_hits.end());
    for (unsigned int i = 1; i < collection_of_calo_spurious_hits.size(); i++)
      {
	if (collection_of_calo_spurious_hits.size() > 1) {
	  double delta_t = collection_of_calo_spurious_hits[i].timestamp - collection_of_calo_spurious_hits[i-1].timestamp;
	  ftmp.out() << collection_of_calo_spurious_hits[i-1].timestamp << ' ' <<  delta_t << std::endl;
	}
	// collection_of_calo_spurious_hits[i].tree_dump(std::clog, "A Calo spurious hit");
      }
    ftmp.close ();
    
    // Select geiger GID : 
    geomtools::geom_id geiger_gid_pattern(1204, 
					 module_number, 
					 geomtools::geom_id::ANY_ADDRESS,  // Side
					 geomtools::geom_id::ANY_ADDRESS,  // Layer
					 geomtools::geom_id::ANY_ADDRESS); // Row

    std::vector<geomtools::geom_id> collection_of_geiger_gid;
    my_manager.get_mapping().compute_matching_geom_id(geiger_gid_pattern,
						      collection_of_geiger_gid);

    std::clog << "Geiger GID vector : " << collection_of_geiger_gid.size() << std::endl;

    std::vector<geiger_spurious_hit> collection_of_geiger_spurious_hits;
    generate_pool_of_geiger_spurious_hit(&random_generator,
					 collection_of_geiger_gid,
					 collection_of_geiger_spurious_hits); 
    
    std::clog << "Number of calo spurious hits : " << collection_of_calo_spurious_hits.size() << std::endl;
    std::clog << "Number of geiger spurious hits : " << collection_of_geiger_spurious_hits.size() << std::endl;
    
    
    if (draw){
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test gnuplot draw' ");
      g1.cmd("set grid");
      g1.cmd("set xrange [0:1]");
      {
	std::ostringstream plot_cmd;
	plot_cmd << "plot '" << ftmp.get_filename() << "' using 1:2 with lines";
	g1.cmd(plot_cmd.str());
	g1.showonscreen(); // window output
	geomtools::gnuplot_drawer::wait_for_key();
	usleep(200);
      }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }
    
    std::clog << "Enter a value to close the program" << std::endl;
    std::string key ="";
    std::cin >> key;
    
    std::clog << "The end." << std::endl;
  }

  catch (std::exception & error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  }

  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }

  falaise::terminate();
  return error_code;
}

void generate_pool_of_calo_spurious_hit(mygsl::rng * rdm_gen_,
					const std::vector<geomtools::geom_id> & gid_collection_,
					std::vector<calo_spurious_hit> & calo_spurious_hit_collection_)
{
  int hit_count = 0;
  // Create spurious hits during a time interval for each calo GID :
  for (unsigned int i = 0; i < gid_collection_.size(); i++)
    {
      double time_interval = 1.; // second
      double calo_self_triggering_frequency = 5.; // Hertz
      double mean_number = time_interval * calo_self_triggering_frequency;
      double sigma_gauss = std::sqrt(mean_number);
      std::size_t number_of_calo_hit = 0;
      std::string distrib = "";
      // Number of calo hit during time_interval for a calo block (identified by his GID) :
      // If > 20 gaussian distribution
      if (mean_number > 20) {
	number_of_calo_hit = rdm_gen_->gaussian(mean_number, sigma_gauss);
	distrib = "gaussian";
      }
      // Else Poisson distribution
      else {
	number_of_calo_hit = rdm_gen_->poisson(mean_number);
	distrib = "poisson";
      }
	
      if (number_of_calo_hit < 0) number_of_calo_hit = 0;
	
      double yield_threshold = 0.15; // Calorimeter signal E 15-100% yield
      for (unsigned int j = 0; j < number_of_calo_hit; j++)
	{
	  calo_spurious_hit a_csh;
	  a_csh.set_geom_id(gid_collection_[i]);
	  a_csh.set_hit_id(hit_count);
	  a_csh.timestamp = rdm_gen_->flat(0, time_interval);
	  // Percentage > threshold (15% for example)
	  a_csh.yield = rdm_gen_->flat(yield_threshold, 1);
	  calo_spurious_hit_collection_.push_back(a_csh);
	  hit_count++;
	}
    }

  return;
}

void generate_pool_of_geiger_spurious_hit(mygsl::rng * rdm_gen_,
					const std::vector<geomtools::geom_id> & gid_collection_,
					std::vector<geiger_spurious_hit> & geiger_spurious_hit_collection_)
{
  int hit_count = 0;
  // Create spurious hits during a time interval for each geiger GID :
  for (unsigned int i = 0; i < gid_collection_.size(); i++)
    {
      double time_interval = 1.; // second
      double geiger_self_triggering_frequency = 10.; // Hertz
      double mean_number = time_interval * geiger_self_triggering_frequency;
      double sigma_gauss = std::sqrt(mean_number);
      std::size_t number_of_geiger_hit = 0;
      std::string distrib = "";
      // Number of geiger hit during time_interval for a geiger cell (identified by his GID) :
      // If > 20 gaussian distribution
      if (mean_number > 20) {
	number_of_geiger_hit = rdm_gen_->gaussian(mean_number, sigma_gauss);
	distrib = "gaussian";
      }
      // Else Poisson distribution
      else {
	number_of_geiger_hit = rdm_gen_->poisson(mean_number);
	distrib = "poisson";
      }
	
      if (number_of_geiger_hit < 0) number_of_geiger_hit = 0;
	
      // Introduce dead time : the geiger cell can not trigger again during a given time (1ms tipycally) 




      for (unsigned int j = 0; j < number_of_geiger_hit; j++)
	{
	  geiger_spurious_hit a_gsh;
	  a_gsh.set_geom_id(gid_collection_[i]);
	  a_gsh.set_hit_id(hit_count);
	  a_gsh.timestamp = rdm_gen_->flat(0, time_interval);
	  geiger_spurious_hit_collection_.push_back(a_gsh);
	  hit_count++;
	}
    }

  return;
}
