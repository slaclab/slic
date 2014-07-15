// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/PrimaryGeneratorAction.cc,v 1.38 2013-06-26 01:57:02 jeremy Exp $

// SLIC
#include "EventSourceManager.hh"
#include "LcioManager.hh"
#include "MCParticleManager.hh"
#include "PrimaryGeneratorAction.hh"
#include "TrackManager.hh"
#include "RunManager.hh"
#include "SlicApplication.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// LCIO
#include "IMPL/MCParticleImpl.h"
#include "EVENT/LCCollection.h"

namespace slic {

PrimaryGeneratorAction::PrimaryGeneratorAction() :
		Module("PrimaryGeneratorAction", false) {
    _manager = EventSourceManager::instance();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {

    /* Print begin of event message. */
    printBeginEventMessage(anEvent);

    /* Reset the TrackManager's state from previous event. */
    TrackManager::instance()->reset();

    /* Reset the MCParticleManager's state from previous event. */
    MCParticleManager::instance()->reset();

    /* Check if run needs to be aborted before generating events. */
    if (RunManager::instance()->isRunAborted()) {
        G4RunManager::GetRunManager()->AbortRun();
        log() << LOG::warning << LOG::name << LOG::sep << "Run was aborted.  Events will not be generated." << LOG::done;
    } else {
        /* Begin event hook of EventSourceManager which will read the next input event if applicable. */
        _manager->beginEvent(anEvent);

        /* Generate primary vertices using the event generator's (previously read) current event. */
        _manager->generateNextEvent(anEvent);
    }

	/* If the event source hit the end of file, then abort the run. */
	if (_manager->isEOF()) {
		SlicApplication::instance()->setAborting(true);
	}
}

void PrimaryGeneratorAction::printBeginEventMessage(G4Event* anEvent) {
	log() << LOG::okay << ">>>> BeginEvent <" << StringUtil::toString(anEvent->GetEventID()) << ">" << LOG::done;
}
}
