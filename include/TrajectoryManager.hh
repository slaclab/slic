// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/TrajectoryManager.hh,v 1.36 2013-11-06 00:23:36 jeremy Exp $

#ifndef SLIC_TRAJECTORYMANAGER_HH
#define SLIC_TRAJECTORYMANAGER_HH 1

// LCDD
#include "lcdd/geant4/UserRegionInformation.hh"
#include "lcdd/hits/TrackInformation.hh"

// SLIC
#include "Module.hh"
#include "Singleton.hh"
#include "Trajectory.hh"

// Geant4
#include "G4Track.hh"
#include "G4TrackingManager.hh"

// STL
#include <map>

class G4Event;

namespace slic {

/**
 * @class TrajectoryManager
 *
 * @brief Provides runtime management of Trajectory and TrackInformation objects,
 * mainly to update them so that the LcioMcpManager can apply the correct status
 * settings to output MCParticles.
 *
 * @todo Add current track ID and G4Track instance variables to simplify
 * internal handling and function signatures.
 */

class TrajectoryManager: public Module, public Singleton<TrajectoryManager> {

public:

    /**
     * Map of track ID to Trajectory.
     */
    typedef std::map<G4int, Trajectory*> TrackIDToTrajectoryMap;

public:

    /**
     * Class destructor.
     */
    virtual ~TrajectoryManager();

    /**
     * Class constructor.
     */
    TrajectoryManager();

public:

    /**
     * Begin of event action.
     * Clears the TrackIDToTrajectoryMap for new data.
     * @param[in] event The G4Event that is beginning.
     */
    void beginEvent(const G4Event* event) {
        clearTrackIDToTrajectoryMap();
    }

    /**
     * Pre-tracking action.
     * @param[in] track The new G4Track.
     */
    void preTracking(const G4Track* track);

    /**
     * Post-tracking action.
     * @param[in] track The G4Track that is about to be deleted.
     */
    void postTracking(const G4Track* track);

    /**
     * The stepping action.
     * @param[in] step The current G4Step.
     */
    void stepping(const G4Step* step);

    /**
     * Find a Trajectory by trackID.
     * @param[in] trackID The track ID.
     * @return The matching Trajectory or null if not found.
     */
    Trajectory* findTrajectory(G4int trackID);

    /**
     * Find a Trajectory from a G4Track.
     * @param[in] aTrack The G4Track.
     * @return The matching Trajectory or null if not found.
     */
    Trajectory* findTrajectory(const G4Track* aTrack) {
        return findTrajectory(aTrack->GetTrackID());
    }

    /**
     * Find a Trajectory from a G4Track.
     * @param[in] aTrack The G4Track.
     * @return The matching Trajectory or null if not found.
     */
    Trajectory* findTrajectory(G4Track* aTrack) {
        return findTrajectory((const_cast<const G4Track*>(aTrack))->GetTrackID());
    }

    /**
     * Print out the TrackIDToTrajectoryMap.
     */
    void printTrackIDToTrajectoryMap() {
        log() << LOG::debug << "TrackIDToTrajectoryMap" << LOG::done;
        log() << LOG::debug << "trackID | trjPtr" << LOG::done;
        for (TrackIDToTrajectoryMap::iterator iter = m_trackIDToTrajectory.begin(); iter != m_trackIDToTrajectory.end(); iter++) {
            log() << LOG::debug << iter->first << " " << iter->second << LOG::done;
        }
    }

    /**
     * Set the G4TrackingManager.
     * @param[in] trackManager The G4TrackingManager to be used.
     */
    inline void setTrackingManager(const G4TrackingManager* trackManager) {
        m_trackingManager = const_cast<G4TrackingManager*>(trackManager);
    }

private:

    /**
     * Handle a primary track, e.g. one without a parent.
     * This function will set its TrackInformation and create a Trajectory for it.
     * @param[in] The G4Track which is a primary.
     */
    void handlePrimaryTrack(const G4Track* aTrack);

    /**
     * Setup the TrackInformation for a primary track.
     * @param[in] aTrack The G4Track which is a primary.
     */
    void setPrimaryTrackInformation(const G4Track* aTrack);

