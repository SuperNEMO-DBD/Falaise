========================================================
SimRC - Simulated Running Conditions
========================================================

:authors: F.Mauger
:date: 2022-06-01

The  Simulated Running  Conditions  module allows  to apply  realistic
running  conditions  to idealized  simulated  events  produced by  the
flsimulate program.

It is based on several fundamental actions:

- **event timestamping**  : each simulated  event is associated  to an
  unique timestamp distributed along running periods (using a specific
  list of  runs) with respect  to a specific activity  distribution of
  the  simulated decays  (by  default, we  use  a *constant  activity*
  model).
  
  - It is possible to  select a list of runs,  to use a single
    default  run of  30  months (nominal  exposure  for the  SuperNEMO
    demonstrator experiment)
  - It  is possible  to define  an  *activity model*  with a  changing
    activity value  depending on  different running  periods (example:
    can be useful for modeling  unstable radon activity in the tracker
    gas). Several activity models are provided in Falaise (*constant*,
    *constant  per   period*,  *exponential   decay*),  more   can  be
    implemented.
  
- **tracker cell status  tagging** : the simulated  truth tracker hits
  are tagged with a specific  status that informs if the corresponding
  cell is dead, off, has no anode or cathode signal(s)...  This status
  depends on  the current running  period the event  timestamp belongs
  to.
- **calorimeter OM status tagging**  : the simulated truth calorimeter
  hits  (main wall,  X-calon gamma  veto) are  tagged with  a specific
  status that  informs if the  corresponding OM is dead,  off...  This
  status depends  on the  current running  period the  event timestamp
  belongs to.
       
The SimRC modules uses several new classes implemented in Falaise:

* ``falaise/snemo/physics_model/`` for activity models classes
* ``falaise/snemo/rc/`` for run and  run list definition, tracker cell
  and OM status, event timestamps distributions...
* ``falaise/snemo/services/``  for high  level  services (for  tracker
  cell status, OM status...)
  
