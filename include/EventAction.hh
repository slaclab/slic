#ifndef SLIC_EVENTACTION_HH_
#define SLIC_EVENTACTION_HH_ 1

// SLIC
#include "Module.hh"
#include "PluginManagerAccessor.hh"

// LCDD
#include "lcdd/hits/CalorimeterHit.hh"
#include "lcdd/hits/TrackerHit.hh"

// Geant4
#include "G4UserEventAction.hh"
#include "G4Timer.hh"

class G4UImessenger;

namespace slic {

/**
 * @class EventAction
 * @brief Implementation of G4UserEventAction.
 * @note  Calls actions of TrajectoryManager and LcioManager.
 */
class EventAction: public G4UserEventAction, public Module, public PluginManagerAccessor {

public:

    /**
     * Class constructor.
     */
    EventAction();

    /**
     * Class destructor.
     */
    virtual ~EventAction();

public:

    /**
     * Callback for the beginning of a Geant4 event.
     * @param[in] anEvent The Geant4 event.
     */
    void BeginOfEventAction(const G4Event *anEvent);

    /**
     * Callback for the end of a Geant4 event.
     * @param[in] anEvent The Geant4 event.
     */
    virtual void EndOfEventAction(const G4Event *anEvent);

    /**
     * Get the singleton instance of this class.
     * @return The EventAction object.
     */
    static EventAction* getEventAction();

    /**
     * Enable or disable the event processing timer.
     * @param[in] et The event timer setting; true to turn on.
     */
    void enableEventTimer(bool et = true);

    /**
     * Start the event timer.
     */
    void startEventTimer();

    /**
     * Stop the event timer.
     */
    void stopEventTimer();

private:

    /**
     * Print the end of event message.
     * @param[in] anEvent The Geant4 event.
     */
    void printEndEventMessage(const G4Event *anEvent);

private:

    bool m_enableEventTimer;
    mutable G4Timer m_eventTimer;

    G4UImessenger* m_messenger;
};
}

#endif
