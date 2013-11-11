// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/EventDebugger.hh,v 1.7 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_EVENTDEBUGGER_HH
#define SLIC_EVENTDEBUGGER_HH 1

// slic
#include "Module.hh"
#include "Singleton.hh"

// geant4
#include "globals.hh"

// std
#include <string>
#include <vector>

class G4Run;
class G4Event;

namespace slic {
class EventDebuggerMessenger;

/**
 * @class EventDebugger
 * @brief Manages debugging of individual G4Events.
 */
class EventDebugger: public Module, public Singleton<EventDebugger> {
public:

    typedef std::vector<G4int> DebugEventList;

public:

    /**
     * Class constructor.
     */
    EventDebugger();

    /**
     * Class destructor.
     */
    virtual ~EventDebugger();

public:

    /**
     * Turn event debugging on or off.
     * @param[in] e The event debugging setting.
     */
    void enableDebug(bool e = true);

    /**
     * Add an event number that should be debugged.
     * @param[in] eventNumber The event number to debug.
     */
    void addDebugEvent(G4int eventNumber);

    /**
     * Clear the event number list.
     */
    void clearDebugEvents();

    /**
     * Check if a given event should be debugged.
     * @param[in] eventNumber True if event should be debugged; false if not.
     */
    bool haveDebugEvent(G4int eventNumber) const;

    /**
     * Set a Geant4 macro to be executed on debug events.
     * @param[in] macro The Geant4 macro string to be executed on debug events.
     */
    void setDebugMacro(std::string macro);

    /**
     * Set the macro for cleaning up after debugging an event.
     * @param[in] macro The Geant4 macro string to be executed on debug events.
     */
    void setCleanupMacro(std::string macro);

    /**
     * Begin of run hook.
     * @param[in] run The Geant4 run.
     */
    void beginRun(const G4Run* run);

    /**
     * Begin of event hook.
     * @param[in] event The Geant4 event.
     */
    void beginEvent(const G4Event* event);

    /**
     * End of event hook.
     * @param[in] event The Geant4 event.
     */
    void endEvent(const G4Event* event);

private:

    /**
     * Sort the event number list in ascending order.
     */
    void sortDebugEvents();

    /**
     * Execute the debug macro.
     */
    void execDebugMacro();

    /**
     * Execute the cleanup macro.
     */
    void execCleanupMacro();

private:

    EventDebuggerMessenger* m_messenger;
    std::string m_debugMacro;
    std::string m_cleanupMacro;
    DebugEventList m_events;
    bool m_debugging;
    bool m_haveDebugMacro;
    bool m_haveCleanupMacro;
    bool m_forceDebugMode;
};
}

#endif
