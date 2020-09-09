#include "FastFiberBasicPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

using namespace std;
FastFiberBasicPrimaryGeneratorAction::FastFiberBasicPrimaryGeneratorAction(G4bool useGPS)
: G4VUserPrimaryGeneratorAction(), fUseGPS(useGPS)
{
  if (fUseGPS) initGPS();
  else initPtcGun();
}

void FastFiberBasicPrimaryGeneratorAction::initPtcGun() {
  fParticleGun = new G4ParticleGun(1);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName = "";
  particleTable->FindParticle(particleName="e-");
  particleTable->FindParticle(particleName="e+");
  particleTable->FindParticle(particleName="mu+");
  particleTable->FindParticle(particleName="pi0");
  particleTable->FindParticle(particleName="pi+");
  particleTable->FindParticle(particleName="kaon0L");
  particleTable->FindParticle(particleName="neutron");
  particleTable->FindParticle(particleName="proton");
  particleTable->FindParticle(particleName="opticalphoton");
}

void FastFiberBasicPrimaryGeneratorAction::initGPS() {
  fGPS = new G4GeneralParticleSource();
}

FastFiberBasicPrimaryGeneratorAction::~FastFiberBasicPrimaryGeneratorAction() {
  if (fUseGPS) delete fGPS;
  else if (fParticleGun) delete fParticleGun;
}

void FastFiberBasicPrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  if (fUseGPS) {
    fGPS->GeneratePrimaryVertex(event);

    return;
  }

  fParticleGun->GeneratePrimaryVertex(event);
}
