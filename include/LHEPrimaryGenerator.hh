#pragma once

#include <memory>
#include <string>

//~~ SLIC ~~//
#include "LHEReader.hh"
#include "PrimaryGenerator.hh"

// Forward decs
class G4Event;
class G4PrimaryVertex;

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
  ~LHEPrimaryGenerator() = default;

  /**
   * Open the given file for processing.
   */
  void open(const std::string filename) final override {
    reader_->open(filename);
  };

  /**
   * Generate vertices in the Geant4 event.
   * @param anEvent The Geant4 event.
   */
  void GeneratePrimaryVertex(G4Event* event) final override;

 private:
  // The LHE reader with the event data.
  std::unique_ptr<LHEReader> reader_;

  /// The current LHE event being processed.
  std::shared_ptr<LHEEvent> lhe_event_;

};  // LHEPrimaryGenerator
}  // namespace slic
