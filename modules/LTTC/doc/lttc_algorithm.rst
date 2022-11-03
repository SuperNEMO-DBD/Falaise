====================================================
LTTC Algorithm Documentation
====================================================

:author: F.Mauger
:date: 2021-11-18


Data structures
==================

Configuration ``lttc_algo::config`` 
---------------------------------------

* ``logging`` : verbosity level
* ``mode`` : line/helix research mode
* Step 1 configuration

  * ``step1_ntbins`` :
  * ``step1_nrbins`` :
  * ``step1_track_threshold`` :
  
* Step 2 configuration

  * ``step2_max_nlines`` :
  * ``step2_delta_theta`` :
  * ``step2_delta_r`` :
  * ``step2_gauss_threshold`` :
  * ``step2_track_threshold`` :
  
* Step 3 configuration
 
  * ``step3_nsigma`` :



Input data ``lttc_algo::input_data`` 
---------------------------------------

Attributes:

* ``hits `` (``tracker_hit_collection``)


Internal working data
----------------------------------------
   
* class ``map_type`` :
   
  * ``id`` : ``int``
  * ``tmin`` : ``double``
  * ``tmax`` : ``double``
  * ``rmin`` : ``double``
  * ``rmax`` : ``double``
  * ``nt`` : ``size_t``
  * ``nr`` : ``size_t``
  * ``dt`` : ``double``
  * ``dr`` : ``double``
  * ``bins`` : ``std::vector<double>``
  * ``sorted_bins`` : ``std::vector<int>``
    
* class ``cluster`` :
   
  * ``id`` : ``int``
  * ``peak_bin_index`` : ``int``
  * ``peak_height`` : ``double``
  * ``peak_tbin`` : ``int``
  * ``peak_rbin`` : ``int``
  * ``threshold_height`` : ``double``
  * ``itmin`` : ``int``
  * ``itmax`` : ``int``
  * ``irmin`` : ``int``
  * ``irmax`` : ``int``
  * ``t_step`` : ``double``
  * ``t_min`` : ``double``
  * ``t_min`` : ``double``
  * ``r_step`` : ``double``
  * ``r_min`` : ``double``
  * ``r_min`` : ``double``
  * ``nbins_over_threshold`` : ``size_t``
  * ``hits_per_bin_ratio`` : ``double``
  * ``t_mean`` : ``double``
  * ``r_mean`` : ``double``
  * ``t_err`` : ``double``
  * ``r_err`` : ``double``
  * ``bins`` : ``std::map<int, double>``
  * ``line_data`` : ``line``
    
* class ``clustering_data`` :
   
  * ``clusters`` : ``std::vector<cluster>``
  * ``unclustered`` : ``std::set<int>``
   
* class ``cluster_finder`` :
   
  * ``debug`` : ``bool``
  * ``trmap`` : ``const map_type *``
   
* class ``trc_data`` :
  
  * ``trmap`` : ``map_type``
  * ``clustering`` : ``clustering_data``
   
* class ``step1_data`` : ``trc_data``
  
* class ``trc_ref`` :
 
  * ``ptrmap`` : ``const map_type *``
  * ``pcluster`` : ``const cluster *``
   
* class ``step2_data`` :
 
  * ``trmaps`` : ``std::vector<trc_data>``
  * ``clusters`` : ``std::vector<trc_ref>``
   
* class ``step3_data`` :
 
  * ``HC`` : ``int_array2_type = boost::multi_array<int, 2>``
   
.. * class ``cluster_data`` :
..
..  * ``ref`` : ``trc_ref``
..  * ``hits`` : ``std::set<int>``


Output data ``lttc_algo::output_data`` 
---------------------------------------
 
* class ``hit_cluster_data`` :

  * ``id`` : ``int``
  * ``flags`` : ``uint32_t``
  * ``hits`` : ``std::set<int>``
  * ``far_hit_1`` : ``int``
  * ``far_hit_2`` : ``int``
  * ``line_data`` : ``line``
    
   
* class ``hit_clustering_data`` :

  * ``clusters`` :  ``std::vector<hit_cluster_data>``
  * ``unclustered_hits`` :  ``std::set<int>`` 

    
* class ``lttc_algo::output_data`` :
    
  * ``hit_clustering`` : ``hit_clustering_data``


.. end

