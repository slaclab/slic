#ifndef SLIC_TRACKUTIL_HH_
#define SLIC_TRACKUTIL_HH_ 1

// LCDD
#include "lcdd/geant4/UserRegionInformation.hh"

// SLIC
#include "TrackManager.hh"
#include "UserTrackInformation.hh"

// Geant4
#include "G4TransportationManager.hh"

namespace slic {

/**
 * @class TrackUtil
 * @brief Miscellaneous helper methods for G4Track and related classes
 */
class TrackUtil {

public:

    /**
     * Get the UserRegionInformation of this G4Track based on its start position.
     * @param track The G4Track.
     * @return The UserRegionInformation for this G4Track.
     */
    static UserRegionInformation* getRegionInformation(const G4Track* track) {
        G4VPhysicalVolume* volume = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->LocateGlobalPointAndSetup(
                    track->GetPosition());
        G4Region* region = volume->GetLogicalVolume()->GetRegion();
        UserRegionInformation* regionInfo = dynamic_cast<UserRegionInformation*>(region->GetUserInformation());
        return regionInfo;
    }

    /**
     * Setup the UserTrackInformation for the G4Track, which also requires first
     * creating a TrackSummary containing all the track information.  This will register
     * the TrackSummary in the global list for the event.
     * @return The UserTrackInformation that was created for the track.
     */
    static UserTrackInformation* setupUserTrackInformation(const G4Track* track, G4bool save) {

        /* Create new TrackSummary. */
        TrackSummary* trackSummary = new TrackSummary(track, save);

        /* Add it to the master list. */
        TrackManager::instance()->getTrackSummaries()->push_back(trackSummary);

        /* Associate it to its track ID. */
        (*TrackManager::instance()->getTrackSummaryMap())[track->GetTrackID()] = trackSummary;

        /* Create new UserTrackInformation. */
        UserTrackInformation* trackInfo = new UserTrackInformation(trackSummary);

        /* Set the UserTrackInformation on the track. */
        ((G4Track*) track)->SetUserInformation(trackInfo);

        return trackInfo;
    }

    /**
     * Get the UserTrackInformation for the track.
     * @return The UserTrackInformation for the track, which may be NULL.
     */
    static UserTrackInformation* getUserTrackInformation(const G4Track* track) {
        return dynamic_cast<UserTrackInformation*>(track->GetUserInformation());
    }
};

}

#endif
