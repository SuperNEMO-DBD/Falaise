#include "TrackDetails.h"

using namespace std;

TrackDetails::TrackDetails()
{}

TrackDetails::TrackDetails(const geomtools::manager * geometry_manager,
                           snemo::datamodel::particle_track track)
{
  foilmostVertex_.SetXYZ(-9999,-9999,-9999);
  direction_.SetXYZ(-9999,-9999,-9999);
  projectedVertex_.SetXYZ(-9999,-9999,-9999);
  this->Initialize(geometry_manager, track);
}

void TrackDetails::Initialize(const geomtools::manager * geometry_manager,
                              snemo::datamodel::particle_track track)
{
  geometry_manager_= geometry_manager;
  track_=track;
  hasTrack_=true;
  this->Initialize();
}

// Populates all relevant branche based on type of particle (gamma, alpha, electron)
// Returns true if it has identified a particle type and initialized
// Returns false if it can't work out what sort of particle it is

bool TrackDetails::Initialize()
{
  if (!hasTrack_) return false; // You can't get the track details unless there is a track
  charge_=(int)track_.get_charge();
  // Populate everything you can about the track
  switch (charge_)
    {
    case snemo::datamodel::particle_track::CHARGE_NEUTRAL:
      {
        particleType_=GAMMA;
        PopulateCaloHits();
        return true;
      } // end case neutral (gammas)

      // Any of these will make a track
    case snemo::datamodel::particle_track::CHARGE_POSITIVE:
    case snemo::datamodel::particle_track::CHARGE_NEGATIVE:
    case snemo::datamodel::particle_track::CHARGE_UNDEFINED: // Used for straight tracks
      makesTrack_=true;
      break;
    default:
      {
        particleType_= UNKNOWN; // Nothing we can do here so we are done
        return false;
      }
    }//end switch


  if (!track_.has_trajectory())
    {
      particleType_=UNKNOWN;
      return false;
    }

  // Now we have only charged particles remaining there are a few things we can do:
  // Identify electron candidates
  // Identify alpha candidates
  // Get edgemost inner vertex, regardless of whether they have associated calorimeters etc

  const snemo::datamodel::tracker_trajectory & the_trajectory = track_.get_trajectory();
  const snemo::datamodel::tracker_cluster & the_cluster = the_trajectory.get_cluster();

  // Number of hits and lengths of track
#ifdef NEW_DATAMODEL_API
  trackerHitCount_ = the_cluster.size(); // Currently a track only contains 1 cluster
#else
  trackerHitCount_ = the_cluster.get_number_of_hits();
#endif
  trackLength_ = the_trajectory.get_pattern().get_shape().get_length();

  if (trackLength_ == 0.0)
    {
      particleType_= UNKNOWN;
      return false;
    }

  // Get details about the vertex position
  vertexInTracker_ = SetFoilmostVertex();
  vertexOnFoil_ = SetFoilmostVertex();
  if (SetDirection()) SetProjectedVertex(); // Can't project if no direction!

  // ALPHA candidates are undefined charge particles associated with a delayed hit and no associated hit
  if (track_.get_charge()==snemo::datamodel::particle_track::CHARGE_UNDEFINED && !track_.has_associated_calorimeter_hits() && the_cluster.is_delayed()>0)
    {
      particleType_=ALPHA;

#ifdef NEW_DATAMODEL_API
      const snemo::datamodel::TrackerHitHdlCollection& trackerHits = the_cluster.hits();
#else
      const snemo::datamodel::calibrated_tracker_hit::collection_type& trackerHits = the_cluster.get_hits();
#endif

      // iterate only once, we only want the first hit
      for (const auto& iHit : trackerHits) {
        const snemo::datamodel::calibrated_tracker_hit& a_delayed_gg_hit = iHit.get();
        delayTime_ = (a_delayed_gg_hit.get_delayed_time());
      }

      return true;
    }
  // ELECTRON candidates are prompt and have an associated calorimeter hit. No charge requirement as yet
  else if (the_cluster.is_delayed()<=0 && track_.has_associated_calorimeter_hits())
    {
      particleType_=ELECTRON;
      charge_= track_.get_charge();
      PopulateCaloHits(); // As it has an associated hit, we can calculate the hit fractions
      return true;
    }


  return false; // Not an alpha or an electron, what could it be?
} // end Initialize

