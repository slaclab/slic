#ifndef SLIC_EVENTSOURCE_HH_
#define SLIC_EVENTSOURCE_HH_ 1

// SLIC
#include "Module.hh"

// Geant4
#include "G4VPrimaryGenerator.hh"

class G4Event;
class G4Run;

namespace slic {

/**
 * @class EventSource
 * @brief The virtual base class for a physics event source such as a StdHep file.
 */
class EventSource: public Module {

    protected:

        /**
         * Class constructor.
         */
        EventSource(const std::string& name);

    public:

        /**
         * Class destructor.
         */
        virtual ~EventSource();

    public:

        /**
         * Generate an event.
         * @param[in] event The Geant4 event to be modified.
         */
        virtual void generate(G4Event* event) = 0;

        /**
         * Dump information about the current event.
         */
        virtual void printCurrentEvent() = 0;

        /**
         * Reset the event source.
         */
        virtual void reset();

        /**
         * Skip a number of events in the source.
         * @param[in] n The number of events to skip.
         */
        virtual unsigned int skipEvents(unsigned int n);

        /**
         * Hook for beginning of an event.
         * @param[in] event The Geant4 event.
         */
        virtual void beginEvent(const G4Event* event);

        /**
         * Hook for end of an event.
         * @param[in] event The Geant4 event.
         */
        virtual void endEvent(const G4Event* event);

        /**
         * Hook for the beginning of a run.
         * @param[in] run The Geant4 run.
         */
        virtual void beginRun(const G4Run* run);

        /**
         * Hook for the end of a run.
         * @param[in] run The Geant4 run.
         */
        virtual void endRun(const G4Run* run);

    private:

        std::string m_name;
        int nEventsGenerated;

};
}

#endif
