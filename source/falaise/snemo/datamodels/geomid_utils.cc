// falaise/snemo/datamodels/geomid_utils.cc

// Ourselves:
#include <falaise/snemo/datamodels/geomid_utils.h>

namespace snemo {

  namespace datamodel {

    int om_num (const geomtools::geom_id & gid)
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

	default:
	  break;
	}

      return -1;
    }

    int gg_num (const geomtools::geom_id & gid)
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

	default:
	  break;
	}

      return label;
    }

    std::string gg_label (const geomtools::geom_id & gid)
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

