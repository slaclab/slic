#include "PrimaryGeneratorAction.hh"

// SLIC
#include "MCParticleManager.hh"
#include "TrackManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

namespace slic {

PrimaryGeneratorAction::PrimaryGeneratorAction() :
  G4VUserPrimaryGeneratorAction() { 
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {

    /* Reset the TrackManager's state from previous event. */
    TrackManager::instance()->reset();

    /* Reset the MCParticleManager's state from previous event. */
    MCParticleManager::instance()->reset();

    /* Check if run needs to be aborted before generating events. */
    if (RunManager::instance()->isRunAborted()) {
        G4RunManager::GetRunManager()->AbortRun();
    } else {
        /* Begin event hook of EventSourceManager which will read the next input event if applicable. */
        //_manager->beginEvent(anEvent);

        /* Generate primary vertices using the event generator's (previously read) current event. */
        //_manager->generateNextEvent(anEvent);
    }

    /* If the event source hit the end of file, then abort the run. */
    //if (_manager->isEOF()) {
    //    SlicApplication::instance()->setAborting(true);
    //}

    //m_pluginManager->generatePrimary(anEvent);
}
}
