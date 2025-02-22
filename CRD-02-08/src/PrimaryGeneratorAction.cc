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
/// \file B1/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B1::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <cmath>

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4float ProtonEnergyPDF(G4float energy)
{
  G4float coefficients[] = {-3.63368404e-22,  6.58544274e-19, -4.89942784e-16,  1.89603394e-13,
       -3.88206792e-11,  3.26017170e-09,  1.69760542e-07, -4.84766632e-05,
        1.51380624e-03,  1.27002268e-01,  3.85838211e-01};
  G4float result = 0;
  for (int i = 0; i < 11; i++) {
    result += coefficients[i] * pow(energy, i);
  }
  return result;
}

G4float RandomProtonEnergy()
{
  G4float energy = 0;
  const G4float max = 6.379347596983015 * MeV;
  while (true) {
    energy = G4UniformRand() * 1000 * MeV;
    if (G4UniformRand() * max < ProtonEnergyPDF(energy)) {
      break;
    }
  }
  return energy;
}

G4ThreeVector RandomUnitSpherePoint(G4float theta_t_lo, G4float theta_t_hi, G4float phi_lo, G4float phi_hi)
{
  // random number from 0 to 2pi
  G4float random_phi = G4UniformRand() * (phi_hi - phi_lo) + phi_lo;

  // random number from -1 to 1
  G4float theta_input_rand = G4UniformRand() * (theta_t_hi - theta_t_lo) + theta_t_lo;

  // maps theta so that we get uniform points on a sphere
  G4float random_theta = acos(theta_input_rand);

  G4ThreeVector r = G4ThreeVector(
    sin(random_theta) * cos(random_phi),
    sin(random_theta) * sin(random_phi),
    cos(random_theta)
  ); // want vector to point in
  return r;
}

G4ThreeVector RandomUnitSpherePoint()
{
  return RandomUnitSpherePoint(-1.0, 1.0, 0.0, 2 * CLHEP::pi);
}

G4ThreeVector RandomVectorNudge(G4ThreeVector v, G4float mag)
{
  G4ThreeVector random_nudge = RandomUnitSpherePoint() * v.mag() * mag;
  G4ThreeVector nudged_vector = v + random_nudge;
  nudged_vector = nudged_vector.unit() * v.mag();
  return nudged_vector;
}

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName = "proton");
  fParticleGun->SetParticleDefinition(particle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // this function is called at the begining of each event
  //

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get Envelope volume
  // from G4LogicalVolumeStore.

  G4double envSizeXY = 0;
  G4double envSizeZ = 0;

  if (!fEnvelopeBox) {
    G4LogicalVolume* envLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
    if (envLV) fEnvelopeBox = dynamic_cast<G4Box*>(envLV->GetSolid());
  }

  if (fEnvelopeBox) {
    envSizeXY = fEnvelopeBox->GetXHalfLength() * 2.;
    envSizeZ = fEnvelopeBox->GetZHalfLength() * 2.;
  }
  else {
    G4ExceptionDescription msg;
    msg << "Envelope volume of box shape not found.\n";
    msg << "Perhaps you have changed geometry.\n";
    msg << "The will be place at the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()", "MyCode0002", JustWarning, msg);
  }

  G4ThreeVector r = RandomUnitSpherePoint() * sqrt((envSizeXY * envSizeXY) + (envSizeZ * envSizeZ)) * 0.7;
  G4ThreeVector v = RandomVectorNudge(-r, 0.3).unit();

  fParticleGun->SetParticlePosition(r);
  fParticleGun->SetParticleEnergy(RandomProtonEnergy() * MeV);
  fParticleGun->SetParticleMomentumDirection(v);

  fParticleGun->GeneratePrimaryVertex(event);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1
