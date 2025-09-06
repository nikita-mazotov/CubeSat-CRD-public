// EventAction.hh
// Nikita Mazotov, Yale Cubesat, 01/08/2025

#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>
#include <tuple>

class G4Event;

namespace B1
{
class RunAction;

/// Event action class
class EventAction : public G4UserEventAction
{
public:
    explicit EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    // Thread-local energy deposition accumulator
    void AddEdep(G4double edep) { fEdep += edep; }

    // Step hits
    void AddStepHit(const std::tuple<G4double,G4double,G4double,G4double,G4double>& hit) {
        fStepHits.push_back(hit);
    }

    // Specialized detector hits
    void AddSiPMHit(const std::tuple<G4double,G4double,G4double,G4double,G4double>& hit) {
        fSiPMHits.push_back(hit);
        // Small diagnostic print so you see the per-event insertion
        G4cout << "[EventAction] AddSiPMHit: event-local sipmHits now=" << fSiPMHits.size()
            << " (this=" << this << ")" << G4endl;
    }

    void AddMCHit(const std::tuple<G4double,G4double,G4double,G4double,G4double>& hit) {
        fMCHits.push_back(hit);
    }

private:
    RunAction* fRunAction = nullptr;

    G4double fEdep = 0.; // Thread-local per event

    std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> fStepHits;
    std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> fSiPMHits;
    std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> fMCHits;
};

}  // namespace B1

#endif



