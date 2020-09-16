#include "FastFiberBasicEventAction.hh"
#include "FastFiberBasicSiPMSD.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "g4root.hh"

G4VHitsCollection* GetHC(const G4Event* event, G4int collId) {
  auto hce = event->GetHCofThisEvent();
  if (!hce) {
    G4ExceptionDescription msg;
    msg << "No hits collection of this event found." << G4endl;
    G4Exception("FastFiberBasicEventAction::EndOfEventAction()",
                "FastFiberBasicCode001", JustWarning, msg);
    return nullptr;
  }

  auto hc = hce->GetHC(collId);
  if (!hc) {
    G4ExceptionDescription msg;
    msg << "Hits collection " << collId << " of this event not found." << G4endl;
    G4Exception("FastFiberBasicEventAction::EndOfEventAction()",
                "FastFiberBasicCode001", JustWarning, msg);
  }

  return hc;
}

FastFiberBasicEventAction::FastFiberBasicEventAction()
: G4UserEventAction(), fVerbose(0), fSiPMcollID(-1) {
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  DefineCommands();
}

FastFiberBasicEventAction::~FastFiberBasicEventAction() {}

void FastFiberBasicEventAction::BeginOfEventAction(const G4Event*) {
  auto sdManager = G4SDManager::GetSDMpointer();

  fSiPMcollID = sdManager->GetCollectionID("SiPMHitCollection");
}

void FastFiberBasicEventAction::EndOfEventAction(const G4Event* event) {
  auto aM = G4AnalysisManager::Instance();

  FastFiberBasicSiPMHitsCollection* hc = dynamic_cast<FastFiberBasicSiPMHitsCollection*>( GetHC(event, 0) ); // only 1 SD in this example

  G4int entries = 0;
  if (hc) entries = hc->entries();

  for (G4int ent = 0; ent < entries; ent++) {
    auto hit = dynamic_cast<FastFiberBasicSiPMHit*>( (*hc)[ent] );

    if (fVerbose>0) hit->Print();

    for (G4int iHit = 0; iHit < hit->GetPhotonCount(); iHit++) {
      aM->FillNtupleDColumn(0, hit->GetEnergies().at(iHit) );
      aM->FillNtupleDColumn(1, hit->GetTimes().at(iHit) );
      aM->FillNtupleDColumn(2, hit->GetPositions().at(iHit).x() );
      aM->FillNtupleDColumn(3, hit->GetPositions().at(iHit).y() );
      aM->FillNtupleDColumn(4, hit->GetPositions().at(iHit).z() );
      aM->FillNtupleDColumn(5, hit->GetMomentums().at(iHit).x() );
      aM->FillNtupleDColumn(6, hit->GetMomentums().at(iHit).y() );
      aM->FillNtupleDColumn(7, hit->GetMomentums().at(iHit).z() );
      aM->FillNtupleDColumn(8, hit->GetPolarizations().at(iHit).x() );
      aM->FillNtupleDColumn(9, hit->GetPolarizations().at(iHit).y() );
      aM->FillNtupleDColumn(10, hit->GetPolarizations().at(iHit).z() );

      aM->AddNtupleRow();
    }
  }
}

void FastFiberBasicEventAction::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/fastfiber/event/", "event action control");

  G4GenericMessenger::Command& verboseCmd = fMessenger->DeclareProperty("verbose",fVerbose,"verbose level");
  verboseCmd.SetParameterName("verbose",true);
  verboseCmd.SetDefaultValue("0");
}
