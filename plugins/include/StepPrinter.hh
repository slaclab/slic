#ifndef PLUGINS_STEPPRINTER_H
#define PLUGINS_STEPPRINTER_H

//~~~ SLIC ~~~//
#include "UserActionPlugin.hh"

// Forward declarations
class G4Step;

namespace slic {

/**
 * User stepping action used to print the details of a step.
 */
class StepPrinter : public UserActionPlugin {

public:
  /// Default constructor
  StepPrinter() = default;

  /// Destructor
  ~StepPrinter() = default;

  /// @return The name of this plugin
  std::string getName() { return "StepPrinter"; }

  /// @return True if this plugin has a stepping action
  bool hasSteppingAction() { return true; }

  /**
   * Stepping action called when a step is taken during tracking of
   * a particle.
   *
   * @param[in] step Geant4 step
   */
  void stepping(const G4Step *step) final override;

private:
  /// The track ID to filter on
  int trackID_{-9999};

}; // StepPrinter

} // namespace slic

#endif // PLUGINS_STEPPRINTER_H
