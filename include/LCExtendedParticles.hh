#ifndef SLIC_LCEXTENDEDPARTICLES_HH
#define SLIC_LCEXTENDEDPARTICLES_HH

// slic
#include "LCExtendedDecay.hh"

// geant4
#include "G4VPhysicsConstructor.hh"
#include "G4Decay.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"

namespace slic {

/**
 * @class LCExtendedParticles
 * @brief This class models the extended particle set from HepPDT.
 */
class LCExtendedParticles: public G4VPhysicsConstructor {
public:

    /**
     * Class constructor.
     * @param[in] name The name of the physics to be constructed.
     */
    LCExtendedParticles(const G4String& name = "LCExtendedParticles");

    /**
     * Class destructor.
     */
    virtual ~LCExtendedParticles();

    /**
     * Construct the particles.
     */
    void ConstructParticle();

    /**
     * Construct the processes.
     */
    void ConstructProcess();

private:

    G4Decay m_decay;
    G4hIonisation m_ionise;
    G4hMultipleScattering m_scatter;
};
}

#endif
