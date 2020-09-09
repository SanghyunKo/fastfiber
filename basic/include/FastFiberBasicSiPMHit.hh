#ifndef FastFiberBasicSiPMHit_h
#define FastFiberBasicSiPMHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class G4VTouchable;

class FastFiberBasicSiPMHit : public G4VHit {
  public:
    FastFiberBasicSiPMHit();
    virtual ~FastFiberBasicSiPMHit();
    FastFiberBasicSiPMHit(const FastFiberBasicSiPMHit &right);

    const FastFiberBasicSiPMHit& operator=(const FastFiberBasicSiPMHit &right);
    G4bool operator==(const FastFiberBasicSiPMHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);

    virtual void Print();

    void clear();

    inline void SetSiPMnum(G4int sipmNum) { fSiPMnum = sipmNum; }
    inline G4int GetSiPMnum() { return fSiPMnum; }

    inline void IncPhotonCount() { fPhotons++; }
    inline G4int GetPhotonCount() { return fPhotons; }

    inline void PushEnergy(G4double en) { fEnergy.push_back(en); }
    inline std::vector<G4double> GetEnergies() { return fEnergy; }

    inline void PushTime(G4double theTime) { fTime.push_back(theTime); }
    inline std::vector<G4double> GetTimes() { return fTime; }

    inline void PushPosition(G4ThreeVector pos) { fPos.push_back(pos); }
    inline std::vector<G4ThreeVector> GetPositions() { return fPos; }

    inline void PushMomentum(G4ThreeVector mom) { fMomentum.push_back(mom); }
    inline std::vector<G4ThreeVector> GetMomentums() { return fMomentum; }

    inline void PushPolarization(G4ThreeVector pol) { fPolarization.push_back(pol); }
    inline std::vector<G4ThreeVector> GetPolarizations() { return fPolarization; }

  private:
    G4int fSiPMnum;
    G4int fPhotons;

    // store properties of each optical photon for validation purpose - avoid large number of photons!
    std::vector<G4double> fEnergy;
    std::vector<G4double> fTime;
    std::vector<G4ThreeVector> fPos;
    std::vector<G4ThreeVector> fMomentum;
    std::vector<G4ThreeVector> fPolarization;
};

typedef G4THitsCollection<FastFiberBasicSiPMHit> FastFiberBasicSiPMHitsCollection;

extern G4ThreadLocal G4Allocator<FastFiberBasicSiPMHit>* FastFiberBasicSiPMHitAllocator;

inline void* FastFiberBasicSiPMHit::operator new(size_t){
  if(!FastFiberBasicSiPMHitAllocator) FastFiberBasicSiPMHitAllocator = new G4Allocator<FastFiberBasicSiPMHit>;
  return (void*) FastFiberBasicSiPMHitAllocator->MallocSingle();
}

inline void FastFiberBasicSiPMHit::operator delete(void *aHit){
  FastFiberBasicSiPMHitAllocator->FreeSingle((FastFiberBasicSiPMHit*) aHit);
}

#endif
