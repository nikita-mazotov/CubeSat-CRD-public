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
// Run action implementation
// Nikita Mazotov, Yale Cubesat, 03/09/2025

#include "RunAction.hh"
#include "G4AccumulableManager.hh"
#include "G4RunManager.hh"
#include <fstream>

namespace B1
{

// Mutex and global containers (shared across threads)
G4Mutex RunAction::fAllHitsMutex = G4MUTEX_INITIALIZER;
std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> RunAction::fGlobalSiPMHits;
std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> RunAction::fGlobalMCHits;
std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>> RunAction::fGlobalStepHits;

RunAction::RunAction()
{
    auto* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->RegisterAccumulable(fEdep);
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    // Clear global hits at the start of a run
    G4AutoLock lock(&fAllHitsMutex);
    fGlobalStepHits.clear();
    fGlobalSiPMHits.clear();
    fGlobalMCHits.clear();
    lock.unlock();

    auto* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Reset();
}

void RunAction::EndOfRunAction(const G4Run*)
{
    auto* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Merge();  // merge thread-local accumulables

    G4cout << "[RunAction] EndOfRunAction: totals before writing: SiPM="
           << fGlobalSiPMHits.size()
           << " MC=" << fGlobalMCHits.size()
           << " Step=" << fGlobalStepHits.size() << G4endl;

    if (!IsMaster()) return;  // only master writes CSV

    std::ofstream outFile("all_hits.csv");
    outFile << "x,y,z,time,energy,type\n";

    // SiPM hits
    if (fGlobalSiPMHits.empty()) {
        outFile << "n/a,n/a,n/a,n/a,n/a,SiPM_EMPTY\n";
    } else {
        for (const auto& h : fGlobalSiPMHits)
            outFile << std::get<0>(h) << "," << std::get<1>(h) << "," << std::get<2>(h)
                    << "," << std::get<3>(h) << "," << std::get<4>(h) << ",SiPM\n";
    }

    // MC hits
    if (fGlobalMCHits.empty()) {
        outFile << "n/a,n/a,n/a,n/a,n/a,MC_EMPTY\n";
    } else {
        for (const auto& h : fGlobalMCHits)
            outFile << std::get<0>(h) << "," << std::get<1>(h) << "," << std::get<2>(h)
                    << "," << std::get<3>(h) << "," << std::get<4>(h) << ",MC\n";
    }

    // Step hits
    if (fGlobalStepHits.empty()) {
        outFile << "n/a,n/a,n/a,n/a,n/a,STEP_EMPTY\n";
    } else {
        for (const auto& h : fGlobalStepHits)
            outFile << std::get<0>(h) << "," << std::get<1>(h) << "," << std::get<2>(h)
                    << "," << std::get<3>(h) << "," << std::get<4>(h) << ",Step\n";
    }

    outFile.close();
    G4cout << "[RunAction] All hits written, total SiPM hits: "
           << fGlobalSiPMHits.size() << G4endl;
}

void RunAction::AddEdep(G4double edep)
{
    fEdep += edep;  // thread-safe via G4Accumulable
}

void RunAction::MergeSiPMHits(const std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>>& hits)
{
    if (hits.empty()) {
        G4cout << "[RunAction] MergeSiPMHits called with 0 hits (no-op)" << G4endl;
        return;
    }
    G4AutoLock lock(&fAllHitsMutex);
    size_t before = fGlobalSiPMHits.size();
    fGlobalSiPMHits.insert(fGlobalSiPMHits.end(), hits.begin(), hits.end());
    G4cout << "[RunAction] MergeSiPMHits: added " << hits.size()
           << " hits (total now " << fGlobalSiPMHits.size() << ", before " << before << ")" << G4endl;
}

void RunAction::MergeMCHits(const std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>>& hits)
{
    if (hits.empty()) {
        G4cout << "[RunAction] MergeMCHits called with 0 hits (no-op)" << G4endl;
        return;
    }
    G4AutoLock lock(&fAllHitsMutex);
    size_t before = fGlobalMCHits.size();
    fGlobalMCHits.insert(fGlobalMCHits.end(), hits.begin(), hits.end());
    G4cout << "[RunAction] MergeMCHits: added " << hits.size()
           << " hits (total now " << fGlobalMCHits.size() << ", before " << before << ")" << G4endl;
}

void RunAction::MergeStepHits(const std::vector<std::tuple<G4double,G4double,G4double,G4double,G4double>>& hits)
{
    if (hits.empty()) {
        G4cout << "[RunAction] MergeStepHits called with 0 hits (no-op)" << G4endl;
        return;
    }
    G4AutoLock lock(&fAllHitsMutex);
    size_t before = fGlobalStepHits.size();
    // TEMPORARILY DISABLED TO AVOID HUGE FILES 
    // ALSO BECAUSE IDK WHAT IT DOES
    
    //fGlobalStepHits.insert(fGlobalStepHits.end(), hits.begin(), hits.end());
    //G4cout << "[RunAction] MergeStepHits: added " << hits.size()
    //       << " hits (total now " << fGlobalStepHits.size() << ", before " << before << ")" << G4endl;
}

} // namespace B1
