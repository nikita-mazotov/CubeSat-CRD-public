// EventAction.cc
// Nikita Mazotov, Yale Cubesat, 01/08/2025

#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"

namespace B1
{

EventAction::EventAction(RunAction* runAction)
    : fRunAction(runAction) {}

void EventAction::BeginOfEventAction(const G4Event*)
{
    fEdep = 0.;
    fStepHits.clear();
    fSiPMHits.clear();
    fMCHits.clear();

    G4cout << "[EventAction] BeginOfEventAction: this=" << this
           << " fRunAction=" << fRunAction
           << " (stepHits=" << fStepHits.size()
           << " sipm=" << fSiPMHits.size()
           << " mc=" << fMCHits.size() << ")" << G4endl;
}

void EventAction::EndOfEventAction(const G4Event*)
{
    G4cout << "[EventAction] EndOfEventAction: this=" << this
           << " before merge: stepHits=" << fStepHits.size()
           << " sipmHits=" << fSiPMHits.size()
           << " mcHits=" << fMCHits.size()
           << " fRunAction=" << fRunAction << G4endl;

    // If we have an owned RunAction pointer, use it.
    if (fRunAction) {
        fRunAction->AddEdep(fEdep);
        if (!fStepHits.empty()) fRunAction->MergeStepHits(fStepHits);
        if (!fSiPMHits.empty()) fRunAction->MergeSiPMHits(fSiPMHits);
        if (!fMCHits.empty()) fRunAction->MergeMCHits(fMCHits);
    } else {
        // Fallback: try to fetch RunAction from the RunManager and call merges.
        auto* urun = G4RunManager::GetRunManager()->GetUserRunAction();
        if (urun) {
            // safe cast through const to avoid "casts away qualifiers" warnings
            auto* runAction = const_cast<RunAction*>(
                static_cast<const RunAction*>(urun));
            if (runAction) {
                G4cout << "[EventAction] fRunAction was null — using RunManager fallback: "
                       << runAction << G4endl;
                runAction->AddEdep(fEdep);
                if (!fStepHits.empty()) runAction->MergeStepHits(fStepHits);
                if (!fSiPMHits.empty()) runAction->MergeSiPMHits(fSiPMHits);
                if (!fMCHits.empty()) runAction->MergeMCHits(fMCHits);
            } else {
                G4cout << "[EventAction] WARNING: fallback runAction cast failed." << G4endl;
            }
        } else {
            G4cout << "[EventAction] WARNING: no RunAction available to merge into." << G4endl;
        }
    }

    // Clear event-local buffers
    fStepHits.clear();
    fSiPMHits.clear();
    fMCHits.clear();
}

} // namespace B1
