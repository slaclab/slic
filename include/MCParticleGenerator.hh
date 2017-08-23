#ifndef SLIC_MCPARTICLEGENERATOR_HH_
#define SLIC_MCPARTICLEGENERATOR_HH_ 1

// LCIO
#include "EVENT/LCEvent.h"
#include "EVENT/MCParticle.h"
#include "IO/LCReader.h"
#include "IMPL/LCCollectionVec.h"

// Geant4
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"

// STL
#include <set>

using EVENT::MCParticle;
using EVENT::LCCollection;
using EVENT::LCEvent;
using IMPL::LCCollectionVec;
using IO::LCReader;

namespace slic {

class MCParticleManager;

/**
 * @class MCParticleGenerator
 * @brief Converts from MCParticle collection into a G4Event
 */
class MCParticleGenerator {

    public:

        /**
         * Class constructor.
         */
        MCParticleGenerator(G4String eventFile);

        /**
         * Class destructor.
         */
        virtual ~MCParticleGenerator();

        /**
         * Generate a G4Event from an MCParticleCollection in an LCEvent.
         * @param[in,out] event The input G4Event to be created.
         */
        void generateEvent(G4Event* event);

        /**
         * Read the next LCEvent.
         */
        void readNextEvent();

        /**
         * Get the current LCEvent.
         * @return The current LCEvent.
         */
        LCEvent* getCurrentEvent();

    private:

        LCEvent* _lcevent;
        LCReader* _reader;
        MCParticleManager* _particleManager;
};

}

#endif
