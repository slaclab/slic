// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioMcpFilter.cc,v 1.7 2013-11-06 00:23:37 jeremy Exp $
#include "LcioMcpFilter.hh"

// slic
#include "LcioMcpPrinter.hh"

// lcio
#include "IMPL/LCCollectionVec.h"
#include "EVENT/LCIO.h"

// stl
#include <cassert>

using EVENT::LCIO;

namespace slic {

LcioMcpFilter::LcioMcpFilter() :
		Module("LcioMcpFilter") {
	;
}

LcioMcpFilter::~LcioMcpFilter() {
	;
}

EVENT::LCCollection* LcioMcpFilter::filterSimParticles(EVENT::LCCollection* mcpColl) {
	assert(mcpColl);

	// new coll w/ gen parts only
	EVENT::LCCollection* genMcpColl = new IMPL::LCCollectionVec(LCIO::MCPARTICLE);

	// iterate over input collection
	int mcpSize = mcpColl->getNumberOfElements();
	for (int i = 0; i < mcpSize; i++) {

#ifdef SLIC_DEBUG      
		log() << LOG::debug << "handling mcp <" << i << ">" << LOG::done;
#endif

		// next input Mcp
		EVENT::MCParticle* mcp = static_cast<EVENT::MCParticle*>(mcpColl->getElementAt(i));

		// primary e.g. no parents? 
		if (0 == mcp->getParents().size()) {

#ifdef SLIC_LOG
			log() << LOG::debug << "handling primary Mcp" << LOG::done;
#endif

			// make new Mcp (function checks if gen part)
			makeParticle(mcp, genMcpColl);

			// add to coll
			//if ( simMcp ) {
			//  genMcpColl->addElement( simMcp );
			//}
		}
	}

#ifdef SLIC_LOG
	log() << LOG::debug << "input coll size <" << mcpSize << ">" << LOG::done;
	log() << LOG::debug << "gen coll size <" << genMcpColl->getNumberOfElements() << ">" << LOG::done;
	log() << LOG::debug << "generator mcp coll..." << LOG::done;
#endif

	//LcioMcpPrinter* prn = new LcioMcpPrinter();
	//prn->printMcpCollection( genMcpColl );
	//delete prn;

	m_mcpList.clear();

	return genMcpColl;
}

IMPL::MCParticleImpl* LcioMcpFilter::makeParticle(EVENT::MCParticle* mcp, EVENT::LCCollection* mcpColl) {
	IMPL::MCParticleImpl* mcpGen = 0;

	// has non-0 gen status and was not added?
	if (mcp->getGeneratorStatus() && !wasAdded(mcp)) {

		//log() << LOG::debug << "gen part" << LOG::done;

		// new Mcp part
		mcpGen = new IMPL::MCParticleImpl();

		// PDG
		mcpGen->setPDG(mcp->getPDG());

		// gen stat
		mcpGen->setGeneratorStatus(mcp->getGeneratorStatus());

		// mass
		mcpGen->setMass(mcp->getMass());

		// sim stat
		mcpGen->setSimulatorStatus(mcp->getSimulatorStatus());

		// momentum
		float p[3] = { mcp->getMomentum()[0], mcp->getMomentum()[1], mcp->getMomentum()[2] };
		mcpGen->setMomentum(p);

		// vtx
		double vtx[3] = { mcp->getVertex()[0], mcp->getVertex()[1], mcp->getVertex()[2] };
		mcpGen->setVertex(vtx);

		// endp
		double endp[3] = { mcp->getEndpoint()[0], mcp->getEndpoint()[1], mcp->getEndpoint()[2] };
		mcpGen->setEndpoint(endp);

		// time
		mcpGen->setTime(mcp->getTime());

		// charge
		mcpGen->setCharge(mcp->getCharge());

		// add it
		mcpColl->addElement(mcpGen);
		m_mcpList.push_back(mcp);

		// add sim daughters
		addSimDaughters(mcp, mcpGen, mcpColl);
	}
	//else {
	//  log() << LOG::debug << "skipping sim part and its daughters" << LOG::done;
	//}

	return mcpGen;
}

void LcioMcpFilter::addSimDaughters(EVENT::MCParticle* mcp, EVENT::MCParticle* parMcp,
		EVENT::LCCollection* mcpColl) {
	int nmcp = mcp->getNumberOfDaughters();

	//log() << LOG::debug << "handling <" << nmcp << "> dau parts" << LOG::done;

	// has at least 1 dau?
	if (nmcp) {

		// iterate over dau parts
		for (int i = 0; i < nmcp; i++) {
			EVENT::MCParticle* dauMcp = mcp->getDaughter(i);

			// dau != null? (safety check)
			if (dauMcp) {

				// make a new dau (could be null )
				IMPL::MCParticleImpl* newDauMcp = makeParticle(dauMcp, mcpColl);

				// made a new dau?
				if (newDauMcp) {
					//log() << LOG::debug << "add gen dau" << LOG::done;

					// set the dau's par
					newDauMcp->addParent(parMcp);
				}
			}
		}
	}
}

bool LcioMcpFilter::wasAdded(EVENT::MCParticle* mcp) {
	bool f = false;
	for (std::vector<EVENT::MCParticle*>::const_iterator iter = m_mcpList.begin(); iter != m_mcpList.end();
			iter++) {
		if ((*iter) == mcp) {
			f = true;
			break;
		}
	}
	return f;
}
}
