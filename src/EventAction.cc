// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/EventAction.cc,v 1.32 2013-06-26 01:57:02 jeremy Exp $

// LCDD
#include "EventAction.hh"
#include "lcdd/util/StringUtil.hh"

// SLIC
#include "LcioManager.hh"
#include "TrajectoryManager.hh"
#include "EventSourceManager.hh"
#include "SlicApplication.hh"
#include "EventMessenger.hh"
#include "EventDebugger.hh"

// Geant4
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4Event.hh"

namespace slic {

EventAction::EventAction() :
		G4UserEventAction(), Module("EventAction", false), m_enableEventTimer(false) {
	m_messenger = new EventMessenger();
}

EventAction::~EventAction() {
}

void EventAction::BeginOfEventAction(const G4Event *anEvent) {
	// check aborted
	if (!anEvent->IsAborted()) {

		// debugger
		//EventDebugger::instance()->beginEvent( anEvent );

		// trj mgr
		TrajectoryManager::instance()->beginEvent(anEvent);

		/* MCP begin event */
		LcioMcpManager::instance()->beginEvent(anEvent);
	}
#ifdef SLIC_LOG
	else {
		log().warning("aborted EventAction::BeginOfEventAction");
	}
#endif
}

void EventAction::EndOfEventAction(const G4Event *anEvent) {
	// check aborted
	if (!SlicApplication::instance()->isAborting()) {
		// LcioManager's action
		LcioManager::instance()->endEvent(anEvent);
	}

	// event source (generator) action
	EventSourceManager::instance()->endEvent(anEvent);

	// event timer
	stopEventTimer();

	// debugger
	//EventDebugger::instance()->endEvent(anEvent);

	// end event mesg
	printEndEventMessage(anEvent);
}

void EventAction::printEndEventMessage(const G4Event *anEvent) {
	log() << LOG::okay << ">>>> EndEvent <" + StringUtil::toString(anEvent->GetEventID()) + ">" << LOG::endl
			<< LOG::done;
}

EventAction* EventAction::getEventAction() {
	const EventAction* ea =
			static_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
	return const_cast<EventAction*>(ea);
}

void EventAction::stopEventTimer() {
	if (m_enableEventTimer) {
		m_eventTimer.Stop();
#ifdef SLIC_LOG
		log() << LOG::okay << "Event Timer: " << m_eventTimer << LOG::done;
#endif
	}
}

void EventAction::startEventTimer() {
	if (m_enableEventTimer) {
		m_eventTimer.Start();
#ifdef SLIC_LOG
		log() << LOG::verbose << "Started Event Timer." << LOG::done;
#endif
	}
}

void EventAction::enableEventTimer(bool et) {
	m_enableEventTimer = et;
}
}
