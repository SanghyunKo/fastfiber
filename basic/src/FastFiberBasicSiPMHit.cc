#include "FastFiberBasicSiPMHit.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<FastFiberBasicSiPMHit>* FastFiberBasicSiPMHitAllocator = 0;

FastFiberBasicSiPMHit::FastFiberBasicSiPMHit()
: fSiPMnum(0), fPhotons(0) {
  // reserve vectors for the # of optical photons up to 1000
  fEnergy.reserve(1000);
  fTime.reserve(1000);
  fPos.reserve(1000);
  fMomentum.reserve(1000);
  fPolarization.reserve(1000);
}

FastFiberBasicSiPMHit::~FastFiberBasicSiPMHit() {}

FastFiberBasicSiPMHit::FastFiberBasicSiPMHit(const FastFiberBasicSiPMHit &right) : G4VHit() {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fEnergy = right.fEnergy;
  fTime = right.fTime;
  fPos = right.fPos;
  fMomentum = right.fMomentum;
  fPolarization = right.fPolarization;
}

const FastFiberBasicSiPMHit& FastFiberBasicSiPMHit::operator=(const FastFiberBasicSiPMHit &right) {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fEnergy = right.fEnergy;
  fTime = right.fTime;
  fPos = right.fPos;
  fMomentum = right.fMomentum;
  fPolarization = right.fPolarization;
  return *this;
}

G4bool FastFiberBasicSiPMHit::operator==(const FastFiberBasicSiPMHit &right) const{
  return (fSiPMnum==right.fSiPMnum);
}

void FastFiberBasicSiPMHit::clear() {
  fSiPMnum = 0;
  fPhotons = 0;
  fEnergy.clear();
  fTime.clear();
  fPos.clear();
  fMomentum.clear();
  fPolarization.clear();
}
void FastFiberBasicSiPMHit::Print() {
  G4cout << "FastFiberBasicSiPMHit::Print()" << G4endl;
  G4cout << "SiPMnum = " << fSiPMnum << ", # of photons = " << fPhotons << G4endl;

  for (G4int pho = 0; pho < fPhotons; pho++) {
    G4cout << "==================================================" << G4endl;
    G4cout << "Opticalphoton " << pho << G4endl;
    G4cout << "Energy = " << fEnergy.at(pho) << G4endl;
    G4cout << "Time = " << fTime.at(pho) << G4endl;
    G4cout << "Photon position     | x = " << std::setw(12) << fPos.at(pho).x() << " | y = " << std::setw(12) << fPos.at(pho).y() << " | z = " << std::setw(12) << fPos.at(pho).z() << G4endl;
    G4cout << "Photon momentum     | x = " << std::setw(12) << fMomentum.at(pho).x() << " | y = " << std::setw(12) << fMomentum.at(pho).y() << " | z = " << std::setw(12) << fMomentum.at(pho).z() << G4endl;
    G4cout << "Photon polarization | x = " << std::setw(12) << fPolarization.at(pho).x() << " | y = " << std::setw(12) << fPolarization.at(pho).y() << " | z = " << std::setw(12) << fPolarization.at(pho).z() << G4endl;
  }
}
