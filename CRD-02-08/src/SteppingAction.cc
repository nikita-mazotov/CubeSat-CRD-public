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
/// \file B1/src/SteppingAction.cc
/// \brief Implementation of the B1::SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

namespace B1
{

SteppingAction::SteppingAction(EventAction* eventAction)
    : fEventAction(eventAction),
      fScoringVolume(nullptr) {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // Lazy initialization of scoring volume
    if (!fScoringVolume) {
        const auto detConstruction = static_cast<const DetectorConstruction*>(
            G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        fScoringVolume = detConstruction->GetScoringVolume();
    }

    G4LogicalVolume* volume =
        step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

    if (volume != fScoringVolume) return;

    G4double edepStep = step->GetTotalEnergyDeposit();
    fEventAction->AddEdep(edepStep);  // thread-local per event

    auto pos  = step->GetPreStepPoint()->GetPosition();
    auto time = step->GetPreStepPoint()->GetGlobalTime();

    std::tuple<G4double,G4double,G4double,G4double,G4double> hit{
        pos.x()/CLHEP::mm,
        pos.y()/CLHEP::mm,
        pos.z()/CLHEP::mm,
        time/CLHEP::ns,
        edepStep/CLHEP::eV
    };

    fEventAction->AddStepHit(hit);
}

} // namespace B1
