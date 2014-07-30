#include "TrackingAction.hh"

// LCDD
#include "lcdd/detectors/CurrentTrackState.hh"

// SLIC
#include "TrackUtil.hh"
#include "UserTrackInformation.hh"

// Geant4
#include "G4TransportationManager.hh"

namespace slic {

void TrackingAction::PreUserTrackingAction(const G4Track* track) {

    G4int trackID = track->GetTrackID();
    G4int parentID = track->GetParentID();

    G4bool isTruePrimary = (track->GetParentID() == 0);
    UserRegionInformation* regionInfo = TrackUtil::getRegionInformation(track);
    G4bool insideTrackerRegion = regionInfo->getStoreSecondaries();
    UserTrackInformation* trackInfo = TrackUtil::getUserTrackInformation(track);
    bool hasTrackInfo = (trackInfo != NULL);
    bool aboveEnergyThreshold = (track->GetKineticEnergy() > regionInfo->getThreshold());

    // This happens when backscattering track are un-suspended.
    if (hasTrackInfo) {
        // Track is backscattering?
        if (trackInfo->getTrackSummary()->getBackScattering()) {
            // Update current track ID for calorimeter SDs and return.
            CurrentTrackState::setCurrentTrackID(trackID);
            return;
        }
        return;
    }

    // Primary OR in tracker region and above energy threshold?
    if (isTruePrimary || (insideTrackerRegion && aboveEnergyThreshold)) {

        // Update track ID for calorimeter SDs.
        CurrentTrackState::setCurrentTrackID(trackID);

        // Turn on trajectory storage.
        fpTrackingManager->SetStoreTrajectory(true);

        // Has track information?
        if (!hasTrackInfo) {
            // Create the track information and save it.
            TrackUtil::setupUserTrackInformation(track, true);
        }

    } else {
        // Inside tracker region and below energy threshold?
        if (insideTrackerRegion && !aboveEnergyThreshold) {
            // Has track information?
            if (!hasTrackInfo) {
                // Create track information but do not save it.
                TrackUtil::setupUserTrackInformation(track, false);
            }
        } else {
            // Track is outside tracking region so turn off trajectory storage.
            // These are generally tracks from calorimeter showers.
            fpTrackingManager->SetStoreTrajectory(false);
        }
    }
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) {

    // Get the track information and summary.
    UserTrackInformation* trackInfo = TrackUtil::getUserTrackInformation(track);
    TrackSummary* trackSummary = NULL;
    if (trackInfo != NULL)
        trackSummary = trackInfo->getTrackSummary();

    // Track has secondaries?
    if (fpTrackingManager->GimmeSecondaries()->size() > 0 && trackInfo == NULL) {
        // Tracks with secondaries always get track information but they are not saved by default.
        trackSummary = TrackUtil::setupUserTrackInformation(track, false)->getTrackSummary();
    }

    // Track summary exists?
    if (trackSummary != NULL) {
        // Update the track summary.
        trackSummary->update(track);
    }
}

} // namespace slic
