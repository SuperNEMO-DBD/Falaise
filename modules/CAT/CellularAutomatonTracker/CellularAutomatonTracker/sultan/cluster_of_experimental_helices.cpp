#include <sultan/cluster_of_experimental_helices.h>
#include <algorithm>

namespace SULTAN {
  namespace topology{


    bool cluster_of_experimental_helices::can_store__optimist(experimental_helix a, double nsigmas){

      //
      // can store a in cluster
      // only if it adds at least one new cell id
      //
      // 1st case
      //  __ min__ ........ __max__  __a__
      //  min < a, max !> a
      //  false
      //
      // 2nd case
      //  __ min__ .....__max__a__
      //  min < a, max > a
      //  true
      //
      // 3rd case
      //  __ min__ .....__a__max__
      //  min < a, max > a
      //  true
      //
      // 4th case
      //  __ min__ ...__a__...max__
      //  min < a, max > a
      //  true
      //
      // 5th case
      //  __ min___a__...max__
      //  min < a, max > a
      //  true
      //
      // 6th case
      //  __ a___min__...max__
      //  min < a, max > a
      //  true
      //
      // 6th case
      //  __ a__  ___min__.....max__
      //  min !< a, max > a
      //  false
      //

      if( !helices_.size() ) return true;

      if( !helix_min_.is_less_than__optimist(a,nsigmas) ) return false;

      if( !helix_max_.is_more_than__optimist(a,nsigmas) ) return false;

      return different_cells(a);

    }

    bool cluster_of_experimental_helices::can_merge__optimist(cluster_of_experimental_helices cluster, double nsigmas)const{

      //
      // can merge cluster with this?
      // only if it adds at least one new cell id
      //
      // 1st case
      //  ___ this___  ...cluster...
      //  min < cluster_max, max !> cluster_min
      //  false
      //
      // 2nd case
      //  ___ this__..cluster...
      //  min < cluster_max, max > cluster_min
      //  true
      //
      // 3rd case
      //  ...cluster..__ this___
      //  min < cluster_max, max > cluster_min
      //  true
      //
      // 4th case
      //  ...cluster... ___ this___
      //  min !< cluster_max, max > cluster_min
      //  false
      //

      if( !helices_.size() ) return true;

      if( !cluster.helices_.size() ) return true;

      if( !helix_min_.is_less_than__optimist(cluster.helix_max_,nsigmas) ) return false;

      if( !helix_max_.is_more_than__optimist(cluster.helix_min_,nsigmas) ) return false;

      return different_cells(cluster);
    }

    bool cluster_of_experimental_helices::different_cells(topology::experimental_helix b)const{
      std::vector<size_t> bids = b.ids();
      for(std::vector<size_t>::const_iterator id = ids_.begin(); id !=ids_.end(); ++id){
        if( std::find(bids.begin(), bids.end(), *id) == bids.end() )
          return true;
      }

      for(std::vector<size_t>::const_iterator id = bids.begin(); id !=bids.end(); ++id){
        if( std::find(ids_.begin(), ids_.end(), *id) == ids_.end() )
          return true;
      }

      return false;

    }

    bool cluster_of_experimental_helices::different_cells(topology::cluster_of_experimental_helices b)const{
      std::vector<size_t> bids = b.ids();
      for(std::vector<size_t>::const_iterator id = ids_.begin(); id !=ids_.end(); ++id){
        if( std::find(bids.begin(), bids.end(), *id) == bids.end() )
          return true;
      }

      for(std::vector<size_t>::const_iterator id = bids.begin(); id !=bids.end(); ++id){
        if( std::find(ids_.begin(), ids_.end(), *id) == ids_.end() )
          return true;
      }

      return false;

    }

