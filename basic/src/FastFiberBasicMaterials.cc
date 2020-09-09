#include "FastFiberBasicMaterials.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include <stdio.h>
#include <algorithm>
#include <cmath>

FastFiberBasicMaterials* FastFiberBasicMaterials::fInstance = 0;

FastFiberBasicMaterials::FastFiberBasicMaterials() {
  fNistMan = G4NistManager::Instance();
  CreateMaterials();
}

FastFiberBasicMaterials::~FastFiberBasicMaterials() {
  if (fInstance!=0) delete fInstance;
  fInstance = 0;
}

FastFiberBasicMaterials* FastFiberBasicMaterials::GetInstance() {
  if (fInstance==0) fInstance = new FastFiberBasicMaterials();

  return fInstance;
}

G4Material* FastFiberBasicMaterials::GetMaterial(const G4String matName) {
  G4Material* mat = fNistMan->FindOrBuildMaterial(matName);

  if (!mat) mat = G4Material::GetMaterial(matName);
  if (!mat) {
    std::ostringstream o;
    o << "Material " << matName << " not found!";
    G4Exception("FastFiberBasicMaterials::GetMaterial","",FatalException,o.str().c_str());
  }

  return mat;
}

G4OpticalSurface* FastFiberBasicMaterials::GetOpticalSurface(const G4String surfName) {
  if (surfName=="mirrorSurf") return fMirrorSurf;
  if (surfName=="sipmSurf") return fSiPMSurf;
  else {
    std::ostringstream o;
    o << "OpticalSurface " << surfName << " not found!";
    G4Exception("FastFiberBasicMaterials::GetOpticalSurface","",FatalException,o.str().c_str());
  }

  return nullptr;
}