double TrackDetails::GetBeta()
{
  if (IsGamma()) return 1.; // Moves at the speed of light
  if (energy_==0) return 0; // Don't know this if we don't have calo hits
  return TMath::Sqrt(energy_ * (energy_ + 2 * ELECTRON_MASS)) / (energy_ +  ELECTRON_MASS);
}

double TrackDetails::GenerateGammaTrackLengths(TrackDetails *electronTrack)
{
  // We don't know the length of a gamma track, but we can calculate a length
  // from the assumed vertex (namely the vertex of an "associated" electron)
  // to the first calorimeter hit
  if (!IsGamma()) return -1;
  if (!electronTrack->IsElectron()) return -1;
  if (foilmostVertex_.x()==-9999 || electronTrack->GetFoilmostVertexX()==-9999) return -1;
  trackLength_=(foilmostVertex_ - electronTrack->GetFoilmostVertex()).Mag();
  projectedLength_=(foilmostVertex_ - electronTrack->GetProjectedVertex()).Mag();
  return trackLength_;
}

TVector3 TrackDetails::GenerateGammaTrackDirection(TrackDetails *electronTrack)
{
  // This assumes that the track is from the foilmost vertex of the electron
  // to the calo that is hit earliest for the gamma
  TVector3 failVector;
  failVector.SetXYZ(0,0,0);
  if (!IsGamma()) return failVector; // One gamma and one electron
  if (!electronTrack->IsElectron()) return failVector;
  if (foilmostVertex_.x()==-9999 || electronTrack->GetFoilmostVertexX()==-9999) return failVector; // They need real vertex positions
  if (!vertexInTracker_) return failVector; // needs to share a vertex with an electron
  direction_=(foilmostVertex_ - electronTrack->GetFoilmostVertex()).Unit();
  return direction_;
}

bool TrackDetails::GenerateAlphaProjections(TrackDetails *electronTrack)
{
  // In a 1e1alpha topology, we can recalculate projected track lengths
  // for the alpha particle based on the electron projections
  if (!IsAlpha()) return false;
  if (!electronTrack->IsElectron()) return false;

  // We need to look at the hits in the alpha track, so get its associated cluster
  const snemo::datamodel::tracker_trajectory & the_trajectory = track_.get_trajectory();
  const snemo::datamodel::tracker_cluster & the_cluster = the_trajectory.get_cluster();

  std::vector<TVector3> vertexPositionDelayedHit;

  //want to store the vector position of the delayed hit
#ifdef NEW_DATAMODEL_API
  const snemo::datamodel::TrackerHitHdlCollection& trackerHits = the_cluster.hits();
#else
  const snemo::datamodel::calibrated_tracker_hit::collection_type& trackerHits = the_cluster.get_hits();
#endif

  for (const auto& iHit : trackerHits) {
    const snemo::datamodel::calibrated_tracker_hit& a_delayed_gg_hit = iHit.get();
    TVector3 delayedHitPos;
    delayedHitPos.SetXYZ(a_delayed_gg_hit.get_x(), a_delayed_gg_hit.get_y(), a_delayed_gg_hit.get_z());
    vertexPositionDelayedHit.push_back(delayedHitPos);
  }

  // Here we want to examine the number of hits in the alpha, then find different alpha lengths for each category
  if(trackerHitCount_ == 1){
    //Alpha length will be the distance to the prompt track
    //projected length will be distance to foil projected electron from delayed hit vertex
    projectedLength_ = (electronTrack->GetProjectedVertex() - vertexPositionDelayedHit.at(0)).Mag();
    projectedVertex_=electronTrack->GetProjectedVertex();
    return true;
  }
  else if(trackerHitCount_ == 2){
    //track length here is from the middle of the furthest delayed hit back to the prompt track
    //projected alpha should be to the one with the larger magnitude x coord back to projected electron vertex
    if(TMath::Abs(vertexPositionDelayedHit.at(0).X()) >= TMath::Abs(vertexPositionDelayedHit.at(1).X())){
      projectedLength_= (electronTrack->GetProjectedVertex() - vertexPositionDelayedHit.at(0)).Mag();
    }
    else{
      projectedLength_ = (electronTrack->GetProjectedVertex() - vertexPositionDelayedHit.at(1)).Mag();
    }
    projectedVertex_=electronTrack->GetProjectedVertex();
    return true;
  }
  else if(trackerHitCount_ > 2){
    //track length is genuine alpha trackLength - back to foil or wire
    //want the vertex separation between projected tracks to the foil, use track direction
    //want the lenth to project back to the foil, if vertex is not on the foil
    double alphaTrackExtension = (foilmostVertex_ - projectedVertex_).Mag();
    double totalDistance = alphaTrackExtension + trackLength_;
    projectedLength_ = (crossesFoil_) ? alphaTrackExtension:totalDistance;
    return true;
  }

  return false; // Zero or negative tracker hit count for the alpha
}