    bool cluster_of_experimental_helices::add_helix(experimental_helix a){
      // return true if the helix we are adding changes the min or the max of the cluster

      // update cluster min if necessary
      bool changed_min = false;

      experimental_helix min = helix_min();
      if( min.x0().value() > a.x0().value() ){
        changed_min = true;
        min.set_x0(a.x0());
      }
      if( min.y0().value() > a.y0().value() ){
        changed_min = true;
        min.set_y0(a.y0());
      }
      if( min.z0().value() > a.z0().value() ){
        changed_min = true;
        min.set_z0(a.z0());
      }
      if( min.R().value() > a.R().value() ){
        changed_min = true;
        min.set_R(a.R());
      }
      if( min.H().value() > a.H().value() ){
        changed_min = true;
        min.set_H(a.H());
      }
      if( changed_min ){
        set_helix_min(min);
      }

      // update cluster max if necessary
      bool changed_max = false;

      experimental_helix max = helix_max();
      if( max.x0().value() < a.x0().value() ){
        changed_max = true;
        max.set_x0(a.x0());
      }
      if( max.y0().value() < a.y0().value() ){
        changed_max = true;
        max.set_y0(a.y0());
      }
      if( max.z0().value() < a.z0().value() ){
        changed_max = true;
        max.set_z0(a.z0());
      }
      if( max.R().value() < a.R().value() ){
        changed_max = true;
        max.set_R(a.R());
      }
      if( max.H().value() < a.H().value() ){
        changed_max = true;
        max.set_H(a.H());
      }

      if( changed_max ){
        set_helix_max(max);
      }


      // add helix
      helices_.push_back(a);
      std::vector<size_t> ids = a.ids();
      for( std::vector<size_t>::const_iterator id = ids.begin(); id != ids.end(); ++id)
        add_id(*id);

      return changed_min || changed_max;
    }


    void cluster_of_experimental_helices::add_helices(std::vector<experimental_helix> as){
      helices_.insert(helices_.end(),as.begin(),as.end());
    }


    cluster_of_experimental_helices cluster_of_experimental_helices::merge(cluster_of_experimental_helices b)const{

      // create min for new cluster
      experimental_helix mina = this->helix_min();
      experimental_helix minb = b.helix_min();

      experimental_helix newmin(experimental_min(mina.x0(), minb.x0()),
                                experimental_min(mina.y0(), minb.y0()),
                                experimental_min(mina.z0(), minb.z0()),
                                experimental_min(mina.R(), minb.R()),
                                experimental_min(mina.H(), minb.H()));

      b.set_helix_min(newmin);

      // create max for new cluster
      experimental_helix maxa = this->helix_max();
      experimental_helix maxb = b.helix_max();

      experimental_helix newmax(experimental_max(maxa.x0(), maxb.x0()),
                                experimental_max(maxa.y0(), maxb.y0()),
                                experimental_max(maxa.z0(), maxb.z0()),
                                experimental_max(maxa.R(), maxb.R()),
                                experimental_max(maxa.H(), maxb.H()));

      b.set_helix_max(newmax);

      // merge the two clusters in the new cluster
      b.add_helices(helices());
      std::vector<size_t> ids = this->ids();
      for( std::vector<size_t>::const_iterator id = ids.begin(); id != ids.end(); ++id)
        b.add_id(*id);


      return b;

    }

    void cluster_of_experimental_helices::add_id(size_t id){
      std::vector<size_t>::const_iterator fid = std::find(ids_.begin(), ids_.end(), id);
      if( fid == ids_.end() ){
        ids_.push_back(id);
      }
    }

    void cluster_of_experimental_helices::add_assigned_id(size_t id){
      std::vector<size_t>::const_iterator fid = std::find(assigned_ids_.begin(), assigned_ids_.end(), id);
      if( fid == assigned_ids_.end() ){
        assigned_ids_.push_back(id);
      }
    }

    experimental_helix cluster_of_experimental_helices::average_helix()const{

      experimental_double x0, y0, z0, R, H;
      std::vector<experimental_double> x0s, y0s, z0s, Rs, Hs;

      for(std::vector<experimental_helix>::const_iterator ih=helices_.begin(); ih!=helices_.end(); ++ih){
        x0s.push_back(ih->x0());
        y0s.push_back(ih->y0());
        z0s.push_back(ih->z0());
        Rs.push_back(ih->R());
        Hs.push_back(ih->H());
      }

      x0=weighted_average(x0s);
      y0=weighted_average(y0s);
      z0=weighted_average(z0s);
      R=weighted_average(Rs);
      H=weighted_average(Hs);

      return experimental_helix(x0, y0, z0, R, H);

    }

  }

}
