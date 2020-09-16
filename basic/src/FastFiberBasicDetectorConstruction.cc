#include "FastFiberBasicDetectorConstruction.hh"
#include "FastFiberBasicSiPMSD.hh"
#include "FastFiberModel.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

FastFiberBasicDetectorConstruction::FastFiberBasicDetectorConstruction()
: G4VUserDetectorConstruction(), fSipmLogic(nullptr) {
  DefineMaterials();

  fVisAttrGreen = new G4VisAttributes(G4Colour(0.3,0.7,0.3,0.7));
  fVisAttrGreen->SetVisibility(true);
  fVisAttrGray = new G4VisAttributes(G4Colour(0.3,0.3,0.3,0.5));
  fVisAttrGray->SetVisibility(true);
  fVisInvisible = new G4VisAttributes(G4Colour(0.,0.,0.));
  fVisInvisible->SetVisibility(false);
}

FastFiberBasicDetectorConstruction::~FastFiberBasicDetectorConstruction() {
  delete fSipmLogic;

  delete fVisAttrGray;
  delete fVisAttrGreen;
  delete fVisInvisible;
}

void FastFiberBasicDetectorConstruction::DefineMaterials() {
  fMaterials = FastFiberBasicMaterials::GetInstance();
}

G4VPhysicalVolume* FastFiberBasicDetectorConstruction::Construct() {
  G4bool checkOverlaps = true;

  // experimental hall (world volume)
  auto worldSolid = new G4Box("worldBox",5.*m/2.,5.*m/2.,5.*m/2.);
  auto worldLogical = new G4LogicalVolume(worldSolid,FindMaterial("G4_AIR"),"worldLogical");
  worldLogical->SetVisAttributes(fVisInvisible);
  auto worldPhysical = new G4PVPlacement(0,G4ThreeVector(),worldLogical,"worldPhysical",0,false,0,checkOverlaps);

  mRegion = new G4Region("fiberRegion");

  G4double fibreLength = 1.0*m;
  G4double displacementX = 1.0*m;

  // single cladding WLS fibre (Kuraray Y-11(200)S)
  G4Tubs* cladSolid = new G4Tubs("cladSolid",0.,0.5*mm,fibreLength/2.,0.*deg,360.*deg);
  G4Tubs* coreSolid = new G4Tubs("coreSolid",0.,0.47*mm,fibreLength/2.,0.*deg,360.*deg);

  G4LogicalVolume* cladLogic = new G4LogicalVolume(cladSolid,FindMaterial("PMMA"),"cladLogic");
  cladLogic->SetVisAttributes(fVisAttrGray);
  G4LogicalVolume* coreLogic = new G4LogicalVolume(coreSolid,FindMaterial("Polystyrene_WLS"),"coreLogic");
  coreLogic->SetVisAttributes(fVisAttrGreen);

  // region of FastFiberModel
  cladLogic->SetRegion(mRegion);
  mRegion->AddRootLogicalVolume(cladLogic);

  new G4PVPlacement(0,G4ThreeVector(displacementX,0.,0.),cladLogic,"cladPhysical",worldLogical,false,0,checkOverlaps);
  new G4PVPlacement(0,G4ThreeVector(),coreLogic,"corePhysical",cladLogic,false,0,checkOverlaps);

  // SiPM box and mirror placed at the end of the fibre
  G4Box* sipmSolid = new G4Box("sipmSolid",1.5*mm/2.,1.5*mm/2.,1.0*mm/2.);
  fSipmLogic = new G4LogicalVolume(sipmSolid,FindMaterial("G4_Si"),"sipmLogic");
  fSipmLogic->SetVisAttributes(fVisAttrGreen);
  new G4PVPlacement(0,G4ThreeVector(displacementX,0.,fibreLength/2.+1.0*mm/2.),fSipmLogic,"sipmPhysical",worldLogical,false,0,checkOverlaps);
  new G4LogicalSkinSurface("sipmSurf",fSipmLogic,FindSurface("sipmSurf"));

  G4Box* mirrorSolid = new G4Box("mirrorSolid",1.5*mm/2.,1.5*mm/2.,1.0*mm/2.);
  G4LogicalVolume* mirrorLogic = new G4LogicalVolume(mirrorSolid,FindMaterial("G4_Ag"),"mirrorLogic");
  new G4PVPlacement(0,G4ThreeVector(displacementX,0.,-fibreLength/2.-1.0*mm/2.),mirrorLogic,"mirrorPhysical",worldLogical,false,0,checkOverlaps);
  new G4LogicalSkinSurface("mirrorSurf",mirrorLogic,FindSurface("mirrorSurf"));

  return worldPhysical;
}

void FastFiberBasicDetectorConstruction::ConstructSDandField() {
  auto sdManager = G4SDManager::GetSDMpointer();

  auto sipmSD = new FastFiberBasicSiPMSD("FastFiberBasicSiPMSD");
  sdManager->AddNewDetector(sipmSD);
  fSipmLogic->SetSensitiveDetector(sipmSD);

  new FastFiberModel("FastFiberModel",mRegion);
}