double TrackDetails::GetProjectedTimeVariance()
{
  return GetTotalTimeVariance(projectedLength_);
}
double TrackDetails::GetTotalTimeVariance()
{
  return GetTotalTimeVariance(trackLength_);
}
double TrackDetails::GetTotalTimeVariance(double thisTrackLength)
{
  double totalTimeVariance = 0;
  if (IsElectron())
    {
      double theoreticalTimeOfFlight=thisTrackLength/ (GetBeta() * LIGHT_SPEED);
      totalTimeVariance = pow(timeSigma_,2)
        + pow(energySigma_,2)
        * pow((theoreticalTimeOfFlight*ELECTRON_MASS*ELECTRON_MASS),2)
        / pow( (energy_ * (energy_+ELECTRON_MASS) * (energy_+ 2 * ELECTRON_MASS) ),2);
    }
  if (IsGamma())
    {
      totalTimeVariance = timeSigma_ * timeSigma_ + trackLengthSigma_ * trackLengthSigma_;
    }

  return totalTimeVariance;
}

bool TrackDetails::PopulateCaloHits()
{
  if ( !hasTrack_) return false;
  double thisEnergy=0;
  double thisXwallEnergy=0;
  double thisVetoEnergy=0;
  double thisMainWallEnergy=0;
  double firstHitTime=-1.;
  int firstHitType=0;
  double energySigmaSq=0;

  // Store the energies
  // There could be multiple hits for a gamma so we need to add them up
  for (unsigned int hit=0; hit<track_.get_associated_calorimeter_hits().size();++hit)
    {

      geomtools::vector_3d loc (0,0,0);

      const snemo::datamodel::calibrated_calorimeter_hit & calo_hit = track_.get_associated_calorimeter_hits().at(hit).get();
      double thisHitEnergy=calo_hit.get_energy();

      // Sum the energies
      thisEnergy +=  thisHitEnergy;
      energySigmaSq += calo_hit.get_sigma_energy()*calo_hit.get_sigma_energy(); // Add in quadrature

      // We want to know what fraction of the energy was deposited in each calo wall
      int hitType=calo_hit.get_geom_id().get_type();
      if (hitType==MAINWALL)
        thisMainWallEnergy+= thisHitEnergy;
      else if (hitType==XWALL)
        thisXwallEnergy+= thisHitEnergy;
      else if (hitType==GVETO)
        thisVetoEnergy+= thisHitEnergy;
      else cout<<"WARNING: Unknown calorimeter type "<<hitType<<endl;

      // Get the coordinates of the hit with the earliest time
      if (firstHitTime==-1 || calo_hit.get_time()<firstHitTime)
        {
          firstHitTime=calo_hit.get_time();
          // Find out which calo wall it hit first
          firstHitType=hitType;
          // We need the uncertainty in the first hit time
          timeSigma_= calo_hit.get_sigma_time();
          // For gammas, we will set the vertex to this calo hit
          if (IsGamma())
            {
              // Get the vertex position
              const geomtools::mapping & the_mapping = geometry_manager_->get_mapping();
              // I got this from PTD2root but I don't understand what the two alternatives mean
              if (! the_mapping.validate_id(calo_hit.get_geom_id())) {
                std::vector<geomtools::geom_id> gids;
                the_mapping.compute_matching_geom_id(calo_hit.get_geom_id(), gids); // front calo block = last entry
                const geomtools::geom_info & info = the_mapping.get_geom_info(gids.back()); // in vector gids
                loc  = info.get_world_placement().get_translation();
              }
              else {
                const geomtools::geom_info & info = the_mapping.get_geom_info(calo_hit.get_geom_id());
                loc  = info.get_world_placement().get_translation();
              }
              foilmostVertex_.SetXYZ(loc.x(),loc.y(),loc.z());
            }
        }
    }
  time_=firstHitTime;
  energy_=thisEnergy;
  energySigma_= TMath::Sqrt(energySigmaSq);
  firstHitType_=firstHitType;
  // And the fraction of the energy deposited in each wall
  mainwallFraction_=thisMainWallEnergy/thisEnergy;
  xwallFraction_=thisXwallEnergy/thisEnergy;
  vetoFraction_=thisVetoEnergy/thisEnergy;

  if (track_.has_vertices()) // There isn't any time ordering to the vertices so check them all
    {
      for (unsigned int iVertex=0; iVertex<track_.get_vertices().size();++iVertex)
        {
          const snemo::datamodel::VertexHdl & vertex = track_.get_vertices().at(iVertex);
          if (vertex->is_on_source_foil() || vertex->is_on_wire() || vertex->is_on_source_gap() )
            {
              vertexInTracker_ = true; // On wire OR foil OR foil gap - just not calo to calo gammas
            }
        }
    }

  return true;
}

