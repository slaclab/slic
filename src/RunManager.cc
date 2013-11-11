// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/RunManager.cc,v 1.23 2013-06-26 01:57:02 jeremy Exp $

// SLIC
#include "RunManager.hh"
#include "EventAction.hh"
#include "EventSourceManager.hh"
#include "ModuleRegistry.hh"
#include "PhysicsListManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SlicApplication.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"

// LCDD
#include "lcdd/core/LCDDParser.hh"
#include "lcdd/core/LCDDProcessor.hh"

// Geant4
#include "G4StateManager.hh"
#include "G4UImanager.hh"

namespace slic {

RunManager::RunManager() :
		Module("RunManager", false), m_userActionsInitialized(false), m_abortRun(false) {
}

RunManager::~RunManager() {
}

void RunManager::initializeUserActions() {
	SetUserAction(new PrimaryGeneratorAction);
	SetUserAction(new RunAction);
	SetUserAction(new EventAction);
	SetUserAction(new TrackingAction);
	SetUserAction(new SteppingAction);
	m_userActionsInitialized = true;
}

void RunManager::Initialize() {
	// This makes sure that physics initialization occurs before other user actions.
	G4RunManager::Initialize();

	// Initialize user actions here to avoid ordering problems.
	if (!m_userActionsInitialized)
		initializeUserActions();

	// Initialize the event generation manager.
	EventSourceManager::instance();

	// Setup the default event source.
	EventSourceManager::instance()->setupEventSource();

	// Print list of registered modules.
#ifdef SLIC_LOG
	ModuleRegistry::instance()->print();
#endif
}

void RunManager::InitializePhysics() {
	// Initialize the physics list.
	PhysicsListManager::instance()->initializePhysicsList();

	// Call the G4RunManager's intitialization method.
	G4RunManager::InitializePhysics();

	// Check if the LCDD subsystem got some limits.
	LCDDProcessor* lcdd = LCDDProcessor::instance();
	PhysicsListManager* pmgr = PhysicsListManager::instance();
	if (lcdd->getLimitSetsBegin() != lcdd->getLimitSetsEnd()) {
		pmgr->enableLimits(true);
	}

	// Enable physics limits, if necessary.
	if (pmgr->enableLimits()) {
		pmgr->setupUserLimitsProcesses();
	}
}

void RunManager::InitializeGeometry() {
	if (!LCDDParser::instance()->isValidSetup()) {
		//G4Exception("", "", FatalException, "Current LCDD geometry setup is not valid.");
		SlicApplication::instance()->setReturnCode(SlicApplication::INVALID_GEOMETRY_SETUP);
		this->abortRun();
	} else {
		G4RunManager::InitializeGeometry();
	}
}

void RunManager::BeamOn(G4int n_event, const char* macroFile, G4int n_select) {

	// Reset abort run flag.
	m_abortRun = false;

	// Attempt to initialize Geant4 if not in idle state.
	if (G4StateManager::GetStateManager()->GetCurrentState() != G4State_Idle) {
		this->Initialize();
	}

	// Check if a generator was setup.
	if (EventSourceManager::instance()->getCurrentSource() == 0) {
		log() << LOG::fatal << "No event generator was setup." << LOG::done;
		abortRun(SlicApplication::GENERATOR_NOT_SETUP);
	}

	// Check if run should be aborted due to initialization errors.
	if (!this->isRunAborted()) {

		// Run number of events that were requested.
		m_numberOfEventsToRun = n_event;
		G4RunManager::BeamOn(n_event, macroFile, n_select);

		// Print number of events that were actually generated.
		EventSourceManager* eventManager = EventSourceManager::instance();
		eventManager->printNumEventsGenerated();

		// Check if event underflow occurred where number of events provided was less than requested.
		int nEventsGenerated = eventManager->getNumEventsGenerated();
		if (nEventsGenerated < m_numberOfEventsToRun) {
			log() << LOG::warning << LOG::head << "Generated " << nEventsGenerated << " events but " << n_event
					<< " were requested." << LOG::done;
			// Set event underflow error but do not override an existing return code.
			if (SlicApplication::instance()->getReturnCode() == SlicApplication::OKAY) {
				SlicApplication::instance()->setReturnCode(SlicApplication::EVENT_UNDERFLOW);
			}
		}
	} else {
		log() << LOG::warning << LOG::head << "Run was aborted before G4 event loop was executed." << LOG::done;
	}
}

int RunManager::getNumberOfEventsToRun() {
	return m_numberOfEventsToRun;
}

bool RunManager::isRunAborted() {
	return m_abortRun;
}
} // namespace slic
