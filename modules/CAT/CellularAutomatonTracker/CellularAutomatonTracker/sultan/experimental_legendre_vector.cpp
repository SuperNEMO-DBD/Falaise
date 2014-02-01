/* -*- mode: c++ -*- */

#include <sultan/experimental_legendre_vector.h>

namespace SULTAN {
  namespace topology{

    using namespace std;
    using namespace mybhep;


    void experimental_legendre_vector::set_helices(std::vector<experimental_helix> a){helices_ = a;}
    
    std::vector<experimental_helix> experimental_legendre_vector::helices(){return helices_;}
    
    void experimental_legendre_vector::set_clusters(std::vector<cluster_of_experimental_helices> a){clusters_ = a;}
    
    std::vector<cluster_of_experimental_helices> experimental_legendre_vector::clusters(){return clusters_;}
    
    void experimental_legendre_vector::set_nsigmas(double a){nsigmas_=a;}

    void experimental_legendre_vector::set_index_of_largest_cluster(int a){index_of_largest_cluster_=a;}

    double experimental_legendre_vector::get_nsigmas(){return nsigmas_;}

    int experimental_legendre_vector::get_index_of_largest_cluster(){return index_of_largest_cluster_;}

    void experimental_legendre_vector::add_helix(experimental_helix a){
      helices_.push_back(a);
    }
    
    void experimental_legendre_vector::reset(){
      helices_.clear();
      clusters_.clear();
      index_of_largest_cluster_=-1;
    }
    
    void experimental_legendre_vector::get_neighbours(experimental_helix a, std::vector<experimental_helix>* neighbours){
      experimental_double dx, dy, dz, dR, dH;
      neighbours->clear();
      for(std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end(); ++ip){
	if( !a.different_cells(*ip) ) continue;
	dx = a.x0() - ip->x0();
	if( fabs(dx.value()) > get_nsigmas()*dx.error() ) continue;
	dy = a.y0() - ip->y0();
	if( fabs(dy.value()) > get_nsigmas()*dy.error() ) continue;
	dz = a.z0() - ip->z0();
	if( fabs(dz.value()) > get_nsigmas()*dz.error() ) continue;
	dR = a.R() - ip->R();
	if( fabs(dR.value()) > get_nsigmas()*dR.error() ) continue;
	dH = a.H() - ip->H();
	if( fabs(dH.value()) > get_nsigmas()*dH.error() ) continue;
	neighbours->push_back(*ip);
      }
      return;
    }
    
    experimental_helix experimental_legendre_vector::max(std::vector<experimental_helix> * neighbours){
      experimental_helix r;
      size_t n = 0;
      size_t nmax = 0;
      std::vector<experimental_helix> neis;
      for(std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end(); ++ip){
	get_neighbours(*ip, &neis);
	
	if( neis.size() > nmax ){
	  nmax = neis.size();
	  *neighbours = neis;
	  r = *ip;
	}
      }
      
      return r;
    }

    experimental_helix experimental_legendre_vector::max(std::vector<size_t> * neighbouring_cells){
      experimental_helix r;
      size_t n = 0;
      size_t nmax = 0;
      std::vector<experimental_helix> neis, neis_best;
      for(std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end(); ++ip){
	get_neighbours(*ip, &neis);
	
	if( neis.size() > nmax ){
	  nmax = neis.size();
	  neis_best = neis;
	  r = *ip;
	}
      }
      
      neighbouring_cells->clear();
      std::vector<size_t> ids;
      for(std::vector<experimental_helix>::const_iterator ip = neis_best.begin(); ip != neis_best.end(); ++ip){
	ids = ip->ids();
	for(std::vector<size_t>::const_iterator id = ids.begin(); id!=ids.end(); ++id){
	  if( std::find(neighbouring_cells->begin(), neighbouring_cells->end(), *id) == neighbouring_cells->end() )
	    neighbouring_cells->push_back(*id);
	}
      }

      return r;
    }

