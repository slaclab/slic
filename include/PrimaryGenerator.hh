#pragma once

#include <string>

//~~ Geant4 ~~//
#include "G4VPrimaryGenerator.hh"

//~~ SLIC ~~//
#include "Factory.hh"

// Forward Declarations
class G4Event;

namespace slic {

/**
 * @class PrimaryGenerator
 * @brief Interface that defines a simulation primary generator.
 *
 * This class inherits from the Geant4 Primary Genertor template,
 * and is used as a common reference for all of the other PrimaryGenerators.
 */
class PrimaryGenerator : public G4VPrimaryGenerator {
 public:
  /**
   * Constructor.
   *
   * @param name Name given the to class instance.
   */
  PrimaryGenerator(const std::string& name) : name_(name){};

  /// Factory for primary generators
  using Factory =
      ::slic::Factory<PrimaryGenerator, std::shared_ptr<PrimaryGenerator>,
                      const std::string&>;

  /// Destructor
  virtual ~PrimaryGenerator() = default;

  /**
   * Generate a primary vertex.
   *
   * This function must be defined by derived generators.
   */
  virtual void GeneratePrimaryVertex(G4Event*) = 0;

  /**
   * Open the file of the given name for reading.
   *
   * @param[in] filename The path to the file to open.
   */
  virtual void open(const std::string filename) = 0;

  /**
   * Set whether an event should be re-generated.
   *
   * @param[in] regen Set to true to re-generate the event.
   */
  void regenEvent(bool& regen) { regen_ = regen; }

  /// @return true if the event should be re-generated.
  bool regenEvent() { return regen_; }

 protected:
  /// Name of the PrimaryGenerator
  std::string name_{""};

  /// Flag indicating whether an event should be regenerated
  bool regen_{false};

};  // PrimaryGenerator
}  // namespace slic

/**
 * @macro DECLARE_GENERATOR
 *
 * Defines a builder for the declared class
 * and then registers the class as a generator
 * with the Factory
 */
#define DECLARE_GENERATOR(CLASS)                                            \
  namespace {                                                               \
  auto v = ::slic::PrimaryGenerator::Factory::get().declare<CLASS>(#CLASS); \
  }
