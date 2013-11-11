// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/PrimaryGeneratorAction.hh,v 1.25 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_PRIMARYGENERATORACTION_HH
#define SLIC_PRIMARYGENERATORACTION_HH 1

// slic
#include "Module.hh"

// G4
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleGun.hh"
#include "G4RunManager.hh"

namespace slic {

/**
 * @class PrimaryGeneratorAction
 * @brief Implementation of G4VUserPrimaryGeneratorAction.
 * @note  Delegates all control to EventSourceManager.
 */
class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction, public Module {

public:

    /**
     * Class constructor.
     */
    PrimaryGeneratorAction();

    /**
     * Class destructor.
     */
    virtual ~PrimaryGeneratorAction();

public:

    /**
     * Generate G4PrimaryParticle objects into the G4Event.
     * @param[in] anEvent The target G4Event.
     */
    virtual void GeneratePrimaries(G4Event *anEvent);

    /**
     * Apply Lorentz transformation to event.  The lorentz transformation
     * is retrieved from the singleton EventSourceManager.
     * @param[in] anEvent The current Geant4 event.
     */
    void applyLorentzTransformation(G4Event *anEvent);

    /**
     * Apply Z smearing to event.  The Z spread parameter is retrieved
     * from the singleton EventSourceManager.
     * @param[in] anEvent The current Geant4 event.
     */
    void zsmear(G4Event* anEvent);

private:

    /**
     * Begin the beginning of event message.
     */
    void printBeginEventMessage(G4Event* anEvent);
};
}

#endif