// Return true if vertex is on the foil
// Populate the inner vertex
bool TrackDetails::SetFoilmostVertex()
{
  if ( !hasTrack_) return false;
  double closestX=9999;
  bool hasVertexOnFoil=false;

  if (track_.has_vertices()) // There isn't any time ordering to the vertices so check them all
    {
      for (unsigned int iVertex=0; iVertex<track_.get_vertices().size();++iVertex)
        {
          const snemo::datamodel::VertexHdl & vertex = track_.get_vertices().at(iVertex);
          if (vertex->is_on_source_foil())
            {
              hasVertexOnFoil = true;
            }
          const geomtools::vector_3d & vertexTranslation = vertex->get_spot().get_placement().get_translation();
          // Get details for the vertex nearest the source foil, which is at x = 0
          if (TMath::Abs(vertexTranslation.x()) < closestX) // this is nearer the foil
            {
              closestX=TMath::Abs(vertexTranslation.x());
              foilmostVertex_.SetXYZ(vertexTranslation.x(),vertexTranslation.y(),vertexTranslation.z());
            } // end for each vertex
        }
    }
  return hasVertexOnFoil;
}

// Populates the direction_ vector with the direction of the track at the foilmost end
// Returns true if you managed to set it, false if not
bool TrackDetails::SetDirection()
{
  if ( !hasTrack_) return false;
  if (!track_.has_trajectory()) return false; // Can't get the direction without a trajectory!
  const snemo::datamodel::base_trajectory_pattern & the_base_pattern = track_.get_trajectory().get_pattern();
  geomtools::vector_3d foilmost_end;
  geomtools::vector_3d outermost_end;

  if (the_base_pattern.get_pattern_id()=="line") {
    const geomtools::line_3d & the_shape = (const geomtools::line_3d&)the_base_pattern.get_shape();
    // Find the two ends of the track
    geomtools::vector_3d one_end=the_shape.get_first();
    geomtools::vector_3d the_other_end=the_shape.get_last();
    // which is which?
    foilmost_end = ((TMath::Abs(one_end.x()) < TMath::Abs(the_other_end.x())) ? one_end: the_other_end);
    outermost_end = ((TMath::Abs(one_end.x()) >= TMath::Abs(the_other_end.x())) ? one_end: the_other_end);
    geomtools::vector_3d direction = the_shape.get_direction_on_curve(the_shape.get_first()); // Only the first stores the direction for a line track
    int multiplier = (direction.x() * outermost_end.x() > 0)? 1: -1; // If the direction points the wrong way, reverse it
    // This will always point inwards towards the foil
    direction_.SetXYZ(direction.x() * multiplier, direction.y() * multiplier, direction.z() * multiplier);
    if(foilmost_end.x() * outermost_end.x() < 0 && TMath::Abs(foilmost_end.x()) > FOIL_CELL_GAP){
      crossesFoil_=true;
    }
  } //end line track
  else {
    const geomtools::helix_3d & the_shape = (const geomtools::helix_3d&)the_base_pattern.get_shape();
    // Find the two ends of the track
    geomtools::vector_3d one_end=the_shape.get_first();
    geomtools::vector_3d the_other_end=the_shape.get_last();
    // which is which?
    foilmost_end = ((TMath::Abs(one_end.x()) < TMath::Abs(the_other_end.x())) ? one_end: the_other_end);
    outermost_end = ((TMath::Abs(one_end.x()) >= TMath::Abs(the_other_end.x())) ? one_end: the_other_end);

    geomtools::vector_3d direction = the_shape.get_direction_on_curve(foilmost_end); // Not the same on a curve
    int multiplier = (direction.x() * outermost_end.x() > 0)? 1: -1; // If the direction points the wrong way, reverse it
    // This will also point in towards the foil. Is that misleading in the case of a track that curves towards the foil and then out again? Not a problem when looking for bb events, but would it be misleading in cases of tracks from the wires?
    direction_.SetXYZ(direction.x() * multiplier, direction.y() * multiplier, direction.z() * multiplier);
  }// end helix track
  if(foilmost_end.x() * outermost_end.x() < 0 && TMath::Abs(foilmost_end.x()) > FOIL_CELL_GAP){
    crossesFoil_=true;
  }
  return true;
}

