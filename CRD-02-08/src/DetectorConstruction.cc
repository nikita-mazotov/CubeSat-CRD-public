//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Trd.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4ThreeVector.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4SDManager.hh"
#include "SiPMSD.hh"
#include "G4LogicalVolumeStore.hh"


namespace B1
{

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4NistManager* nist = G4NistManager::Instance();
  G4bool checkOverlaps = true;

  // Define envelope and world
  G4double env_sizeX = 25.20 * mm, env_sizeY = 25.34 * mm, env_sizeZ = 28.3 * mm;
  G4double world_sizeX = 1.2 * env_sizeX;
  G4double world_sizeY = 1.2 * env_sizeY;
  G4double world_sizeZ = 1.2 * env_sizeZ;

  const G4int nEntries = 11;
  G4double photonEnergy[nEntries] = {
    2.431*eV, 2.480*eV, 2.530*eV, 2.583*eV, 2.638*eV,
    2.696*eV, 2.755*eV, 2.818*eV, 2.883*eV, 2.952*eV, 3.024*eV
  };

  G4Material* env_mat = nist->FindOrBuildMaterial("G4_Al");
  G4Material* world_mat = nist->BuildMaterialWithNewDensity("Vacuum", "G4_AIR", 3.8e-12 * kg / cm3);
  G4MaterialPropertiesTable* mptWorld = new G4MaterialPropertiesTable();
  G4double rindexVac[nEntries];
  for (int i=0;i<nEntries;i++) rindexVac[i] = 1.0;
  mptWorld->AddProperty("RINDEX", photonEnergy, rindexVac, nEntries);
  world_mat->SetMaterialPropertiesTable(mptWorld);


  auto solidWorld = new G4Box("World", 0.5 * world_sizeX, 0.5 * world_sizeY, 0.5 * world_sizeZ);
  auto logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, checkOverlaps);

  // Aluminum shell
  auto solidAlShell = new G4Box("AluminumShell", 0.5 * env_sizeX, 0.5 * env_sizeY, 0.5 * env_sizeZ);
  auto logicAlShell = new G4LogicalVolume(solidAlShell, env_mat, "AluminumShell");
  new G4PVPlacement(nullptr, G4ThreeVector(), logicAlShell, "AluminumShell", logicWorld, false, 0, checkOverlaps);
  logicAlShell->SetVisAttributes(new G4VisAttributes(G4Colour(0.8, 0.8, 0.8, 0.3)));

  // Scintillator definition
  G4Material* scint_mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  G4double scint_X = env_sizeX - 14 * mm;
  G4double scint_Y = env_sizeY - 14 * mm;
  G4double scint_Z = env_sizeZ - 14 * mm;

  G4MaterialPropertiesTable* mptScint = new G4MaterialPropertiesTable();
  G4double scintSpectrum[nEntries] = { 0.1, 0.2, 0.3, 0.42, 0.55, 0.7, 0.85, 1.0, 0.98, 0.7, 0.2 };
  G4double refractiveIndex[nEntries] = { 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58, 1.58 };
  G4double absLength[nEntries] = { 2*m, 2*m, 2*m, 2*m, 2*m, 2*m, 2*m, 2*m, 2*m, 2*m, 2*m };

 mptScint->AddProperty("FASTCOMPONENT", photonEnergy, scintSpectrum, nEntries, true);
 mptScint->AddConstProperty("SCINTILLATIONYIELD", 10000./MeV);   // 100 photons/MeV for test, 10000 run 
 mptScint->AddConstProperty("RESOLUTIONSCALE", 1.0);
 mptScint->AddConstProperty("FASTTIMECONSTANT", 2.1*ns, true);
 mptScint->AddConstProperty("YIELDRATIO", 1.0, true);                // all fast component
 mptScint->AddProperty("RINDEX", photonEnergy, refractiveIndex, nEntries);
 mptScint->AddProperty("ABSLENGTH", photonEnergy, absLength, nEntries);
 scint_mat->SetMaterialPropertiesTable(mptScint);

  auto solidScint = new G4Box("Scintillator", 0.5 * scint_X, 0.5 * scint_Y, 0.5 * scint_Z);
  auto logicScint = new G4LogicalVolume(solidScint, scint_mat, "Scintillator");
  auto scintillatorPhys = new G4PVPlacement(nullptr, G4ThreeVector(), logicScint, "Scintillator", logicAlShell, false, 0, checkOverlaps);
  logicScint->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 0.5)));

  // Reflective coating on scintillator
  auto paintSurface = new G4OpticalSurface("ReflectivePaint");
  paintSurface->SetType(dielectric_metal);
  paintSurface->SetModel(unified);
  paintSurface->SetFinish(ground);

  const G4int nPaint = 2;
  G4double energiesPaint[nPaint] = {2.0*eV, 3.5*eV};
  G4double reflect90[nPaint] = {0.9, 0.9};
  auto mptPaint = new G4MaterialPropertiesTable();
  mptPaint->AddProperty("REFLECTIVITY", energiesPaint, reflect90, nPaint);
  paintSurface->SetMaterialPropertiesTable(mptPaint);

  new G4LogicalSkinSurface("ScintPaintSurface", logicScint, paintSurface);

