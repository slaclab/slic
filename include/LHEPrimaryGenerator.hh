#pragma once

#include <string> 

//~~ SLIC ~~//
#include "LHEReader.hh"
#include "PrimaryGenerator.hh" 

// Forward decs
class G4Event; 

namespace slic {

/**
 * @class LHEPrimaryGenerator
 * @brief Generates a Geant4 event from an LHEEvent
 */
class LHEPrimaryGenerator : public PrimaryGenerator {
 public:
  /**
   * Class constructor.
   * @param name The instance name of this generator.
   */
  LHEPrimaryGenerator(const std::string& name);

  /// Destructor
  virtual ~LHEPrimaryGenerator();

  /**
   * Generate vertices in the Geant4 event.
   * @param anEvent The Geant4 event.
   */
  void GeneratePrimaryVertex(G4Event* event) final override;

 private:
  // The LHE reader with the event data.
  LHEReader* reader_;

}; // LHEPrimaryGenerator
} // slic
