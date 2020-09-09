#ifndef FastFiberBasicMaterials_h
#define FastFiberBasicMaterials_h 1

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"

class FastFiberBasicMaterials {
public:
  virtual ~FastFiberBasicMaterials();
  static FastFiberBasicMaterials* GetInstance();
  G4Material* GetMaterial(const G4String);
  G4OpticalSurface* GetOpticalSurface(const G4String);

private:
  FastFiberBasicMaterials();
  void CreateMaterials();
  void calculateWLSabslen(G4double* absPure, G4double* WLSAbsSpec, G4double* WLSAbslen, int nEnt);

  static FastFiberBasicMaterials* fInstance;
  G4NistManager* fNistMan;
  G4Material* fAir;
  G4Material* fFluoPoly;
  G4Material* fPMMA;
  G4Material* fPS_WLS;
  G4OpticalSurface* fMirrorSurf;
  G4OpticalSurface* fSiPMSurf;
};

#endif