    /**
     * Handle a secondary track, which is the daughter of another.
     * This method will set the status flags on this track's Trajectory,
     * or create a TrackInformation object and Trajectory for it,
     * if needed.
     * @param[in] aTrack The G4Track which is a secondary.
     */
    void handleSecondaryTrack(const G4Track* aTrack);

    /**
     * Create a Trajectory for a secondary G4Track.  Called from handleSecondaryTrack().
     * @param[in] aTrack The secondary G4Track.
     */
    Trajectory* createSecondaryTrajectory(const G4Track* aTrack);

    /**
     * Setup the TrackInformation for a secondary.
     * @param[in] aTrack The secondary G4Track.
     */
    void setupSecondaryTrackInformation(const G4Track* aTrack);

    /**
     * Copy the G4Track's TrackInformation to its secodaries.
     * @param[in] aTrack The parent G4Track.
     */
    void copySecondaryTrackInformationFromParent(const G4Track* aTrack);

    /**
     * Check if this track requires manual trajectory setup, e.g. if it
     * has a SimTrackerHit assigned to it but no Trajectory is being stored.
     * @param[in] aTrack The G4Track.
     * @return True if G4Track needs a Trajectory to be setup; false if not.
     */
    bool needsManualTrajectorySetup(const G4Track* aTrack);

    /**
     * Set the final status flag of a G4Track.
     * @param[in] aTrack The G4Track.
     */
    void setTrajectoryFinalStatus(const G4Track* aTrack);

    /**
     * Get the number of secondaries at the endpoint of the track.
     * @param[in] aTrack The G4Track.
     * @return The number of secondaries produced at the end of the track.
     */
    G4int getNumberOfSecondariesAtEnd(const G4Track* aTrack);

    /**
     * Return true if this track is primary, i.e. it has no parent.
     * @return True if this track is primary; false if it is a secondary.
     */
    inline bool isPrimaryTrack(const G4Track* aTrack) const {
        return (aTrack->GetParentID() == 0);
    }

    /**
     * Return true if energy is less than region's threshold to create a trajectory or if this
     * flag has been set on the TrackInformation.
     * @return True if track is below threshold; false if not.
     */
    inline bool isBelowThreshold(const G4Track* aTrack) const {
        return ((aTrack->GetKineticEnergy() < UserRegionInformation::getRegionInformation(aTrack)->getThreshold())
                || (TrackInformation::getTrackInformation(aTrack)->getBelowThreshold() == true));
    }

    /**
     * Check if the particle is backscattered back into a tracking region.
     * This checks that the original tracking status is "not in a tracking region",
     * which works ok because this function is only relevent when the track is
     * currently within the tracking region.
     * @return true if track is backscattered; false if not.
     */
    inline bool isBackscatter(const G4Track* aTrack) const {
        return TrackInformation::getTrackInformation(aTrack)->getOriginalTrackingStatus() != TrackInformation::eInTrackingRegion;
    }

    /**
     * Return true if the G4Track associated with the G4Step is in the "alive" state.
     * @return true if the G4Track from the G4Step is alive; false if not.
     */
    bool isAlive(const G4Step* aStep) {
        return aStep->GetTrack()->GetTrackStatus() == fAlive;
    }

    /**
     * Get the current Trajectory from the G4TrackingManager.
     * @return The Trajectory from the G4TrackingManager.
     */
    Trajectory* getCurrentTrajectory() {
        return static_cast<Trajectory*>(m_trackingManager->GimmeTrajectory());
    }

    /**
     * Fill the track ID to Trajectory map from the G4Event.
     * @param[in] event The G4Event.
     * @param[in] clearIt Set to true to clear the map before filling it.
     */
    void fillTrajectoryMap(const G4Event* event, bool clearIt = true);

    /**
     * Add a Trajectory to map by its track ID.
     * @param[in] trajectory The Trajectory to add.
     */
    void addTrackIDToTrajectoryLink(Trajectory* trajectory);

    /**
     * Clear the TrackIDToTrajectoryMap.
     */
    void clearTrackIDToTrajectoryMap() {
        m_trackIDToTrajectory.clear();
    }

    /**
     * Set the tracking flag from the UserRegionInformation, i.e. in or outside tracking region.
     * @param[out] trackInformation The TrackInformation to update.
     * @param[in] regionInformation The region information with the status.
     */
    void setTrackingFlagFromRegionInformation(TrackInformation* trackInformation, UserRegionInformation* regionInformation);

