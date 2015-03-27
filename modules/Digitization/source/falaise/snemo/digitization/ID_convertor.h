// ID_convertor.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
//

#ifndef __ID_convertor_h
#define __ID_convertor_h 1


#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

#include <bayeux/geomtools/geom_id.h>

#include <bayeux/geomtools/manager.h>


namespace snemo {
  
  namespace digitization {
    
    class ID_convertor{

    public : 

      ID_convertor();
      ID_convertor(const geomtools::manager & a_manager_);
      virtual ~ID_convertor(); // Default D-tor
  
    private : 
      bool _debug_;
      const geomtools::manager * _geo_manager_;
  
    public :
  
      bool is_debug() const;
      void set_debug( bool );
      geomtools::geom_id convert_GID_to_EID(const geomtools::geom_id & geom_id_);
      geomtools::geom_id convert_EID_to_GID(const geomtools::geom_id & geom_id_);
      void convert_EID_to_GID();
      void initialize(const geomtools::manager & mgr_);
      
      void tree_dump(std::ostream & out_,
		     const std::string & title_ ,
		     const std::string & indent_,
		     bool inherit_);
    };



  } // end of namespace digitization

} // end of namespace snemo


#endif /* ID_convertor */