    cluster_of_experimental_helices experimental_legendre_vector::max_cluster(experimental_helix * helix, bool * found){

      cluster_of_experimental_helices best_cluster;

      if( index_of_largest_cluster_ < 0 || index_of_largest_cluster_ > clusters_.size() ){
	if( print_level() >= mybhep::NORMAL )
	  std::clog << " problem: index_of_largest_cluster " << index_of_largest_cluster_ << " clusters size " << clusters_.size() <<  std::endl;
	*found = false;
	return best_cluster;
      }
      
      *found = true;
      best_cluster = clusters_[index_of_largest_cluster_];
      *helix = best_cluster.average_helix();

      return best_cluster;
    }

    void experimental_legendre_vector::add_helix_to_clusters(experimental_helix a){

      clock.start(" experimental_legendre_vector: add_helix_to_clusters ", "cumulative");

      bool a_maximum_is_already_defined = (index_of_largest_cluster_ >= 0);

      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " add helix "; a.print_ids(); std::clog << " when there are " << clusters_.size() << " clusters (";
	for(std::vector<cluster_of_experimental_helices>::iterator ic = clusters_.begin(); ic!=clusters_.end(); ++ic)
	  std::clog << ic->ids().size() << ", ";
	std::clog << ") largest " << index_of_largest_cluster_ << std::endl;
      }

      if( !a_maximum_is_already_defined ){
	create_cluster(a);
	index_of_largest_cluster_=0;
	clock.stop(" experimental_legendre_vector: add_helix_to_clusters ");
	return;
      }

      bool added = false;
      bool changed = false;
      for(std::vector<cluster_of_experimental_helices>::iterator ic = clusters_.begin(); ic!=clusters_.end(); ++ic){

	if( print_level() >= mybhep::VVERBOSE ){
	  std::clog << " compare helix with cluster " << ic - clusters_.begin() << " of " << ic->ids().size() << " cells " << std::endl;
	}

	if( ic->can_store__optimist(a, get_nsigmas()) ){
	  changed = ic->add_helix(a);
	  added = true;


	  if( ic->ids().size() > clusters_[index_of_largest_cluster_].ids().size() )
	    index_of_largest_cluster_ = ic - clusters_.begin();


	  if( print_level() >= mybhep::VVERBOSE ){
	    std::clog << " can store, changed: " << changed << std::endl;
	  }

	  if( changed ) merge_cluster_of_index(ic - clusters_.begin());

	  break;

	}
      }

      if( !added ){
	create_cluster(a);
      }

      clock.stop(" experimental_legendre_vector: add_helix_to_clusters ");
      return;

    }
    
    void experimental_legendre_vector::merge_cluster_of_index(size_t i){

      clock.start(" experimental_legendre_vector: merge_cluster_of_index ", "cumulative");

      cluster_of_experimental_helices _ic = clusters_[i];
      for(std::vector<cluster_of_experimental_helices>::const_iterator jc = clusters_.begin(); jc!=clusters_.end(); ++jc){
	if( jc - clusters_.begin() == i ) continue;
	if( jc->can_merge__optimist(_ic, nsigmas_) ){

	  if( print_level() >= mybhep::VVERBOSE ){
	    std::clog << " merge " << i << "[" << clusters_[i].ids().size() << "] and " << jc - clusters_.begin() << " [" << jc->ids().size() << "]" << std::endl;
	  }
	  
	  _ic = _ic.merge(*jc);
	  clusters_[i]=_ic;
	  clusters_.erase(clusters_.begin() + (jc - clusters_.begin()));
	  break;
	}
      }

      size_t max_n_ids = 0;
      for(std::vector<cluster_of_experimental_helices>::const_iterator jc = clusters_.begin(); jc!=clusters_.end(); ++jc){
	if( jc->ids().size() > max_n_ids ){
	  max_n_ids = jc->ids().size();
	  index_of_largest_cluster_ = jc - clusters_.begin();
	}
      }

      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " --> (";
	for(std::vector<cluster_of_experimental_helices>::iterator qc = clusters_.begin(); qc!=clusters_.end(); ++qc)
	  std::clog << qc->ids().size() << ", ";
	std::clog << ") largest " << index_of_largest_cluster_ << std::endl;
      }

