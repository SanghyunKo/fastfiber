#include "FastFiberBasicActionInitialization.hh"
#include "FastFiberBasicPrimaryGeneratorAction.hh"
#include "FastFiberBasicRunAction.hh"
#include "FastFiberBasicEventAction.hh"

#include "G4GenericMessenger.hh"

using namespace std;
FastFiberBasicActionInitialization::FastFiberBasicActionInitialization()
: G4VUserActionInitialization()
{
  // G4GenericMessenger::Command::SetDefaultValue does not work on the master thread, remain uninitialized
  fUseGPS = false;

  DefineCommands();
}

FastFiberBasicActionInitialization::~FastFiberBasicActionInitialization() {
  if (fMessenger) delete fMessenger;
}

void FastFiberBasicActionInitialization::BuildForMaster() const {
  SetUserAction(new FastFiberBasicRunAction());
}

void FastFiberBasicActionInitialization::Build() const {
  SetUserAction(new FastFiberBasicPrimaryGeneratorAction(fUseGPS));
  SetUserAction(new FastFiberBasicRunAction());
  SetUserAction(new FastFiberBasicEventAction());
}

void FastFiberBasicActionInitialization::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/fastfiber/action/", "action initialization control");

  G4GenericMessenger::Command& gpsCmd = fMessenger->DeclareProperty("useGPS",fUseGPS,"use GPS");
  gpsCmd.SetParameterName("useGPS",true,true);
}
