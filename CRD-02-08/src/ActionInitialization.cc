// ActionInitialization.cc
// N. Mazotov, Yale Cubesat, 17/08/2025



#include "ActionInitialization.hh"

#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
{
    // Only RunAction is needed for the master thread
    auto runAction = new RunAction();
    SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
    // Primary generator action must be set first
    SetUserAction(new PrimaryGeneratorAction());

    // Create RunAction
    auto runAction = new RunAction();
    SetUserAction(runAction);

    // Create EventAction and pass pointer to RunAction
    auto eventAction = new EventAction(runAction);
    SetUserAction(eventAction);

    // Create SteppingAction and pass pointer to EventAction
    // (SteppingAction can query eventAction for hits if needed)
    SetUserAction(new SteppingAction(eventAction));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B1
