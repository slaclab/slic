#ifndef SLIC_LCSUSYPHYSICS_H 
#define SLIC_LCSUSYPHYSICS_H 1

// slic
#include "Module.hh"
#include "LCSUSYDecay.hh"

// geant4
#include "G4VPhysicsConstructor.hh"
#include "G4ProcessManager.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"

namespace slic {

/**
 * @class LCSUSYPhysics
 * @brief Physics processors and particles for SUSY.
 */
class LCSUSYPhysics: public G4VPhysicsConstructor {

public:

    /**
     * Class constructor.
     * @param[in] name The name of the G4VPhysicsConstructor.
     */
	LCSUSYPhysics(const G4String& name = "LCSUSYPhysics");

	/**
	 * Class destructor.
	 */
	virtual ~LCSUSYPhysics();

	/**
	 * This is called from Construct() to create and register the SUSY particles.
	 */
	void ConstructParticle();

	/**
	 * This is called from Construct() to create and register the physics processes.
	 */
	void ConstructProcess();

private:

	G4hMultipleScattering m_scatter;
	LCSUSYDecay m_decay;
	G4hIonisation m_ionise;
};
}

#endif
