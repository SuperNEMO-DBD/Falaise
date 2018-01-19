===================================
Registered tags
===================================


Categories of URN tags
======================

Supported categories of various published tags:

* ``experiment`` : identifier/tag   associated  to   an  experiment
  (example: BiPo3, SuperNEMO demonstrator)
* ``geosetup`` : identifier/tag associated to a geometry setup
* ``expsetup`` : identifier/tag associated to an experimental setup
* ``simsetup`` : identifier/tag associated to a simulation setup
* ``recsetup`` : identifier/tag associated to a reconstruction setup
* ``service`` :  identifier/tag associated to the configuration  of a
  service management system
* ``configuration`` : identifier/tag associated to  the configuration
  of  some   generic  system  or  service   (geometry,  reconstruction
  modules...)
* ``variant`` :  identifier/tag associated  to the configuration  of a
  variant service
* ``varprofile`` : identifier/tag associated  to a variant  profile. A
  varprofile tag must have one topic:

  * ``variants`` :  the topic  for the  identification of  the variant
    service configuration it is based on.


Composition of a setup configuration
====================================

A *setup*  tag which has a dependee  tag of the ``variant``  category may
have 2 specific topics relative to variant support:

 * ``varprofiles`` :  the topic  for the  list of  *official* variant
   profiles registered within the setup and associated to the variant
   service used by the setup.
 * ``defvarprofile``  :  the topic  for  the  unique default  variant
   profile (typically  an alias  of one in  the list  of *official*
   variant profiles).


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