// Populate the projectedVertex_ vector with where the vertex would be if it were projected back to the foil
// At the moment this uses a simple linear projection; would be better to project the helix
// Return false if the vertex does not project back to the foil (track would not intersect foil or we don't have enough info)
bool TrackDetails::SetProjectedVertex()
{
  // Check that we have the necessary to do this calculation
  if (GetFoilmostVertexX()==-9999 || GetDirectionX()==-9999 || GetTrackLength()==0 || !hasTrack_) return false;

  double scale=foilmostVertex_.X()/direction_.X();
  projectedVertex_=foilmostVertex_ - scale*direction_; // The second term is the extension to the track to project it back with a straight line
  projectedLength_=trackLength_+TMath::Abs(scale*(direction_).Mag());
  // The direction has been chosen so it will always point outwards from the foil.
  // The calculation should always give a projected X coordinate of 0
  // But if it projects in such a way that the y or z values are outside the detector, we should return false
  if (TMath::Abs(projectedVertex_.Y()) > MAXY || TMath::Abs(projectedVertex_.Z()) > MAXZ)
    {
      return false;
    }
  return true;
}

// Getters for the vertex information

// Foilmost vertex
double TrackDetails::GetFoilmostVertexX()
{
  return foilmostVertex_.X();
}
double TrackDetails::GetFoilmostVertexY()
{
  return foilmostVertex_.Y();
}
double TrackDetails::GetFoilmostVertexZ()
{
  return foilmostVertex_.Z();
}
TVector3 TrackDetails::GetFoilmostVertex()
{
  return foilmostVertex_;
}
bool TrackDetails::HasFoilVertex()
{
  return vertexOnFoil_;
}
bool TrackDetails::HasTrackerVertex()
{
  return vertexInTracker_;  // was before vertexOnFoil_
}
// Foil-projected vertex
double TrackDetails::GetProjectedVertexX()
{
  return projectedVertex_.X();
}
double TrackDetails::GetProjectedVertexY()
{
  return projectedVertex_.Y();
}
double TrackDetails::GetProjectedVertexZ()
{
  return projectedVertex_.Z();
}
TVector3 TrackDetails::GetProjectedVertex()
{
  return projectedVertex_;
}
// Track direction at the inner vertex
double TrackDetails::GetDirectionX()
{
  return direction_.X();
}
double TrackDetails::GetDirectionY()
{
  return direction_.Y();
}
double TrackDetails::GetDirectionZ()
{
  return direction_.Z();
}
TVector3 TrackDetails::GetDirection()
{
  return direction_;
}

