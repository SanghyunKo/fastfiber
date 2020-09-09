#include "FastFiberBasicRunAction.hh"

#include "g4root.hh"

FastFiberBasicRunAction::FastFiberBasicRunAction()
: G4UserRunAction()
{
  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Default settings
  analysisManager->SetNtupleMerging(true); // Note: merging ntuples is available only with Root output
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetFileName("FastFiberBasic");

  // Creating ntuple
  analysisManager->CreateNtuple("FastFiberBasic", "opticalphotons");
  analysisManager->CreateNtupleDColumn("Energy"); // column Id = 0
  analysisManager->CreateNtupleDColumn("Time"); // column Id = 1
  analysisManager->CreateNtupleDColumn("PosX"); // column Id = 2
  analysisManager->CreateNtupleDColumn("PosY"); // column Id = 3
  analysisManager->CreateNtupleDColumn("PosZ"); // column Id = 4
  analysisManager->CreateNtupleDColumn("MomX"); // column Id = 5
  analysisManager->CreateNtupleDColumn("MomY"); // column Id = 6
  analysisManager->CreateNtupleDColumn("MomZ"); // column Id = 7
  analysisManager->CreateNtupleDColumn("PolX"); // column Id = 8
  analysisManager->CreateNtupleDColumn("PolY"); // column Id = 9
  analysisManager->CreateNtupleDColumn("PolZ"); // column Id = 10

  analysisManager->FinishNtuple();
}

FastFiberBasicRunAction::~FastFiberBasicRunAction() {
  delete G4AnalysisManager::Instance();
}

void FastFiberBasicRunAction::BeginOfRunAction(const G4Run*) {
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  analysisManager->OpenFile();
}

void FastFiberBasicRunAction::EndOfRunAction(const G4Run*) {
  // save histograms & ntuple

  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}
