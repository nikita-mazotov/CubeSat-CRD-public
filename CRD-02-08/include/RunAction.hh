#ifndef B1RunAction_h
#define B1RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "G4Threading.hh"
#include <vector>
#include <tuple>

namespace B1
{
class RunAction : public G4UserRunAction
{
public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    // Thread-safe energy deposition
    void AddEdep(G4double edep);  // do NOT make this const

    // Hit merging
    void MergeSiPMHits(const std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>>& hits);
    void MergeMCHits(const std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>>& hits);
    void MergeStepHits(const std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>>& hits);

private:
    static G4Mutex fAllHitsMutex;

    

    //std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> fAllSiPMHits;
    //std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> fAllMCHits;
    //std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> fAllStepHits;

    // Global merged hits
    static std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> fGlobalSiPMHits;
    static std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> fGlobalMCHits;
    static std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> fGlobalStepHits;

    // Thread-local accumulators (not strictly needed anymore if you always merge immediately)
    G4Accumulable<G4double> fEdep;

};

} // namespace B1


#endif
