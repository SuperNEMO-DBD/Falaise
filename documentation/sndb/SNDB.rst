==============================================
Falaise - Support for the SuperNEMO Database
==============================================


The  SuperNEMO Database,  hosted  at CCIN2P3,  can  be connected  from
Falaise, in  read-only mode.   This feature  is implemented  through a
dedicated *Database Service*.

The DB Servcice
=====================

The  class ``snemo::db_service``  implements  the  service that  gives
access  to  the SuperNEMO  database.   When  activated from  any  data
processing chain  in Falaise (``flsimulate``,  ``flreconstruct``), the
``DB`` service opens first a  connection to the database server, using
a dedicated read-only generic account.

The configuration of the connection info can be stored by default in a
file named ``~/.sndb.conf``  (which must be created  with ``600`` Unix
access mode by the user). The format of this file is:

* Case 1 : from a computing node in the CCIN2P3 private network

  .. code::
   
     host=[DB server name]
     port=[DB server port]
     database=[name of the database]
     user=[ID of the DB read only user]
     password=[password of the DB read only user]
  ..

* Case 2 :  from a local computer outside the  CCIN2P3 private network
  and using a SSH tunnel (see dedicated documentation elsewhere)

  .. code::
   
     host=localhost
     port=[forwarded local port to the DB server port]
     database=[name of the database]
     user=[ID of the DB read only user]
     password=[password of the DB read only user]
  ..

Falaise uses the *MariaDB C++ connector* library to interface with the
MariaDB  server. All  this  magic  is implemented  under  the hood  in
various  Falaise's software  components. However,  it must  be noticed
that a  specific class is responsible  to describe the content  of the
DB, namely  ``snemo::db::sndb``.  The embedded instance  of this class
acts like a DB  proxy. It is hosted in the DB  service. It keeps track
of all DB tables available on the DB server side and provides tools to
access DB records (``select`` SQL requests).

Configuration of the DB support within Falaise
================================================

DB service
----------

The  DB service  must be  initialized through  a set  of configuration
properties that are stored in a dedicated file like:

 ``resources/snemo/demonstrator/db/0.1/services/DbService.conf``

or, for short within Falaise: 

 ``r@falaise:snemo/demonstrator/db/0.1/services/DbService.conf``

See: ``falaise/snemo/services/db_service.h``

SNDB proxy
----------

 The SNDB  proxy object, used by  the DB service, must  be initialized
through  a  set of  configuration  properties  that  are stored  in  a
dedicated file like:

``resources/snemo/demonstrator/db/sndb/1.0/main.conf``

or, for short within Falaise: 

``@falaise:snemo/demonstrator/db/sndb/1.0/main.conf``


See: ``falaise/snemo/db/sndb.hpp``
   

Examples
========


For a reconstruction data processing pipeline

The ``resources/snemo/demonstrator/reconstruction/3.1/ReconstructionServices.conf`` contains the list of configuration files for various services.




.. end
   