      return;

      clock.stop(" experimental_legendre_vector: merge_cluster_of_index ");
    }

    void experimental_legendre_vector::merge_clusters(){

      for(std::vector<cluster_of_experimental_helices>::const_iterator ic = clusters_.begin(); ic!=clusters_.end()-1; ++ic)
	merge_cluster_of_index(ic - clusters_.begin());

    }

    void experimental_legendre_vector::create_cluster(experimental_helix a){
      cluster_of_experimental_helices newc;
      newc.add_helix(a);
      newc.set_helix_min(a);
      newc.set_helix_max(a);
      clusters_.push_back(newc);

    }

    void experimental_legendre_vector::print_clocks() {
      clock.dump();
    }

    bool experimental_legendre_vector::assign_cell(size_t cell_id){

      size_t maxn = 0;
      std::vector<size_t> ids;
      size_t cluster_index;

      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " assign cell " << cell_id << " when there are " << clusters_.size() << " clusters " << std::endl;
      }



#if 0
      // method 1
      // assign the cell to that cluster of triplets/helices
      // which contains the largest number of different cells

      // loop on clusters of triplets/cells
      for(std::vector<cluster_of_experimental_helices>::const_iterator ic = clusters_.begin(); ic!=clusters_.end(); ++ic){
	ids = ic->ids();
	if( print_level() >= mybhep::VVERBOSE ){
	  std::clog << " cluster " << ic - clusters_.begin() << " has " << ids.size() << " cells "; ic->print_ids(); std::clog << " " << std::endl;
	}

	// if the cluster contains cell_id
	if( std::find(ids.begin(), ids.end(), cell_id) != ids.end() ){
	  // look for the cluster with largest number of different cells
	  if( ids.size() > maxn ){
	    maxn = ids.size();
	    cluster_index = ic - clusters_.begin();
	  }
	  if( print_level() >= mybhep::VVERBOSE ){
	    std::clog << " contained in cluster with " << ids.size() << " different cells, maximum now is " << maxn << std::endl;
	  }
	}
      }

      // pick the containing cluster with the largest n of different cells
      if( maxn > 0 ){
	if( print_level() >= mybhep::VVERBOSE ){
	  std::clog << " finally assigned to cluster " << cluster_index << " containing " << maxn << " different cells " << std::endl;
	}
	clusters_[cluster_index].add_assigned_id(cell_id);
	return true;
      }
      
      if( print_level() >= mybhep::VVERBOSE ){
	std::clog << " finally not assigned " << std::endl;
      }

      return false;
#else
      // method 2
      // assign the cell to any cluster of triplets/helices
      // which contains >= 4 different cells

      size_t min_n_different_cells = 4;
      bool assigned = false;
      
      // loop on clusters of triplets/cells
      for(std::vector<cluster_of_experimental_helices>::iterator ic = clusters_.begin(); ic!=clusters_.end(); ++ic){
	ids = ic->ids();
	if( print_level() >= mybhep::VVERBOSE ){
	  std::clog << " cluster " << ic - clusters_.begin() << " has " << ids.size() << " cells "; ic->print_ids(); std::clog << " " << std::endl;
	}
	
	// if the cluster contains cell_id
	if( std::find(ids.begin(), ids.end(), cell_id) != ids.end() ){
	  
	  if( print_level() >= mybhep::VVERBOSE ){
	    std::clog << " contained in cluster with " << ids.size() << " different cells, minimum is " << min_n_different_cells << std::endl;
	  }
	  // if the cluster contains enough different cells
	  if( ids.size() >= min_n_different_cells ){
	    ic->add_assigned_id(cell_id);
	    assigned = true;
	  }
	}
      }
    
      return assigned;
#endif

    }
    
  }
  
  
}


