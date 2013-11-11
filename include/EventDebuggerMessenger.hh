// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/EventDebuggerMessenger.hh,v 1.6 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_EVENTDEBUGGERMESSENGER_HH
#define SLIC_EVENTDEBUGGERMESSENGER_HH 1

// geant4
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"

class G4UIcmdWithAString;
class G4UIcmdWithABool;

namespace slic {

/**
 * @class EventDebuggerMessenger
 * @brief G4UImessenger for the EventDebugger.
 */
class EventDebuggerMessenger: public G4UImessenger {

public:

    /**
     * Class constructor.
     */
    EventDebuggerMessenger();

    /**
     * Class destructor.
     */
    virtual ~EventDebuggerMessenger();

public:

    /**
     * Process the macro command.
     * @param[in] cmd The macro command.
     * @param[in] newVals The command arguments.
     */
    void SetNewValue(G4UIcommand* cmd, G4String newVals);

private:

    /**
     * Define the Geant4 macro commands.
     */
    void defineCommands();

private:
    G4UIdirectory* m_debugDir;
    G4UIcmdWithAString* m_debugMacroCmd;
    G4UIcmdWithAString* m_cleanupMacroCmd;
    G4UIcommand* m_clearDebugEventsCmd;
    G4UIcmdWithABool* m_enableDebugCmd;
    G4UIcommand* m_addDebugEventsCmd;
};
}

#endif
