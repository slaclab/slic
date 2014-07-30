#ifndef SLIC_EVENTMESSENGER_HH_
#define SLIC_EVENTMESSENGER_HH_ 1

// Geant4
#include "G4UImessenger.hh"

class G4UIcmdWithABool;

namespace slic {

/**
 * @class EventMessenger
 * @brief Messenger for event-related commands.
 */
class EventMessenger: public G4UImessenger {
public:

    /**
     * Class constructor.
     */
    EventMessenger();

    /**
     * Class destructor.
     */
    virtual ~EventMessenger();

public:

    /**
     * Process the macro command.
     * @param[in] command The macro command.
     * @param[in] newValues The argument values.
     */
    void SetNewValue(G4UIcommand* command, G4String newValues);

private:

    /**
     * Define the commands that this messenger with handle.
     */
    void defineCommands();

private:
    G4UIcmdWithABool* m_eventTimerCmd;
};
}

#endif
