#include "SteppingAction.hh"

// SLIC
#include "UserTrackInformation.hh"
#include "TrackUtil.hh"

// LCDD
#include "lcdd/geant4/UserRegionInformation.hh"

// Geant4
#include "G4SteppingManager.hh"
#include "G4UserSteppingAction.hh"
#include "G4Track.hh"

namespace slic {

SteppingAction::SteppingAction() : Module("SteppingAction", false, true) {
}

SteppingAction::~SteppingAction() {
}

void SteppingAction::UserSteppingAction(const G4Step* step) {

    /* Check for back scattering. */
    if (isBackScattering(step)) {

        // Get track information.
        UserTrackInformation* trackInfo =
                (UserTrackInformation*)fpSteppingManager->GetTrack()->GetUserInformation();

        // Need to start making track information if doesn't exist because these tracks or daughters could leave tracker hits.
        if (trackInfo == NULL)
            // Add new track information but don't save.  The tracker hit processor can turn saving on.
            trackInfo = TrackUtil::setupUserTrackInformation(fpSteppingManager->GetTrack(), false);

        TrackSummary* trackSummary = trackInfo->getTrackSummary();

        //G4cout << "setting backscattering flag" << G4endl;
        trackInfo->getTrackSummary()->update(fpSteppingManager->GetTrack());
        trackInfo->getTrackSummary()->setBackScattering();

        // Suspend backscattering tracks to postpone them to the end of shower development.
        fpSteppingManager->GetTrack()->SetTrackStatus(fSuspend);

        //G4cout << "suspended backscattering track " << fpSteppingManager->GetTrack()->GetTrackID() << G4endl;
    }

	/* Check if the track should be killed. */
	checkKillTrack(step);
}

void SteppingAction::checkKillTrack(const G4Step* aStep) {
	/* Check if the region of the pre-point has tracking killing enabled. */
	UserRegionInformation* regionInfo = UserRegionInformation::getRegionInformation(aStep->GetPreStepPoint());
	if (regionInfo->getKillTracks()) {
		/* Kill the track if region track kill flag is set to true. */
		log() << LOG::always << "Killing track: " << aStep->GetTrack()->GetTrackID() << LOG::done;
		aStep->GetTrack()->SetTrackStatus(fStopAndKill);
	}
}

bool SteppingAction::isBackScattering(const G4Step* step) {

    /* Get pre-point info. */
    const G4StepPoint* prePoint = step->GetPreStepPoint();
    const G4StepPoint* postPoint = step->GetPostStepPoint();
    const UserRegionInformation* prePointRegionInfo =
            (UserRegionInformation*)prePoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetRegion()->GetUserInformation();
    G4bool prePointInsideTrackerRegion = prePointRegionInfo->getStoreSecondaries();

    /* Get post-point info. */
    G4bool postPointInsideTrackerRegion = false;
    if (postPoint->GetTouchableHandle()->GetVolume() != NULL) {
        const UserRegionInformation* postPointRegionInfo =
                (UserRegionInformation*)postPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetRegion()->GetUserInformation();
        postPointInsideTrackerRegion = postPointRegionInfo->getStoreSecondaries();
    }

    /* Track is backscattering if it steps from a non-tracking region back into a tracking region. */
    return ((!prePointInsideTrackerRegion) && postPointInsideTrackerRegion);
}

} // namespace slic
