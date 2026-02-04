// falaise/snemo/datamodels/geomid_utils.cc

// Ourselves:
#include <falaise/snemo/datamodels/geomid_utils.h>

// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/logger.h>

namespace snemo {

  namespace datamodel {

    std::size_t number_of_oms()
    {
      return 717u;
    }

    std::size_t number_of_ggs()
    {
      return 2034u;
    }

    bool om_num_is_valid(const std::uint32_t num_)
    {
      if (num_ >= number_of_oms()) return false;
      return true;
    }

    bool gg_num_is_valid(const std::uint32_t num_)
    {
      if (num_ >= number_of_ggs()) return false;
      return true;
    }

    int om_num(const geomtools::geom_id & gid)
    {
      switch (gid.get_type())
        {
        case 1302: // MWALL case
        case 1301:
          return gid.get(1)*20*13 + gid.get(2)*13 + gid.get(3);

        case 1232: // XWALL case
        case 1231:
          return 520 + gid.get(1)*2*2*16 + gid.get(2)*2*16 + gid.get(3)*16 + gid.get(4);

        case 1252: // GVETO case
        case 1251:
          return 520 + 128 + gid.get(1)*2*16 + gid.get(2)*16 + gid.get(3);

        case 90: // Reference OM
          return gid.get(0);

        default:
          break;
        }

      return -1;
    }

    bool is_main_wall_om(const geomtools::geom_id & gid_)
    {
      if (gid_.get_type() == 1301 or gid_.get_type() == 1302) {
        return true;
      }
      return false;
    }
    
    bool is_xwall_om(const geomtools::geom_id & gid_)
    {
      if (gid_.get_type() == 1231 or gid_.get_type() == 1232) {
        return true;
      }
      return false;
    }
    
    bool is_gveto_om(const geomtools::geom_id & gid_)
    {
      if (gid_.get_type() == 1252 or gid_.get_type() == 1253) {
        return true;
      }
      return false;
    }

    bool is_reference_om(const geomtools::geom_id & gid_)
    {
      if (gid_.get_type() == 90) {
        DT_THROW_IF(gid_.get_depth() != 1, std::logic_error, "Invalid depth");
        return true;
      }
      return false;
    }

    const std::map<int, geomtools::geom_id> & om_map()
    {
      static std::map<int, geomtools::geom_id> _om_map;
      if (_om_map.empty()) {
        // fill the inverted map
        const int moduleNumber = 0;
        for (int side = 0; side < 2; side++) {
          for (int col = 0; col < 20; col++) {
            for (int row = 0; row < 13; row++) {
              geomtools::geom_id gid(1301, moduleNumber, side, col, row);
              auto num = snemo::datamodel::om_num(gid);
              _om_map[num] = gid;
            }
          }
          for (int wall = 0; wall < 2; wall++) {
            for (int col = 0; col < 2; col++) {
              for (int row = 0; row < 16; row++) {
                geomtools::geom_id gid(1231, moduleNumber, side, wall, col, row);
                auto num = snemo::datamodel::om_num(gid);
                _om_map[num] = gid;
              }
            }
          }
          for (int wall = 0; wall < 2; wall++) {
            for (int row = 0; row < 16; row++) {
              geomtools::geom_id gid(1251, moduleNumber, side, wall, row);
              auto num = snemo::datamodel::om_num(gid);
              _om_map[num] = gid;
            }
          }
        }
        // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Number of OM map entries = " << _om_map.size());
        // DT_THROW(std::runtime_error, "Test geomid_utils.cc");
        // Reference OMs:
        for (int num = 712; num <= 716; num++) {
          geomtools::geom_id gid(90, num);
          _om_map[num] = gid;
        }
        // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Number of OM map entries = " << _om_map.size());
      }
      return _om_map;
    }
    
    geomtools::geom_id om_gid(const int num_, bool block_, bool back_part_)
    {
      const auto & omMap = om_map();
      auto found = omMap.find(num_);
      DT_THROW_IF(found == omMap.end(), std::logic_error,
                  "Invalid OM number " << num_ << "!");
      auto gid = found->second;
      if (block_) {
        int omType = gid.get_type();
        gid.set_type(omType + 1); // MWall: 1301->1302, XWall: 1231->1232, GVeto:1252->1253
        if (gid.get_type() == 1302) {
          gid.set(4, back_part_ ? 0 : 1); // Set the back or front part of the main calo block
        }
      }
      return gid;
    }

    int gg_num(const geomtools::geom_id & gid)
    {
      switch (gid.get_type())
        {
        case 1204: // GG case
        case 1203:
          return gid.get(1)*1017 + gid.get(3)*9 + gid.get(2);

        default:
          break;
        }

      return -1;
    }

    const std::map<int, geomtools::geom_id> & gg_map()
    {
      static std::map<int, geomtools::geom_id> _gg_map;
      if (_gg_map.empty()) {
        // fill the inverted map
        const int moduleNumber = 0;
        for (int side = 0; side < 2; side++) {
          for (int layer = 0; layer < 9; layer++) {
            for (int row = 0; row < 113; row++) {
              geomtools::geom_id gid(1203, moduleNumber, side, layer, row);
              auto num = snemo::datamodel::gg_num(gid);
              _gg_map[num] = gid;
            }
          }
        }
      }
      return _gg_map;
    }

    geomtools::geom_id gg_gid(const int num_, bool cell_core_)
    {
      auto found = gg_map().find(num_);
      DT_THROW_IF(found == gg_map().end(), std::logic_error,
                  "Invalid geiger cell number " << num_ << "!");
      auto gid = found->second;
      if (cell_core_) {
        int ggType = gid.get_type();
        gid.set_type(ggType + 1); // 1203->1204
      }
      return gid;
    }

    std::string om_label (const geomtools::geom_id & gid)
    {
      std::string label;

      switch (gid.get_type())
        {
        case 1302: // MWALL case
        case 1301:
          label = "M:";
          label += std::to_string(gid.get(1));
          label += '.';
          label += std::to_string(gid.get(2));
          label += '.';
          label += std::to_string(gid.get(3));
          return label;

        case 1232: // XWALL case
        case 1231:
          label = "X:";
          label += std::to_string(gid.get(1));
          label += '.';
          label += std::to_string(gid.get(2));
          label += '.';
          label += std::to_string(gid.get(3));
          label += '.';
          label += std::to_string(gid.get(4));
          return label;

        case 1252: // GVETO case
        case 1251:
          label = "G:";
          label += std::to_string(gid.get(1));
          label += '.';
          label += std::to_string(gid.get(2));
          label += '.';
          label += std::to_string(gid.get(3));
          return label;
          
        case 90: // Reference OM
          label = "R:";
          label += std::to_string(gid.get(0));
          
        default:
          break;
        }

      return label;
    }

    std::string gg_label(const geomtools::geom_id & gid)
    {
      std::string label;

      switch (gid.get_type())
        {
        case 1204: // GG case
        case 1203:
          label = "GG:";
          label += std::to_string(gid.get(1));
          label += '.';
          label += std::to_string(gid.get(3));
          label += '.';
          label += std::to_string(gid.get(2));
          return label;

        default:
          break;
        }

      return label;
    }
 
  } // end of namespace datamodel

} // end of namespace snemo

