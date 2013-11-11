// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioPrimaryGenerator.cc,v 1.42 2013-11-06 00:23:37 jeremy Exp $

// SLIC
#include "LogStream.hh"
#include "LcioPrimaryGenerator.hh"
#include "LcioManager.hh"
#include "LcioMcpManager.hh"
#include "LcioMcpUtil.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// Geant4
#include "G4ParticleTable.hh"

// STL
#include <sstream>

using IMPL::MCParticleImpl;
using EVENT::LCCollection;
using EVENT::LCIO;

namespace slic {

LcioPrimaryGenerator::LcioPrimaryGenerator(LcioManager* mgr) :
		Module("LcioPrimaryGenerator") {
	m_mgr = mgr;

	m_mcpManager = LcioMcpManager::instance();
}

LcioPrimaryGenerator::~LcioPrimaryGenerator() {
}

void LcioPrimaryGenerator::generatePrimaryVertexFromMcpCollection(LCCollection* mcpVec, G4Event* anEvent) {

#ifdef SLIC_LOG
	log() << LOG::debug << "********** Generating Event from LCIO MCParticles **********" << LOG::endl << LOG::done;
#endif

	assert( mcpVec);
	assert( mcpVec->getTypeName() == LCIO::MCPARTICLE);

	G4int nhep = mcpVec->getNumberOfElements();

	if (nhep < 1) {
		return;
	}

//#ifdef SLIC_LOG
//        log() << LOG::debug << "nInitialMCParticles: " << nhep << LOG::done;
//#endif

	for (int i = 0; i < nhep; i++) {

		MCParticleImpl* mcp = dynamic_cast<MCParticleImpl*>(mcpVec->getElementAt(i));

		//assert( mcp );
		if (mcp == 0)
			G4Exception("", "", FatalException, "Could not find MCParticle at indx.");

		// debug info
		G4ParticleTable* tbl = G4ParticleTable::GetParticleTable();
		G4ParticleDefinition* pdef = tbl->FindParticle(mcp->getPDG());

		G4String pname = "unknown";
		if (pdef != 0)
			pname = pdef->GetParticleName();
		// end debug info

#ifdef SLIC_LOG
		log() << LOG::debug << "generating particle #" << i << ", pdg: " << mcp->getPDG() << ", name: " << pname << LOG::done;
		log() << LOG::debug << "vertex: ( " << mcp->getVertex()[0] << ", " << mcp->getVertex()[1] << ", " << mcp->getVertex()[2] << " )" << LOG::done;
		if (mcp->getEndpoint() != 0)
		log() << LOG::debug << "endp: ( " << mcp->getEndpoint()[0] << ", " << mcp->getEndpoint()[1] << ", " << mcp->getEndpoint()[2] << " )" << LOG::done;
		log() << LOG::debug << "time: " << mcp->getTime() << LOG::done;
#endif

		G4int numPar = mcp->getParents().size();

#ifdef SLIC_LOG
		log() << LOG::debug << "nparents: " << numPar << LOG::done;
#endif

		/*
		 * Handle 4 cases, following Ron Cassell's LCSHEPEvtInterface from LCS package:
		 * case 1: predecay
		 * case 2: not predecay; "final state"
		 * case 3: not predecay; "intermediate" or "documentation" TO BE TRACKED
		 * case 4: not predecay; "intermediate" or "documentation" *NOT* TRACKED
		 */
		// loop vars
		MCParticleImpl* parMcp = 0;
		G4PrimaryParticle* g4parent = 0;

		G4PrimaryParticle* thePrimary = 0;
		G4PrimaryVertex* theVertex = 0;

		G4bool isPreDecay = false;
		G4bool createPrimary = false;
		G4bool createVertex = false;

		// check if case 1 (PreDecay)
		if (numPar > 0) {
			parMcp = dynamic_cast<MCParticleImpl*>(mcp->getParents()[0]);
			g4parent = m_mcpManager->getMaps()->findPrimary(parMcp);

			// Check if the particle has parents, in which case it needs a predecay.
			if (g4parent != 0) {

				isPreDecay = true;
#ifdef SLIC_LOG
				log() << "PREDECAY" << LOG::done;
				log() << LOG::debug << "parent idx: " << LcioMcpManager::instance()->getParticleIndex(mcpVec, parMcp) << LOG::done;
#endif
			}
		}

		// case 1
		if (isPreDecay) {
			createPrimary = true;
			// no vertex
		} else {
			// case 2: final state
			if (mcp->getGeneratorStatus() == 1) {

#ifdef SLIC_LOG
				log() << LOG::debug << "FINAL STATE" << LOG::done;
#endif

				createPrimary = true;
				createVertex = true;
			} else {
				G4double dist = 0;

				// check case 3, if traveled > minDist
				if (mcp->getDaughters().size() > 0) {

					IMPL::MCParticleImpl* firstDau =
							dynamic_cast<IMPL::MCParticleImpl*>(mcp->getDaughters()[0]);

					dist = LcioMcpUtil::computeParticleDistance(mcp, firstDau);

#ifdef SLIC_LOG
					log() << LOG::debug << "mcpDistance=" << dist << LOG::done;
					log() << LOG::debug << "minTrackingDist=" << m_mcpManager->getMinimumTrackingDistance() << LOG::done;
#endif

					// case 3
					if (dist > m_mcpManager->getMinimumTrackingDistance()) {

#ifdef SLIC_LOG
						log() << LOG::debug << "INTERMEDIATE OR DOC TO BE TRACKED" << LOG::done;
#endif

						createPrimary = true;
						createVertex = true;
					}
					// case 4 = no-op
					// *This particle will not be tracked.*
#ifdef SLIC_LOG
					else {
						log() << LOG::debug << "INTERMEDIATE OR DOC THAT WILL NOT BE TRACKED" << LOG::done;
					}
#endif
				}

			}
		}

#ifdef SLIC_LOG
		log() << "isPreDecay: " << isPreDecay << LOG::done;
		log() << "createPrimary: " << createPrimary << LOG::done;
		log() << "createVertex: " << createVertex << LOG::done;
#endif

		// create a primary
		if (createPrimary) {

			thePrimary = createPrimaryParticle(mcp);

			// set daughters for PreDecay
			if (isPreDecay) {

				assert( g4parent);

				//
				// Computation of proper_time from RC
				//

				G4ThreeVector parMom = g4parent->GetMomentum();

#ifdef SLIC_LOG
				log() << "parent p: " << parMom << LOG::done;
#endif

				G4double E = sqrt(
						pow(g4parent->GetMass(), 2) + pow(parMom.x(), 2) + pow(parMom.y(), 2)
								+ pow(parMom.z(), 2));

#ifdef SLIC_LOG
				log() << "particle time: " << mcp->getTime() << LOG::done;
				log() << "parent time: " << parMcp->getTime() << LOG::done;
				log() << "parent mass: " << g4parent->GetMass() << LOG::done;
#endif

				G4double proper_time = ((mcp->getTime() - parMcp->getTime()) * g4parent->GetMass()) / E;

#ifdef SLIC_LOG
				log() << LOG::debug << "parent E: " << E << LOG::done;
				log() << LOG::debug << "proper time: " << proper_time << LOG::done;
#endif

				g4parent->SetDaughter(thePrimary);
				g4parent->SetProperTime(proper_time);

#ifdef SLIC_LOG
				log() << LOG::debug << "mcp decay time: " << mcp->getTime() - parMcp->getTime() << LOG::done;
#endif
			}
		}

		// create a vertex, add primary and set in event
		if (createVertex) {
			theVertex = createPrimaryVertex(mcp);
			theVertex->SetPrimary(thePrimary);
			anEvent->AddPrimaryVertex(theVertex);
		}

		// insert mcp, primary pair into LcioManager's map (could be null)
		if (thePrimary) {
			m_mcpManager->getMaps()->addParticleToPrimary(mcp, thePrimary);
		}

#ifdef SLIC_LOG
		log() << LOG::debug << LOG::endl;
#endif
	}
}

G4PrimaryParticle* LcioPrimaryGenerator::createPrimaryParticle(IMPL::MCParticleImpl* mcp) {
	G4PrimaryParticle* primary = new G4PrimaryParticle(mcp->getPDG(), mcp->getMomentum()[0] * GeV,
			mcp->getMomentum()[1] * GeV, mcp->getMomentum()[2] * GeV);
	primary->SetMass(mcp->getMass() * GeV);

	return primary;
}

G4PrimaryVertex* LcioPrimaryGenerator::createPrimaryVertex(IMPL::MCParticleImpl* mcp) {
#ifdef SLIC_LOG
	log() << "createPrimaryVertexFromMcp" << LOG::done;
	log() << "particle time: " << mcp->getTime() << LOG::done;
#endif

	G4ThreeVector pos = G4ThreeVector(mcp->getVertex()[0], mcp->getVertex()[1], mcp->getVertex()[2]);

	return new G4PrimaryVertex(pos, mcp->getTime());

}
}
