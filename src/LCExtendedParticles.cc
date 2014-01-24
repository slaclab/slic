#include "LCExtendedParticles.hh"

// slic
#include "HepPDTManager.hh"

// geant4
#include "G4ProcessManager.hh"
#include "G4SystemOfUnits.hh"

namespace slic {

LCExtendedParticles::LCExtendedParticles(const G4String& name) :
		G4VPhysicsConstructor(name) {
}

LCExtendedParticles::~LCExtendedParticles() {
}

void LCExtendedParticles::ConstructParticle() {
	// Get the particle data table to be imported.
	HepPDT::ParticleDataTable* pdtbl = HepPDTManager::instance()->getParticleDataTable();

	// Loop over all particles.
	for (HepPDT::ParticleDataTable::const_iterator it = pdtbl->begin(); it != pdtbl->end(); it++) {
		HepPDT::ParticleID id = it->first;
		HepPDT::ParticleData pdata = it->second;

		// Check for a built-in definition of this particle.
		G4ParticleDefinition* pdef = G4ParticleTable::GetParticleTable()->FindParticle(pdata.pid());

		// Add particles that Geant4 does not already know about.  
		if (pdef == 0) {
			G4String name = "extended";
			if (id.isSUSY())
				name = "susy";

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
					"extended", // type
					0, // lepton number
					0, // baryon number
					pdata.pid(), // PDG encoding
					false, // stable 
					DBL_MIN, // lifetime (can be overridden by predefined decay time)
					0, // decay table
					false // short lived
					);
		}
	}
}

void LCExtendedParticles::ConstructProcess() {
	aParticleIterator->reset();

	while ((*aParticleIterator)()) {
		G4ParticleDefinition* pdef = aParticleIterator->value();
		G4ProcessManager* pmgr = pdef->GetProcessManager();
		if (pdef->GetParticleType() == "extended" || pdef->GetParticleType() == "susy") {
			if (pdef->GetPDGCharge() != 0) {
				pmgr->AddProcess(&m_scatter, -1, 1, 1); // multiple scattering
				pmgr->AddProcess(&m_ionise, -1, 2, 2); // ionisation
				pmgr->AddProcess(&m_decay, -1, -1, 2); // decay 
			} else {
				pmgr->AddProcess(&m_scatter, -1, 1, 1); // multiple scattering
				pmgr->AddProcess(&m_decay, -1, -1, 2); // decay 
			}
		}
	}
}
}
