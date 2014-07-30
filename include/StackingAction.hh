#ifndef SLIC_STACKINGACTION_HH_
#define SLIC_STACKINGACTION_HH_ 1

#include "globals.hh"
#include "G4UserStackingAction.hh"

/**
 * @class StackingAction
 *
 * @brief Implemention of Geant4's G4UserStackingAction
 *
 * @note
 * Implementation of G4UserStackingAction for SLIC.
 * This is copied from Mokka and right now it just
 * suspends backscattering tracks until the end
 * of shower development.
 */
class StackingAction : public G4UserStackingAction {

public:
    StackingAction();
    virtual ~StackingAction();

public:

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
};

#endif

