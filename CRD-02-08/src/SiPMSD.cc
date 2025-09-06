// SiPM Sensetive Detector code
// Nikita Mazotov, Yale Cubesat, 02/08/2025

#include "SiPMSD.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "EventAction.hh"
#include <tuple>
#include <vector>

using namespace B1;

SiPMSD::SiPMSD(const G4String& name)
  : G4VSensitiveDetector(name) {}

G4bool SiPMSD::ProcessHits(G4Step* step, G4TouchableHistory*) {

  // Only care about optical photons
  if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
    return false;

  auto preStep = step->GetPreStepPoint();
  auto track = step->GetTrack();

  // Diagnostic: step status
  G4StepStatus stepStatus = preStep->GetStepStatus();
  switch (stepStatus) {
    case fGeomBoundary:
      G4cout << "[SiPMSD] StepStatus: fGeomBoundary at pos " 
             << preStep->GetPosition() << " time = " 
             << preStep->GetGlobalTime()/ns << " ns" << G4endl;
      break;
    case fPostStepDoItProc:
      G4cout << "[SiPMSD] StepStatus: fPostStepDoItProc at pos " 
             << preStep->GetPosition() << G4endl;
      break;
    case fAtRestDoItProc:
      G4cout << "[SiPMSD] StepStatus: fAtRestDoItProc at pos " 
             << preStep->GetPosition() << G4endl;
      break;
    case fUndefined:
      G4cout << "[SiPMSD] StepStatus: fUndefined at pos " 
             << preStep->GetPosition() << G4endl;
      break;
    default:
      G4cout << "[SiPMSD] StepStatus: " << stepStatus 
             << " at pos " << preStep->GetPosition() << G4endl;
      break;
  }

  // Diagnostic: track status
  G4TrackStatus trackStatus = track->GetTrackStatus();
  switch (trackStatus) {
    case fAlive:
      // optional: usually lots of alive tracks
      break;
    case fStopAndKill:
      G4cout << "[SiPMSD] TrackStatus: fStopAndKill at pos "
             << preStep->GetPosition() << G4endl;
      break;
    case fSuspend:
      G4cout << "[SiPMSD] TrackStatus: fSuspend at pos "
             << preStep->GetPosition() << G4endl;
      break;
    case fKillTrackAndSecondaries:
      G4cout << "[SiPMSD] TrackStatus: fKillTrackAndSecondaries at pos "
             << preStep->GetPosition() << G4endl;
      break;
    default:
      G4cout << "[SiPMSD] TrackStatus: " << trackStatus 
             << " at pos " << preStep->GetPosition() << G4endl;
      break;
  }

  // Only register hits at geometry boundary
  //if (stepStatus == fGeomBoundary) {

  // Retrieve current EventAction
  auto evtAction = static_cast<B1::EventAction*>(
      G4EventManager::GetEventManager()->GetUserEventAction());
  if (!evtAction) return false;

  // Build tuple for this hit
  auto hitTuple = std::make_tuple(
    preStep->GetPosition().x() / CLHEP::mm,     // mm
    preStep->GetPosition().y() / CLHEP::mm,     // mm
    preStep->GetPosition().z() / CLHEP::mm,     // mm
    preStep->GetGlobalTime() / CLHEP::ns,       // ns
    track->GetKineticEnergy() / CLHEP::eV       // eV
);

  // Add hit to EventAction
  evtAction->AddSiPMHit(hitTuple);

  G4cout << "[SiPMSD] Recorded photon hit at "
          << preStep->GetPosition()
          << " time = " << preStep->GetGlobalTime()/ns 
          << " ns, energy = " << track->GetKineticEnergy()/eV << " eV"
          << G4endl;
//}

  return true;
}

void SiPMSD::EndOfEvent(G4HCofThisEvent*) {
  // Nothing else needed: EventAction will merge hits into RunAction
  G4cout << "[SiPMSD] EndOfEvent called." << G4endl;
}
