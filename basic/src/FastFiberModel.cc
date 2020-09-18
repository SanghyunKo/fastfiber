#include "FastFiberModel.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ProcessManager.hh"
#include "G4OpProcessSubType.hh"
#include "G4GeometryTolerance.hh"
#include "G4Tubs.hh"

FastFiberData::FastFiberData(G4int id, G4double en, G4double globTime, G4double path, G4ThreeVector pos, G4ThreeVector mom, G4ThreeVector pol, G4int status) {
  trackID = id;
  kineticEnergy = en;
  globalTime = globTime;
  pathLength = path;
  globalPosition = pos;
  momentumDirection = mom;
  polarization = pol;
  mOpBoundaryStatus = status;
  mOpAbsorptionNumIntLenLeft = DBL_MAX;
  mOpWLSNumIntLenLeft = DBL_MAX;
  mStepLengthInterval = 0.;
}

FastFiberData& FastFiberData::operator=(const FastFiberData &right) {
  trackID = right.trackID;
  kineticEnergy = right.kineticEnergy;
  globalTime = right.globalTime;
  pathLength = right.pathLength;
  globalPosition = right.globalPosition;
  momentumDirection = right.momentumDirection;
  polarization = right.polarization;
  mOpBoundaryStatus = right.mOpBoundaryStatus;
  mOpAbsorptionNumIntLenLeft = right.mOpAbsorptionNumIntLenLeft;
  mOpWLSNumIntLenLeft = right.mOpWLSNumIntLenLeft;
  mStepLengthInterval = right.mStepLengthInterval;

  return *this;
}

G4bool FastFiberData::checkRepetitive(const FastFiberData theData, G4bool checkInterval) {
  if ( this->trackID!=theData.trackID ) return false;
  if ( this->mOpBoundaryStatus!=theData.mOpBoundaryStatus ) return false;
  if ( checkInterval && std::abs(this->mStepLengthInterval-theData.mStepLengthInterval) > G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() ) return false;
  return true;
}

FastFiberModel::FastFiberModel(G4String name, G4Region* envelope)
: G4VFastSimulationModel(name,envelope),
  mDataPrevious(0,0.,0.,0.,G4ThreeVector(0),G4ThreeVector(0),G4ThreeVector(0)),
  mDataCurrent(0,0.,0.,0.,G4ThreeVector(0),G4ThreeVector(0),G4ThreeVector(0))
{
  pOpBoundaryProc = nullptr;
  pOpAbsorption = nullptr;
  pOpWLS = nullptr;
  fProcAssigned = false;
  fSafety = 2;
  mNtransport = 0.;
  mTransportUnit = 0.;
  mFiberAxis = G4ThreeVector(0);
  fKill = false;
  fSwitch = true;

  DefineCommands();
}

FastFiberModel::~FastFiberModel() {}

G4bool FastFiberModel::IsApplicable(const G4ParticleDefinition& type) {
  return &type == G4OpticalPhoton::OpticalPhotonDefinition();
}

G4bool FastFiberModel::ModelTrigger(const G4FastTrack& fasttrack) {
  if (!fSwitch) return false; // turn on/off the model

  const G4Track* track = fasttrack.GetPrimaryTrack();

  if ( !checkTotalInternalReflection(track) ) return false; // nothing to do if the track has no repetitive total internal reflection

  auto theTouchable = track->GetTouchableHandle();
  auto solid = theTouchable->GetSolid();

  if ( solid->GetEntityType()!="G4Tubs" ) return false; // only works for G4Tubs at the moment

  G4Tubs* tubs = static_cast<G4Tubs*>(solid);
  G4double fiberLen = 2.*tubs->GetZHalfLength();

  auto fiberPos = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0.,0.,0.));
  mFiberAxis = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformAxis(G4ThreeVector(0.,0.,1.));

  if ( mFiberAxis.dot(mDataCurrent.momentumDirection)*mFiberAxis.dot(mDataPrevious.momentumDirection) < 0 ) return false; // different propagation direction (e.g. mirror)

  auto delta = mDataCurrent.globalPosition - mDataPrevious.globalPosition;
  mTransportUnit = delta.dot(mFiberAxis);

  if ( mTransportUnit < 0. ) return false; // #TODO backward propagation

  // estimate the number of expected total internal reflections before reaching fiber end
  auto fiberEnd = fiberPos + mFiberAxis*fiberLen/2.;
  auto toEnd = fiberEnd - mDataCurrent.globalPosition;
  G4double toEndAxis = toEnd.dot(mFiberAxis);
  G4double maxTransport = std::floor(toEndAxis/mTransportUnit);
  mNtransport = maxTransport - fSafety;

  if ( mNtransport < 1. ) return false; // require at least n = fSafety of total internal reflections at the end

  if ( checkAbsorption(mDataPrevious.GetWLSNILL(), mDataCurrent.GetWLSNILL()) ) return false; // do nothing if WLS happens before reaching fiber end
  if ( checkAbsorption(mDataPrevious.GetAbsorptionNILL(), mDataCurrent.GetAbsorptionNILL()) ) fKill = true; // absorbed before reaching fiber end

  return true;
}

void FastFiberModel::DoIt(const G4FastTrack& fasttrack, G4FastStep& faststep) {
  auto track = fasttrack.GetPrimaryTrack();

  if (fKill) { // absorption
    faststep.ProposeTotalEnergyDeposited(track->GetKineticEnergy());
    faststep.KillPrimaryTrack();

    return;
  }

  double timeUnit = mDataCurrent.globalTime - mDataPrevious.globalTime;
  auto posShift = mTransportUnit*mNtransport*mFiberAxis; // #TODO apply shift for xy direction as well
  double timeShift = timeUnit*mNtransport;

  faststep.ProposePrimaryTrackFinalPosition( track->GetPosition() + posShift, false );
  faststep.ProposePrimaryTrackFinalTime( track->GetGlobalTime() + timeShift );
  faststep.ProposePrimaryTrackFinalKineticEnergy( track->GetKineticEnergy() );
  faststep.ProposePrimaryTrackFinalMomentumDirection( track->GetMomentumDirection(), false );
  faststep.ProposePrimaryTrackFinalPolarization( track->GetPolarization(), false );

  return;
}

