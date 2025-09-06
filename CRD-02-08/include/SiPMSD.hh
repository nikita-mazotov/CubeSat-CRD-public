// SiPM sensitive detector class header file
// Nikita Mazotov, Yale Cubesat, 01/08/2025

#ifndef SIPM_SD_HH
#define SIPM_SD_HH

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4OpticalPhoton.hh"
#include "globals.hh"

#include <vector>
#include <string>

namespace B1 {

class SiPMSD : public G4VSensitiveDetector {
public:
  struct Hit {
    G4ThreeVector position;
    G4double time;
    G4double energy;
  };

  SiPMSD(const G4String& name);
  virtual ~SiPMSD() = default;

  // Accessor
  const std::vector<Hit>& GetHits() const { return hits; }

  void Clear() { hits.clear(); }

  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
  virtual void EndOfEvent(G4HCofThisEvent* hce) override;

private:
  std::vector<Hit> hits;
};

}  // namespace B1

#endif
