// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/EventDebugger.cc,v 1.7 2012-11-27 19:32:19 jeremy Exp $
#include "EventDebugger.hh"

// slic
#include "EventDebuggerMessenger.hh"

// geant4
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4UImanager.hh"

namespace slic {
EventDebugger::EventDebugger() :
		Module("EventDebugger"), m_debugging(false), m_haveDebugMacro(false), m_haveCleanupMacro(false), m_forceDebugMode(
				false) {
	m_messenger = new EventDebuggerMessenger();
}

EventDebugger::~EventDebugger() {
	delete m_messenger;
}

void EventDebugger::enableDebug(bool e) {
	m_forceDebugMode = e;
}

void EventDebugger::addDebugEvent(G4int eventNum) {
	if (!haveDebugEvent(eventNum)) {
		m_events.push_back(eventNum);
#ifdef SLIC_LOG      
		log() << LOG::okay << "EventDebugger - Added debug event <" << eventNum << ">" << LOG::done;
#endif
	}
#ifdef SLIC_LOG
	else {
		log() << LOG::warning << "EventDebugger - Ignoring dup event #" << eventNum << LOG::done;
	}
#endif
}

void EventDebugger::setDebugMacro(std::string mac) {
	m_haveDebugMacro = true;
	m_debugMacro = mac;
}

void EventDebugger::setCleanupMacro(std::string mac) {
	m_haveCleanupMacro = true;
	m_cleanupMacro = mac;
}

void EventDebugger::clearDebugEvents() {
	m_events.clear();
}

void EventDebugger::sortDebugEvents() {
	std::sort(m_events.begin(), m_events.end());
}

bool EventDebugger::haveDebugEvent(G4int evtNum) const {
	for (DebugEventList::const_iterator it = m_events.begin(); it != m_events.end(); it++) {
		if ((*it) == evtNum) {
			return true;
		}
	}
	return false;
}

void EventDebugger::beginRun(const G4Run*) {
	sortDebugEvents();
}

void EventDebugger::beginEvent(const G4Event* evt) {
	if (m_forceDebugMode || haveDebugEvent(evt->GetEventID())) {
		m_debugging = true;
		execDebugMacro();
	}
}

void EventDebugger::endEvent(const G4Event*) {
	if (m_debugging) {
		execCleanupMacro();
		m_debugging = false;
	}
}

void EventDebugger::execDebugMacro() {
	if (m_haveDebugMacro) {
		G4UImanager::GetUIpointer()->ApplyCommand("/control/execute " + m_debugMacro);

#ifdef SLIC_LOG
		if ( !m_haveCleanupMacro ) {
			log() << "WARNING: Executed debugging macro, but cleanup macro was not set." << LOG::done;
		}
#endif
	}
#ifdef SLIC_LOG
	else {
		log() << LOG::error << "ERROR: debug macro not set" << LOG::done;
	}
#endif
}

void EventDebugger::execCleanupMacro() {
	if (m_haveCleanupMacro) {
		G4UImanager::GetUIpointer()->ApplyCommand("/control/execute " + m_cleanupMacro);
	}
#ifdef SLIC_LOG
	else {
		log() << LOG::error << "ERROR: cleanup macro not set" << LOG::done;
	}
#endif
}
} // namespace slic
