#ifndef SLIC_TRACKMANAGER_HH_
#define SLIC_TRACKMANAGER_HH_ 1

// SLIC
#include "Module.hh"
#include "TrackSummary.hh"
#include "Singleton.hh"

// LCIO
#include "EVENT/LCEvent.h"

// Geant4
#include "G4Event.hh"

using EVENT::LCEvent;

namespace slic {

/**
 * @class TrackManager
 * @brief Manages access to stored track information used for MCParticle output
 */
class TrackManager : public Singleton<TrackManager>, public Module {

public:

    /**
     * Class constructor.
     */
    TrackManager() : Module("TrackManager") {
        _trackSummaries = new TrackSummaryVector;
        _trackSummaryMap = new TrackSummaryMap;
    }

    /**
     * Class destructor.
     */
    virtual ~TrackManager() {
        delete _trackSummaries;
        delete _trackSummaryMap;
    }

public:

    /**
     * Get the current collection of TrackSummary objects.
     * @return The current collection of TrackSummaries.
     */
    TrackSummaryVector* getTrackSummaries() {
        return _trackSummaries;
    }

    /**
     * Save the TrackSummary vector into a collection of MCParticles in an output LCEvent.
     * @param[in] event The current G4Event.
     * @param[in,out] lcevent The output LCEvent.
     */
    void saveTrackSummaries(const G4Event* event, LCEvent* lcevent);

    /**
     * Reset the state of this object by clearing data structures.
     */
    void reset() {
        /* Clear the vector. */
        for (TrackSummaryVector::iterator it = _trackSummaries->begin();
                it != _trackSummaries->end();
                it++) {
            delete *it;
        }
        _trackSummaries->clear();

        /* Clear the map. */
        _trackSummaryMap->clear();
    }

    /**
     * Get the map of TrackSummary objects to track IDs.
     * @return The TrackSummaryMap.
     */
    TrackSummaryMap* getTrackSummaryMap() {
        return _trackSummaryMap;
    }

    /**
     * Map a TrackSummary to its track ID.
     * @param[in] trackSummary The TrackSummary.
     */
    void registerTrackSummary(TrackSummary* trackSummary) {
        (*_trackSummaryMap)[trackSummary->getTrackID()] = trackSummary;
    }

    /**
     * Find a TrackSummary by track ID.
     * @param[in] trackID The track ID.
     * @return The matching TrackSummary or NULL if not found.
     */
    TrackSummary* findTrackSummary(G4int trackID) {
        return (*_trackSummaryMap)[trackID];
    }

private:

    /**
     * Find the first saved ancestor of a TrackSummary and associate it to
     * the first saved MCParticle ancestor.
     * @param[in] trackSummary The child TrackSummary.
     * @return The first saved ancestor TrackSummary.
     */
    TrackSummary* findFirstSavedAncestor(TrackSummary* trackSummary);

private:

    TrackSummaryVector* _trackSummaries;
    TrackSummaryMap* _trackSummaryMap;
};

}

#endif
