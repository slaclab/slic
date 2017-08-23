#include "EventMessenger.hh"

// SLIC
#include "EventAction.hh"

// Geant4
#include "G4UIcmdWithABool.hh"

namespace slic {
EventMessenger::EventMessenger() {
    defineCommands();
}

EventMessenger::~EventMessenger() {
}

void EventMessenger::SetNewValue(G4UIcommand* cmd, G4String newVals) {
    if (cmd == m_eventTimerCmd) {
        bool et = true;

        if (newVals != G4String("")) {
            et = G4UIcmdWithABool::GetNewBoolValue(newVals);
        }

        EventAction::getEventAction()->enableEventTimer(et);
    } else {
        G4Exception("", "", JustWarning, "Unknown command for this messenger.");
    }
}

void EventMessenger::defineCommands() {
    m_eventTimerCmd = new G4UIcmdWithABool("/event/enableEventTimer", this);
    m_eventTimerCmd->SetGuidance("Enable timing of individual events.");
    m_eventTimerCmd->SetParameterName("enable", 'b', true);
    m_eventTimerCmd->SetDefaultValue(true);
}
}
