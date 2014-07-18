//
////////////////////////////////////////////////////////////////
//                                                            //
//  Title:  Optical  physics for a Linear Collider Detector      //
//  Date:                                         //
//  Author:                                 //
//                                                            //
////////////////////////////////////////////////////////////////
//

#include "LCOpticalPhysics.hh"

// slic
#include "Geant4VersionInfo.hh"

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

void LCOpticalPhysics::ConstructOp() {
	theCerenkovProcess = new G4Cerenkov("Cerenkov");
	theScintillationProcess = new G4Scintillation("Scintillation");
	theAbsorptionProcess = new G4OpAbsorption();
	theRayleighScatteringProcess = new G4OpRayleigh();
	theBoundaryProcess = new G4OpBoundaryProcess();

//  theCerenkovProcess->DumpPhysicsTable();
//  theScintillationProcess->DumpPhysicsTable();
//  theAbsorptionProcess->DumpPhysicsTable();
//  theRayleighScatteringProcess->DumpPhysicsTable();

//#ifdef debug  
//  G4cout << "VERBOSE = " << theCerenkovProcess->GetVerboseLevel() << G4endl;
//  G4cout << "Inside LCOpticalPhysics::ConstructOp()" << G4endl;
	//#endif

	theCerenkovProcess->SetMaxNumPhotonsPerStep(300);

	theScintillationProcess->SetScintillationYieldFactor(1.);
	//    theScintillationProcess->SetTrackSecondariesFirst(true);

	//G4OpticalSurfaceModel themodel = unified;
	//theBoundaryProcess->SetModel(themodel);
#if ( G4VERSION_NUMBER < 960 )
	G4ParticleTable::G4PTblDicIterator* aParticleIterator = G4ParticleTable::GetParticleTable()->GetIterator();
#endif
	aParticleIterator->reset();

	while ((*aParticleIterator)()) {

		G4ParticleDefinition* particle = aParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();
		G4String particleName = particle->GetParticleName();

		if (theCerenkovProcess->IsApplicable(*particle)) {
			//if (Geant4VersionInfo::getMajorVersion() >= 9 && Geant4VersionInfo::getMinorVersion() >= 1) {
				// Geant4 9.1 changes Cerenkov to be a post-step process.
			pmanager->AddProcess(theCerenkovProcess);
			pmanager->SetProcessOrderingToLast(theCerenkovProcess, idxPostStep);
			//} else {
			//	pmanager->AddContinuousProcess(theCerenkovProcess);
			//}
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
