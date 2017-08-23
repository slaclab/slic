#ifndef SLIC_STDHEPEVENTSOURCE_HH_
#define SLIC_STDHEPEVENTSOURCE_HH_ 1

// SLIC
#include "EventSourceWithInputFile.hh"
#include "StdHepGenerator.hh"

namespace slic {
/**
 * @class StdHepEventSource
 * @brief Top-level controller of StdHep input.
 * @note Uses Willy Langeveld's lStdHep and lXDR classes for input processing.
 */
class StdHepEventSource: public EventSourceWithInputFile {

    public:

        /**
         * Class constructor.
         * @param[in] fname The file name.
         */
        StdHepEventSource(const std::string& fname);

    public:

        /**
         * Generate events.
         * @param[in] anEvent The target G4Event.
         */
        void generate(G4Event* anEvent);

        /**
         * Open the current file for reading.
         */
        void open();

        /**
         * Close the current file.
         */
        void close();

        /**
         * Read in the next event.
         */
        void readNextEvent();

        /**
         * Print out the current event.
         */
        void printCurrentEvent();

        /**
         * Begin of run action.
         * @param[in] aRun The G4Run that is starting.
         */
        void beginRun(const G4Run* aRun);

        /**
         * Begin of event action.
         * @param[in] anEvent The G4Event that is tarting.
         */
        void beginEvent(const G4Event* anEvent);

    private:
        StdHepGenerator* m_eventGenerator;
};
}

#endif