// Does the track cross the foil (really it shouldn't)
bool TrackDetails::TrackCrossesFoil()
{
  return crossesFoil_;
}

// What particle is it?
bool TrackDetails::IsGamma()
{
  return (particleType_== GAMMA);
}
bool TrackDetails::IsElectron()
{
  return (particleType_== ELECTRON);
}
bool TrackDetails::IsAlpha()
{
  return (particleType_== ALPHA);
}
bool TrackDetails::IsNegativeElectron()
{
  return (particleType_== ELECTRON &&
          charge_==snemo::datamodel::particle_track::CHARGE_POSITIVE);
}
bool TrackDetails::IsPositron()
{
  return (particleType_== ELECTRON &&
          charge_==snemo::datamodel::particle_track::CHARGE_NEGATIVE);
}
int TrackDetails::GetCharge()
{
  return charge_;
}

// For anything that hits the calo wall
double TrackDetails::GetEnergy()
{
  return (energy_);
}

// For anything that hits the calo wall
double TrackDetails::GetEnergySigma()
{
  return (energySigma_);
}

// For anything that hits the calo wall
double TrackDetails::GetTime()
{
  return (time_);
}

// For anything that hits the calo wall
double TrackDetails::GetTimeSigma()
{
  return (timeSigma_);
}

// Fraction of particle's calo energy that is deposited in the main calo wall (France and Italy sides)
double TrackDetails::GetMainwallFraction()
{
  return (mainwallFraction_);
}
// Fraction of particle's calo energy that is deposited in the X-wall (tunnel & mountain ends)
double TrackDetails::GetXwallFraction()
{
  return (xwallFraction_);
}

// Fraction of particle's calo energy that is deposited in the gamma veto (top / bottom)
double TrackDetails::GetVetoFraction()
{
  return (vetoFraction_);
}

// Where did it hit first?
int TrackDetails::GetFirstHitType()
{
  return (firstHitType_);
}
bool TrackDetails::HitMainwall()
{
  return (firstHitType_ == MAINWALL);
}
bool TrackDetails::HitXwall()
{
  return (firstHitType_ == XWALL);
}
bool TrackDetails::HitGammaVeto()
{
  return (firstHitType_ == GVETO);
}

// Details of the track
double TrackDetails::GetTrackLength()
{
  return (trackLength_);
}

double TrackDetails::GetTrackLengthSigma()
{
  if (this->IsElectron()) return 0;
  if (this->IsGamma()) return 0.9;// Corresponds to 0.9 ns, justified in docdb 3799 page 10
  return 0; // What is the track length uncertainty for an alpha? That can go here once we know
}

double TrackDetails::GetProjectedTrackLength()
{
  return (projectedLength_);
}

double TrackDetails::GetDelayTime()
{
  return (delayTime_);
}
int TrackDetails::GetTrackerHitCount()
{
  return trackerHitCount_;
}

// Does it make a track? (charged particle)
bool TrackDetails::MakesTrack()
{
  return makesTrack_;
}
