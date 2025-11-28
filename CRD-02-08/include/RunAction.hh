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
