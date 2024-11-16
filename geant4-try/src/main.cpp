#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "QGSP_BERT.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

int main(int argc, char** argv) {
    // Run manager construction
    G4RunManager* runManager = new G4RunManager();

    // Set up default physics list
    runManager->SetUserInitialization(new QGSP_BERT);

    // Initialize visualization
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    // User interface manager
    G4UImanager* uiManager = G4UImanager::GetUIpointer();

    // UI executive for command interface
    G4UIExecutive* uiExecutive = new G4UIExecutive(argc, argv);
    G4cout << "hello world" << G4endl;

    uiExecutive->SessionStart();
    

    // Clean up
    delete visManager;
    delete runManager;

    return 0;
}