/* TEMPORARILY DISABLED REFLECTIVE SURFACE
  // --- Placeholder: attach a dummy surface with zero reflectivity ---
  // This allows photons to freely exit the scintillator
  auto dummySurface = new G4OpticalSurface("DummySurface");
  dummySurface->SetType(dielectric_dielectric);
  dummySurface->SetModel(unified);
  dummySurface->SetFinish(polished);

  auto dummyMPT = new G4MaterialPropertiesTable();
  const G4int nDummy = 2;
  G4double dummyEnergies[nDummy] = {2.0*eV, 3.5*eV};
  G4double dummyReflectivity[nDummy] = {0.0, 0.0};  // fully transparent
  dummyMPT->AddProperty("REFLECTIVITY", dummyEnergies, dummyReflectivity, nDummy);
  dummySurface->SetMaterialPropertiesTable(dummyMPT);

  // Apply dummy surface as a skin surface (optional)
  new G4LogicalSkinSurface("ScintDummySurface", logicScint, dummySurface);
*/
  // Detector definition
  G4Material* detector_mat = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  G4MaterialPropertiesTable* mptDet = new G4MaterialPropertiesTable();
  G4double rindexSiO2[nEntries];
  for (int i=0;i<nEntries;i++) rindexSiO2[i] = 1.46;   // typical fused silica
  mptDet->AddProperty("RINDEX", photonEnergy, rindexSiO2, nEntries);
  detector_mat->SetMaterialPropertiesTable(mptDet);

  G4double detector_thickness = 1.0 * mm;
  G4double detectorSide = 7 * mm;
  auto solidDetector = new G4Box("PhotonDetector", 0.5 * detectorSide, 0.5 * detectorSide, 0.5 * detector_thickness);
  logicDetector = new G4LogicalVolume(solidDetector, detector_mat, "PhotonDetector");
  logicDetector->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.5)));

  G4double z_offset = 0.5 * scint_Z + 0.5 * detector_thickness;
  auto detectorPhys = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, z_offset), logicDetector,
                                        "PhotonDetector", logicAlShell, false, 0, checkOverlaps);

  // Interface between scintillator and detector
  auto surface = new G4OpticalSurface("ScintToDetectorSurface");
  surface->SetType(dielectric_dielectric);
  surface->SetModel(unified);
  surface->SetFinish(polished);

  auto rpt = new G4MaterialPropertiesTable();
  G4double reflectivity[nPaint] = {0.0, 0.0};
  rpt->AddProperty("REFLECTIVITY", energiesPaint, reflectivity, nPaint);
  surface->SetMaterialPropertiesTable(rpt);
/*
  auto rpt = new G4MaterialPropertiesTable();
  rpt->AddProperty("REFLECTIVITY", dummyEnergies, dummyReflectivity, nDummy);
  surface->SetMaterialPropertiesTable(rpt);
*/

  new G4LogicalBorderSurface("ScintToDetectorBorder", scintillatorPhys, detectorPhys, surface);


  // Set scoring volume
  fScoringVolume = logicScint;
  return physWorld;
}

// Multithreading sensetive detector construction
void DetectorConstruction::ConstructSDandField()
{
  auto* sdMan = G4SDManager::GetSDMpointer();
  sdMan->SetVerboseLevel(1);

  // Create and register SD
  auto* sipmSD = new SiPMSD("SiPM_SD");
  sdMan->AddNewDetector(sipmSD);

  G4cout << "[DetectorConstruction] Setting SiPM_SD on volume: "
       << logicDetector->GetName() << G4endl;

  // IMPORTANT: fetch the *thread-local* LV by name (not a stored pointer)
  auto* detLV =
      G4LogicalVolumeStore::GetInstance()->GetVolume("PhotonDetector");
  if (!detLV) {
    G4Exception("DetectorConstruction::ConstructSDandField",
                "SiPM001", FatalException,
                "Cannot find logical volume 'PhotonDetector' in LV store.");
    return;
  }

  detLV->SetSensitiveDetector(sipmSD);

  // Optional debug
  G4cout << "[ConstructSDandField] Attached SiPM_SD to LV="
         << detLV->GetName() << " @ " << detLV << G4endl;
}


} // namespace B1
