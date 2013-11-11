//
////////////////////////////////////////////////////////////////
//                                                            //
//  Title:  Optical physics for a Linear Collider Detector      //
//  Date:                                         //
//  Author:                                 //
//                                                            //
////////////////////////////////////////////////////////////////
//

#ifndef LCOpticalPhysics_hh
#define LCOpticalPhysics_hh 1

// Geant4
#include "G4VPhysicsConstructor.hh"
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessManager.hh"

/**
 * @class LCOpticalPhysics
 * @brief Optical physics constructor.
 */
class LCOpticalPhysics: public G4VPhysicsConstructor {

public:

    /**
     * The class constructor.
     * @param[in] name The name of the G4VPhysicsConstructor.
     */
	LCOpticalPhysics(const G4String& name = "optical");

	/**
	 * Class destructor.
	 */
	virtual ~LCOpticalPhysics();

	/**
	 * This method will be invoked in the Construct() method where
	 * each particle type will be instantiated.
	 */
	void ConstructParticle();

	/**
	 * This method will be invoked in the Construct() method.
	 * Each physics process will be instantiated and registered
	 * to the process manager of each particle type
	 */
	void ConstructProcess();

	/**
	 * This method constructs physics processes and registers them.
	 */
	void ConstructOp();

protected:
	G4Cerenkov* theCerenkovProcess;
	G4Scintillation* theScintillationProcess;
	G4OpAbsorption* theAbsorptionProcess;
	G4OpRayleigh* theRayleighScatteringProcess;
	G4OpBoundaryProcess* theBoundaryProcess;
};

#endif

