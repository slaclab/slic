// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/RunAction.cc,v 1.23 2013-06-26 01:57:02 jeremy Exp $

// SLIC
#include "LcioManager.hh"
#include "EventSourceManager.hh"
#include "EventDebugger.hh"

// LCDD
#include "lcdd/core/GeometryManager.hh"

// Geant4
#include "RunAction.hh"
#include "G4Run.hh"

namespace slic {

RunAction::RunAction() :
		Module("RunAction", false) {
}

RunAction::~RunAction() {
}

void RunAction::BeginOfRunAction(const G4Run *aRun) {
	// Start the run clock.
	startRunTimer();

	// Print the run start message.
#ifdef SLIC_LOG
	printBeginOfRunMessage( aRun );
#endif

	// Execute LcioManager's beginRun action.
	LcioManager::instance()->beginRun(aRun);

	// Execute EventSourceManager's beginRun action
	EventSourceManager::instance()->beginRun(aRun);

	// Execute GeometryManager's beginRun action.
	GeometryManager::instance()->beginRun(aRun);

	// Execute EventDebugger's beginRun action.
	//EventDebugger::instance()->beginRun( aRun );
}

void RunAction::EndOfRunAction(const G4Run *run) {
	// LcioManager's action
	LcioManager::instance()->endRun(run);

	// event source mgr
	EventSourceManager::instance()->endRun(run);

	// stop run clock
	stopRunTimer();

	// end message
#ifdef SLIC_LOG
	printEndOfRunMessage( run );
#endif
}

void RunAction::startRunTimer() {
	// start run timer
	m_runTimer.Start();
#ifdef SLIC_LOG
	log() << LOG::okay << "Started Run Timer." << LOG::done;
#endif
}

void RunAction::stopRunTimer() {
	// stop run timer
	m_runTimer.Stop();

#ifdef SLIC_LOG
	log() << LOG::okay << "Run Timer: " << m_runTimer << LOG::done;
#endif
}

void RunAction::printBeginOfRunMessage(const G4Run* aRun) {
	log() << LOG::okay << ">>>> BeginRun <" << aRun->GetRunID() << ">" << LOG::done;
}

void RunAction::printEndOfRunMessage(const G4Run* aRun) {
	log() << LOG::okay << ">>>> EndRun <" << aRun->GetRunID() << ">" << LOG::done;
}
}
