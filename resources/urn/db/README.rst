===================================
Registered tags
===================================

File contents
================

* ``experiment.conf`` : Definition of the URNs associated to Falaise managed experiments

  - Contains URN item of the ``"experiment"`` category.
  - Currently contains only one URN item for the SuperNEMO demonstrator experiment.
  
* ``snemo_geometry-5.X.conf`` : Definition of different versions of the geometry configuration.

  - Contains URN items of the ``"geomsetup"`` category associated to URN items of the ``"variants"``,
    ``"varprofile"`` and ``"services"`` categories.
  
* ``snemo_setup-2.X.conf`` : Definition of different versions of the experimental setup.

  -  An experimental setup is typically defined by a geometry setup and possibly by a
     hardware setup (electronics, control system...)
  - Contains URN items of the ``"expsetup"`` category associated to URN items of the ``"variants"``,
    ``"varprofile"`` and ``"services"`` categories.
  
* ``snemo_vertex-5.X.conf`` : Definition of different versions of the vertex generation system (for simulations).
  
* ``snemo_decays-1.4.conf`` : Definition of different versions of the primary decays generation system (for simulations).
  
* ``snemo_simulation-2.4.conf`` : Definition of the simulation setup version 2.4.

  -  Contains URN items of the ``"simsetup"`` category associated to URN items of the ``"variants"``,
    ``"varprofile"``,  ``"configuration"`` and  ``"services"`` categories.
   
* ``snemo_simulation-2.5.conf`` : Definition of the simulation setup version 2.5.
 
* ``snemo_reconstruction-3.X.conf`` : Definition of different versions of the reconstruction setup.

  - Contains URN items of the ``"recsetup"`` category associated to URN items of the ``"variants"``,
    ``"varprofile"``,  ``"configuration"`` and  ``"services"`` categories.
 


Categories of URN tags
======================

Supported categories of various published URN tags:

* ``experiment`` : identifier/tag   associated  to   an  experiment
  (example: ``"BiPo3"``, ``"SuperNEMO demonstrator"``)
* ``geomsetup`` : identifier/tag associated to a geometry setup
* ``expsetup`` : identifier/tag associated to an experimental setup
* ``simsetup`` : identifier/tag associated to a simulation setup
* ``recsetup`` : identifier/tag associated to a reconstruction setup
* ``recpipeline`` : identifier/tag associated to a reconstruction pipeline
* ``services`` :  identifier/tag associated to the configuration  of a
  service management system
* ``configuration`` : identifier/tag associated to  the configuration
  of  some   generic  system  or  service   (geometry,  reconstruction
  modules, vertex or decay generation...)
* Variant system :

  * ``varregistry`` :  identifier/tag associated  to the configuration  of a
    variant registry 
  * ``varprofile`` : identifier/tag associated  to a variant  profile. A
    varprofile tag must have one topic:
  * ``defvarprofile`` : identifier/tag associated  to a default variant  profile which is generally defined as an alias
    of a varprofile item
  * ``vargdm`` : identifier/tag associated  to variant global dependency manager rules.
  * ``varservice`` :  identifier/tag associated  to the configuration  of a
    variant service which should contains items of types: ``varregistry``, ``varprofile``, ``defvarprofile`` and possibly ``vargdm``


Composition of a simulation setup configuration
=================================================

TO BE REVIEWED

A simulation *setup*  tag which has a dependee  tag of the ``variants``  category may
also have two specific topics relative to *variants* support:

 * ``varprofiles`` :  the topic  for the  list of  *official* variant
   profiles registered within the setup and associated to the variant
   service used by the setup.
 * ``defvarprofile``  : the  topic  for the  unique *default*  variant
   profile (typically an alias of a published variant profile from the
   list of *official* variant profiles).


Examples
========

Layout of a mock simulation setup
-------------------------------------

.. code::
   
   ########################################
   #   Simulation variant setup tag 7.0   #
   ########################################

   [urn="urn:snemo:demonstrator:simulation:variants:7.0" category="varregistry"]
   description : string = "Simulation variants setup (tag 7.0) for SuperNEMO demonstrator simulation"

   [urn="urn:snemo:demonstrator:simulation:variants:service:7.0:profiles:basic-1.0" category="varprofile"]
   description : string = "Variant profile 'basic-7.0' for the SuperNEMO demonstrator simulation setup"
 
   [urn="urn:snemo:demonstrator:simulation:variants:service:7.0:profiles:basic-2.0" category="varprofile"]
   description : string = "Variant profile 'basic-2.0' for the SuperNEMO demonstrator simulation setup"
 
   [urn="urn:snemo:demonstrator:simulation:variants:service:7.0:profiles:default" category="defvarprofile"]
   description : string    = "Default variant profile for the SuperNEMO demonstrator simulation"
   topics      : string[1] = "alias_of"
   topic.alias_of.component : string  = "urn:snemo:demonstrator:simulation:variants:service:7.0:profiles:basic-1.0"
  
   [urn="urn:snemo:demonstrator:simulation:variants:service:7.0:gdm" category="vargdm"]
   description : string = "Simulation variants service GDM"

   [urn="urn:snemo:demonstrator:simulation:variants:service:7.0" category="varservice"]
   description : string = "SuperNEMO demonstrator simulation variants service"
   topics : string[4] = "registries" "profiles" "defprofile" "gdm"
   topic.registries.components : string[4] = \
     "urn:snemo:demonstrator:geometry:variants:10.0"   \
     "urn:snemo:demonstrator:vertex:variants:11.0"     \
     "urn:snemo:demonstrator:decays:variants:9.0"     \
     "urn:snemo:demonstrator:simulation:variants:7.0" 
   topic.profiles.components : string[2] = \
     "urn:snemo:demonstrator:simulation:variants:service:7.0:profiles:basic-1.0" \
     "urn:snemo:demonstrator:simulation:variants:service:7.0:profiles:basic-2.0"
   topic.defprofile.component : string = \
     "urn:snemo:demonstrator:simulation:variants:service:7.0:profiles:default"
   topic.gdm.component : string = \
     "urn:snemo:demonstrator:simulation:variants:service:7.0:gdm"

   
   ################################
   #   Simulation setup tag 4.0   #
   ################################
     
   [urn="urn:snemo:demonstrator:simulation:4.0:services" category="services"]
   description              : string = "SuperNEMO demonstrator simulation services"
   topics                   : string[1] = "geometry" "db"
   topic.geometry.component : string = "urn:snemo:demonstrator:geometry:7.0"
   topic.db.component       : string = "urn:snemo:demonstrator:db:3.0"

   [urn="urn:snemo:demonstrator:simulation:4.0"  category="simsetup"]
   description : string = "SuperNEMO demonstrator simulation"
   
   #@description List of components the simulation setup is based on
   topics : string[5] = "variants" "setup" "vertex" "decays" "services"

   #@description List of dependencies
   topic.variants.components   : string[1] = "urn:snemo:demonstrator:simulation:variants:service:7.0"
   topic.setup.component       : string = "urn:snemo:demonstrator:setup:5.3"
   topic.vertex.component      : string = "urn:snemo:demonstrator:vertex:7.1"
   topic.decays.component      : string = "urn:snemo:demonstrator:decays:2.8"
   topic.services.component    : string = "urn:snemo:demonstrator:setup:5.3:services"
..

.. end
