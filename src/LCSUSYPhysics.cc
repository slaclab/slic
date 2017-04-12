#include "LCSUSYPhysics.hh"

// SLIC
#include "HepPDTManager.hh"
#include "LCSUSYDecay.hh"

// Geant4
#include "G4Version.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnknownDecay.hh"
#include "G4SystemOfUnits.hh"

namespace slic {
LCSUSYPhysics::LCSUSYPhysics(const G4String& name) :
		G4VPhysicsConstructor(name) {
}

LCSUSYPhysics::~LCSUSYPhysics() {
}

#ifndef aParticleIterator
#define aParticleIterator ((subInstanceManager.offset[g4vpcInstanceID])._aParticleIterator)
#endif

void LCSUSYPhysics::ConstructParticle() {

    // Get the particle data table to be imported.
	HepPDT::ParticleDataTable* pdtbl = HepPDTManager::instance()->getParticleDataTable();

	// Loop over all particles.
	for (HepPDT::ParticleDataTable::const_iterator it = pdtbl->begin(); it != pdtbl->end(); it++) {
		HepPDT::ParticleID id = it->first;
		HepPDT::ParticleData pdata = it->second;

		// Only adding SUSY particles.
		if (id.isSUSY()) {
			// Create a G4ParticleDefinition for this SUSY particle.
			new G4ParticleDefinition(pdata.PDTname(), // name 
					pdata.mass().value() * GeV, // mass
					pdata.totalWidth().value(), // width
					pdata.charge(), // charge
					0, // 2*spin
					0, // parity
					0, // C-conjugation
					0, // 2*isospin
					0, // 2*isospin3
					0, // G-parity
					"susy", // type
					0, // lepton number
					0, // baryon number
					pdata.pid(), // PDG encoding
					false, // stable 
					1e30, // lifetime (DEBUG)
					0, // decay table
					false // short lived
					);
		}
	}
}

void LCSUSYPhysics::ConstructProcess() {
	aParticleIterator->reset();
	while ((*aParticleIterator)()) {
		G4ParticleDefinition* pdef = aParticleIterator->value();
		G4ProcessManager* pmgr = pdef->GetProcessManager();
		if (m_decay.IsApplicable(*pdef)) {
			if (pdef->GetPDGCharge() != 0) {
				pmgr->AddProcess(&m_scatter, -1, 1, 1); // multiple scattering
				pmgr->AddProcess(&m_ionise, -1, 2, 2); // ionisation
				pmgr->AddProcess(&m_decay, -1, -1, 3); // decay
			} else {
				pmgr->AddProcess(&m_scatter, -1, 1, 1); // multiple scattering
				pmgr->AddProcess(&m_decay, -1, -1, 2); // decay
			}
		}
	}
}
}
