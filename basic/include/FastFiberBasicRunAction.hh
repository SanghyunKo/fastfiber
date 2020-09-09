#ifndef FastFiberBasicRunAction_h
#define FastFiberBasicRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class FastFiberBasicRunAction : public G4UserRunAction {
public:
  FastFiberBasicRunAction();
  virtual ~FastFiberBasicRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
};

#endif
