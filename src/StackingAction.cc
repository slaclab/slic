#include "StackingAction.hh"

// Geant4
#include "G4Track.hh"
#include "G4DecayProducts.hh"
#include "G4ios.hh"

namespace slic {

StackingAction::StackingAction() {
}

StackingAction::~StackingAction() {
}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* aTrack) {
    // FIXME: Need to determine how these are supposed to work together!
    G4ClassificationOfNewTrack classification = m_pluginManager->stackingClassifyNewTrack(aTrack);
    if(aTrack->GetTrackStatus() == fSuspend) classification = fWaiting;
}

void StackingAction::NewStage() {
    m_pluginManager->stackingNewStage();
}

void StackingAction::PrepareNewEvent() {
    m_pluginManager->stackingPrepareNewEvent();
}

}
