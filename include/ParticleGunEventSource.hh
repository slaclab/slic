// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/include/ParticleGunEventSource.hh,v 1.9 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_PARTICLEGUNEVENTSOURCE_HH
#define SLIC_PARTICLEGUNEVENTSOURCE_HH 1

// slic
#include "EventSource.hh"

// geant4
#include "G4ParticleGun.hh"

namespace slic {

/**
 * @class ParticleGunEventSource
 * @brief Wrapper for the Geant4 particle gun.
 */
class ParticleGunEventSource: public EventSource {

public:

    /**
     * Class constructor.
     */
    ParticleGunEventSource();

    /**
     * Class destructor.
     */
    virtual ~ParticleGunEventSource();

public:

    /**
     * Generate an event.
     * @param[in] event The target G4Event to fill.
     */
    virtual void generate(G4Event* event);

    /**
     * Dump information on the current event to the log.
     */
    void printCurrentEvent();

    /**
     * Get the associated G4ParticleGun.
     * @return The G4ParticleGun associated with this source.
     */
    G4ParticleGun* getParticleGun();

private:
    G4ParticleGun* m_generator;
};
}

#endif
