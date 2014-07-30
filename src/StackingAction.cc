#include "StackingAction.hh"

// Geant4
#include "G4Track.hh"
#include "G4DecayProducts.hh"
#include "G4ios.hh"

StackingAction::StackingAction() {
}

StackingAction::~StackingAction() {
}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* aTrack) {
    G4ClassificationOfNewTrack classification = fUrgent;
    if(aTrack->GetTrackStatus()==fSuspend) classification = fWaiting;
    return classification;
}

