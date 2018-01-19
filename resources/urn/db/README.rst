===================================
Registered tags
===================================


Categories of URN tags
======================

Supported categories of various published tags:

* ``experiment``   :  identifier/tag   associated  to   an  experiment
  (example: BiPo3, SuperNEMO demonstrator)
* ``geosetup`` : identifier/tag associated to a geometry setup
* ``expsetup`` : identifier/tag associated to an experimental setup
* ``simsetup`` : identifier/tag associated to a simulation setup
* ``recsetup`` : identifier/tag associated to a reconstruction setup
* ``service`` :  identifier/tag associated  to the configuration  of a
  service management system
* ``configuration`` :  identifier/tag associated to  the configuration
  of  some   generic  system  or  service   (geometry,  reconstruction
  modules...)
* ``variant`` :  identifier/tag associated  to the configuration  of a
  variant service
* ``varprofile`` :  identifier/tag associated  to a variant  profile A
  varprofile tag should have 1 topic:

  * ``variants`` :  the topic  for the  identification of  the variant
    service configuration it is based on.

A *setup*  tag which has a dependee  tag of the ``variant``  category may
have 2 specific topics relative to variant support:

  * ``varprofiles`` :  the topic  for the  list of  *official* variant
    profiles registered within the setup and associated to the variant
    service used by the setup.
  * ``defvarprofile``  :  the topic  for  the  unique default  variant
      profile (typically  an alias  of one in  the list  of *official*
      variant profiles)
