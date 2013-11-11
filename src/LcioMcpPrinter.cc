// $Header: /nfs/slac/g/lcd/cvs/lcdroot/slic/src/LcioMcpPrinter.cc,v 1.22 2013-11-06 00:23:37 jeremy Exp $

// SLIC
#include "LcioMcpPrinter.hh"

// LCDD
#include "lcdd/util/StringUtil.hh"

// LCIO
#include "IMPL/LCCollectionVec.h"

using EVENT::LCCollection;
using IMPL::LCCollectionVec;
using EVENT::MCParticle;
using EVENT::MCParticleVec;

using namespace std;
using std::ios;

namespace slic {

const int LcioMcpPrinter::idx_width = 4;
const int LcioMcpPrinter::ptr_width = 10;
const int LcioMcpPrinter::pdg_width = 5;
const int LcioMcpPrinter::par_width = 15;
const int LcioMcpPrinter::dau_width = 15;
const int LcioMcpPrinter::mom_width = 20;
const int LcioMcpPrinter::vtx_width = 20;
const int LcioMcpPrinter::end_width = 20;
const int LcioMcpPrinter::time_width = 6;
const int LcioMcpPrinter::mass_width = 6;
const int LcioMcpPrinter::chrg_width = 4;
const int LcioMcpPrinter::e_width = 6;
const int LcioMcpPrinter::stat_width = 5;
const int LcioMcpPrinter::flags_width = 16;

LcioMcpPrinter::LcioMcpPrinter() :
		Module("LcioMcpPrinter"), m_coll(0), m_collName("") {
}

LcioMcpPrinter::~LcioMcpPrinter() {
}

void LcioMcpPrinter::printParticleCollection(const std::string& collName, EVENT::LCCollection* coll) {
	m_coll = coll;
	m_collName = collName;
	printParticleCollection();
}

void LcioMcpPrinter::printParticleCollection() {
	head();

	if (m_coll != 0) {

		//      LCCollectionVec* mcpCollVec = static_cast<LCCollectionVec*> ( m_coll );

		if (m_coll->getNumberOfElements() > 0) {

			log() << LOG::debug << "Collection Name: " << m_collName << LOG::done;
			log() << LOG::debug << "# of Particles: " << m_coll->getNumberOfElements() << LOG::done;

			colLabels();

			LCCollectionVec* vec = static_cast<LCCollectionVec*>(m_coll);
			for (LCCollectionVec::iterator iter = vec->begin(); iter != vec->end(); iter++) {
				printParticle(static_cast<EVENT::MCParticle*>(*iter));
			}

#ifdef SLIC_LOG
			flagLabels();
#endif

		} else {
			log() << LOG::debug << "COLLECTION IS EMPTY" << LOG::done;
		}
	} else {
		log() << LOG::debug << "NO CURRENT MCPARTICLE COLLECTION" << LOG::done;
	}

	foot();
}

void LcioMcpPrinter::flagLabels() {
	line();

	log() << LOG::debug << "FLAGS: createdInSimulation  Backscatter  vertexIsNotEndpointOfParent  "
			<< "DecayedInTracker  DecayedInCalorimeter  LeftDetector  Stopped";

	line();
}

void LcioMcpPrinter::printParticle(EVENT::MCParticle* mcp) {
	// index
	log().getOutputStream().width(idx_width);
	log().getOutputStream() << left;
	log() << LOG::debug << findParticleIndex(mcp);

	sep();

	// ptr
	log().getOutputStream().width(ptr_width);
	log().getOutputStream() << left;
	log() << LOG::debug << mcp;

	sep();

	// PDG
	log().getOutputStream().width(pdg_width);
	log() << LOG::debug << left << mcp->getPDG();

	sep();

	// parents
	log().getOutputStream().width(par_width);
	log() << LOG::debug << left << makeIndexVectorString(findParentIndices(mcp));

	sep();

	// daughters
	log().getOutputStream().width(dau_width);
	log() << LOG::debug << left << makeIndexVectorString(findDaughterIndices(mcp));

	sep();

	// momentum
	log().getOutputStream().width(mom_width);
	log() << LOG::debug << left << makeDoubleArray3String(mcp->getMomentum());

	sep();

	// vertex
	log().getOutputStream().width(vtx_width);
	log() << LOG::debug << left << makeDoubleArray3String(mcp->getVertex());

	sep();

	// endpoint
	log().getOutputStream().width(end_width);
	log() << LOG::debug << left << makeDoubleArray3String(mcp->getEndpoint());

	sep();

	// time
	log().getOutputStream().width(time_width);
	log() << LOG::debug << left << mcp->getTime();

	sep();

	// mass
	log().getOutputStream().width(mass_width);
	log() << LOG::debug << left << mcp->getMass();

	sep();

	// charge
	log().getOutputStream().width(chrg_width);
	log() << LOG::debug << mcp->getCharge();

	sep();

	// energy
	log().getOutputStream().width(e_width);
	log() << LOG::debug << mcp->getEnergy();

	// gen status
	log() << LOG::debug << mcp->getGeneratorStatus();

#ifdef SLIC_LOG
	sep();
	log() << LOG::debug << left << makeSimStatusCodesString( mcp );
#endif

	line();
	line();
}

std::string LcioMcpPrinter::makeDoubleArray3String(const double* arr) {

	return "(" + StringUtil::toString(arr[0]) + ", " + StringUtil::toString(arr[1]) + ", "
			+ StringUtil::toString(arr[2]) + ")";
}

std::string LcioMcpPrinter::makeFloatArray3String(const float* arr) {
	return "(" + StringUtil::toString(arr[0]) + ", " + StringUtil::toString(arr[1]) + ", "
			+ StringUtil::toString(arr[2]) + ")";
}

std::string LcioMcpPrinter::makeSimStatusCodesString(EVENT::MCParticle* mcp) {
	return StringUtil::toString(mcp->isCreatedInSimulation()) + " "
			+ StringUtil::toString(mcp->isBackscatter()) + " "
			+ StringUtil::toString(mcp->vertexIsNotEndpointOfParent()) + " "
			+ StringUtil::toString(mcp->isDecayedInTracker()) + " "
			+ StringUtil::toString(mcp->isDecayedInCalorimeter()) + " "
			+ StringUtil::toString(mcp->hasLeftDetector()) + " " + StringUtil::toString(mcp->isStopped());
}

std::string LcioMcpPrinter::makeIndexVectorString(const IndexVector& idxVec) {
	std::string idx_vec;
	idx_vec = "[ ";

	for (IndexVector::const_iterator iter = idxVec.begin(); iter != idxVec.end(); iter++) {
		int this_idx = (*iter);
		idx_vec += StringUtil::toString(this_idx) + " ";
	}

	idx_vec += "]";
	return idx_vec;
}

LcioMcpPrinter::IndexVector LcioMcpPrinter::findDaughterIndices(EVENT::MCParticle* mcp) {
	IndexVector dauIdxs;
	const EVENT::MCParticleVec& dauVec = mcp->getDaughters();
	for (EVENT::MCParticleVec::const_iterator iter = dauVec.begin(); iter != dauVec.end(); iter++) {
		dauIdxs.push_back(findParticleIndex((const_cast<MCParticle*>(*iter))));
	}

	return dauIdxs;
}

LcioMcpPrinter::IndexVector LcioMcpPrinter::findParentIndices(EVENT::MCParticle* mcp) {
	IndexVector parIdxs;
	const EVENT::MCParticleVec& parVec = mcp->getParents();
	for (EVENT::MCParticleVec::const_iterator iter = parVec.begin(); iter != parVec.end(); iter++) {
		parIdxs.push_back(findParticleIndex((const_cast<MCParticle*>(*iter))));
	}

	return parIdxs;
}

int LcioMcpPrinter::findParticleIndex(EVENT::MCParticle* mcp) {
	int idx = 0;
	bool fnd = false;
	LCCollectionVec* mcpCollVec = static_cast<LCCollectionVec*>(m_coll);
	for (LCCollectionVec::iterator iter = mcpCollVec->begin(); iter != mcpCollVec->end(); iter++) {
		// ptr compare
		if ((static_cast<MCParticle*>(*iter)) == mcp) {
			fnd = true;
			break;
		}
		idx++;
	}

	if (!fnd) {
		idx = -1;
	}

	return idx;
}

inline void LcioMcpPrinter::sep() {
	log() << LOG::debug << "|";
}

inline void LcioMcpPrinter::line() {
	log() << LOG::debug << LOG::done;
}

inline void LcioMcpPrinter::head() {
	log() << LOG::debug << "---------------------MCParticle Collection---------------------";

	line();
}

inline void LcioMcpPrinter::foot() {
	log() << LOG::debug << "---------------------------------------------------------------";

	line();
}

void LcioMcpPrinter::colLabels() {
	log().getOutputStream().width(idx_width);
	log() << LOG::debug << left << "idx";

	sep();

	log().getOutputStream().width(ptr_width);
	log() << LOG::debug << left << "ptr";

	sep();

	log().getOutputStream().width(pdg_width);
	log() << LOG::debug << left << "pdg";

	sep();

	log().getOutputStream().width(par_width);
	log() << LOG::debug << left << "par";

	sep();

	log().getOutputStream().width(dau_width);
	log() << LOG::debug << left << "dau";

	sep();

	log().getOutputStream().width(mom_width);
	log() << LOG::debug << left << "mom";

	sep();

	log().getOutputStream().width(vtx_width);
	log() << LOG::debug << left << "vtx";

	sep();

	log().getOutputStream().width(end_width);
	log() << LOG::debug << left << "end";

	sep();

	log().getOutputStream().width(time_width);
	log() << LOG::debug << left << "time";

	sep();

	log().getOutputStream().width(mass_width);
	log() << LOG::debug << left << "mass";

	sep();

	log().getOutputStream().width(chrg_width);
	log() << LOG::debug << left << "chg";

	sep();

	log().getOutputStream().width(e_width);
	log() << LOG::debug << "E";

	sep();

#ifdef SLIC_LOG
	log().getOutputStream().width(flags_width);
	log() << LOG::debug << "flags";
#endif

	line();
}
}
