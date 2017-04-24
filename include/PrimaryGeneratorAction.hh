#ifndef SLIC_PRIMARYGENERATORACTION_HH_
#define SLIC_PRIMARYGENERATORACTION_HH_ 1

// SLIC
#include "EventSourceManager.hh"
#include "Module.hh"
#include "PluginManagerAccessor.hh"

// Geant4
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleGun.hh"
#include "G4RunManager.hh"

namespace slic {

/**
 * @class PrimaryGeneratorAction
 * @brief Implementation of G4VUserPrimaryGeneratorAction.
 */
class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction, public Module, public PluginManagerAccessor {

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

private:

    /**
     * Begin the beginning of event message.
     */
    void printBeginEventMessage(G4Event* anEvent);

private:

    EventSourceManager* _manager;
};
}

#endif
