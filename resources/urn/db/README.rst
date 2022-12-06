===================================
Registered tags
===================================

File contents
================

* ``experiment_urns.conf`` : Definition of the URNs associated to Falaise managed experiments.

  - Contains URN item of the ``"experiment"`` category.
  - Currently contains only one URN item for the SuperNEMO demonstrator experiment.
  
* ``snemo_geometry_urns.conf`` : Definition of different versions of the geometry configuration.

  - Contains URN items of the ``"geomsetup"`` category associated to URN items of the ``"variants"``,
    ``"varprofile"`` and ``"services"`` categories.
  
* ``snemo_setup_urns.conf`` : Definition of different versions of the experimental setup.

  -  An experimental setup is typically defined by a geometry setup and possibly by a
     hardware setup (electronics, control system...)
  - Contains URN items of the ``"expsetup"`` category associated to URN items of the ``"variants"``,
    ``"varprofile"`` and ``"services"`` categories.
  
* ``snemo_simulation_urns.conf`` : Definition of different versions of the simulation setup.

  -  Contains URN items of the ``"simsetup"`` category associated to URN items of the ``"variants"``,
    ``"varprofile"``,  ``"configuration"`` and  ``"services"`` categories.
  
* ``snemo_reconstruction_urns.conf`` : Definition of different versions of the reconstruction setup.

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
* ``services`` :  identifier/tag associated to the configuration  of a
  service management system
* ``configuration`` : identifier/tag associated to  the configuration
  of  some   generic  system  or  service   (geometry,  reconstruction
  modules...)
* ``variants`` :  identifier/tag associated  to the configuration  of a
  variant service
* ``varprofile`` : identifier/tag associated  to a variant  profile. A
  varprofile tag must have one topic:

  * ``variants`` :  the topic  for the  identification of  the variant
    service configuration it is based on.


Composition of a setup configuration
====================================

A *setup*  tag which has a dependee  tag of the ``variants``  category may
also have two specific topics relative to *variants* support:

 * ``varprofiles`` :  the topic  for the  list of  *official* variant
   profiles registered within the setup and associated to the variant
   service used by the setup.
 * ``defvarprofile``  : the  topic  for the  unique *default*  variant
   profile (typically an alias of a published variant profile from the
   list of *official* variant profiles).


Examples
========

Layout of a simulation setup
----------------------------

  ################################
  #   Simulation setup tag 3.0   #
  ################################

  [urn="urn:snemo:demonstrator:simulation:3.0:variants:profiles:basic-1.0" category="varprofile"]
  description : string = "Variant profile 'basic-1.0' for the SuperNEMO demonstrator simulation setup (tag 3.0)"
  topics      : string[1] = "variants"
  topic.variants.component : string = "urn:snemo:demonstrator:simulation:3.0:variants"

  [urn="urn:snemo:demonstrator:simulation:3.0:variants:profiles:basic-2.0" category="varprofile"]
  description : string = "Variant profile 'basic-2.0' for the SuperNEMO demonstrator simulation setup (tag 3.0)"
  topics      : string[1] = "variants"
  topic.variants.component : string = "urn:snemo:demonstrator:simulation:3.0:variants"

  [urn="urn:snemo:demonstrator:simulation:3.0:variants:profiles:basic-3.0" category="varprofile"]
  description : string = "Variant profile 'basic-3.0' for the SuperNEMO demonstrator simulation setup (tag 3.0)"
  topics      : string[1] = "variants"
  topic.variants.component : string = "urn:snemo:demonstrator:simulation:3.0:variants"

  [urn="urn:snemo:demonstrator:simulation:3.0:variants:profiles:default" category="varprofile"]
  description : string = "Default variant profile for the SuperNEMO demonstrator simulation setup (tag 2.1)"
  topics      : string[1] = "alias_of"
  topic.alias_of.component : string = "urn:snemo:demonstrator:simulation:3.0:variants:profiles:basic-1.0"

  [urn="urn:snemo:demonstrator:simulation:3.0"  category="simsetup"]
  #@description List of components the simulation setup is based on
  topics : string[7] = "variants" "varprofiles" "defvarprofile" "expsetup" "services" "vertexes" "decays"
  #@description The variant service component
  topic.variants.component      : string    = "urn:snemo:demonstrator:simulation:3.0:variants"
  #@description The list of official variant profiles published with this simulation setup
  topic.varprofiles.components  : string[3] = \
    "urn:snemo:demonstrator:simulation:3.0:variants:profiles:basic-1.0" \
    "urn:snemo:demonstrator:simulation:3.0:variants:profiles:basic-2.0" \
    "urn:snemo:demonstrator:simulation:3.0:variants:profiles:basic-3.0"
  #@description The default variant profile of this simulation setup
  topic.defvarprofile.component : string    = "urn:snemo:demonstrator:simulation:3.0:variants:profiles:default"
  #@description The experimental setup associated to this simulation setup
  topic.expsetup.component      : string    = "urn:snemo:demonstrator:setup:2.0"
  #@description The service support associated to this simulation setup
  topic.services.component      : string    = "urn:snemo:demonstrator:simulation:3.0:services"
  #@description The vertex generator configuration associated to this simulation setup
  topic.vertexes.component      : string    = "urn:snemo:demonstrator:simulation:vertexes:5.0"
  #@description The primaries generator configuration associated to this simulation setup
  topic.decays.component        : string    = "urn:snemo:demonstrator:simulation:decays:1.3"



..
  end
