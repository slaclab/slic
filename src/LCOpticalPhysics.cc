#include "LCOpticalPhysics.hh"

// Geant4
#include "G4Version.hh"

LCOpticalPhysics::LCOpticalPhysics(const G4String& name) :
		G4VPhysicsConstructor(name) {
}

LCOpticalPhysics::~LCOpticalPhysics() {
}

void LCOpticalPhysics::ConstructParticle() {
	// optical photon
	G4OpticalPhoton::OpticalPhotonDefinition();
}

void LCOpticalPhysics::ConstructProcess() {
	ConstructOp();
}

#ifndef aParticleIterator
#define aParticleIterator ((subInstanceManager.offset[g4vpcInstanceID])._aParticleIterator)
#endif

void LCOpticalPhysics::ConstructOp() {
	theCerenkovProcess = new G4Cerenkov("Cerenkov");
	theScintillationProcess = new G4Scintillation("Scintillation");
	theAbsorptionProcess = new G4OpAbsorption();
	theRayleighScatteringProcess = new G4OpRayleigh();
	theBoundaryProcess = new G4OpBoundaryProcess();

	theCerenkovProcess->SetMaxNumPhotonsPerStep(300);
	theScintillationProcess->SetScintillationYieldFactor(1.);

//if ( G4VERSION_NUMBER < 1000 )
//	G4ParticleTable::G4PTblDicIterator* aParticleIterator = G4ParticleTable::GetParticleTable()->GetIterator();
//#endif
	aParticleIterator->reset();

	while ((*aParticleIterator)()) {

		G4ParticleDefinition* particle = aParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();
		G4String particleName = particle->GetParticleName();

		if (theCerenkovProcess->IsApplicable(*particle)) {
			pmanager->AddProcess(theCerenkovProcess);
			pmanager->SetProcessOrderingToLast(theCerenkovProcess, idxPostStep);
		}
		if (theScintillationProcess->IsApplicable(*particle)) {
			pmanager->AddProcess(theScintillationProcess);
			pmanager->SetProcessOrderingToLast(theScintillationProcess, idxAtRest);
			pmanager->SetProcessOrderingToLast(theScintillationProcess, idxPostStep);
		}
		if (particleName == "opticalphoton") {
			pmanager->AddDiscreteProcess(theAbsorptionProcess);
			pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
			pmanager->AddDiscreteProcess(theBoundaryProcess);
		}
	}
}
