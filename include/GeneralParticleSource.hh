#pragma once

#include <string>

//~~ Geant4 ~~//
#include "G4GeneralParticleSource.hh"

//~~ SLIC ~~//
#include "PrimaryGenerator.hh"

// Forward decs
class G4Event;

namespace slic {

/**
 * @class GeneralParticleSource
 * @brief Class that extends the functionality of G4GeneralParticleSource.
 *  This class is still configured using the gps macro commands.
 */
class GeneralParticleSource : public PrimaryGenerator {
 public:
  /**
   * Class constructor.
   * @param name The instance name of this generator.
   */
  GeneralParticleSource(const std::string& name) : PrimaryGenerator(name){};

  /// Destructor
  ~GeneralParticleSource() = default;

  /**
   * Generate vertices in the Geant4 event.
   * @param event The Geant4 event.
   */
  void GeneratePrimaryVertex(G4Event* event) final override;

  /// This method is not used
  void open(std::string filename) final override{};

 private:
  /**
   * The underlying Geant4 GPS implementation.
   *
   * The creation of this class creates a new messenger that we can pass
   * commands to.
   */
  G4GeneralParticleSource gps_;

};  // GeneralParticleSource
}  // namespace slic
