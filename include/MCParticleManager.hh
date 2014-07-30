#ifndef SLIC_MCPARTICLE_MANAGER_HH_
#define SLIC_MCPARTICLE_MANAGER_HH_ 1

// SLIC
#include "Module.hh"
#include "Singleton.hh"

// LCIO
#include "EVENT/LCCollection.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/MCParticleImpl.h"
#include "EVENT/MCParticle.h"

// Geant4
#include "globals.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"

// STL
#include <map>
#include <set>

using std::map;
using std::set;

using EVENT::LCCollection;
using EVENT::MCParticle;
using IMPL::MCParticleImpl;
using IMPL::LCCollectionVec;

namespace slic {

typedef map<G4int, MCParticle*> TrackIDMap;
typedef map<MCParticle*, G4PrimaryParticle*> PrimaryParticleMap;
typedef PrimaryParticleMap::iterator PrimaryParticleMapIterator;
typedef set<MCParticle*> MCParticleSet;

/**
 * @class MCParticleManager
 * @brief Management of global MCParticle information
 *
 * @note
 * This class is responsible for management of MCParticle information
 * and provides central utilities for use by event generation classes
 * such as the MCParticleGenerator and StdHepGenerator.
 */
class MCParticleManager : public Singleton<MCParticleManager>, Module {

public:

    /**
     * Class constructor.
     */
    MCParticleManager()
        : Module("MCParticleManager"), _mcpVec(0) {
    }

    /**
     * Class destructor.
     */
    virtual ~MCParticleManager() {}

    /**
     * Get the current MCParticle collection.
     */
    LCCollectionVec* getMCParticleCollection() {
        return _mcpVec;
    }

    /**
     * Add an MCParticle associated with a track ID.
     * @param[in] trackId The track ID.
     * @param[in] particle The MCParticle object.
     */
    void addMCParticleTrackID(G4int trackId, MCParticle* particle) {
        if (trackId <= 0) {
            return;
        }
        _trackIDMap[trackId] = particle;
    }

    /**
     * Find an MCParticle from a track ID.
     * @param[in] trackId The track ID.
     * @return The found MCParticle or NULL if not found.
     */
    MCParticle* findMCParticle(G4int trackId) {
        return _trackIDMap[trackId];
    }

    /**
     * Get the map of MCParticles to G4PrimaryParticles.
     * @return The current PrimaryParticleMap.
     */
    PrimaryParticleMap* getPrimaryParticleMap() {
        return &_primaryMap;
    }

    /**
     * Reset the state of the MCParticleManager.
     */
    void reset() {
        _trackIDMap.clear();
        _primaryMap.clear();
        _visitedParticles.clear();
        _mcpVec = 0;
    }

    /**
     * Create an empty MCParticle collection.
     */
    void createMCParticleCollection() {
        if (_mcpVec == 0) {
            _mcpVec = new LCCollectionVec(EVENT::LCIO::MCPARTICLE);
        }
    }

    /**
     * Generate a G4Event from a collection of MCParticles.
     * @param[in] mcparticles The MCParticle collection.
     * @param[in] event The output G4Event.
     */
    void generateEvent(LCCollectionVec* mcparticles, G4Event* event);

    /**
     * Apply a Lorentz Transformation angle to a collection of MCParticles.
     * @param[in] mcparticles The MCParticle collection.
     * @param[in] angle The Lorentz Transformation angle.
     */
    void applyLorentzTransformation(LCCollection* mcparticles, const G4double angle);

    /**
     * Get the next smeared Z position.
     * @param[in] The RMS of the smearing algorithm.
     * @return The smeared Z position.
     */
    G4double smearZPosition(const G4double rms);

private:

    /**
     * Get the relevant G4ParticleParticle objects from an MCParticle.
     * @param[in] p The MCParticle.
     * @return The set of relevant G4PrimaryParticle objects.
     */
    std::set<G4PrimaryParticle*> getRelevantParticles(MCParticle* p);

    /**
     * Set the MCParticle collection.
     * @param[in] mcpVec The MCParticle collection.
     */
    void setMCParticleCollection(LCCollectionVec* mcpVec) {
        _mcpVec = mcpVec;
    }

private:

    /**
     * Map of MCParticle to G4PrimaryParticle.
     */
    PrimaryParticleMap _primaryMap;

    /**
     * Set keeping track of which MCParticles have been processed in generator.
     */
    MCParticleSet _visitedParticles;

    /**
     * Map of MCParticle to track ID.
     */
    TrackIDMap _trackIDMap;

    /**
     * The output MCParticle collection.  This pointer is "taken" from the input reader,
     * because it is added to the output event, which then automatically deletes it once the
     * event is written (I think?).
     */
    LCCollectionVec* _mcpVec;
};

}

#endif
