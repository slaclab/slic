#ifndef PLUGINS_PAIRCONV_FILTER_H
#define PLUGINS_PAIRCONV_FILTER_H

//~~~ SLIC ~~~//
#include "UserActionPlugin.hh"

// Forward declarations
class G4Step;

namespace slic {

/**
 * Class defining a UserAction that filters out events where a pair conversion
 * occurs.
 */
class PairConvFilter : public UserActionPlugin {

public:
  /// Default constructor
  PairConvFilter() = default;

  /// Destructor
  ~PairConvFilter() = default;

  /// @return The name of this plugin
  std::string getName() { return "PairConvFilter"; }

  /// @return True if this plugin has a stepping action
  bool hasSteppingAction() { return true; }

  /// @return True if this plugin has an event action
  bool hasEventAction() { return true; }

  /**
   * Implementmthe stepping action which performs the filtering.
   *
   * @param[in] step Geant4 step
   */
  void stepping(const G4Step *step) final override;

  /**
   * Method called at the end of the event.
   *
   * @param event Geant4 event object.
   */
  void endEvent(const G4Event *) final override;

private:
  /// Flag that denotes whether a conversion has been found.
  bool hasPairConv_{false};

}; // PairConvFilter
} // namespace slic

#endif // PLUGINS_PAIRCONVFILTER_H_
