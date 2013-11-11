// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/EventMessenger.cc,v 1.5 2012-11-27 19:32:19 jeremy Exp $
#include "EventMessenger.hh"

// slic
#include "EventAction.hh"

// geant4
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
