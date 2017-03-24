===================================
Registered tags
===================================


Categories of URN tags
======================

Supported categories of various published tags:

* ``experiment`` : identifier/tag associated
  to an experiment (example: BiPo3, SuperNEMO demonstrator)
* ``expsetup`` : identifier/tag associated to an experimental setup
* ``simsetup`` : identifier/tag associated to a simulation setup
* ``recsetup`` : identifier/tag associated to a reconstruction setup
* ``variant`` : identifier/tag associated to the configuration of a variant service
  A variant tag may have 3 *system* topics:

  * ``__parent__`` : the topic for the identification of the *parent*
    configuration/setup to which the variant service is related
  * ``__profiles__`` : the topic for the list of *official* variant profiles
    registered within this variant service
  * ``__default_profile__`` : the topic for the unique default variant profile
      (typically chosen from the list of *official* variant profiles)

* ``varprofile`` : identifier/tag associated to a variant profile
  A varprofile tag may have 1 topic:

  * ``__parent__`` : the topic for the identification of the *parent*
    variant service configuration

* ``service`` : identifier/tag associated to the configuration of a services management system
* ``configuration`` : identifier/tag associated to the configuration of some generic system or service (geometry, reconstruction modules...)