    /**
     * Using the parent's endpoint and the secondaries starting position to determine
     * the value of the vertexIsNotEndpointOfParent flag.
     * @param[in] endpoint The parent's endpoint.
     * @param[in] secondary The secondary G4Track.
     * @return true if vertex is not endpoint of parent; false if not.
     */
    bool vertexIsNotEndpointOfParent(const G4ThreeVector& endpoint, G4Track* secondary) {
        //assert(secondary);

        bool r = false;

        if (!endpoint.isNear(secondary->GetPosition())) {
            r = true;
        }

        return r;
    }

    /**
     * Get the Trajectory's point position by index.
     * @param[in] trajectory The Trajectory.
     * @param[in] positionIndex The index into the vector of point positions.
     * @return The Trajectory's point position.
     */
    static G4ThreeVector getTrajectoryPointPosition(Trajectory* trajectory, G4int positionIndex = 0) {
        // check null trajectory
        if (!trajectory) {
            G4Exception("TrajectoryManager::getTrajectoryPointPosition()", "", FatalException, "Trajectory is null.");
        }

        // get number of trajectories
        G4int numPnts = trajectory->GetPointEntries();

        // check idx argument against number of trajectories
        if ((positionIndex > (numPnts - 1)) || positionIndex < 0) {
            G4Exception("TrajectoryManager::getTrajectoryPointPosition()", "", FatalException, "The trajectory position index is invalid.");
        }

        // return point at idx
        return trajectory->GetPoint(positionIndex)->GetPosition();
    }

    /**
     * Get a Trajectory's vertex point, which is the first entry in the point position vector.
     * @param[in] The Trajectory.
     * @return The point position at the vertex.
     */
    inline static G4ThreeVector getTrajectoryVertex(Trajectory* trajectory) {
        return getTrajectoryPointPosition(trajectory, 0);
    }

    /**
     * Get a trajectory's endpoint, which is the last entry in the point position vector.
     * @param[in] trajectory The Trajectory.
     * @return The Trajectory's endpoint position.
     */
    inline static G4ThreeVector getTrajectoryEndpoint(Trajectory* trajectory) {
        return getTrajectoryPointPosition(trajectory, trajectory->GetPointEntries() - 1);
    }

    /**
     * Create a new trajectory from a G4Track.
     * @param[in] aTrack The source G4Track.
     * @return The output Trajectory from the track data.
     */
    Trajectory* createTrajectoryFromTrack(const G4Track* aTrack);

    /**
     * Get the current Trajectory being processed.
     * @return Pointer to the current trajectory being processed.
     */
    inline Trajectory* getCurrentTrajectory() const {
        return m_currentTrajectory;
    }

    /**
     * Get the current G4Track being processed.
     * @return The current G4Track.
     */
    inline const G4Track* getCurrentTrack() const {
        return m_currentTrack;
    }

    /**
     * Get the current track ID.
     * @return The current G4Track's ID.
     */
    inline G4int getCurrentTrackID() const {
        return m_currentTrackID;
    }

    /**
     * Print the status flags of a G4Track.
     * @param[in] track The G4Track.
     */
    void printStatusFlags(const G4Track* track);

    /**
     * Print the status flags from the TrackInformation.
     * @param[in] trackInformation The TrackInformation.
     */
    void printStatusFlags(TrackInformation* trackInformation);

    /**
     * Print the data of the G4Track.
     * @param[in] track The G4Track.
     * @param[in] isSecondary Set to true if track is a secondary; false if not.
     */
    void printTrack(const G4Track* track, bool isSecondary = false);

    /**
     * Print out the Trajectory.
     * @param[in] trajectory The Trajectory.
     */
    void printTrajectory(const Trajectory* trajectory);

    /**
     * Print out the secondaries of the current track.
     */
    void printSecondaries();

    /**
     * Print out physical volume information from the G4StepPoint.
     * @param[in] stepPoint The G4StepPoint.
     */
    void printPhysVolDbg(G4StepPoint* stepPoint);


private:

    G4TrackingManager* m_trackingManager;
    TrackIDToTrajectoryMap m_trackIDToTrajectory;
    Trajectory* m_currentTrajectory;
    const G4Track* m_currentTrack;
    G4int m_currentTrackID;
};
}

#endif
