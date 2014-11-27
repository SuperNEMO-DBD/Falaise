// -*- mode: c++ ; -*-
/* detector_manager.h
 * Author (s) :     Xavier Garrido <<garrido@lal.in2p3.fr>>
 * Creation date: 2010-06-30
 * Last modified: 2011-02-19
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Description:
 *   Detector Manager
 *
 * History:
 *
 */

#ifndef FALAISE_SNEMO_VISUALIZATION_DETECTOR_DETECTOR_MANAGER_H
#define FALAISE_SNEMO_VISUALIZATION_DETECTOR_DETECTOR_MANAGER_H 1

#include <map>
#include <vector>
#include <string>

// Interfaces
#include <falaise/snemo/utils/singleton.h>
#include <datatools/i_tree_dump.h>

namespace geomtools {
  class i_shape_3d;
  class geom_id;
  class geom_info;
}

class TGeoVolume;

namespace geomtools {
  class manager;
}

namespace snemo {

  namespace visualization {

    namespace detector {

      class i_volume;

      /// \brief The detector manager which handle all geometry volumes
      class detector_manager : public utils::singleton<detector_manager>,
                               public datatools::i_tree_dumpable
      {
      public:

        /// Experimental setup enum
        enum setup_label_type
          {
            UNDEFINED             = -1,
            SNEMO                 = 0,
            BIPO3                 = 1,
            TEST_BENCH            = 2,
            HPGE                  = 3,
            BIPO1                 = 4,
            TRACKER_COMMISSIONING = 5,
            SNEMO_DEMONSTRATOR    = 6
          };

        /// Volume dictionnary
        typedef std::map<geomtools::geom_id, i_volume*> volume_dict_type;

        /// Return initialization status
        bool is_initialized() const;

        /// Return construction status
        bool is_constructed() const;

        /// Return experimental setup enum
        setup_label_type get_setup_label() const;

        /// Return experimental setup name
        const std::string & get_setup_label_name() const;

        /// Return if 'volume_name_' is a special volume
        bool is_special_volume(const std::string & volume_name_) const;

        /// Get a mutable reference to volume
        i_volume * grab_volume(const geomtools::geom_id & id_);

        /// Get a non-mutable reference to volume
        const i_volume * get_volume(const geomtools::geom_id & id_) const;

        /// Get a list of matching geom_id
        void get_matching_ids(const geomtools::geom_id & id_,
                              std::vector<geomtools::geom_id> & vids_) const;

        /// Get volume name associated to geom_id
        std::string get_volume_name(const geomtools::geom_id & id_) const;

        /// Get volume category associated to geom_id
        std::string get_volume_category(const geomtools::geom_id & id_) const;

        /// Check if geometry manager has been externalized
        bool has_external_geometry_manager() const;

        /// Set external geometry manager
        void set_external_geometry_manager(geomtools::manager & geometry_manager_);

        /// Get a mutable reference to geometry manager
        geomtools::manager & grab_geometry_manager();

        /// Get a non-mutable reference to geometry manager
        const geomtools::manager & get_geometry_manager() const;

        /// Get a mutable reference to the 'world' volume
        TGeoVolume * grab_world_volume();

        /// Get non-mutable reference to the 'world' volume
        const TGeoVolume * get_world_volume() const;

        /// Initialize detector manager
        void initialize(const std::string & geo_manager_config_file_ = "");

        /// Construct geometrical volumes
        void construct();

        /// Update detector manager
        void update();

        /// Draw geometrical volumes
        void draw();

        /// Reset detector manager
        void reset();

        /// Smart print
        virtual void tree_dump(std::ostream & out_         = std::clog,
                               const std::string & title_  = "",
                               const std::string & indent_ = "",
                               bool inherit_               = false) const;

        /// Default print
        void dump() const;

      private:

        /// Forbid default constructor
        detector_manager();

        /// Forbid destructor (done within singleton object)
        virtual ~detector_manager();

        /// Non copyable constructor
        detector_manager(const detector_manager&);

        /// Non-copyable assignation
        detector_manager & operator=(const detector_manager&);

        /// Make the class singleton friend
        friend class utils::singleton<detector_manager>;

        /// Initialization of detector manager
        void _at_init_(const std::string & geo_manager_config_file_);

        /// Construction of detector manager
        void _at_construct_();

        /// Read geometry detector configuration
        void _read_detector_config_();

        /// Set volume categories with their visibility attributes
        void _set_categories_(std::vector<std::string> & only_categories_) const;

        /// Construct volume given its 3D shape and its geometry info
        void _set_volume_(const geomtools::geom_info & ginfo_);

        /// Add each volume contained in geo mapping
        void _add_volumes_();

        /// Set world coordinates
        void _set_world_dimensions_();

      private:
        bool                _initialized_;                   //!< Initialization flag
        bool                _constructed_;                   //!< Construction flag

        std::string         _setup_label_name_;              //!< Experimental setup label name
        setup_label_type    _setup_label_;                   //!< Experimental setup label

        std::vector<std::string> _special_volume_name_;      //!< List of special volumes

        volume_dict_type    _volumes_;                       //!< Volume dictionnary

        bool                _has_external_geometry_manager_; //!< External geometry manager

        std::string          _geo_manager_config_file_;      //!< Geometry manager configuration file
        geomtools::manager * _geo_manager_;                  //!< Geometry manager

        TGeoVolume        * _world_volume_;                  //!< ROOT world volume
        double              _world_length_;                  //!< World length
        double              _world_width_;                   //!< World width
        double              _world_height_;                  //!< World height
      };

    } // end of namespace detector

  } // end of namespace visualization

} // end of namespace snemo

#endif // FALAISE_SNEMO_VISUALIZATION_DETECTOR_DETECTOR_MANAGER_H

// end of detector_manager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
