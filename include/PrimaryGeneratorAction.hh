#pragma once

/*~~ Geant4 ~~*/
#include "G4VUserPrimaryGeneratorAction.hh"

// Forward declarations
class G4Event; 

namespace slic {

/**
 * @class PrimaryGeneratorAction
 * @brief Implementation of Geant4 primary generator action
 */
class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction { 
 public:
  /// Constructor.
  PrimaryGeneratorAction();

  /// Destructor.
  virtual ~PrimaryGeneratorAction() = default;

  /**
   * Generate primaries for the event.
   *
   * @param event The Geant4 event.
   */
  void GeneratePrimaries(G4Event* event) final override;

 private:
  bool smear_;  
  double beamspot_dx_{0}; 
  double beamspot_dy_{0}; 
  double beamspot_dz_{0}; 

}; // PrimaryGeneratorAction
} // namespace slic
