#ifndef SLIC_TRACKSUMMARY_HH_
#define SLIC_TRACKSUMMARY_HH_ 1

// LCIO
#include "lcio.h"
#include "IMPL/MCParticleImpl.h"

// Geant4
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Allocator.hh"

// STL
#include <vector>
#include <bitset>

using IMPL::MCParticleImpl;

class G4Track;

namespace slic {

class TrackSummary;
class TrackManager;

typedef std::vector<TrackSummary*> TrackSummaryVector;
typedef std::map<G4int, TrackSummary*> TrackSummaryMap;

/**
 * @class TrackSummary
 * @brief Container for all information from G4Track that may be persisted to MCParticle output collection
 */
class TrackSummary {

    public:

        /**
         * Class constructor.
         */
        TrackSummary(const G4Track* track, G4bool toBeSaved, G4bool backScattering = false);

        /**
         * Class destructor.
         */
        virtual ~TrackSummary() {
        }

        /**
         * New operator.
         */
        inline void *operator new(size_t);

        /**
         * Delete operator.
         */
        inline void operator delete(void *);

        /**
         * Update this object from a G4Track.
         * @param[in] track The G4Track with update information.
         */
        void update(const G4Track* track);

        /**
         * Get the charge.
         * @return The charge.
         */
        G4double getCharge() const {
            return _charge;
        }

        /**
         * Get the endpoint of the particle.
         * @return The end point of the particle in mm.
         */
        G4ThreeVector getEndPoint() const {
            return _endPoint;
        }

        /**
         * Get the total energy including momentum and mass.
         * @return The total energy in GeV.
         */
        G4double getEnergy() const {
            return sqrt(_momentum[0] * _momentum[0] + _momentum[1] * _momentum[1] + _momentum[2] * _momentum[2] + _mass * _mass);
        }

        /**
         * Get the mass in GeV.
         * @return The mass.
         */
        G4double getMass() const {
            return _mass;
        }

        /**
         * Get the HEPEVT or "generator" status of this particle, which is zero if this particle
         * was generated in the simulation as a secondary.
         * @return The HEPEVT status.
         */
        G4int getHepEvtStatus() const {
            return _hepEvtStatus;
        }

        /**
         * Get the simulator status, which is zero if this is a generator particle.
         * @return The simulator status.
         */
        G4int getSimulatorStatus() const {
            return _simstatus.to_ulong();
        }

        /**
         * Get the momentum (px, py, pz).
         * @return The momentum.
         */
        G4ThreeVector getMomentum() const {
            return _momentum;
        }

        /**
         * Get the parent particle.
         * @return The parent particle.
         */
        TrackSummary* getParent() const {
            return _parent;
        }

        /**
         * Get the PDG ID.
         * @return The PDG ID.
         */
        G4int getPDG() const {
            return _PDG;
        }

        /**
         * Get the vertex (x, y, z) in mm.
         * @return The vertex.
         */
        const G4ThreeVector& getVertex() const {
            return _vertex;
        }

        /**
         * Get the track ID.
         * @return The track ID.
         */
        G4int getTrackID() const {
            return _trackID;
        }

        /**
         * Get the parent particle's track ID.
         * @return The parent's track ID.
         */
        G4int getParentID() const {
            return _parentTrackID;
        }

        /**
         * Get the track length in mm.
         * @return The track length.
         */
        G4double getTrackLength() const {
            return _trackLength;
        }

        /**
         * Return true if this particle will be saved to the output collection.
         * @return True if particle will be saved; false if not.
         */
        G4bool getToBeSaved() const {
            return _toBeSaved;
        }

        /**
         * Set that this particle will be saved.
         */
        void setToBeSaved() {
            _toBeSaved = true;
        }

        /**
         * Return the back scatter status of this particle.
         * @return True if particle is back scatter; false if not.
         */
        G4bool getBackScattering() const {
            return _backScattering;
        }

        /**
         * Set back scatter status to true.
         */
        void setBackScattering() {
            _backScattering = true;
        }

        /**
         * Set toBeSaved flag on this particle's parent.
         */
        void setParentToBeSaved();

        /**
         * Get the MCParticle of this particle.
         * @return The MCParticle of this particle or NULL if not built yet.
         */
        MCParticleImpl* getMCParticle() {
            return _mcparticle;
        }

        /**
         * Set the MCParticle.
         * @param[in] mcp The MCParticle.
         */
        void setMCParticle(MCParticleImpl* mcp) {
            _mcparticle = mcp;
        }

        /**
         * Set hasTrackerHit status to true.
         */
        void setHasTrackerHit() {
            _hasTrackerHit = true;
        }

        /**
         * Build the MCParticle for this TrackSummary.
         */
        void buildMCParticle();

        /**
         * Find the parent particle.
         * @return The parent particle or this if has no parent.
         */
        TrackSummary* findParent() const;

    private:

// Member data

        G4double _charge;
        G4ThreeVector _endPoint;
        G4double _energy;
        G4double _mass;
        G4int _hepEvtStatus;
        std::bitset<32> _simstatus;
        G4ThreeVector _momentum;
        G4ThreeVector _momentumAtEndpoint;
        TrackSummary* _parent;
        G4int _PDG;
        G4ThreeVector _vertex;
        G4double _globalTime;

        G4bool _toBeSaved;

        G4bool _backScattering;
        G4int _trackID;
        G4int _parentTrackID;
        G4double _trackLength;

        MCParticleImpl* _mcparticle;
        G4bool _mcParticleIsUpToDate;

        G4bool _hasTrackerHit;

        static TrackManager* m_trackManager;
};

extern G4Allocator<TrackSummary> TrackSummaryAllocator;

inline void* TrackSummary::operator new(size_t) {
    void *aTrackSummary;
    aTrackSummary = (void*) TrackSummaryAllocator.MallocSingle();
    return aTrackSummary;
}

inline void TrackSummary::operator delete(void* aTrackSummary) {
    TrackSummaryAllocator.FreeSingle((TrackSummary*) aTrackSummary);
}

}

#endif
