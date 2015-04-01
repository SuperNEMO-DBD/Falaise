// snemo/digitization/ID_convertor.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ID_CONVERTOR_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ID_CONVERTOR_H 

// Standard library :

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>

// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/manager.h>
// - Bayeux/datatools :
#include <bayeux/datatools/logger.h>

// Ourselves:
#include <snemo/digitization/mapping.h>

namespace snemo {

  namespace geometry { 
    class gg_locator;
    class calo_locator;
    class xcalo_locator;
    class gveto_locator;
  }

  namespace digitization {
    
    class ID_convertor : boost::noncopyable {

    public : 

      ID_convertor();
      ID_convertor(const geomtools::manager & mgr_, int module_number_);
      virtual ~ID_convertor(); // Default D-tor
  
    private : 
      bool _initialized_;
      datatools::logger::priority _logging_;
      int _module_number_;
      const geomtools::manager * _geo_manager_;
      boost::scoped_ptr<geometry::gg_locator>  _gg_locator_; 
      boost::scoped_ptr<geometry::calo_locator> _calo_locator_;
      boost::scoped_ptr<geometry::xcalo_locator> _xcalo_locator_;
      boost::scoped_ptr<geometry::gveto_locator> _gveto_locator_;
  
    // protected :
      
    //   const geometry::gg_locator & _get_gg_locator() const;
    //   const geometry::calo_locator & _get_calo_locator() const;
    //   const geometry::xcalo_locator & _get_xcalo_locator() const;
    //   const geometry::gveto_locator & _get_gveto_locator() const;

    public :
  
      bool is_initialized() const;
      void initialize();
      void reset();
      void set_logging(datatools::logger::priority);
      datatools::logger::priority get_logging() const;
      geomtools::geom_id convert_GID_to_EID(const geomtools::geom_id & geom_id_) const;
      geomtools::geom_id convert_EID_to_GID(const geomtools::geom_id & geom_id_) const;
      void convert_EID_to_GID();
      void set_geo_manager(const geomtools::manager & mgr_);
      void set_module_number(int);
			void find_gg_eid_and_tp_bit_index(const geomtools::geom_id & geom_id_,
																		 geomtools::geom_id & electronic_id_,
																		 int & bit_index_) const;
      
      void tree_dump(std::ostream & out_,
		     const std::string & title_ ,
		     const std::string & indent_,
		     bool inherit_);
    };



  } // end of namespace digitization

} // end of namespace snemo


#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_ID_CONVERTOR_H 

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
