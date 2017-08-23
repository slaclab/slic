#ifndef SLIC_LHEEVENTSOURCE_HH_
#define SLIC_LHEEVENTSOURCE_HH_ 1

// SLIC
#include "EventSourceWithInputFile.hh"
#include "LHEPrimaryGenerator.hh"

namespace slic {
/**
 * @class LHEEventSource
 * @brief Top-level controller of LHE input.
 */
class LHEEventSource: public EventSourceWithInputFile {

    public:

        /**
         * Class constructor.
         * @param[in] fname The file name.
         */
        LHEEventSource(const std::string& fname);

        virtual ~LHEEventSource();

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
         * @param[in] anEvent The G4Event that is starting.
         */
        void beginEvent(const G4Event* anEvent);

    private:
        LHEPrimaryGenerator* m_eventGenerator;
        LHEReader* m_reader;
};
}

#endif
