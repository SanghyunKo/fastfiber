#ifndef FastFiberBasicEventAction_h
#define FastFiberBasicEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4GenericMessenger.hh"
#include "globals.hh"

#include <vector>

class FastFiberBasicEventAction : public G4UserEventAction {
public:
  FastFiberBasicEventAction();
  virtual ~FastFiberBasicEventAction();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

private:
  void DefineCommands();

  G4GenericMessenger* fMessenger;
  G4int fVerbose;
  G4int fSiPMcollID;
};

#endif
