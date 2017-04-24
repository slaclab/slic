#ifndef SLIC_TRACKINGACTION_HH_
#define SLIC_TRACKINGACTION_HH_ 1

#include "PluginManagerAccessor.hh"

// Geant4
#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"

namespace slic {

class TrajectoryManager;

/**
 * @class TrackingAction
 * @brief Implementation of G4UserTrackingAction
 */
class TrackingAction: public G4UserTrackingAction, public PluginManagerAccessor {

public:

    /**
     * Class constructor.
     */
	TrackingAction() {
	}

	/**
	 * Class destructor.
	 */
	~TrackingAction() {
	}

public:

	/**
	 * Pre tracking action.
	 * @param[in] aTrack The G4Track.
	 */
	void PreUserTrackingAction(const G4Track*);

	/**
	 * Post tracking action.
	 * @param[in] aTrack The G4Track.
	 */
	void PostUserTrackingAction(const G4Track*);
};
}

#endif
