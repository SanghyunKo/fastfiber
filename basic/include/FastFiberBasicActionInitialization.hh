#ifndef FastFiberBasicActionInitialization_h
#define FastFiberBasicActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

class G4GenericMessenger;

class FastFiberBasicActionInitialization : public G4VUserActionInitialization {
public:
  FastFiberBasicActionInitialization();
  virtual ~FastFiberBasicActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  void DefineCommands();

  G4GenericMessenger* fMessenger;
  G4bool fUseGPS;
};

#endif
