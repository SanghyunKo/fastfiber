#ifndef FastFiberBasicDetectorConstruction_h
#define FastFiberBasicDetectorConstruction_h 1

#include "FastFiberBasicMaterials.hh"

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VisAttributes.hh"
#include "G4Region.hh"

#include <vector>

class G4VPhysicalVolume;
class G4Material;
class G4GenericMessenger;

class FastFiberBasicDetectorConstruction : public G4VUserDetectorConstruction {
  public:
    FastFiberBasicDetectorConstruction();
    virtual ~FastFiberBasicDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    void ConstructMaterials();

  private:
    void DefineCommands();
    void DefineMaterials();
    G4Material* FindMaterial(G4String matName) { return fMaterials->GetMaterial(matName); }
    G4OpticalSurface* FindSurface(G4String surfName) { return fMaterials->GetOpticalSurface(surfName); }

    G4GenericMessenger* fMessenger;
    FastFiberBasicMaterials* fMaterials;

    G4LogicalVolume* fSipmLogic;
    G4Region* mRegion;

    G4VisAttributes* fVisAttrGray;
    G4VisAttributes* fVisAttrGreen;
    G4VisAttributes* fVisInvisible;
};

#endif
