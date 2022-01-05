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
  bool hasSteppingAction() final override { return true; }

  /// @return True if this plugin has an event action
  bool hasEventAction() final override { return true; }

  /// @return True if this plugin has a stacking action
  bool hasStackingAction() final override { return true; }

  /// @return True if this plugin has a run action
  bool hasRunAction() final override { return true; } 

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

  /**
   * Method called at the end of the run.  This will be used to print out
   * a summary of the total conversion statistics.
   *
   * @param run Geant4 run object.
   */
  void endRun(const G4Run* run) final override; 

  /**
   * Classify a track. In the case of this filter, the track is pushed to the
   * waiting stack if a pair conversion hasn't been found and the particle
   * is not a photon.
   *
   * @param currentTrackClass The current track classification.
   * @return A new track classification if the particle is not a photon.
   */
  G4ClassificationOfNewTrack stackingClassifyNewTrack(
      const G4Track *,
      const G4ClassificationOfNewTrack &currentTrackClass) final override;

private:
  /// Pointer to the current track being processed. 
  G4Track *currentTrack_{nullptr};
  
  /// Flag that denotes whether a conversion has been found.
  bool hasPairConv_{false};

  /// Counter to keep track of the pair conversion count
  short pairConvCount_{0};

}; // PairConvFilter
} // namespace slic

#endif // PLUGINS_PAIRCONVFILTER_H_
