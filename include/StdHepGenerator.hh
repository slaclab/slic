#ifndef SLIC_STDHEPGENERATOR_HH_
#define SLIC_STDHEPGENERATOR_HH_ 1

// SLIC
#include "MCParticleManager.hh"

// LCIO
#if HAVE_LCSTDHEPRDRNEW_H
#include "UTIL/LCStdHepRdrNew.h"
#else
#include "UTIL/LCStdHepRdr.h"
#endif
#include "EVENT/MCParticle.h"
#include "IMPL/LCCollectionVec.h"

// Geant4
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"

// STL
#include <set>

#if HAVE_LCSTDHEPRDRNEW_H
using UTIL::LCStdHepRdrNew;
#else
using UTIL::LCStdHepRdr;
#endif

using EVENT::MCParticle;
using EVENT::LCCollection;
using IMPL::LCCollectionVec;

namespace slic {

/**
 * @class StdHepGenerator
 * @brief Converts from StdHep input to a G4Event
 * @note Borrows heavily from Mokka's HepLCIOInterfaceNew class.
 */
class StdHepGenerator {

    public:

        /**
         * Class constructor.
         */
        StdHepGenerator(G4String eventFile);

        /**
         * Class destructor.
         */
        virtual ~StdHepGenerator();

        /**
         * Generate a G4Event from the current collection of MCParticles.
         * @param[in] event The output G4Event.
         */
        void generateEvent(G4Event* event);

        /**
         * Read the next event from the StdHep file.
         */
        void readNextEvent();

        /**
         * Get the current MCParticle collection.
         * @return The current MCParticle collection.
         */
        LCCollectionVec* getCurrentParticleCollection();

    private:
#if HAVE_LCSTDHEPRDRNEW_H
        LCStdHepRdrNew* _reader;
#else
        LCStdHepRdr* _reader;
#endif
        LCCollectionVec* _particles;
};

}

#endif