G4bool FastFiberModel::checkTotalInternalReflection(const G4Track* track) {
  if (!fProcAssigned) setPostStepProc(track); // locate OpBoundaryProcess only once

  if ( track->GetTrackStatus()==fStopButAlive || track->GetTrackStatus()==fStopAndKill ) return false;
  if ( mDataCurrent.trackID != track->GetTrackID() ) reset(); // reset when moving to the next track

  // accumulate step length
  mDataCurrent.AddStepLengthInterval( track->GetStepLength() );

  G4int theStatus = pOpBoundaryProc->GetStatus();

  // skip exceptional iteration with FresnelReflection
  if ( theStatus==G4OpBoundaryProcessStatus::FresnelReflection ) mDataCurrent.SetOpBoundaryStatus(theStatus);

  // some cases have a status StepTooSmall when the reflection happens between the boundary of cladding & outer volume (outside->cladding) since the outer volume is not a G4Region
  if ( theStatus==G4OpBoundaryProcessStatus::TotalInternalReflection || theStatus==G4OpBoundaryProcessStatus::StepTooSmall ) {
    if ( theStatus!=G4OpBoundaryProcessStatus::TotalInternalReflection ) { // skip StepTooSmall if the track already has TotalInternalReflection
      if ( mDataCurrent.GetOpBoundaryStatus()==G4OpBoundaryProcessStatus::TotalInternalReflection ) return false;
      if ( mDataPrevious.GetOpBoundaryStatus()==G4OpBoundaryProcessStatus::TotalInternalReflection ) return false;
    }

    G4int trackID = track->GetTrackID();
    G4double kineticEnergy = track->GetKineticEnergy();
    G4double globalTime = track->GetGlobalTime();
    G4double pathLength = track->GetStepLength();
    G4ThreeVector globalPosition = track->GetPosition();
    G4ThreeVector momentumDirection = track->GetMomentumDirection();
    G4ThreeVector polarization = track->GetPolarization();

    auto candidate = FastFiberData(trackID,kineticEnergy,globalTime,pathLength,globalPosition,momentumDirection,polarization,theStatus);
    if ( pOpAbsorption!=nullptr ) candidate.SetAbsorptionNILL( pOpAbsorption->GetNumberOfInteractionLengthLeft() );
    if ( pOpWLS!=nullptr ) candidate.SetWLSNILL( pOpWLS->GetNumberOfInteractionLengthLeft() );

    G4bool repetitive = false;
    if ( candidate.checkRepetitive(mDataCurrent,false) && mDataCurrent.checkRepetitive(mDataPrevious) ) repetitive = true;

    mDataPrevious = mDataCurrent;
    mDataCurrent = candidate;

    return repetitive;
  }

  return false;
}

G4bool FastFiberModel::checkAbsorption(const G4double prevNILL, const G4double currentNILL) {
  if ( prevNILL < 0. || currentNILL < 0. ) return false; // the number of interaction length left has to be reset
  if ( prevNILL==currentNILL ) return false; // no absorption

  G4double deltaNILL = prevNILL - currentNILL;

  if ( currentNILL - deltaNILL*( mNtransport + fSafety ) < 0. ) return true; // absorbed before reaching fiber end

  return false;
}

void FastFiberModel::setPostStepProc(const G4Track* track) {
  G4ProcessManager* pm = track->GetDefinition()->GetProcessManager();
  auto postStepProcessVector = pm->GetPostStepProcessVector();

  for (int np = 0; np < postStepProcessVector->entries(); np++) {
    auto theProcess = (*postStepProcessVector)[np];

    auto theType = theProcess->GetProcessType();

    if ( theType!=fOptical ) continue;

    if ( theProcess->GetProcessSubType()==G4OpProcessSubType::fOpBoundary ) pOpBoundaryProc = dynamic_cast<G4OpBoundaryProcess*>(theProcess);
    else if ( theProcess->GetProcessSubType()==G4OpProcessSubType::fOpAbsorption ) pOpAbsorption = dynamic_cast<G4OpAbsorption*>(theProcess);
    else if ( theProcess->GetProcessSubType()==G4OpProcessSubType::fOpWLS ) pOpWLS = dynamic_cast<G4OpWLS*>(theProcess);
  }

  fProcAssigned = true;

  return;
}

void FastFiberModel::reset() {
  mNtransport = 0.;
  mTransportUnit = 0.;
  mFiberAxis = G4ThreeVector(0);
  fKill = false;
  mDataCurrent.SetOpBoundaryStatus(0);
  mDataPrevious.SetOpBoundaryStatus(0);
}

void FastFiberModel::DefineCommands() {
  mMessenger = new G4GenericMessenger(this, "/fastfiber/model/", "fastfiber model control");
  G4GenericMessenger::Command& safetyCmd = mMessenger->DeclareProperty("safety",fSafety,"min number of total internal reflection");
  safetyCmd.SetParameterName("safety",true);
  safetyCmd.SetDefaultValue("2.");

  G4GenericMessenger::Command& switchCmd = mMessenger->DeclareProperty("on",fSwitch,"turn on fastfiber model");
  switchCmd.SetParameterName("on",true);
  switchCmd.SetDefaultValue("True");
}
