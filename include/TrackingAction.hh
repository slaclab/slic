// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/TrackingAction.hh,v 1.10 2013-11-06 00:23:36 jeremy Exp $
#ifndef SLIC_TRACKINGACTION_HH
#define SLIC_TRACKINGACTION_HH 1

// geant4
#include "G4UserTrackingAction.hh"
#include "G4TrackingManager.hh"

namespace slic {

class TrajectoryManager;

/**
 * @class TrackingAction
 * @brief Implementation of G4UserTrackingAction
 */
class TrackingAction: public G4UserTrackingAction {

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