void FastFiberBasicMaterials::CreateMaterials() {
  fNistMan->FindOrBuildMaterial("G4_AIR");

  G4String symbol;
  G4double a, z, density;
  G4int ncomponents, natoms;
  G4Element* H  = new G4Element("Hydrogen",symbol="H" , z=1., a=1.01*g/mole);
  G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z=6., a=12.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z=8., a=16.00*g/mole);
  G4Element* F  = new G4Element("Fluorine",symbol="F" , z=9., a=18.9984*g/mole);

  fAir = G4Material::GetMaterial("G4_AIR");

  fFluoPoly = new G4Material("FluorinatedPolymer", density=1.43*g/cm3, ncomponents=2);
  fFluoPoly->AddElement(C, 2);
  fFluoPoly->AddElement(F, 2);

  fPS_WLS = new G4Material("Polystyrene_WLS", density=1.05*g/cm3, ncomponents=2);
  fPS_WLS->AddElement(C, natoms=8);
  fPS_WLS->AddElement(H, natoms=8);

  fPMMA = new G4Material("PMMA", density= 1.19*g/cm3, ncomponents=3);
  fPMMA->AddElement(C, natoms=5);
  fPMMA->AddElement(H, natoms=8);
  fPMMA->AddElement(O, natoms=2);

  G4MaterialPropertiesTable* mpAir = nullptr;
  G4MaterialPropertiesTable* mpPS_WLS = nullptr;
  G4MaterialPropertiesTable* mpPMMA = nullptr;
  G4MaterialPropertiesTable* mpFluoPoly = nullptr;
  G4MaterialPropertiesTable* mpMirrorSurf = nullptr;
  G4MaterialPropertiesTable* mpSiPM = nullptr;

  G4double opEn[] = {
    2.06640*eV, 2.10143*eV, 2.13766*eV, 2.17516*eV, 2.21400*eV, 2.25426*eV, 2.29600*eV, 2.33932*eV, 2.38431*eV, 2.43106*eV, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    2.47968*eV, 2.53029*eV, 2.58300*eV, 2.63796*eV, 2.69531*eV, 2.75520*eV, 2.81782*eV, 2.88335*eV, 2.95200*eV, 3.09960*eV, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    3.54241*eV, 4.13281*eV // 350, 300
  };

  const G4int nEnt = sizeof(opEn) / sizeof(G4double);

  G4double RI_Air[nEnt]; std::fill_n(RI_Air,nEnt,1.0);
  mpAir = new G4MaterialPropertiesTable();
  mpAir->AddProperty("RINDEX",opEn,RI_Air,nEnt);
  fAir->SetMaterialPropertiesTable(mpAir);

  G4double RI_PMMA[nEnt] = {
    1.4920, 1.4924, 1.4929, 1.4933, 1.4938, 1.4943, 1.4948, 1.4954, 1.4960, 1.4966, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    1.4973, 1.4981, 1.4989, 1.4997, 1.5006, 1.5016, 1.5026, 1.5038, 1.5050, 1.5052, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    1.5152, 1.5306 // 350, 300
  };
  G4double AbsLen_PMMA[nEnt] = {
    39.48*m, 48.25*m, 54.29*m, 57.91*m, 54.29*m, 33.40*m, 31.02*m, 43.43*m, 43.43*m, 41.36*m, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510,
    39.48*m, 37.76*m, 36.19*m, 36.19*m, 33.40*m, 31.02*m, 28.95*m, 25.55*m, 24.13*m, 21.71*m, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400,
    2.171*m, 0.434*m // 350, 300
  };
  mpPMMA = new G4MaterialPropertiesTable();
  mpPMMA->AddProperty("RINDEX",opEn,RI_PMMA,nEnt);
  mpPMMA->AddProperty("ABSLENGTH",opEn,AbsLen_PMMA,nEnt);
  fPMMA->SetMaterialPropertiesTable(mpPMMA);

  G4double RI_FluoPoly[nEnt]; std::fill_n(RI_FluoPoly, nEnt, 1.42);
  mpFluoPoly = new G4MaterialPropertiesTable();
  mpFluoPoly->AddProperty("RINDEX",opEn,RI_FluoPoly,nEnt);
  fFluoPoly->SetMaterialPropertiesTable(mpFluoPoly);

  G4double RI_PS[nEnt] = {
    1.5904, 1.5914, 1.5924, 1.5935, 1.5947, 1.5959, 1.5972, 1.5986, 1.6000, 1.6016 , // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    1.6033, 1.6051, 1.6070, 1.6090, 1.6112, 1.6136, 1.6161, 1.6170, 1.6230, 1.62858, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    1.65191, 1.69165 // 400, 350, 300
  };
  G4double AbsLen_PS[nEnt] = {
    5.429*m, 13.00*m, 14.50*m, 16.00*m, 18.00*m, 16.50*m, 17.00*m, 14.00*m, 16.00*m, 15.00*m, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    14.50*m, 13.00*m, 12.00*m, 10.00*m, 8.000*m, 7.238*m, 4.000*m, 1.200*m, 0.500*m, 0.200*m, // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    0.200*m, 0.100*m // 350, 300
  };

  // from Kuraray Y-11(200)S datasheet
  G4double wlsEmission_PS[nEnt] = {
    0.,   0.01, 0.02, 0.03, 0.05, 0.1,  0.2,  0.25, 0.35, 0.5 , // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    0.8 , 0.7 , 1.  , 0.8 , 0.2 , 0.02, 0.  , 0.  , 0.  , 0.  , // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    0.  , 0. // 350, 300
  };
  G4double wlsAbsorption_PS[nEnt] = {
    0.,   0.,    0.,   0.,    0.,   0.,   0.,    0.,    0.,   0.,    // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    0.,   -0.05, -0.3, -0.6, -0.9, -0.75, -0.99, -0.7,  -0.6, -0.3,  // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    0.,   0. // 350, 300
  };
  G4double wlsAbslen_PS[nEnt]; std::fill_n(wlsAbslen_PS, nEnt, 100.*m);
  calculateWLSabslen(AbsLen_PS,wlsAbsorption_PS,wlsAbslen_PS,nEnt);

  for (G4int ent = 0; ent < nEnt; ent++) {
    std::cout << "WLS abslen at " << h_Planck*c_light/opEn[ent]*mm/nm << " nm = " << wlsAbslen_PS[ent] << " mm" << std::endl;
  }

  mpPS_WLS = new G4MaterialPropertiesTable();
  mpPS_WLS->AddProperty("RINDEX",opEn,RI_PS,nEnt);
  mpPS_WLS->AddProperty("ABSLENGTH",opEn,AbsLen_PS,nEnt);
  mpPS_WLS->AddProperty("WLSABSLENGTH",opEn,wlsAbslen_PS,nEnt);
  mpPS_WLS->AddProperty("WLSCOMPONENT",opEn,wlsEmission_PS,nEnt);
  mpPS_WLS->AddConstProperty("WLSTIMECONSTANT",8.8*ns);
  fPS_WLS->SetMaterialPropertiesTable(mpPS_WLS);

  G4double mirrorRef[nEnt]; std::fill_n(mirrorRef,nEnt,1.);
  mpMirrorSurf = new G4MaterialPropertiesTable();
  mpMirrorSurf->AddProperty("REFLECTIVITY",opEn,mirrorRef,nEnt);
  fMirrorSurf = new G4OpticalSurface("FilterSurf",glisur,polished,dielectric_metal);
  fMirrorSurf->SetMaterialPropertiesTable(mpMirrorSurf);

  // from Hamamatsu MPPC S13615-1025
  G4double refl_SiPM[nEnt]; std::fill_n(refl_SiPM, nEnt, 0.);
  G4double eff_SiPM[nEnt] = {
    0.18, 0.188, 0.195, 0.203, 0.21, 0.218, 0.225, 0.233, 0.24, 0.245, // 600, 590, 580, 570, 560, 550, 540, 530, 520, 510
    0.25, 0.255, 0.26 , 0.255, 0.25, 0.245, 0.24 , 0.23 , 0.22, 0.21 , // 500, 490, 480, 470, 460, 450, 440, 430, 420, 400
    0.17, 0.10 // 350, 300
  };
  mpSiPM = new G4MaterialPropertiesTable();
  mpSiPM->AddProperty("REFLECTIVITY",opEn,refl_SiPM,nEnt);
  mpSiPM->AddProperty("EFFICIENCY",opEn,eff_SiPM,nEnt);
  fSiPMSurf = new G4OpticalSurface("SiPMSurf",glisur,polished,dielectric_metal);
  fSiPMSurf->SetMaterialPropertiesTable(mpSiPM);
}

void FastFiberBasicMaterials::calculateWLSabslen(G4double* absPure, G4double* WLSAbsSpec, G4double* WLSAbslen, int nEnt) {
  for (G4int ent = 0; ent < nEnt; ent++) {
    if (WLSAbsSpec[ent]==-1.) { // avoid inf or nan
      WLSAbslen[ent]=0.1*mm;
      continue;
    } else {
      if (WLSAbsSpec[ent]==0.) { // avoid inf or nan
        WLSAbslen[ent] = 10.*m;
        continue;
      }
      G4double effAbslen = 1./( 1./absPure[ent] - std::log(1.+WLSAbsSpec[ent])/10.*cm ); // Kuraray specification measured with L=10cm
      WLSAbslen[ent] = 1./( 1./effAbslen - 1./absPure[ent] );
    }
  }
}
