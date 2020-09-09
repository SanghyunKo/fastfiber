#ifndef FastFiberBasicPrimaryGeneratorAction_h
#define FastFiberBasicPrimaryGeneratorAction_h 1

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Types.hh"
#include "G4GeneralParticleSource.hh"

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

class FastFiberBasicPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  FastFiberBasicPrimaryGeneratorAction(G4bool useGPS);
  virtual ~FastFiberBasicPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

private:
  void DefineCommands();
  void initGPS();
  void initPtcGun();

  G4bool fUseGPS;
  G4ParticleGun* fParticleGun;
  G4GeneralParticleSource* fGPS;
};

#endif
