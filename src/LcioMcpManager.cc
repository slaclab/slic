// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioMcpManager.cc,v 1.75 2013-11-06 00:23:37 jeremy Exp $
#include "LcioMcpManager.hh"

// slic
#include "LcioMcpMessenger.hh"
#include "LcioManager.hh"
#include "LcioMcpFactory.hh"
#include "LcioMcpPrinter.hh"
#include "TrajectoryManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventSourceManager.hh"

// lcdd
#include "lcdd/util/StringUtil.hh"

// lcio
#include "IMPL/LCCollectionVec.h"

// geant4
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"

// std
#include <sstream>
#include <cmath>

using IMPL::MCParticleImpl;
using EVENT::LCCollection;
using EVENT::MCParticle;
using EVENT::LCIO;
using IMPL::LCCollectionVec;

namespace slic {

const double LcioMcpManager::m_NAN = std::sqrt((double) -1.0);
const double LcioMcpManager::DEFAULT_MIN_TRACKING_DISTANCE = 1.0;

LcioMcpManager::~LcioMcpManager() {
}

LcioMcpManager::LcioMcpManager() :
		Module("LcioMcpManager"), m_minimumTrackingDistance(DEFAULT_MIN_TRACKING_DISTANCE), m_enablePrintFinal(
				false), m_enablePrintInitial(false), m_writeInitialMCParticleCollection(false) {
	// create new Mcp messenger instance
	m_messenger = new LcioMcpMessenger();

	// setup mcp printer
	m_printer = new LcioMcpPrinter();

	/* Setup the MCParticle factory. */
	m_factory = new LcioMcpFactory(this);

	/* Create the MCParticle data maps object. */
	m_maps = new LcioMcpMaps(this);

	/* Check NAN != NAN */
	assert( m_NAN != m_NAN);
}

void LcioMcpManager::endEvent(const G4Event* event) {
#ifdef SLIC_LOG
	log() << LOG::debug << "LcioMcpManager::endEvent" << LOG::done;
#endif
	/*
	 // DEBUG - dump traj cont
	 G4TrajectoryContainer* trajectories = event->GetTrajectoryContainer();        
	 if ( trajectories ) 
	 {
	 G4int ntrajectories = trajectories->entries();

	 if (ntrajectories == 0)
	 log() << LOG::debug << "Trajectories container is empty for this event!" << LOG::done;
	 
	 Trajectory* trj = 0;
	 for ( int i=0; i < ntrajectories; i++ ) 
	 { 
	 trj = static_cast<Trajectory*> ( (*trajectories)[i] );
	 #ifdef SLIC_LOG
	 log() << LOG::debug << "trajectoryIndex: " << i << LOG::done;
	 log() << LOG::debug << "TID: " << trj->GetTrackID() << LOG::done;

	 //log() << "Trajectory (Name TID PID time): " << ((int)trj->GetTrackID()) << " " << ((int)trj->GetParentID()) << " " << ((std::string)trj->GetParticleName()) << " " << log::DONE;
	 log() << LOG::done;
	 #endif
	 }            
	 }
	 else
	 {
	 #ifdef SLIC_LOG
	 log() << LOG::debug << "No trajectories container for this event!" << LOG::done;
	 #endif
	 }
	 // END DEBUG
	 */

	/*
	 * Create the Mcp collection. Factory will
	 * retrieve current event from G4EventManager.
	 */
	m_factory->createFinalMcpCollection(event);

	if (m_enablePrintFinal) {
		printMcpCollection("final");
	}

#ifdef SLIC_DEBUG
	// print out Mcp processing maps
	m_maps->printMaps();
#endif
}

EVENT::LCCollection* LcioMcpManager::findParticleCollection(EVENT::LCEvent* event) {
	const std::vector<std::string>* collNames = event->getCollectionNames();

	EVENT::LCCollection* mcpColl = 0;
	for (std::vector<std::string>::const_iterator iter = collNames->begin(); iter != collNames->end();
			iter++) {
		EVENT::LCCollection* thisColl = event->getCollection(*iter);
		if (thisColl->getTypeName() == LCIO::MCPARTICLE) {
			mcpColl = thisColl;
			break;
		}
	}
	return mcpColl;
}

EVENT::LCCollection* LcioMcpManager::findParticleCollection(const std::string& collName) {
	return m_mcpColls[collName];
}

void LcioMcpManager::setInitialMcpCollection(EVENT::LCCollection* mcpColl) {
	if (mcpColl) {
		m_mcpColls["initial"] = mcpColl;
	} else {
		G4Exception("", "", FatalException, "Initial collection cannot be set to null.");
	}
}

EVENT::LCCollection* LcioMcpManager::createParticleCollection(const std::string& collName) {
#ifdef SLIC_LOG
	log() << LOG::debug << "creating mcp coll <" << collName << ">" << LOG::done;
#endif

	LCCollection* coll = 0;
	if (findParticleCollection(collName)) {
		G4Exception("", "", FatalException, "Attempting to create duplicate Mcp collection.");
	} else {
		coll = new LCCollectionVec(LCIO::MCPARTICLE);
		registerMcpCollection(collName, coll);
	}

	return coll;
}

void LcioMcpManager::registerMcpCollection(const std::string& collName, EVENT::LCCollection* mcpColl) {
	if (mcpColl->getTypeName() != LCIO::MCPARTICLE) {
		G4Exception("", "", FatalException, "The LCCollection does not contain MCParticle objects.");
	}

	if (m_mcpColls[collName]) {
		log() << LOG::warning << "WARNING: Mcp collection " + collName + " is already registered"
				<< LOG::done;
	} else {
		m_mcpColls[collName] = mcpColl;
	}
}

void LcioMcpManager::deleteInitialMcpCollection() {
	LCCollection* mcpVec = getInitialMcpCollection();

	if (0 != mcpVec) {
		delete mcpVec;
	}
}

void LcioMcpManager::reset() {

	// Initial MCParticle collection needs deletion if it wasn't added to the event.
	if (!writeInitialMCParticleCollection()) {
		deleteInitialMcpCollection();
	}

	// Clear the map of MCParticle collections.
	m_mcpColls.clear();

	// Clear the maps of MCParticle runtime data from this event.
	m_maps->clear();

	// NOTE: The final MCParticle collection is deleted by the LCEvent destructor.
}

// get MCP collection generated at EndOfEvent
EVENT::LCCollection* LcioMcpManager::getFinalMcpCollection() {
	if (!findParticleCollection("final")) {
		createParticleCollection("final");
	}

	return m_mcpColls["final"];
}

EVENT::LCCollection* LcioMcpManager::getInitialMcpCollection() {
	if (!findParticleCollection("initial")) {
		createParticleCollection("initial");
	}

	return m_mcpColls["initial"];
}

void LcioMcpManager::setMinimumTrackingDistance(double minDist) {
	m_minimumTrackingDistance = minDist;
}

double LcioMcpManager::getMinimumTrackingDistance() {
	return m_minimumTrackingDistance;
}

LcioMcpMaps* LcioMcpManager::getMaps() {
	return m_maps;
}

void LcioMcpManager::enablePrintFinal(bool p) {
	m_enablePrintFinal = p;
}

void LcioMcpManager::enablePrintInitial(bool p) {
	m_enablePrintInitial = p;
}

void LcioMcpManager::beginEvent(const G4Event*) {
	if (m_enablePrintInitial) {
		printMcpCollection("initial");
	}
}

void LcioMcpManager::printMcpCollection(const std::string& collName) {
	LCCollection* coll = findParticleCollection(collName);

	if (coll != 0) {
		printMcpCollection(collName, coll);
	}
#ifdef SLIC_LOG
	else {
		log() << LOG::warning << "LcioMcpManager::printMcpCollection - " + collName + " was not found" << LOG::done;
	}
#endif
}

void LcioMcpManager::printMcpCollection(const std::string& collName, EVENT::LCCollection* coll) {
	m_printer->printParticleCollection(collName, coll);
}

int LcioMcpManager::getParticleIndex(EVENT::LCCollection* collection, EVENT::MCParticle* particle) {
	if (collection->getTypeName() != LCIO::MCPARTICLE)
		return -1;

	for (int i = 0, n = collection->getNumberOfElements(); i < n; i++) {
		EVENT::MCParticle *nextParticle = (MCParticle*) collection->getElementAt(i);
		if (nextParticle == particle) {
			return i;
		}
	}
	return -1;
}
}
