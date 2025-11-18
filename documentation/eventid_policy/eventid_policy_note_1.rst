================================================================
SuperNEMO data model - Event identifier policy ; note 1
================================================================


:author: F.Mauger
:date: 2024-04-03
:Context: *Answer to Tomas Krizak on Slack*

The  *official* event  identifier should  be fetched  from the  *event
header*  (``"EH"`` bank)  through the  ``get_id()`` method.   However,
this  makes sense  only  if  this attribute  has  been  filled by  the
pipeline before you get the  bank!  Typically, ``flsimulate`` does not
set any run  number if the ``--R/--run-number`` switch is  not used at
MC data production step. So the  default run number is ``-2``. For the
event number  within the  run, it  starts from  ``0`` by  default (see
``flsimulate --help``).

For  real  data,  I don't  know  what  is  done  now. Maybe  Manu  has
implemented some  import of the  raw run number  to enforce it  in the
event  records with  ``"EH"``  and ``"UDD"``  banks.  Maybe not.  This
*raw->UDD data* bridge is still  preliminary.  However, in the future,
the *event header* bank + its ``get_id`` method should be the official
way to identify an event record unambiguously.

We may also find a run and event number in the ``"UDD"`` bank. I think
this is some temporary technique used  by Manu to make sure the bridge
behaves properly. Not sure we keep  these attributes in the future (to
be discussed).

Now what is the event header's  ``get_mc_run_id`` method for ? This is
related to the **real conditions**  algorithm (RC) possibly applied to
MC data before mock calibration (and before future *SD->UDD* process).

MC  data are  generated  using  some ideal  hypotheses  for a  *almost
perfect* detector, or  at least an idealized approximation  of it. The
*RC*  algorithm  is  supposed  to downgrade  the  perfectly  operating
detector  to  take  into  account some  missing  tracker  cells,  some
switched off PMTs or any other conditions in the real world at a given
time. So when we apply the RC algo, a set of idealized *untimestamped*
MC events  is associated  to a  given data taking  period in  the real
world. This real life period is decomposed in a set of real data runs,
each associated to some real life  running conditions. For that we use
also some  weighting factors (but this  is a technical detail  in this
discussion, while important  when we use a non  uniform activity model
associated  to the  MC  data,  typically to  model  fluctuations of  a
background source).  So  after the RC algo has  processed the ``"SD"``
data, each event record is tagged  with two attributes: the run number
of the real data run whose real  conditions has been applied to the MC
event, and a virtual timestamp to  **fakely** date the MC event within
this  run.  That  is  the  current story  of  the  stuff available  in
``"EH"``. These RC-aware run number  and timestamp of MC events should
allow  us  to  identify  any  additional  real  life  criteria/running
conditions  at  reconstruction/analysis processing  stages,  typically
through a connection to the database.


