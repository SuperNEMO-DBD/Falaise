File ``calo_hit.h``:

.. code:: sh

	  // Standard library:
	  #include <string>
	  #include <vector>

	  // Third party:
	  // - Boost:
	  // - Bayeux/datatools:
	  // Interface base class from datatools to support serialization tools:
	  #include <datatools/i_serializable.h>

	  struct calo_hit
	  : public datatools::i_serializable
	  {
	     ...

	     private:

	     uint32_t    _foo_;
	     std::string _bar_;
	     std::vector<double> _fuzz_;

	     /** This macro declares the serialization interface
	      *  within the Bayeux/datatools framework
	      */
	     DATATOOLS_SERIALIZATION_DECLARATION();
	  };
..


File ``calo_hit.cc``:

.. code:: sh
   namespace fecom {
   // This macro instantiate dedicated serialization code for the
   // "fecom::calo_hit" class:
   DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(
     calo_hit,
     "fecom::calo_hit")
   ...
}
..



File ``calo_hit-serial.ipp``:


.. code:: c++

   // Third party:
   // - Boost:
   // Support for inheritance from an serializable class :
   #include <boost/serialization/base_object.hpp>
   #include <boost/serialization/string.hpp>
   #include <boost/serialization/vector.hpp>
   // Support for XML 'key-value' based archives:
   #include <boost/serialization/nvp.hpp>
   // - Bayeux/datatools:
   // Datatools support for serializable objects :
   #include <datatools/i_serializable.ipp>

   /** The main Boost/Serialization template method for class calo_hit
   *  @arg ar an archive type (ASCII, XML or binary)
   *  @arg version the archive version number (not used)
   */
   template<class Archive>
   void raw_hit::serialize(Archive & ar, const unsigned int version)
   {
     ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
     ar & boost::serialization::make_nvp("foo",   _foo_);
     ar & boost::serialization::make_nvp("bar",   _bar_);
     ar & boost::serialization::make_nvp("fuzz",  _fuzz_);
     return;
   }
..



File ``commissioning_event.h``:

.. code:: c++

  #include <boost/cstdint.hpp>
  #include <boost/serialization/set.hpp>
  #include <datatools/i_serializable.h>
  #include <calo_hit.h>

  struct commissioning_event : public datatools::i_serializable
  {
  private:
   uint32_t _trigger_id_;
   std::set<calo_hit, calo_hit::compare>  _calo_hits_;
   DATATOOLS_SERIALIZATION_DECLARATION();
  };
  #include <boost/serialization/export.hpp>
  BOOST_CLASS_EXPORT_KEY2(commissioning_event, "fecom::commissioning_event");
..

File ``commissioning_event.cc``:

.. code:: c++
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(commissioning_event,"fecom::commissioning_event")
..


File ``commissioning_event.ipp``:


.. code:: c++
  template<class Archive>
  void commissioning_event::serialize(Archive & ar, const unsigned int version)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("trigger_id", _trigger_id_);
    ar & boost::serialization::make_nvp("calo_hits",  _calo_hits_);
    return;
  }
..

File ``the_serializable.h``:

.. code:: sh

   #include <boost/serialization/assume_abstract.hpp>
   #include <boost/serialization/export.hpp>
   #include <datatools/archives_instantiation.h>
   // Include the specific serialization code of serializable classes:
   #include <calo_hit.ipp>
   /*
   #include <tracker_channel_hit.ipp>
   #include <tracker_hit.ipp>
   */
   #include <commissioning_event.ipp>
   // Force instantiation of template code associated to the classes above
   // for the Boost archive classes defined in Bayeux/datatools:
   DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(calo_hit)
   /*
   DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(tracker_XXX_hit)
   */
   DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(commissioning_event)
   // Special implentation macro to register the class through the
   // Boost/Serialization system :
   BOOST_CLASS_EXPORT_IMPLEMENT(commissioning_event)
..

File ``convert.cxx``:

.. code:: sh

  #include <datatools/io_factory.h>
  int main()
  {
    {
      datatools::data_writer serializer("comevent.xml", datatools::using_multiple_archives);
      fecom::commissioning_event ev;
      ev.set_xxx();
      serializer.store(ev);
    }
    {
      datatools::data_reader deserializer("comevent.xml", datatools::using_multiple_archives);
      fecom::commissioning_event ev;
      deserializer.load(ev);
    }
    return 0;
   }
..
