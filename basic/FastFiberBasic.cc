#include <iostream>

#include "FastFiberBasicActionInitialization.hh"
#include "FastFiberBasicDetectorConstruction.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4OpticalPhysics.hh"
#include "G4FastSimulationPhysics.hh"
#include "FTFP_BERT.hh"
#include "Randomize.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

int main(int argc, char** argv) {
  #ifdef G4UI_USE
  // Detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) ui = new G4UIExecutive(argc, argv);
  #endif

  // Construct the default run manager
  #ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
  #else
  G4RunManager* runManager = new G4RunManager;
  #endif

  // Mandatory user initialization classes
  runManager->SetUserInitialization(new FastFiberBasicDetectorConstruction());

  // physics module
  G4VModularPhysicsList* physicsList = new FTFP_BERT;

  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  physicsList->RegisterPhysics(opticalPhysics);
  opticalPhysics->Configure(kCerenkov, true);
  opticalPhysics->Configure(kScintillation, true);
  opticalPhysics->Configure(kWLS, true);
  opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
  opticalPhysics->SetTrackSecondariesFirst(kScintillation, true);
  opticalPhysics->SetTrackSecondariesFirst(kWLS, true);

  G4FastSimulationPhysics* fastsimPhysics = new G4FastSimulationPhysics();
  fastsimPhysics->ActivateFastSimulation("opticalphoton");
  physicsList->RegisterPhysics(fastsimPhysics);

  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new FastFiberBasicActionInitialization());

  // Visualization manager construction
  #ifdef G4VIS_USE
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  #endif
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if ( argc != 1 ) {
    // execute an argument macro file if exist
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  } else {
    #ifdef G4UI_USE
    #ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    #else
    UImanager->ApplyCommand("/control/execute init.mac");
    #endif
    if (ui->IsGUI()) { UImanager->ApplyCommand("/control/execute gui.mac"); }
    // start interactive session
    ui->SessionStart();
    delete ui;
    #endif
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  #ifdef G4VIS_USE
  delete visManager;
  #endif
  delete runManager;

  return 0;
}
