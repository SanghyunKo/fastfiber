#include "FastFiberBasicSiPMSD.hh"
#include "FastFiberBasicSiPMHit.hh"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"

FastFiberBasicSiPMSD::FastFiberBasicSiPMSD(G4String name)
: G4VSensitiveDetector(name), fSiPMHitCollection(nullptr), fHCID(-1) {
  collectionName.insert("SiPMHitCollection");
}

FastFiberBasicSiPMSD::~FastFiberBasicSiPMSD() {}

void FastFiberBasicSiPMSD::Initialize(G4HCofThisEvent* hitsCE) {
  fSiPMHitCollection = new FastFiberBasicSiPMHitsCollection(SensitiveDetectorName,collectionName[0]);

  if (fHCID<0) fHCID = GetCollectionID(0);
  hitsCE->AddHitsCollection( fHCID, fSiPMHitCollection );
}

G4bool FastFiberBasicSiPMSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if(step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  G4int SiPMnum = step->GetPostStepPoint()->GetTouchable()->GetVolume()->GetCopyNo();
  G4int nofHits = fSiPMHitCollection->entries();

  G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
  G4double energy = step->GetTrack()->GetTotalEnergy();
  G4ThreeVector momentum = step->GetTrack()->GetMomentum();
  G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
  G4ThreeVector polar = step->GetTrack()->GetPolarization();

  FastFiberBasicSiPMHit* hit = nullptr;

  for (G4int i = 0; i < nofHits; i++) {
    if ( (*fSiPMHitCollection)[i]->GetSiPMnum()==SiPMnum ) {
      hit = (*fSiPMHitCollection)[i];
      break;
    }
  }

  if (hit==nullptr) {
    hit = new FastFiberBasicSiPMHit();
    hit->SetSiPMnum(SiPMnum);

    fSiPMHitCollection->insert(hit);
  }

  hit->IncPhotonCount();
  hit->PushEnergy(energy);
  hit->PushTime(hitTime);
  hit->PushPosition(pos);
  hit->PushMomentum(momentum);
  hit->PushPolarization(polar);

  return true;
}

void FastFiberBasicSiPMSD::EndOfEvent(G4HCofThisEvent* ) {}

void FastFiberBasicSiPMSD::clear() {}
