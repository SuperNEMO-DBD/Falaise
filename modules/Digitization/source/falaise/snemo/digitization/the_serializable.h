/// \file falaise/snemo/digitization/the_serializable.h

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_THE_SERIALIZABLE_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_THE_SERIALIZABLE_H

// Third party:
// - Boost :
#include <boost/serialization/export.hpp>
// - Bayeux/datatools :
#include <datatools/archives_instantiation.h>

// This project:
#include <snemo/digitization/calo_tp.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::digitization::calo_tp)

#include <snemo/digitization/calo_tp_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::digitization::calo_tp_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::digitization::calo_tp_data)

#include <snemo/digitization/calo_ctw.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::digitization::calo_ctw)

#include <snemo/digitization/calo_ctw_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::digitization::calo_ctw_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::digitization::calo_ctw_data)

#include <snemo/digitization/geiger_tp.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::digitization::geiger_tp)

#include <snemo/digitization/geiger_tp_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::digitization::geiger_tp_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::digitization::geiger_tp_data)

#include <snemo/digitization/geiger_ctw.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::digitization::geiger_ctw)

#include <snemo/digitization/geiger_ctw_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snemo::digitization::geiger_ctw_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snemo::digitization::geiger_ctw_data)

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_THE_SERIALIZABLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
