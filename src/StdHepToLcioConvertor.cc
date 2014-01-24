// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/StdHepToLcioConvertor.cc,v 1.44 2013-11-06 00:23:37 jeremy Exp $
#include "StdHepToLcioConvertor.hh"

// slic
#include "LcioMcpManager.hh"
#include "HepPDTManager.hh"

// lcio
#include "IMPL/LCEventImpl.h"
#include "UTIL/LCTOOLS.h"

// geant4
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4PhysicalConstants.hh"

// stl
#include <cassert>
#include <cmath>

// slic
#include "LcioMcpManager.hh"

using EVENT::LCIO;
using EVENT::LCCollection;
using IMPL::LCEventImpl;
using IMPL::MCParticleImpl;
using UTIL::LCTOOLS;

namespace slic {
StdHepToLcioConvertor::StdHepToLcioConvertor(lStdHep* rdr) :
		Module("StdHepToLcioConverter"), m_reader(rdr) {
	assert(rdr);
}

StdHepToLcioConvertor::StdHepToLcioConvertor() :
		Module("StdHepToLcioConverter"), m_reader(0) {
}

StdHepToLcioConvertor::~StdHepToLcioConvertor() {
}

void StdHepToLcioConvertor::setParticleCollection(EVENT::LCCollection* mcpColl) {
	m_currentMcpColl = mcpColl;
}

EVENT::LCCollection* StdHepToLcioConvertor::getParticleCollection() {
	return m_currentMcpColl;
}

void StdHepToLcioConvertor::fillParticleCollection() {
	// Reset the MCParticle list.
	//m_currentMcpColl->clear();
	//G4cout << "m_currentMcpColl.size =" << m_currentMcpColl->getNumberOfElements() << G4endl;

#ifdef SLIC_LOG
	log() << LOG::debug << "********** Creating LCIO MCParticle Collection from StdHep **********" << LOG::done;
#endif

	//assert( m_reader );
	//assert( m_currentMcpColl );

#ifdef SLIC_LOG
	log() << LOG::debug << "******** loop 1: create MCParticles ********" << LOG::done;
#endif

	/* Loop #1 : Create MCParticles with no parentage or daughter links. */
	int ntracks = m_reader->nTracks();
	//G4cout << "StdHep.ntracks = " << ntracks << G4endl;
	for (int ihep = 0; ihep < ntracks; ihep++) {

#ifdef SLIC_DEBUG
		printTrack( ihep );
#endif

#ifdef SLIC_DEBUG
		checkParentage( ihep );
#endif

		// create MCP and add to coll
		createParticle(ihep);

#ifdef SLIC_LOG
		log() << LOG::debug << LOG::endl << "--" << LOG::endl << LOG::done;
#endif
	}

#ifdef SLIC_LOG
	log() << LOG::debug << "******** loop 2: parent setup ********" << LOG::done << LOG::done;
#endif

	/* Loop #2 : Set the MCParticle parentage. */
	for (int ihep = 0; ihep < ntracks; ihep++) {

#ifdef SLIC_LOG
		printIndex( ihep );
//            printMothers( ihep );      
#endif

		MCParticleImpl* mcp = dynamic_cast<MCParticleImpl*>(m_currentMcpColl->getElementAt(ihep));

		if (mcp) {
			setupParents(ihep, mcp);
		}

#ifdef SLIC_LOG
		log() << LOG::debug << LOG::done << "--" << LOG::done << LOG::done;
#endif

	}

#ifdef SLIC_LOG
	log() << LOG::debug << "******** loop 3: daughter setup ********" << LOG::done << LOG::done;
#endif

	/* Loop #3 : Setup the daughters. */
	for (int ihep = 0; ihep < ntracks; ihep++) {

#ifdef SLIC_LOG
		printIndex( ihep );
		printDaughters( ihep );
#endif

		MCParticleImpl* mcp = dynamic_cast<MCParticleImpl*>(m_currentMcpColl->getElementAt(ihep));

		if (mcp) {
			setupDaughters(ihep, mcp);
		} else {
			log() << LOG::error << "mcp is null; ihep <" << ihep << "> does not appear to be a valid idx!"
					<< LOG::done;
		}

#ifdef SLIC_LOG
		log() << LOG::debug << LOG::done << "--" << LOG::done << LOG::done;
#endif

	}

#ifdef SLIC_LOG
	log() << LOG::debug << LOG::done;
#endif
}

IMPL::MCParticleImpl* StdHepToLcioConvertor::createParticle(int ihep) {
	// new MCP
	MCParticleImpl* mcp = new MCParticleImpl();

	// PDG
	int pdgid = m_reader->pid(ihep);
	mcp->setPDG(pdgid);

	G4ParticleDefinition* pdef = G4ParticleTable::GetParticleTable()->FindParticle(pdgid);

#ifdef SLIC_LOG
	if ( pdef != 0 ) {
		log() << LOG::debug << "found definition for particle <" << pdef->GetParticleName() << "> with pdgid <" << pdgid << ">" << LOG::done;
	}
	else {
		log() << LOG::debug << "no definition found for particle with PDG ID <" << pdgid << ">" << LOG::done;
	}
#endif

	/*
	 * Geant4 Particle definition exists for this MCParticle,
	 * so set the charge from it.
	 */
	if (pdef != 0) {
		mcp->setCharge(pdef->GetPDGCharge());
	}
	/*
	 * No Geant4 particle definition, so flag charge as invalid.
	 */
	else {
		mcp->setCharge(LcioMcpManager::m_NAN);
	}

	// momentum vec
	float p[3] = { m_reader->Px(ihep), m_reader->Py(ihep), m_reader->Pz(ihep) };
	mcp->setMomentum(p);

	// mass
	mcp->setMass(m_reader->M(ihep));

	// vertex
	double vtx[3] = { m_reader->X(ihep), m_reader->Y(ihep), m_reader->Z(ihep) };
	mcp->setVertex(vtx);

	// generator status
	mcp->setGeneratorStatus(m_reader->status(ihep));

	// sim status = none
	mcp->setSimulatorStatus(0);

	// creation time
	mcp->setTime(m_reader->T(ihep) / c_light);

#if LCIO_VERSION_GE(1, 60)

	// Spin and color flow code only applicable to StdHep4 files.
	if(m_reader->isStdHepEv4())
	{
		// Set particle spin.
		float spin[3];
		spin[0] = m_reader->spinX(ihep);
		spin[1] = m_reader->spinY(ihep);
		spin[2] = m_reader->spinZ(ihep);
		mcp->setSpin(spin);

		// Set particle color flow.
		int colorFlow[2] = {m_reader->colorflow(ihep, 0), m_reader->colorflow(ihep, 1)};
		mcp->setColorFlow(colorFlow);
	}
#endif

	// Add particle to collection.
	m_currentMcpColl->addElement(mcp);

	return mcp;
}

// util functions
void StdHepToLcioConvertor::setupParents(int ihep, IMPL::MCParticleImpl* mcp) {
//#ifdef SLIC_LOG
//        log() << LOG::debug << "setupParents" << LOG::done;
//#endif

	// get parent indices
	int mom1_idx = m_reader->mother1(ihep) - 1;
	int mom2_idx = m_reader->mother2(ihep) - 1;

	std::vector<int> parVec;
	makeIndexVec(mom1_idx, mom2_idx, parVec);

	if (parVec.size() > 0) {
		for (std::vector<int>::iterator it = parVec.begin(); it != parVec.end(); it++) {
			if (*it != ihep) {
				addParent(*it, mcp);
			}
//#ifdef SLIC_LOG
//                else 
//                {
//                    log() << LOG::debug << "Ignoring parent pointing to self for ihep <" << ihep << ">" << LOG::done;
//                }
//#endif
		}
	}
}

void StdHepToLcioConvertor::setupDaughters(int ihep, IMPL::MCParticleImpl* mcp) {
//#ifdef SLIC_LOG
//        log() << LOG::debug << "setupDaughters for ihep <" << ihep << "> and MCParticle <" << mcp << ">" << LOG::done;   
//#endif
#ifdef SLIC_LOG
	log() << LOG::debug << "MCParticle idx: " << LcioMcpManager::instance()->getParticleIndex(LcioMcpManager::instance()->getInitialMcpCollection(), mcp) << LOG::done;
#endif

	// get dau indices
	int dau1_idx = m_reader->daughter1(ihep) % 10000 - 1;
	int dau2_idx = m_reader->daughter2(ihep) % 10000 - 1;

#ifdef SLIC_LOG
	log() << LOG::debug << "dau1 <" << dau1_idx << ">" << LOG::done;
	log() << LOG::debug << "dau2 <" << dau2_idx << ">" << LOG::done;
#endif

	// Check for valid daughter indices.  Some generators such as Whizard put bad values in these fields.
	if (!isValidIndex(dau1_idx) || !isValidIndex(dau2_idx)) {
#ifdef SLIC_LOG
		log() << LOG::debug << "ihep <" << ihep << "> has invalid daughter index range <" << dau1_idx << ", " << dau2_idx << ">." << LOG::done;
#endif            
		return;
	}

	std::vector<int> dauVec;
	dauVec = makeIndexVec(dau1_idx, dau2_idx, dauVec);

#ifdef SLIC_LOG
	log() << LOG::debug << "printing dau vec ..." << LOG::done;
	for (std::vector<int>::const_iterator it = dauVec.begin();
			it != dauVec.end();
			it++)
	{
		log() << LOG::debug << (*it) << " ";
	}
	log () << LOG::done;

#endif

	if (dauVec.size() > 0) {
		for (std::vector<int>::iterator it = dauVec.begin(); it != dauVec.end(); it++) {
			if (*it != ihep) {
				addDaughter(*it, mcp);
			}
#ifdef SLIC_LOG
			else
			{
				log() << LOG::error << "Ignoring daughter pointing to self!" << LOG::done;
			}
#endif
		}
	}
#ifdef SLIC_LOG
	else {
		log() << LOG::debug << "particle has no daughters" << LOG::done;
	}
#endif
}

bool StdHepToLcioConvertor::hasParent(IMPL::MCParticleImpl* dauMcp, IMPL::MCParticleImpl* parMcp) {
	bool isPar = false;
	if (dauMcp && parMcp) {
		int numParents = dauMcp->getParents().size();

		MCParticleImpl* pMomMcp;

		// find if already lists as parent
		for (int i = 0; i < numParents; i++) {

			pMomMcp = static_cast<MCParticleImpl*>(dauMcp->getParents()[i]);

			if (pMomMcp == parMcp) {
				isPar = true;
				break;
			}
		}
	}

	return isPar;
}

void StdHepToLcioConvertor::addDaughter(int dauIdx, IMPL::MCParticleImpl* parMcp) {
	if (dauIdx >= 0) {

		MCParticleImpl* dauMcp = static_cast<MCParticleImpl*>(m_currentMcpColl->getElementAt(dauIdx));

#ifdef SLIC_LOG
		log() << LOG::debug << "found dau particle <" << dauMcp << ">" << LOG::done;
#endif            

//            assert( dauMcp );

		if (dauMcp) {
			if (!hasParent(dauMcp, parMcp)) {
				dauMcp->addParent(parMcp);

#ifdef SLIC_LOG
				log() << LOG::debug << "added daughter <" << dauIdx << ">" << LOG::done;
#endif
			}
#ifdef SLIC_LOG
			else {
				log() << LOG::debug << "daughter <" << dauIdx << "> already has this parent" << LOG::done;
			}
#endif
		}
#ifdef SLIC_LOG
		else {
			log() << LOG::error << "WARNING: dauMcp or parMcp is null!" << LOG::done;
		}
#endif

	}
}

void StdHepToLcioConvertor::addParent(int parIdx, IMPL::MCParticleImpl* mcp) {
#ifdef SLIC_LOG
	log() << LOG::debug << "addMcpParent" << LOG::done;
#endif

	if (parIdx >= 0) {

		/* If index is > size of collection, the particle's parent doesn't exist yet.  We need to die! */
		if (parIdx > m_currentMcpColl->getNumberOfElements()) {
#ifdef SLIC_LOG
			log() << LOG::fatal << "StdHepToLcioConverter::addMcpParent - Parent index is out of range <" << parIdx << ">" << LOG::done;
#endif
			G4Exception("", "", FatalException, "StdHep index (ihep) is out of range.");
		}

		MCParticleImpl* parMcp = static_cast<MCParticleImpl*>(m_currentMcpColl->getElementAt(parIdx));

		if (parMcp) {
			if (!hasParent(mcp, parMcp)) {
				mcp->addParent(parMcp);

#ifdef SLIC_LOG
				log() << LOG::debug << "added parent <" << parIdx << ">" << LOG::done;
#endif
			}
#ifdef SLIC_LOG
			else {
				log() << LOG::debug << "daughter already has parent <" << parIdx << ">" << LOG::done;
			}
#endif
		} else {
			G4Exception("", "", FatalException, "Failed to get parent particle from MCParticle collection.");
		}
	}
#ifdef SLIC_LOG
	else {
		log() << LOG::debug << "ignoring parIdx = -1 " << LOG::done;
	}
#endif
}

std::vector<int> StdHepToLcioConvertor::makeIndexVec(int idx1, int idx2, std::vector<int>& vec) {
#ifdef SLIC_LOG
	log() << LOG::debug << "idx1 <" << idx1 << ">" << LOG::done;
	log() << LOG::debug << "idx2 <" << idx2 << ">" << LOG::done;
#endif

	if (idx1 >= 0 && idx2 >= 0) {

		if (idx1 < idx2) {

#ifdef SLIC_LOG
			log() << LOG::debug << "range: idx1 to idx2" << LOG::done;
#endif

			for (int i = idx1; i < (idx2 + 1); i++) {
				vec.push_back(i);
			}
		} else if (idx1 > idx2) {

#ifdef SLIC_LOG
			log() << LOG::debug << "discrete: idx1 and idx2" << LOG::done;
#endif

			vec.push_back(idx1);
			vec.push_back(idx2);
		}
		// indices are equal
		else {

#ifdef SLIC_LOG
			log() << LOG::debug << "single: idx1 == idx2" << LOG::done;
#endif

			vec.push_back(idx1);
		}
	} else if (idx1 >= 0) {

#ifdef SLIC_LOG
		log() << LOG::debug << "single: idx1 only" << LOG::done;
#endif

		vec.push_back(idx1);
	} else if (idx2 >= 0) {

#ifdef SLIC_LOG
		log() << LOG::debug << "single: idx2 only" << LOG::done;
#endif

		vec.push_back(idx2);
	}

	return vec;
}

void StdHepToLcioConvertor::printIndex(int ihep) {
	log() << LOG::debug << "ihep <" << ihep << ">" << LOG::done;
}

void StdHepToLcioConvertor::printMothers(int ihep) {
	log() << LOG::debug << "mom1 <" << m_reader->mother1(ihep) - 1 << ">" << LOG::done;
	log() << LOG::debug << "mom2 <" << m_reader->mother2(ihep) - 1 << ">" << LOG::done;
}

void StdHepToLcioConvertor::printDaughters(int ihep) {
	log() << LOG::debug << "dau1 <" << m_reader->daughter1(ihep) - 1 << ">" << LOG::done;
	log() << LOG::debug << "dau2 <" << m_reader->daughter2(ihep) - 1 << ">" << LOG::done;
}

void StdHepToLcioConvertor::printTrack(int ihep) {
	printIndex(ihep);

	log() << LOG::debug << "pid <" << m_reader->pid(ihep) << ">" << LOG::done;
	log() << LOG::debug << "M = " << m_reader->M(ihep) << LOG::done;
	log() << LOG::debug << "T = " << m_reader->T(ihep) << LOG::done;
	log() << LOG::debug << "status <" << m_reader->status(ihep) << ">" << LOG::done;

	printMothers(ihep);
	printDaughters(ihep);

	log() << LOG::debug << "P = (" << m_reader->Px(ihep) << ", " << m_reader->Py(ihep) << ", "
			<< m_reader->Pz(ihep) << ")" << LOG::done;

	log() << LOG::debug << "Vtx = (" << m_reader->X(ihep) << ", " << m_reader->Y(ihep) << ", "
			<< m_reader->Z(ihep) << ")" << LOG::done;

	log() << LOG::debug << LOG::done;
}

void StdHepToLcioConvertor::checkParentage(int ihep) {
	int mom1 = m_reader->mother1(ihep) - 1;
	int mom2 = m_reader->mother2(ihep) - 1;
	int dau1 = m_reader->daughter1(ihep) - 1;
	int dau2 = m_reader->daughter2(ihep) - 1;

	// dau1 < ihep
	if (dau1 >= 0 && dau1 < ihep) {
		log() << LOG::debug << "WARNING: dau1 < ihep" << LOG::done;
	}

	// dau2 < ihep
	if (dau2 >= 0 && dau2 < ihep) {
		log() << LOG::debug << "WARNING: dau2 < ihep" << LOG::done;
	}

	// mom1 > ihep
	if (mom1 >= 0 && mom1 > ihep) {
		log() << LOG::debug << "WARNING: mom1 > ihep" << LOG::done;
	}

	// mom2 > ihep
	if (mom2 >= 0 && mom2 > ihep) {
		log() << LOG::debug << "WARNING: mom2 > ihep" << LOG::done;
	}

	// first particle in list has parents
	if (ihep == 0 && (mom1 >= 0 || mom2 >= 0)) {
		log() << LOG::debug << "WARNING: ihep == 0 has parents" << LOG::done;
	}

	// particle past first 2 with no mother
	if (ihep > 1 && (mom1 < 0 && mom2 < 0)) {
		log() << LOG::debug << "WARNING: ihep > 1 with no mother" << LOG::done;
	}

	// mother points to self
	if (ihep == mom1 || ihep == mom2) {
		log() << LOG::debug << "WARNING: mom1 or mom2 == ihep" << LOG::done;
	}

	// daughter points to self
	if (ihep == dau1 || ihep == dau2) {
		log() << LOG::debug << "WARNING: dau1 or dau2 == ihep" << LOG::done;
	}
}

} // namespace slic
