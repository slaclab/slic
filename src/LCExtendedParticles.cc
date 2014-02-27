#include "LCExtendedParticles.hh"

// slic
#include "HepPDTManager.hh"

// geant4
#include "G4ProcessManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTableIterator.hh"

namespace slic {

LCExtendedParticles::LCExtendedParticles(const G4String& name) :
		G4VPhysicsConstructor(name) {
}

LCExtendedParticles::~LCExtendedParticles() {
}

void LCExtendedParticles::ConstructParticle() {

    //std::cout << "LCExtendedParticles::ConstructParticle" << std::endl;

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

		    std::cout << "constructing new particle: " << pdata.PDTname() << std::endl;

			G4String particleType = "extended";
			if (id.isSUSY())
			    particleType = "susy";

			std::cout << "  type: " << particleType << std::endl;

			// Create a G4ParticleDefinition for this SUSY particle.
			new G4ParticleDefinition(
			        pdata.PDTname(), // name
					pdata.mass().value() * GeV, // mass
					pdata.totalWidth().value(), // width
					pdata.charge(), // charge
					0, // 2*spin
					0, // parity
					0, // C-conjugation
					0, // 2*isospin
					0, // 2*isospin3
					0, // G-parity
					particleType, // type
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

    G4ParticleTable::G4PTblDicIterator* particleIterator = G4ParticleTable::GetParticleTable()->GetIterator();
    particleIterator->reset();

	while ((*particleIterator)()) {
		G4ParticleDefinition* pdef = particleIterator->value();
		G4ProcessManager* pmgr = pdef->GetProcessManager();
		if (pdef->GetParticleType() == "extended" || pdef->GetParticleType() == "susy") {

		    std::cout << "assigning processes to particle: " << pdef->GetParticleName() << std::endl;

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
