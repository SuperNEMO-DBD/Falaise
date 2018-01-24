Manage tags in Falaise   {#managetags}
======================

\tableofcontents


Introduction to tags {#managetags_intro}
====================

A tag  is the unique  identifier of  an official configuration  of the
SuperNEMO  experiment.  Such  a configuration  may concern  a specific
geometry model,  the version of a  software component, a model  of the
experiment in the  context of the Monte Carlo  simulation, an assembly
of data processing modules for  the reconstruction pipeline, a special
hardware setup for data collection runs...   A tag can also be used to
identify some specific set of resources provided by/stored in a unique
resource file.

Tag are  thus associated to  some *logical  entities*. The tag  does not
provide the address, if any, of the tagged entity it is associated to.
However, a tag **may** be associated to some concrete resource (i.e. a
file).  The  real location of  such a resource  must be provided  by a
*resolver* tool, when this makes sense. Such a location can be the path
of a resource file in the Falaise installation directory. It can be an
URL to be accessed through a network, using a specific protocol.

A    tag   is    implemented    through   the    URN   concept    (see
https://fr.wikipedia.org/wiki/Uniform_Resource_Name).   An  URN  is  a
simple character  string which uses  the **urn** scheme.

Example: the "urn:snemo:demonstrator:setup:1.0" tag is associated to a
specific model of the SuperNEMO demonstrator experiment, including its
geometry model and possibly the  configuration of the electronics, the
database,     the    control     and    monitoring     system.     The
"urn:snemo:demonstrator:setup:1.0"  tag is  thus **unique**.  Once the
model  of  the experiment,  built  as  the  assembly of  several  core
components,  is represented  in the  Falaise library  and/or companion
tools,  its  unique  tag  is officially  published.  The  relationship
between a specific  setup and its tag is bijective.   This is a strict
rule that must be applied during the full lifecycle of the experiment.



Tag categories {#managetags_tagcategories}
==============

As mentioned above, a tag is associated to a logical entity, i.e. something that makes sense
to remember, store, associate to some other entities.

A tag must be associated to a **category**. A category is represented by a simple character string which identify
a concept or topic. Falaise supports several tag categories:

* ``experiment`` : this category is used to address a specific experiment.

   Example: BiPo3, SuperNEMO demonstrator and ATLAS can be considered as experiments.
   It is thus possible to associate an unique tag/URN to each of these entities:

   * Atlas: ``"urn:atlas"``
   * SuperNEMO Demonstrator: ``"urn:snemo:demonstrator"``
   * BiPo3: ``"urn:bipo3"

   An experiment entity provides the general context or framework used to operate
   a specific data processing.

* ``geosetup`` : this category is associated to any *geometry model/setup*.

    Example: the SuperNEMO Demonstrator experiment may have distinct
    geometry models which can be chosen from an offical list (this is a mock example):

    * Simple and naive geometry : ``"urn:snemo:demonstrator:geometry:1.0"``
    * R&D model : ``"urn:snemo:demonstrator:geometry:2.0"``
    * Idealised geometry : ``"urn:snemo:demonstrator:geometry:4.0"``
    * Improved geometry	model (but still approximated) : ``"urn:snemo:demonstrator:geometry:4.2"``
    * Realistic geometry of the detector : ``"urn:snemo:demonstrator:geometry:5.0"``
    * Realistic geometry of the detector including a realistic model of the detector's	environment : ``"urn:snemo:demonstrator:geometry:5.2"``

    Depending on the context and the goal to be reached, the user can choose any of these list of geometry setups
    to operate some data processing (simulation, reconstruction...).

* ``expsetup`` : this category is associated to a model of the full *experimental setup*.
    An experimental setup generally includes:

    * a specific geometry model
    * a specific configuration of the electronics, trigger system and DAQ
    * a specific configuration of the control and monitoring system
    * a specific configuration of various companion services (database...)

   As an assembly of more fondamental entities, a specific experimental setup is expected
   to depends on its components. An experimental setup is associated to its unique tag.
   This tag automatically implies the tags (if any) of the components.

   Example: the configuration of the SuperNEMO Demonstrator experimental setup during
   the commissioning phase of the experiment is tagged : ``"urn:snemo:demonstrator:setup:0.2"``.
   It implies the core components with the following list of tags (this is a mock example):

   * ``"urn:snemo:demonstrator:geometry:5.0"``
   * ``"urn:snemo:demonstrator:electronics:0.9-com"``
   * ``"urn:snemo:demonstrator:cms:0.1"``
   * ``"urn:snemo:demonstrator:db:0.3"``

   Any change in the setup of at least one component implies a new experimental setup and thus
   a new tag of the ``expsetup`` category.

* ``simsetup`` : this category is associated to a *simulation setup*
* ``recsetup`` : this category is associated to a *reconstruction setup*
* ``services`` :  this category is associated to the configuration  of a
  *service management system*
* ``configuration`` : the category associated to  the configuration
  of  some   generic  system  or  service which is not classified/specialized as the category listed above
* ``variants`` :  the category associated  to the configuration  of a
  *variant service*
* ``varprofile`` : the category associated  to a *variant  profile*.




Tag registration {#managetags_tagregistration}
================

When a  tag is publised as  the offical and unique  identifier of some
entity  used  within Falaise,  it  must  be  registered in  a  special
database.  A  dedicated *URN  database  service*  is provided  by  the
Falaise library  to manage the  registration of  all tags used  in the
library.

Practically,   tags   are   described   in   files   stored   in   the
``resources/urn/db/`` source directory.  A new URN entry must be added
in one  of the existing files  from this directory in  order to update
the list of published tags.  Eventually, additional files can be added
to address some new categories of  tags. These files are named *URN DB
files*.  They use the ``datatools::multi_properties`` format.

As a minimum, a  tag record provides the URN of  the tag, its category
and a short  description of the entity identified by  the tag.  It can
also contains other informations such  as the dependees of the entity,
addressed by topic (see below).

The  ``resources/urn/db/snemo_setup_db.conf`` files  defines the  full
list of tag registration *URN DB*  files to be loaded when the Falaise
library starts.


Tag dependencies {#managetags_tagdependencies}
================

TBD


Tag to file resolver {#managetags_tagtofileresolver}
====================

TBD
