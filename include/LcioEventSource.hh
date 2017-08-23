#ifndef SLIC_LCIOEVENTSOURCE_HH_
#define SLIC_LCIOEVENTSOURCE_HH_ 1

// SLIC
#include "EventSourceWithInputFile.hh"
#include "MCParticleGenerator.hh"
#include "LcioManager.hh"

// LCIO
#include "EVENT/LCEvent.h"
#include "EVENT/LCCollection.h"
#include "IOIMPL/LCFactory.h"
#include "IO/LCReader.h"

namespace slic {

/**
 * @class LcioEventSource
 * @brief Generates events from the MCParticle collection of an LCIO file
 */
class LcioEventSource: public EventSourceWithInputFile {

    public:

        /**
         * Class constructor.
         */
        LcioEventSource(const std::string& fname = "");

        /**
         * Class destructor.
         */
        virtual ~LcioEventSource();

    public:

        /**
         * Open the LCIO file.
         */
        void open();

        /**
         * Close the LCIO file.
         */
        void close();

        /**
         * Read the next event.
         */
        void readNextEvent();

        /**
         * Print out the current event.
         */
        void printCurrentEvent();

        /**
         * Generate a G4Event from the current MCParticle collection.
         */
        void generate(G4Event* anEvent);

        /**
         * Begin of run action which executes the superclass method.
         */
        void beginRun(const G4Run* aRun);

        /**
         * Begin of event action which executes the superclass method.
         */
        void beginEvent(const G4Event* anEvent);

    private:
        IO::LCReader* m_reader;
        MCParticleGenerator* m_eventGenerator;
};
}

#endif
