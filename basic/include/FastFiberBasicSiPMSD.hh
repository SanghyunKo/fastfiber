#ifndef FastFiberBasicSiPMSD_h
#define FastFiberBasicSiPMSD_h 1

#include "G4VSensitiveDetector.hh"
#include "FastFiberBasicSiPMHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

class FastFiberBasicSiPMSD : public G4VSensitiveDetector {
  public:
    FastFiberBasicSiPMSD(G4String name);
    virtual ~FastFiberBasicSiPMSD();

    virtual void Initialize(G4HCofThisEvent* );
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* );

    virtual void EndOfEvent(G4HCofThisEvent* );
    virtual void clear();

  private:
    FastFiberBasicSiPMHitsCollection* fSiPMHitCollection;
    G4int fHCID;
};

#endif
