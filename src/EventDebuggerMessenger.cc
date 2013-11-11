// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/EventDebuggerMessenger.cc,v 1.6 2013-06-26 01:57:02 jeremy Exp $
#include "EventDebuggerMessenger.hh"

// slic
#include "EventDebugger.hh"

// lcdd
#include "lcdd/util/StringUtil.hh"

// geant4
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

namespace slic {
EventDebuggerMessenger::EventDebuggerMessenger() {
	defineCommands();
}

EventDebuggerMessenger::~EventDebuggerMessenger() {
}

void EventDebuggerMessenger::SetNewValue(G4UIcommand* cmd, G4String newVals) {
	EventDebugger* debugger = EventDebugger::instance();
	if (cmd == m_debugMacroCmd) {
		debugger->setDebugMacro(newVals);
	} else if (cmd == m_cleanupMacroCmd) {
		debugger->setCleanupMacro(newVals);
	} else if (cmd == m_addDebugEventsCmd) {

		std::vector<std::string> strList;
		const std::string str = std::string(newVals);
		const std::string delim = " ";

		StringUtil::split(str, delim, strList);

		EventDebugger::DebugEventList dbgList;
		for (std::vector<std::string>::iterator it = strList.begin(); it != strList.end(); it++) {
			debugger->addDebugEvent(StringUtil::toInt(*it));
		}
	} else if (cmd == m_clearDebugEventsCmd) {
		debugger->clearDebugEvents();
	} else if (cmd == m_enableDebugCmd) {
		debugger->enableDebug(m_enableDebugCmd->GetNewBoolValue(newVals.c_str()));
	}
}

void EventDebuggerMessenger::defineCommands() {
	m_debugDir = new G4UIdirectory("/debug/");
	m_debugDir->SetGuidance("Debugging commands. [SLIC]");

	m_debugMacroCmd = new G4UIcmdWithAString("/debug/debugMacro", this);
	m_debugMacroCmd->SetGuidance("Set macro for debugging.");

	m_cleanupMacroCmd = new G4UIcmdWithAString("/debug/cleanupMacro", this);
	m_cleanupMacroCmd->SetGuidance("Set macro to cleanup debug state.");

	m_addDebugEventsCmd = new G4UIcmdWithAString("/debug/addEvents", this);
	m_addDebugEventsCmd->SetGuidance("Add event IDs to debug, separated by spaces.");

	m_clearDebugEventsCmd = new G4UIcommand("/debug/clearEvents", this);
	m_clearDebugEventsCmd->SetGuidance("Clear the list of events to debug.");

	m_enableDebugCmd = new G4UIcmdWithABool("/debug/enableDebug", this);
	m_enableDebugCmd->SetGuidance("Enable event debugging for all subsequent events.");
	m_enableDebugCmd->SetParameterName("enable", true);
	m_enableDebugCmd->SetDefaultValue(true);
}
} // namespace slic
